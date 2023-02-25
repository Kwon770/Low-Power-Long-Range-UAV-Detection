# 📡 LoRa mesh network of base station
This code is to communicate with UAVs by LoRa mesh network for base station on the ground.
## 🔗 Dependencies
```
RadioHead = 1.121
```

## 🚀 Get Started
1. Open a header in ```... > code > arduino > libraries > NetworkConfig > LoPoDeMesh.h```  to configure a mesh network.
   1. Set the number of nodes, including base station, in ```#define NODES [THE_NUMBER_OF_NODES]```.
2. Open ```GroubdLoRaMesh.ino``` by Arduino IDE.
3. Connect ESP32-WROVER and configure IDE for the connected board.
4. Upload the code.
5. Check the status of network by serial monitor (Baud rate is ```115200```).