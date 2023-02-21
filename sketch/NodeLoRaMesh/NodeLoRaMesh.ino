// Hardware headers
#include <SPI.h>
#include <string.h>

// Network headers
#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <LoPoDeMesh.h>

// Log header
#include <Preferences.h>


RH_RF95 rf95w(FREENOVE_CS, FREENOVE_ITQ);  // LoRa transceiver driver
RHMesh *manager;                           // Mesh network instance
char buf[MAX_MESSAGE_LEN];                 // String buffer for receiving

uint8_t routes[NODES];  // Routing table (Debug data | Gateway data)
int16_t rssi[NODES];    // RSSI table against all other nodes (Debug data | Gateway data)

#define PROPAGATE_INTERVAL 3000   // Interval for propagating visualize route data
unsigned long transmitTime = 0;  // previous transmit time

#define LOG_NAMESPACE "log"        // log namespace name
#define LOG_COUNT_KEY "log-count"  // log count key name
#define LOG_INTERVAL 1000          // Interval for logging
Preferences preferences;           // Log Instance
uint32_t logCount = 0;             // log count
unsigned long logTime = 0;         // previous log time


// Initalize routes[] and rssi[]
void initLoRaData() {
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
  Serial.print(F("Init node.. "));

  // Configure SPI pin in the board
  SPI.begin(FREENOVE_SCK, FREENOVE_MISO, FREENOVE_MOSI, FREENOVE_CS);

  // Configure LoRa driver
  rf95w.setTxPower(LORA_TX_POWER, LORA_TX_USERFO);
  rf95w.setFrequency(LORA_FREQUENCY);
  rf95w.setCADTimeout(LORA_TIMEOUT);

  // Configure Model for configuring LoRa parameter
  if (!rf95w.setModemConfig(LORA_MODEM_CONFIG)) {
    Serial.println(F("!! Modem config failed !!"));
  }
  Serial.println("RF95 Ready");

  // Initalize RadioHead Mesh object
  manager = new RHMesh(rf95w, NODE_ID);
  if (!manager->init()) {
    Serial.println(F("!! init failed !!"));
  } else {
    Serial.println("done");
  }

  manager->setTimeout(1500);
  initLoRaData();
}

// Initalize logging feature
void initLog() {
  // Open namespace with Preferences instance
  Serial.print(F("Logging."));
  while (!preferences.begin(LOG_NAMESPACE, false)) {
    Serial.print('.');
    delay(500);
  }
  Serial.println(F(" Ready"));

  // Get the saved number of log to use as key, otherewise default 0
  if (preferences.isKey(LOG_COUNT_KEY)) {
    logCount = preferences.getInt(LOG_COUNT_KEY);
  }
}

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(BUAD_RATE);
  while (!Serial);  // Wait for serial port to be available

  // Use flash memory for string instead of RAM
  Serial.print(F("[NODE "));
  Serial.print(NODE_ID);
  Serial.println(F("]"));

  // Initialize LoRa network
  initLoRaDriver();

  // * Execute to enable logging
  initLog();
}


// [DEBUG SERIAL] //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exchange error number to error string by flash string
const __FlashStringHelper *getErrorString(uint8_t error) {
  switch (error) {
    case 1:
      return F("INVALID LENGTH");
      break;
    case 2:
      return F("NO ROUTE");
      break;
    case 3:
      return F("TIMEOUT");
      break;
    case 4:
      return F("NO REPLY");
      break;
    case 5:
      return F("UNABLE TO DELIVER");
      break;
  }
  return F("UNKNOWN");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Update routing info(route, rssi) toward all nodes
void updateRouteInfo() {
  for (uint8_t dest = 1; dest <= NODES; dest++) {
    if (dest == NODE_ID) {     // if destination is self
      routes[dest - 1] = 255;  // mark itself

    } else {
      // Test network by RHMesh::doArp()
      manager->doArp(dest);
      RHRouter::RoutingTableEntry *route = manager->getRouteTo(dest);
      if (route == NULL) continue;

      routes[dest - 1] = route->next_hop;  // Update route
      rssi[dest - 1] = rf95w.lastRssi();   // Update rssi

      if (routes[dest - 1] == 0) {  // if there is no route
        rssi[dest - 1] = 0;         // reset rssi
      }
    }
  }
}

// Generate route info as formatted string in buffer
void generateRouteInfoStringInBuf() {
  // Mark start of buffer by null for sprintf(), strcat()
  buf[0] = '\0';

  // Loop all nodes
  for (uint8_t node = 1; node <= NODES; node++) {
    sprintf(buf + strlen(buf), "%d", routes[node - 1]);
    strcat(buf, ",");
    sprintf(buf + strlen(buf), "%d", rssi[node - 1]);

    // Node spliter
    if (node < NODES) {
      strcat(buf, "/");
    }
  }
}

// Transmit route info to ground node
void transmitRouteInfo() {
  // Loop in every PROPAGATE_INTERVAL
  if (millis() - transmitTime > PROPAGATE_INTERVAL) {
    updateRouteInfo();
    generateRouteInfoStringInBuf();

    // [DEBUG SERIAL] //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Serial.print(F("->"));
    Serial.print(GROUND_ID);
    Serial.print(F(" :"));
    Serial.print(buf);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Send route info
    uint8_t error = manager->sendtoWait((uint8_t *)buf, sizeof(buf), GROUND_ID);
    // [DEBUG SERIAL] //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (error != RH_ROUTER_ERROR_NONE) {  // if transmit error is occured, print detail error
      Serial.print(F(" !! "));
      Serial.println(getErrorString(error));
    } else { // if transmit is successful
      Serial.println(F(" OK"));
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Update transmitd time
    transmitTime = millis();
  }
}


// Log char pointer parameter in namespace
void log(char *ptr) {
  // Type casting 'logCount' from Int to Char pointer
  char logCountCharPtr[5];
  itoa(logCount, logCountCharPtr, 10);

  // Save log (key=log count, value=parameter)
  preferences.putString((const char *)logCountCharPtr, ptr);

  // Update logCount
  logCount++;
  // Preferences.h can not overwrite, so after removing, save again
  preferences.remove(LOG_COUNT_KEY);
  preferences.putInt(LOG_COUNT_KEY, logCount);
}

// Log only current node network status (route, rssi)
// void runNetworkLogging() {
//   // Loop in every LOG_INTERVAL
//   if (millis() - logTime > LOG_INTERVAL) {
//     // Update route info and generate string abour ground node
//     updateRouteInfo();
//     generateRouteInfoStringInBuf();

//     // Log buffer
//     log((char *)buf);
//     // [DEBUG SERIAL] //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     Serial.println(buf);
//     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

//     // Update logged time
//     logTime = millis();
//   }
// }

// Log current node network status (route, rssi) and Transmit route info
void runExperimentWithLog() {
  // Loop in every LOG_INTERVAL
  if (millis() - logTime > LOG_INTERVAL) {
    // Update route info and generate string about ground node
    updateRouteInfo();
    generateRouteInfoStringInBuf();

    // Write source at last of buffer and log buffer
    log((char *)buf);
    // [DEBUG SERIAL] //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Serial.println(buf);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Update logged time
    logTime = millis();
  }

  // Transmit route info to all other nodes
  transmitRouteInfo();

  // // Time TFML detection
  // unsigned long start = millis();
  // // detectUAVWithTFML();
  // unsigned long end = millis();

  // // Log time taken by TFML after type casting from Long to Char pointer 
  // char timeCharPtr[5];
  // sprintf(timeCharPtr, "%lu", end - start);
  // log((char *) timeCharPtr);
}

void loop() {
  // * Execute to enable network logging
  // runNetworkLogging();

  // * Execute to enable experiment with log
  runExperimentWithLog();
}
