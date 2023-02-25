# ✈️ LoRa mesh network of UAV
This code is to communicate with UAVs and base station by LoRa mesh network for UAV.
## 🔗 Dependencies
```
RadioHead = 1.121
```

## 🚀 Get Started
1. Open a header in ```~/code/arduino/libraries/NetworkConfig/LoPoDeMesh.h```  to configure a mesh network.
   1. Set the number of nodes, including base station, in ```#define NODES [THE_NUMBER_OF_NODES]```.
   2. Set a node id of UAV in ```#define NODE_ID [NODE_ID]```. **(⚠️ Node id must be unique ⚠️)**
2. Open ```NodeLoRaMesh.ino``` by Arduino IDE.
3. Connect ESP32-WROVER and configure IDE for the connected board.
4. Upload the code.
5. Check the status of network by serial monitor (Baud rate is ```115200```).