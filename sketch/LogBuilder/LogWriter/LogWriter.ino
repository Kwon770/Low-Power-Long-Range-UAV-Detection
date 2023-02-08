#include <Preferences.h>
#include <LoPoDeMesh.h>

#define LOG_INTERVAL 1000
#define LOG_NAMESPACE "log"       // log namespace name
#define LOG_COUNT_KEY "log-count" // log count key name

Preferences preferences;                 // Log Instance
uint32_t logCount = 0;                   // mesh network log count
unsigned long logTime = 0;               // previous log time


void setup() {
  Serial.begin(BUAD_RATE);

  // Open namespace with Preferences instance
  Serial.print("Initializing .");
  while (!preferences.begin(LOG_NAMESPACE, false)){
    Serial.print('.');
    delay(500);
  }
  Serial.println("Done");
  
  // Get the number of log to use as key
  logCount = preferences.getInt(LOG_COUNT_KEY);
  Serial.print("Prev log count => ");
  Serial.println(logCount);
}


void logMeshNetwork() {
  // If time has not been passed by as much as interval, skip
  if (millis() - logTime < LOG_INTERVAL) {
    return;
  }

  // Type casting 'logCount' from Int to Char pointer
  char logCountCharPtr[5];
  itoa(logCount, logCountCharPtr, 10);
  
  // [TEST LOG] //////////////////
  long l = millis();
  char str[256];
  sprintf(str, "%ld", l);
  //////////////////////////////

  /*
  Save log
   @param key     logCountCharPtr = log count
   @param value   str = test log)
  */
  preferences.putString((const char *)logCountCharPtr, str);
  
  //// [TEST DEBUG] ////////////////
  Serial.print(logCountCharPtr);
  Serial.print(":");
  Serial.println(str);
  ////////////////////////////////

  // Update logCount
  logCount++;
  // Preferences.h can not overwrite, so after removing, save again
  preferences.remove(LOG_COUNT_KEY);
  preferences.putInt(LOG_COUNT_KEY, logCount);

  // Update logTime
  logTime = millis();
}

void loop() {
  logMeshNetwork();
}
