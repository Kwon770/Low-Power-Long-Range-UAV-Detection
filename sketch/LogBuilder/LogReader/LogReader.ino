#include <LoPoDeMesh.h>
#include <Preferences.h>

const char* LOG_NAMESPACE = "log"; // log namespace name
const char* LOG_COUNT_KEY = "log-count"; // log count key name

Preferences preferences; // IO Instance
uint32_t logCount = 0; // mesh network log count

void setup() {
  Serial.begin(BUAD_RATE);

  // Open namespace with Preferences instance
  Serial.print("Initializing .");
  while (!preferences.begin(LOG_NAMESPACE, false)){
    Serial.print('.');
    delay(500);
  }
  Serial.println("Done");

  // Get the number of log
  logCount = preferences.getInt(LOG_COUNT_KEY);
  Serial.print("log count => ");
  Serial.println(logCount);
}

void loop() {
  if (logCount == 0) return; // If there is no log, skip

  // print all log by iterating
  Serial.println("[LOGS]");
  for (int logIndex = 0; logIndex < logCount; logIndex++) {
    // Print log index
    Serial.print(logIndex);
    Serial.print(":");
    
    // Type casting "log index" from Int to Char pointer
    char logCountCharPtr[5];
    itoa(logIndex, logCountCharPtr, 10);

    // Print log context
    String log = preferences.getString((const char *)logCountCharPtr, "");
    Serial.println(log);
  }
  Serial.println('---------------');
  Serial.println();

  // Try again
  delay(10000);
}