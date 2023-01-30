#include <WiFi.h>
#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <LoPoDeMesh.h>

#include <WiFiClient.h>
#include <WiFiConfig.h>
#include <SPI.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <MQTTConfig.h>
#include <string.h>


// MQTT info
const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_USERNAME = MQTT_GROUND_USERNAME;
const char* MQTT_PASSWORD = MQTT_GROUND_PASSWORD;
const char* DATATOPIC = "btt_mesh_gateway/data";

WiFiClient espClient; // Arduino-ESP32 WiFiSTA object
PubSubClient mqtt_client(espClient); // MQTT client object

RH_RF95 rf95w(HELTEC_CS, HELTEC_DI0); // LoRa transceiver driver
RHMesh *manager; // Class to manage message delivery and receipt, using the drvier declared above

uint8_t routes[NODES]; // full routing table for mesh
int16_t rssi[NODES]; // signal strength of nodes
char buf[MAX_MESSAGE_LEN]; // string buffer

// Connect to MQTT broker
void mqtt_connect() {
	// Loop until we're reconnected
	while (!mqtt_client.connected()) {
		Serial.print("Connecting to MQTT...  ");

		// Generate randome mqtt client id
		String mqtt_clientId = "gateway-";
		mqtt_clientId += String(random(0xffff), HEX);

		// Attempt to connect
		if (mqtt_client.connect(mqtt_clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) { // if connection is successful
			Serial.println("[Connected]");
		} else { // if connection is failed
			 // Log the reason of fail and Retry after 2000ms
			Serial.print("[Failed, rc=");
			Serial.print(mqtt_client.state());
			Serial.println("]");
			delay(2000);
		}
	}
}

// Initalize or clear all objects and variables related with LoRa
void initLoRaData() {
	// Initalize node array data
	for (uint8_t node = 1; node <= NODES; node++) {
		if (node == GROUND_ID) {
			routes[node - 1] = 255;
		} else {
			routes[node - 1] = 0;
		}
		rssi[node - 1] = 0;
	}

	/*
	// Clear route data
	manager->clearRoutingTable();
	for(uint8_t node = 1; node <= NODES; node++) {
	manager->addRouteTo(node, 0);
	}
	 */
}

// Initalize and confiture all objects and variables related with LoRa
void initLoRaDriver() {
	// Initalize RHMesh
	Serial.print(F("initializing node : "));

	// Configure SPI pin in the board
	SPI.begin(HELTEC_SCK, HELTEC_MISO, HELTEC_MOSI, HELTEC_CS);

	// Initalize RadioHead Mesh object
	manager = new RHMesh(rf95w, GROUND_ID);
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
	}; // Low data-rate, Slow-long modem
	rf95w.setModemRegisters(&modemConfig);
	if (!rf95w.setModemConfig(LORA_MODEM_CONFIG)) { // Recheck modem config
		Serial.println(F("!! Modem config failed !!"));
	}
	Serial.println("RF95 Ready");

	initLoRaData();
}


void setup()   {
	randomSeed(analogRead(0));
	Serial.begin(BUAD_RATE);
	while (!Serial); // Wait till serial is ready

	Serial.println("[Ground Node 1]");

	Serial.print("Connecting to AP - ");
	Serial.print(WIFI_SSID);

	// Initialize WiFi connection
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	// If connection isn't done yet, wait for 250ms till done
	while (WiFi.status() != WL_CONNECTED) {
		delay(250);
		Serial.print(". ");
	}
	Serial.println("[Connected]");

	// Initialize MQTT server connection
	mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
	mqtt_connect();

	// Initialize LoRa network
	initLoRaDriver();
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

/*
   void updateRoutingTable() {
// Update routing data toward all other nodes
for (uint8_t dest = 1; dest <= NODES; dest++) {
if (dest == GROUND_ID) { // if destination is self
routes[dest - 1] = 255; // mark itself
} else {
RHRouter::RoutingTableEntry *route = manager->getRouteTo(dest);
routes[dest - 1] = route->next_hop; // update route

if (routes[dest - 1] == 0) { // if there is no route
rssi[dest - 1]= 0; // reset rssi
}
}
}
}
 */

// Create the routing info by JSON string to each nodes, on buffer
// Element of array start with information of node 1
// n = next node to go to destination (0: impossible to go)
// r = rssi between here to destination
void getRouteInfoJson() {
	buf[0] = '\0';
	strcat(buf, "[");
	for(uint8_t node = 1 ; node <= NODES; node++) {
		strcat(buf, "{\"n\":");
		sprintf(buf + strlen(buf), "%d", routes[node - 1]);
		strcat(buf, ",");
		strcat(buf, "\"r\":");
		sprintf(buf + strlen(buf), "%d", rssi[node - 1]);
		strcat(buf, "}");
		if (node < NODES) {
			strcat(buf, ",");
		}
	}
	strcat(buf, "]");
}

// Publish route info on MQTT server and print it for debugging
// !! You must not change anything in this function !!
// !! Mesh visualizing server will be broken !!
void publishRouteInfo(uint8_t nodeId) {
	// Concatenate strings to format the route info for visualizing server
	const char* str1 = "{\"";
	char str2[4];
	sprintf(str2, "%d", nodeId);
	const char* str3 = "\": ";
	const char* str4 = "}";
	int totalLength = strlen(str1) + strlen(str2) + strlen(str3) + strlen(buf) + strlen(str4);

	// Allocate char pointer for the formatted route info
	char publishBuf[totalLength];

	// Concatenate strings into allocated pointer
	strcpy(publishBuf, str1);
	strcat(publishBuf, str2);
	strcat(publishBuf, str3);
	strcat(publishBuf, buf);
	strcat(publishBuf, str4);
	/*
	   strcpy(publishBuf, str1);
	   strcpy(publishBuf + strlen(str1), str2);
	   strcpy(publishBuf + strlen(str1) + strlen(str2), str3);
	   strcpy(publishBuf + strlen(str1) + strlen(str2) + strlen(str3), route);
	   strcpy(publishBuf + strlen(str1) + strlen(str2) + strlen(str3) + strlen(route), str4);
	 */

	// Print route info in serial for debugging
	Serial.print("[PUB] ");
	Serial.println(publishBuf);

	// Publish data on MQTT server by topic
	mqtt_client.publish(DATATOPIC, publishBuf);
}


void propagateRouteInfo() {
	// Send the route info of current node to all other nodes
	for (uint8_t dest = 1; dest <= NODES; dest++) {
		if (dest == GROUND_ID) continue; // self skip

		//updateRoutingTable();
		getRouteInfoJson();

		// Print transmit info
		Serial.print(F("[SEND] ->"));
		Serial.print(dest);
		Serial.print(F(" :"));
		Serial.print(buf);

		// Send route info 
		uint8_t sendBuf[strlen(buf)];
		memcpy(sendBuf, buf, strlen(buf));
		uint8_t error = manager->sendtoWait((uint8_t *)sendBuf, sizeof(sendBuf), dest);
		if (error != RH_ROUTER_ERROR_NONE) { // if transmit error is occured
						     // Print detail error
			Serial.print(F(" !! "));
			Serial.println(getErrorString(error));
		} else {
			Serial.println(F(" OK")); // if transmit is successful

			// if there is intermediate node in route destination node
			RHRouter::RoutingTableEntry *route = manager->getRouteTo(dest);
			if (route->next_hop != 0) {
				// Update route data(next hop, rssi) by previous transmit info
				routes[dest - 1] = route->next_hop;
				rssi[route->next_hop - 1] = rf95w.lastRssi();
			}
		}

		publishRouteInfo(GROUND_ID);


		// Listen incoming messages
		// Wait for a random length of time before next transmit during listening
		unsigned long nextTransmit = millis() + random(3000, 5000);
		while (nextTransmit > millis()) {

			// Listen incoming trasnmit for wait time
			// Save data on buf and source node info
			uint8_t rcvBuf[MAX_MESSAGE_LEN];
			uint8_t len = sizeof(rcvBuf);
			uint8_t src; // transmit source node
			if (manager->recvfromAck((uint8_t *)rcvBuf, &len, &src)) {
				memcpy(buf, rcvBuf, len);
				buf[len] = '\0';

				// Print received transmit info
				Serial.print("[RECV] ");
				Serial.print(src);
				Serial.print(F("-> :"));
				Serial.println(buf);

				publishRouteInfo(src);

				// if received trasnmit come from intermediate node
				RHRouter::RoutingTableEntry *route = manager->getRouteTo(src);
				if (route->next_hop != 0) { 
					// Update route data(next hop, rssi) by previous transmit info
					routes[src - 1] = route->next_hop;
					rssi[route->next_hop - 1] = rf95w.lastRssi();
				}
			}
		}
	}
}

void loop() {
	// If MQTTT connection isn't done, keep trying and waiting
	if (!mqtt_client.connected()) {
		mqtt_connect();
	}
	mqtt_client.loop();

	delay(50); // Give the ESP time to handle network.

	// Propagate route info to all other nodes
	propagateRouteInfo();
}
