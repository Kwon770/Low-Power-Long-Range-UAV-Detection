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

unsigned long publishInterval = 0;
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

// Initalize objects and variables related to LoRa
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


void updateRouteInfo() {
	// Update routing data toward all other nodes
	for (uint8_t dest = 1; dest <= NODES; dest++) {
		if (dest == GROUND_ID) { // if destination is self
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

void publishRouteInfoInBuf(uint8_t nodeId) {
	strcat(buf, "[");
	sprintf(buf + strlen(buf), "%d", nodeId);

	// Print route info in serial for debugging
	Serial.print("[PUB] ");
	Serial.println(buf);

	// Publish data on MQTT server by topic
	mqtt_client.publish(DATATOPIC, buf);
}

void listenIncomingRouteInfo() {
	uint8_t len = sizeof(buf);
	uint8_t src;
	if (manager->recvfromAck((uint8_t *)buf, &len, &src)) {
		Serial.print("[RECV] ");
		Serial.print(src);
		Serial.print("-> ");
		Serial.println(buf);

		buf[len] = '\0';
		publishRouteInfoInBuf(src);
	}
}

void loop() {
	// If MQTTT connection isn't done, keep trying and waiting
	if (!mqtt_client.connected()) {
		mqtt_connect();
	}
	mqtt_client.loop();
	delay(50); // Give the ESP time to handle network.

	while (millis() > publishInterval) {
		updateRouteInfo();
		generateRouteInfoStringInBuf();
		publishRouteInfoInBuf(1);

		publishInterval = millis() + 3000;
	}
	listenIncomingRouteInfo();
}
