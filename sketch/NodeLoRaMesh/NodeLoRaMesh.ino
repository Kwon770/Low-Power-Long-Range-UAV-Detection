#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <LoPoDeMesh.h>
#include <SPI.h>


RH_RF95 rf95w(HELTEC_CS, HELTEC_DI0); // LoRa transceiver driver
RHMesh *manager; // Class to manage message delivery and receipt, using the drvier declared above

unsigned long propagateInterval = 0;
uint8_t routes[NODES]; // full routing table for mesh
int16_t rssi[NODES]; // signal strength of nodes
char buf[MAX_MESSAGE_LEN]; // string buffer

// Initalize or clear all objects and variables related with LoRa
void initLoRaData() {
	// Initalize node array data
	for (uint8_t node = 1; node <= NODES; node++) {
		if (node == NODE_ID) {
			routes[node - 1] = 255;
		} else {
			routes[node - 1] = 0;
		}
		rssi[node - 1] = 0;
	}
}

// Initalize and confiture all objects and variables related with LoRa
void initLoRaDriver() {
	// Initalize RHMesh
	Serial.print(F("initializing node : "));

	// Configure SPI pin in the board
	SPI.begin(HELTEC_SCK, HELTEC_MISO, HELTEC_MOSI, HELTEC_CS);

	// Initalize RadioHead Mesh object
	manager = new RHMesh(rf95w, NODE_ID);
	if (!manager->init()) {
		Serial.println(F("!! init failed !!"));
	} else {
		Serial.println("done");
	}

	// Configure LoRa driver
	rf95w.setTxPower(LORA_TX_POWER, LORA_TX_USERFO);
	rf95w.setFrequency(LORA_FREQUENCY);
	rf95w.setCADTimeout(LORA_TIMEOUT);

	RH_RF95::ModemConfig modemConfig = {
		LORA_MODEM_1D,
		LORA_MODEM_2E,
		LORA_MODEM_26	
	}; 
	rf95w.setModemRegisters(&modemConfig);
	if (!rf95w.setModemConfig(LORA_MODEM_CONFIG)) { // Recheck modem config
		Serial.println(F("!! Modem config failed !!"));
	}
	Serial.println("RF95 Ready");

	initLoRaData();
}


void setup() {
	randomSeed(analogRead(0));
	Serial.begin(BUAD_RATE);
	while (!Serial); // Wait for serial port to be available

	Serial.print(F("[NODE "));
	Serial.print(NODE_ID);
	Serial.println(F("]"));

	// Use flash memory for string instead of RAM
	Serial.print(F("initializing node : "));

	// Initialize LoRa network
	initLoRaDriver();

	// Print free memory
	Serial.print(F("RAM = "));
	Serial.println(ESP.getFreeHeap());
	Serial.print("SRAM = ");
	Serial.println(ESP.getPsramSize());
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

void updateRouteInfo() {
	// Update routing data toward all other nodes
	for (uint8_t dest = 1; dest <= NODES; dest++) {
		if (dest == NODE_ID) { // if destination is self
			routes[dest - 1] = 255; // mark itself

		} else {
			manager->doArp(dest);
			RHRouter::RoutingTableEntry *route = manager->getRouteTo(dest);
			if (route == NULL) continue;

			routes[dest - 1] = route->next_hop; // update route
			rssi[dest - 1] = rf95w.lastRssi();

			if (routes[dest - 1] == 0) { // if there is no route
				rssi[dest - 1]= 0; // reset rssi
			}

		}
	}
}


void generateRouteInfoStringInBuf() {
	buf[0] = '\0';
	for(uint8_t node = 1 ; node <= NODES; node++) {
		sprintf(buf + strlen(buf), "%d", routes[node - 1]);
		strcat(buf, ",");
		sprintf(buf + strlen(buf), "%d", rssi[node - 1]);
		if (node < NODES) {
			strcat(buf, "/");
		}
	}
}

void propagateRouteInfo() {
	while (millis() > propagateInterval) {
		updateRouteInfo();
		generateRouteInfoStringInBuf();

		// Print transmit info
		Serial.print(F("->"));
		Serial.print(GROUND_ID);
		Serial.print(F(" :"));
		Serial.print(buf);

		// Send route info 
		//uint8_t sendBuf[strlen(buf)];
		//memcpy(sendBuf, buf, strlen(buf));
		uint8_t error = manager->sendtoWait((uint8_t *)buf, sizeof(buf), GROUND_ID);
		if (error != RH_ROUTER_ERROR_NONE) { // if transmit error is occured
						     // Print detail error
			Serial.print(F(" !! ")); 
			Serial.println(getErrorString(error));
		} else {
			Serial.println(F(" OK")); // if transmit is successful
		}

		propagateInterval = millis() + 3000;
	}
}



void loop() {
	// Propagate route info to all other nodes
	propagateRouteInfo();
}
