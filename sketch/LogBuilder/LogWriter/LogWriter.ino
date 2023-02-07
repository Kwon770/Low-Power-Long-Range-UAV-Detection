#include <Preferences.h>

#define MESH_NETWORK_LOG_INTERVAL 1000;
#define MESH_NETWORK_LOG_IO_NAMESPACE "mesh-network-log"
#define MESH_NETWORK_LOG_COUNT_IO_KEY "mesh-network-log-count"

bool isMeshNetworkLogOn = true; // mesh network log enables
uint8_t meshNetworkLogCount = 0; // mesh network log count
unsigned long meshNetworkLogTime = 0; // previous log time

void setup() {
  // Intialize IO if log state is true
  while (isMeshNetworkLogOn && !begin(MESH_NETWORK_LOG_IO_NAMESPACE));
  // Get log count number to use as key
  meshNetworkLogCount = getChar(MESH_NETWORK_LOG_COUNT_IO_KEY);
  meshNetworkLogCount += 1;
}

void logMeshNetwork() {
  // If time has not been passed by as much as interval, skip
  if (millis() - meshNetworkLogTime < MESH_NETWORK_LOG_INTERVAL) {
    return;
  }


  char _meshNetworkLogCount = meshNetworkLogCount + '0';
  // test log
  long l = millis();
  char str[256];
  sprintf(str, "%ld", l);
  //
  putString(_meshNetworkLogCount, str);


  meshNetworkLogCount++;
  putChar(MESH_NETWORK_LOG_COUNT_IO_KEY, meshNetworkLogCount);

  // Update meshNetworkLogTime
  meshNetworkLogTime = millis();
}

void loop() {
  logMeshNetwork();
}
