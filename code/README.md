







### Dependencies
```
Arduino IDE = 1.8.X
[Arduino board manager] esp32 = 2.0.6
[Arduino Library] RadioHead = 1.121

```


### Set up
1. Install [Arduino IDE 1.8.X (legacy version)](https://www.arduino.cc/en/software)
2. Install Arduino-ESP32 on arduino board manager according to [document](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide). **(Use `stable releash link`)**
3. Configure Arduino IDE
    - ```Tools > Board > ESP32 Arduino > ESP32 Wrover module```
    - ```Tools > Upload speed > 115200```



### Get Started
1. Add secret value files excluded in repo
2. Upload `sketch/NodeLoRaMesh/NodeLoRaMesh.h` to ESP32 boards as node
3. Upload `sketch/GroundLoRaMesh/GroundLoRaMesh.h` to ESP32 as GCS
4. Run `node /visual_server/app.js` in terminal