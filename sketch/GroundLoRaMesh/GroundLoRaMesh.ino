#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <SPI.h>
#include <Wire.h>
#include <PubSubClient.h>

// Default ESP buad rate
#define BUAD_RATE 115200

// WiFi AP info
const char* SSID = "SSID";
const char* PASSWORD = "your-wifi-";
// MQTT info
const char* MQTT_SERVER = "lopode-mqtt-server";
int mqtt_port = 8883;
const char* MQTT_USERNAME = "lopode-mqtt-username";
const char* MQTT_PASSWORD = "lopode-mqtt-";
const char* DATATOPIC = "mesh_gateway/data";

WiFiClient espClient; // Arduino-ESP32 WiFiSTA object
PubSubClient mqtt_client(espClient); // MQTT client object

// Connect to MQTT broker
void mqtt_connect() {
	// Loop until we're reconnected
	while (!mqtt_client.connected()) {
		Serial.print("Connecting to MQTT...  ");

		// Generate randome mqtt client id
		String mqtt_clientId = "mesh_gateway-";
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

void setup()   {
	Serial.begin(BUAD_RATE);
	while (!Serial); // Wait till serial is ready

	Serial.print("Connecting to AP - ");
	Serial.print(SSID);

	// Initialize WiFi connection
	WiFi.begin(SSID, PASSWORD);
	// If connection isn't done yet, wait for 250ms till done
	while (WiFi.status() != WL_CONNECTED) {
		delay(250);
		Serial.print(". ");
	}
	Serial.println("[Connected]");

	// Initialize MQTT server connection
	mqtt_client.setServer(MQTT_SERVER, mqtt_port);
	mqtt_connect();
}


void loop() {
	// If MQTTT connection isn't done, keep trying and waiting
	if (!mqtt_client.connected()) {
		mqtt_connect();
	}
	mqtt_client.loop();

	delay(50); // Give the ESP time to handle network.

	// While serial is available, keep checking serial read data
	if (Serial.available()) {
		Serial.setTimeout(100);
		String s = Serial.readStringUntil('\n');
		Serial.println(s.c_str());

		// If route data is sent from other nodes, publish it by MQTT
		if (s.startsWith("node: ")) {
			// Substring sent data to publish only needed data
			// (exclude identifier, noise, ...)
			String data;
			int end = s.indexOf('\r');
			if (end > 0) {
				data = s.substring(6, end);
			} else {
				data = s.substring(6);
			}
			// Publish data on MQTT server by topic
			mqtt_client.publish(DATATOPIC, data.c_str());
		}
	}
}
