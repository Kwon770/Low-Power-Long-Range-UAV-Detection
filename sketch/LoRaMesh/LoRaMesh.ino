#include <EEPROM.h>
#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>

#define LED 9 // LED Pin number
#define NODES 2 // Nodes count
#define BUAD_RATE 115200 // Default ESP32 baud rate


// Node Data
uint8_t nodeId;
uint8_t routes[NODES]; // full routing table for mesh
int16_t rssi[NODES]; // signal strength of nodes

// Singleton instance of the LoRa transceiver driver
RH_RF95 rf95w;
// Class to manage message delivery and receipt, using the drvier declared above
RHMesh *manager;
// Message buffer
char buf[RH_MESH_MAX_MESSAGE_LEN];



// Return free memeory by caculation
int freeMem() {
	extern int __heap_start, *__brkval; // Arduino core memeory variables
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup() {
	randomSeed(analogRead(0));
	pinMode(LED, OUTPUT);
	Serial.begin(BUAD_RATE);
	while (!Serial); // Wait for serial port to be available


	// Read nodeId from the address 0 of EEPROM
	nodeId = EEPROM.read(0);
	// Use flash memory for string instead of RAM
	Serial.print(F("initializing node : "));

	// Initalize driver
	manager = new RHMesh(rf95w, nodeId);
	if (!manager->init()) {
		Serial.println(F("!! init failed !!"));
	} else {
		Serial.println("done");
	}

	rf95w.setTxPower(23, false); // power = 23, useRFO = true
	rf95w.setFrequency(915.0); // frequency = 915MHz
	rf95w.setCADTimeout(500); // timeout = 500ms

	// Set driver modem config
	//  1d,     1e,      26
	// { 0x72,   0x74,    0x04}, // Bw125Cr45Sf128 (the chip default), AGC enabled
	// { 0x92,   0x74,    0x04}, // Bw500Cr45Sf128, AGC enabled
	// { 0x48,   0x94,    0x04}, // Bw31_25Cr48Sf512, AGC enabled
	// { 0x78,   0xc4,    0x0c}, // Bw125Cr48Sf4096, AGC enabled
	// { 0x72,   0xb4,    0x04}, // Bw125Cr45Sf2048, AGC enabled
	RH_RF95::ModemConfig modemConfig = {
		0x78,
		0xc4,
		0x08
	} // Low data-rate, Slow-long modem
	rf95w.setModemRegisters(&modemConfig);
	if (!rf95w.setModemConfig(RH_RF95::Bw125Cr48Sf4096)) { // Recheck modem config
		Serial.println(F("!! Modem config failed !!"));
	}
	Serial.println("RF95 Ready");

	// Initalize node array data
	for (uint8_t node = 0; node < NODES; node++) {
		routes[node] = 0;
		rssi[node] = 0;
	}

	// Print free memory
	Serial.print(F("Memory = "));
	Serial.println(freeMem());
}



// Exchange error number to error string by flash string
const __FlashStringHelper* getErrorString(uint8_t error) {
	switch(error) {
		case 1: return F("INVALID LENGTH");
			break;
		case 2: return F("NO ROUTE");
			break;
		case 3: return F("TIMEOUT");
			break;
		case 4: return F("NO REPLY");
			break;
		case 5: return F("UNABLE TO DELIVER");
			break;
	}
	return F("UNKNOWN");
}

void updateRoutingTable() {
	// Update routing data toward all other nodes
	for (uint8_t dest = 1; dest <= NODES; dest++) {
		RHRouter::RoutingTableEntry *route = manager->getRouteTo(dest);
		if (dest == destId) { // if destination is self
			routes[dest - 1] = 255; // mark itself
		} else {
			routes[dest - 1] = route->next_hop; // update route

			if (routes[dest - 1] == 0) { // if there is no route
				rssi[dest - 1]= 0; // reset rssi
			}
		}
	}
}

// Create the routing info by JSON string to each nodes, on char* p
void getRouteInfoString(char *p, size_t len) {
	p[0] = '\0';
	strcat(p, "[");
	for(uint8_t node = 1 ; node <= NODES; n++) {
		strcat(p, "{\"n\":");
		sprintf(p + strlen(p), "%d", routes[node - 1]);
		strcat(p, ",");
		strcat(p, "\"r\":");
		sprintf(p + strlen(p), "%d", rssi[node - 1]);
		strcat(p, "}");
		if (nide < NODES) {
			strcat(p, ",");
		}
	}
	strcat(p, "]");
}

// Print node info with buffer by char pointer on serial
void printNodeInfo(uint8_t node, char *s) {
  Serial.print(F("node: "));
  Serial.print(F("{"));
  Serial.print(F("\""));
  Serial.print(node);
  Serial.print(F("\""));
  Serial.print(F(": "));
  Serial.print(s);
  Serial.println(F("}"));
}

void loop() {
	// Send the route info of current node to all other nodes
	for (uint8_t dest = 1; dest <= NODES; dest++) {
		if (dest == destId) continue; // self skip


		updateRoutingTable();
		getRouteInfoString(buf, RH_MESH_MAX_MESSAGE_LEN);

		// Print transmit info
		Serial print(F("->"));
		Serial.print(dest);
		Serial.print(F(" :"));
		Serial.print(buf);

		// Send route info 
		uint8_t error = manager->sendtoWait((uint8_t *)buf, strlen(buf), dest);
		if (error != RH_ROUTER_ERROR_NONE) { // if transmit error is occured
			// Print detail error
			Serial.println();
			Serial.println(F(" !! "));
			Serial.println(getErrorString(error));
		} else {
			Serial.println(F(" OK")); // if transmit is successful
			
			// if there is intermediate node in route to destination node
			RHRouter::RoutingTableEntry *route = manager->getRouteTo(dest);
			if (route->next_hope != 0) {
				// Update rssi data by previous transmit info
				rssi[route->next_hop - 1] = rf95w.lastRssi();
			}
		}

		if (nodeId == 1) printNodeInfo(nodeId, buf);  // debugging from Ground node

		// Listen incoming messages
		// Wait for a random length of time before next transmit during listening
		unsigned long nextTransmit = millis() + random(3000, 5000);
		while (nextTransmit > millis()) {
			int waitTime = nextTransmit - millis();
			uint8_t len = sizeof(buf); // transmit buffer size
			uint8_t src; // transmit source node

			// Listen incoming trasnmit for wait time
			// Save data on buf and source node info
			if (manager->recvfromAckTimeout((uint8_t *)buf, &len, waitTime, &src)) {
				buf[len] = '\0'; // null terminate string

				// Print received transmit info
				Serial.print(src);
				Serial.print(F("-> :"));
				Serial.println(buf);

				if (nodeId == 1) printNodeInfo(src, buf); // debugging from Ground node

				// if received trasnmit come from intermediate node
				RHRouter::RoutingTableEntry *route = manager->getRouteTo(src);
				if (route->next_hop != 0) { 
					// Update rssi data by previous transmit info
					rssi[route->next_hop - 1] = rf95w.lastRssi();
				}
			}
	}
}
