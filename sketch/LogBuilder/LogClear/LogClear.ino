#include <LoPoDeMesh.h>
#include <Preferences.h>

const char* MESH_NETWORK_LOG_IO_NAMESPACE = "log"; // log namespace name

Preferences preferences; // IO Instance


void setup() {
  Serial.begin(BUAD_RATE);

  // Open namespace with Preferences instance
  Serial.print("Initializing .");
  while (!preferences.begin(MESH_NETWORK_LOG_IO_NAMESPACE, false))  {
    Serial.print('.');
    delay(500);
  }
  Serial.println("Done");
}


void loop() {
  // Remove all log in namespace 
  if (preferences.clear()) { // Print the result of clear()
    Serial.print("Successfully clear log in ");
    Serial.println(MESH_NETWORK_LOG_IO_NAMESPACE);
  } else {
    Serial.print("!! Failed to clear log in ");
    Serial.println(MESH_NETWORK_LOG_IO_NAMESPACE);
    Serial.println("?? Log may be empty already");
  }

  // Close opened namespace
  preferences.end();

  // Try again
  delay(10000);
}