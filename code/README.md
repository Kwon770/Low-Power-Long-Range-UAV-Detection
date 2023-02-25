# Project code

## 🔗 Dependencies
```
Arduino IDE = 1.8.X
arduino-esp32 = 2.0.6
```


## 💻 Set up
1. Install [Arduino IDE 1.8.X (legacy version)](https://www.arduino.cc/en/software)
2. Install Arduino-ESP32 on arduino board manager according to [document](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide). **(Use `stable releash link`)**
3. Configure Arduino IDE
    - ```Tools > Board > ESP32 Arduino > ESP32 Wrover module```
    - ```Tools > Upload speed > 115200```
    - ```Arduino > Settings > Sketchbook Location > [PATH OF 'CODE' FOLDER]```
4. Model ~~~~~

## 🚀 Get Started
- [Lora mesh network communication of base station](./arduino/sketch/GroundLoRaMesh)
- [Lora mesh network communication of UAV](./arduino/sketch/NodeLoRaMesh)
- [Network Log manager](./arduino/sketch/LogBuilder)
- [Data preprocessing](./dataset_pre-processing)
- [Drone detection model training](./model_training)
- [Drone detection](./arduino/sketch/drone_detection)