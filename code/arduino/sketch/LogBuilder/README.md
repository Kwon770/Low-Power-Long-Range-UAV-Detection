# 📝 Log manager for LoRa mesh network
All code for LoRa mesh network (```GroundLoRaMesh.ino```, ```NodeLoRaMesh.ino```) write the logs and save them in a flesh memory. You can load the saved logs. If you do not clear the logs for a long times while the system run, **heap of board will be flood and the board keep rebooting by a wrong memory reference error.**
## 🚀 Get Started

### Log reader
1. Open ```./LogReader/LogReader.ino``` by Arduino IDE
2. Connect ESP32-WROVER and configure IDE for the connected board
3. Upload the code
4. Check the loaded logs by serial monitor (Baud rate is ```115200```)

### Log Clear
1. Open ```./LogClear/LogClear.ino``` by Arduino IDE
2. Connect ESP32-WROVER and configure IDE for the connected board
3. Upload the code
4. Check the status of clear by serial monitor (Baud rate is ```115200```)