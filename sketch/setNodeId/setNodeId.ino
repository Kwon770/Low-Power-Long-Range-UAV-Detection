#include <RadioHead.h>
#include <EEPROM.h>

// set the number to set as NodeId
uint8_t nodeId = 1;

void setup() {
	// Default baud rate of ESP32 is 115200
	Serial.begin(115200);
	while (!Serial); // Wait for serail port to be available

	Serial.println("Setting node id");

	// Write nodeId on address 0 of EEPROM
	EEPROM.write(0, nodeId);
	Serial.print("set nodeId = ");
	Serial.println(nodeId);

	// Read data on address 0 of EEPROM
	uint8_t readVal = EEPROM.read(0);
	Serial.print("read nodeId: ");
	Serial.println(readVal);

	// Validate the data written on address 0
	if (nodeId != readVal) {
		Serial.println("!!! FAIL !!!")
	} else {
		Serial.println("SUCCESS");
	}

}
