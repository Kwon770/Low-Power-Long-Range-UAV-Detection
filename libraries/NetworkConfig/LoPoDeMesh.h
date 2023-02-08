#ifndef LoPoDeMesh_H
#define LoPoDeMesh_H

#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>

// ESP32 default baud rate
#define BUAD_RATE 115200

// Node Config
#define NODES 2
#define GROUND_ID 1
#define NODE_ID 2
#define MAX_MESSAGE_LEN 100 

// LoRa transceiver config
#define LORA_TX_POWER 23	// transmit power = 23
#define LORA_TX_USERFO false 	// useRfo = false
#define LORA_FREQUENCY 915.0 	// transmit frequency = 915.0 Mhz
#define LORA_TIMEOUT 500 	// transmit time out = 500ms

// LoRa modem config
// Set driver modem config
//  1d,     1e,      26
// { 0x72,   0x74,    0x04}, 	// Bw125Cr45Sf128 (the chip default), AGC enabled
// { 0x92,   0x74,    0x04}, 	// Bw500Cr45Sf128, AGC enabled
// { 0x48,   0x94,    0x04}, 	// Bw31_25Cr48Sf512, AGC enabled
// { 0x78,   0xc4,    0x0c}, 	// Bw125Cr48Sf4096, AGC enabled
// { 0x72,   0xb4,    0x04}, 	// Bw125Cr45Sf2048, AGC enabled
#define LORA_MODEM_1D 0x78
#define LORA_MODEM_2E 0xC4
#define LORA_MODEM_26 0x08
#define LORA_MODEM_CONFIG RH_RF95::Bw125Cr45Sf128

// HELTEC WiFi LoRa V2
#define HELTEC_SCK 5
#define HELTEC_MISO 19
#define HELTEC_MOSI 27
#define HELTEC_CS 18
#define HELTEC_RST 16
#define HELTEC_ITQ 26
#define HELTEC_DI1 35
#define HELTEC_DI2 34

// FREENOVE ESP32-WROVER CAM
// Adafruit RFM95W
#define FREENOVE_SCK 14
#define FREENOVE_MISO 12
#define FREENOVE_MOSI 13
#define FREENOVE_CS 15
#define FREENOVE_ITQ 33

#endif
