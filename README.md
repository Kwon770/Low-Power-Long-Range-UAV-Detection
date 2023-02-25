# BTT - Be The Top
## Member

<div style="text-align: center;">

| <img src="https://github.com/Kwon770/Low-Power-Long-Range-UAV-Detection/blob/main/image/soonchan.JPG?raw=true" width="90" height="90" /> | <img src="https://github.com/Kwon770/Low-Power-Long-Range-UAV-Detection/blob/main/image/gihwan.jpeg?raw=true" width="90" height="90" /> | <img src="https://github.com/Kwon770/Low-Power-Long-Range-UAV-Detection/blob/main/image/nahyeong.jpeg?raw=true" width="90" height="90" /> | <img src="https://github.com/Kwon770/Low-Power-Long-Range-UAV-Detection/blob/main/image/nawon.jpeg?raw=true" width="90" height="90" /> | <img src="https://github.com/Kwon770/Low-Power-Long-Range-UAV-Detection/blob/main/image/karty.jpeg?raw=true" width="90" height="90" /> | <img src="https://github.com/Kwon770/Low-Power-Long-Range-UAV-Detection/blob/main/image/prakshi.png?raw=true" width="90" height="90" /> |
|:--------------------------------------------------------------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------:|:---------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------:|
|                                              [Soonchan Kwon](https://github.com/Kwon770)                                               |                                              [Gihwan Kim](https://github.com/gihwan-kim)                                              |                                              [Nahyeong Kim](https://github.com/gihwan-kim)                                              |                                      [Nawon Kim](https://github.com/nawon219?tab=repositories)                                       |                                                           Karteikay Dhuper                                                           |                                                            Prakshi Chander                                                            |
|                                                              Chonnam Univ                                                              |                                                             Chungnam Univ                                                             |                                                              Chungnam Univ                                                              |                                                             Chonnam Univ                                                             |                                                             Purdue Univ                                                              |                                                              Purdue Univ                                                              |

</div>

## Project title
Airspace Counter Drone System (ACDS) using observer drones to detect unidentified aerial object in LoRa mesh network.


## Research problem statements
With the increasing popularity of UAVs, they have become more easily accessible to
the public, companies, and even terrorists. This has raised the need for having
a Counter Drone System (CDS). There have already been several attempts of implementing a CDS,
but Ground Counter Drone System (GCDS) has a geological restriction and
Airspace Counter Drone System (ACDS) in NASA research uses a WiFi mesh network which is
fast but consumes high-power.
<br/>
<br/>
**Therefore we wanted to make the system which is free from these restrictions.**
The system is ACDS uses observer drones to detect unidentified aerial object in LoRa mesh network.

## Research novelty
1. **Observer drones to detect unidentified aerial object**<br/>
   The system gives a immunity to a geological restriction because we have observer drones that are used for detection.
   <br/>
   <br/>
2. **Low-power and low-cost Machine learning detection**<br/>
   Detection is run on the Micro-controller. Additionally, video does not have to be sent
   to the ground for detection, so we can use low-data-rate and long-range communication
   technology.
   <br/>
   <br/>
3. **LoRa mesh network via multi-hop communication**<br/>
   LoRa allows low-power and long-range communication while multi-hop communication allows
   network coverage to be expanded just by adding a new drone.


## Overview of the system
The system is classified into three:
- Ground LoRa mesh network - Base station where communicate with UAV by LoRa mesh network
- Node LoRa mesh network - UAV which communicate with ground and UAV by LoRa mesh network 
- Detection - Small and simple UAV detection model
<br/>
<br/>
<img src="https://github.com/Kwon770/Low-Power-Long-Range-UAV-Detection/blob/main/image/system-overview.jpg?raw=true">

## Environment setting

The system consist of LoRa mesh network and detection is developed on Arduino and run on ESP32-WROVER.

### The Structure of hardware
