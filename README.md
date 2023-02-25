# BTT - Be The Top
## Member

| <img src="https://s3.us-west-2.amazonaws.com/secure.notion-static.com/15c49188-ace8-43a3-b788-21ffcad927c7/soonchan.jpg?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Content-Sha256=UNSIGNED-PAYLOAD&X-Amz-Credential=AKIAT73L2G45EIPT3X45%2F20230127%2Fus-west-2%2Fs3%2Faws4_request&X-Amz-Date=20230127T163442Z&X-Amz-Expires=86400&X-Amz-Signature=7813391f91d5c4a8e0121c55b5fcd88107216494ed87e5bfb3360b1c739c792f&X-Amz-SignedHeaders=host&response-content-disposition=filename%3D%22soonchan.jpg%22&x-id=GetObject" width="100" height="100" /> | <img src="https://s3.us-west-2.amazonaws.com/secure.notion-static.com/67c90f2f-0fbc-4828-9e62-e16e364b0a62/gihwan.jpg?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Content-Sha256=UNSIGNED-PAYLOAD&X-Amz-Credential=AKIAT73L2G45EIPT3X45%2F20230127%2Fus-west-2%2Fs3%2Faws4_request&X-Amz-Date=20230127T163242Z&X-Amz-Expires=86400&X-Amz-Signature=20608c1d4b0f341a857bf4dad4a2a0ba88e5c7318239891d26be7039deb23282&X-Amz-SignedHeaders=host&response-content-disposition=filename%3D%22gihwan.jpg%22&x-id=GetObject" width="100" height="100" /> | <img src="https://s3.us-west-2.amazonaws.com/secure.notion-static.com/8713a22c-ae55-4081-b3bc-41c994c4ce19/Nahyeong.jpg?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Content-Sha256=UNSIGNED-PAYLOAD&X-Amz-Credential=AKIAT73L2G45EIPT3X45%2F20230127%2Fus-west-2%2Fs3%2Faws4_request&X-Amz-Date=20230127T163415Z&X-Amz-Expires=86400&X-Amz-Signature=350da9137433a78ba5db7265553361de696aa036eb2e013a8127e39bd950161b&X-Amz-SignedHeaders=host&response-content-disposition=filename%3D%22Nahyeong.jpg%22&x-id=GetObject" width="100" height="100" /> | <img src="https://s3.us-west-2.amazonaws.com/secure.notion-static.com/c7b12737-118e-4e21-a8ef-e2c78ca86daf/nawon.jpg?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Content-Sha256=UNSIGNED-PAYLOAD&X-Amz-Credential=AKIAT73L2G45EIPT3X45%2F20230127%2Fus-west-2%2Fs3%2Faws4_request&X-Amz-Date=20230127T163429Z&X-Amz-Expires=86400&X-Amz-Signature=03df828fe8c937ab76663a6f6a8d1ba9e0530adb0fc09f612bbf8da374ddb3d8&X-Amz-SignedHeaders=host&response-content-disposition=filename%3D%22nawon.jpg%22&x-id=GetObject" width="100" height="100" /> |
|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|                                                                                                                                                                                                                                                   [Soonchan Kwon](https://github.com/Kwon770)                                                                                                                                                                                                                                                    |                                                                                                                                                                                                                                                 [Gihwan Kim](https://github.com/gihwan-kim)                                                                                                                                                                                                                                                  |                                                                                                                                                                                                                                                  [Nahyeong Kim](https://github.com/gihwan-kim)                                                                                                                                                                                                                                                   |                                                                                                                                                                                                                                         [Nawon Kim](https://github.com/nawon219?tab=repositories)                                                                                                                                                                                                                                          |
|                                                                                                                                                                                                                                                                   Chonnam Univ                                                                                                                                                                                                                                                                   |                                                                                                                                                                                                                                                                Chungnam Univ                                                                                                                                                                                                                                                                 |                                                                                                                                                                                                                                                                  Chungnam Univ                                                                                                                                                                                                                                                                   |                                                                                                                                                                                                                                                                Chonnam Univ                                                                                                                                                                                                                                                                |


## Project title
UAV interface development for countering danger or terroristic drones


## Research problem statements


## Research novelty
- DL image detection only by ESP32-WROVER, **low power consumption** and **low performance**
- **Long distance**, but **low power consumption** communication by LoRa mesh network


## Overview & Diagram visual
<img src="https://s3.us-west-2.amazonaws.com/secure.notion-static.com/bbec1464-64d7-438f-8151-241cade466a9/new_flow.jpg?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Content-Sha256=UNSIGNED-PAYLOAD&X-Amz-Credential=AKIAT73L2G45EIPT3X45%2F20230127%2Fus-west-2%2Fs3%2Faws4_request&X-Amz-Date=20230127T163455Z&X-Amz-Expires=86400&X-Amz-Signature=3ed8e934c0c382c6ec5fe1bf0c933e7fe5bdc9717cca5cfebbfb0f264059df38&X-Amz-SignedHeaders=host&response-content-disposition=filename%3D%22new%2520flow.jpg%22&x-id=GetObject">


## Environment settings
### Version Specifications
```
Arduino IDE = 1.8.X
[Arduino board manager] esp32 = 2.0.6
[Arduino Library] RadioHead = 1.121
[Arduino Library] PubSubClient = 2.8

node >= latest
```


### Setting guide
1. Install Node
2. Install [Arduino IDE (legacy version)](https://www.arduino.cc/en/software)
3. Install Arduino-ESP32 on arduino board manager according to [document](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide). (Use `stable releash link`)
4. Configure Arduino IDE
   - ```Tools > Board > ESP32 Arduino > ESP32 Wrover module```
   - ```Tools > Upload speed > 115200```
   - [Tutorial video](https://www.youtube.com/watch?v=IuyBaxSqQ50)
5. Clone the [repository](https://github.com/Kwon770/LoPoDe)


### Excluded secret value files

- `libraries/WiFiInfo/WifiInfo.h`

```c
#ifndef WiFiInfo_H
#define WiFiInfo_H

#define WIFI_SSID "your-ap-ssid"
#define WIFI_PASSWORD "your-ap-password"

#endif
```

- `libraries/MQTTInfo/MQTTInfo.h`

```c
#ifndef MQTTInfo_H
#define MQTTInfo_H 

#define MQTT_GROUND_USERNAME "your-mqtt-username"
#define MQTT_GROUND_PASSWORD "your-mqtt-password"

#endif
```

- `visual_server/secret/mqtt-info.json`

```json
{
  "username": "lopode-mqtt-visual",
  "password": "lopode-mqtt-visual-password"
}
```


### Get Started

1. Add secret value files excluded in repo
2. Upload `sketch/NodeLoRaMesh/NodeLoRaMesh.h` to ESP32 boards as node
3. Upload `sketch/GroundLoRaMesh/GroundLoRaMesh.h` to ESP32 as GCS
4. Run `node /visual_server/app.js` in terminal