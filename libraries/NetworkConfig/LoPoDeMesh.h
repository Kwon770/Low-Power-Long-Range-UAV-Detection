#ifndef LoPoDeMesh_H
#define LoPoDeMesh_H

#include <RHRouter.h>
#include <RHMesh.h>
#include <RH_RF95.h>

// ESP32 default baud rate
#define BUAD_RATE 115200

// Node Config
#define NODES 3
#define GROUND_ID 1
#define NODE_ID 2
#define MAX_MESSAGE_LEN 60

// LoRa transceiver config
#define LORA_TX_POWER 20	// transmit power = 20
#define LORA_TX_USERFO false 	// useRfo = false
#define LORA_FREQUENCY 915.0 	// transmit frequency = 915.0 Mhz
#define LORA_TIMEOUT 500 	// transmit time out = 500ms

// LoRa transceiver modem config
// Bw125Cr45Sf128 	: Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range.
// Bw500Cr45Sf128	: Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range.
// Bw31_25Cr48Sf512	: Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
// Bw125Cr48Sf4096	: Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, low data rate, CRC on. Slow+long range.
//  1d,     1e,      26
// { 0x72,   0x74,    0x04}, 	// Bw125Cr45Sf128 (the chip default), AGC enabled
// { 0x92,   0x74,    0x04}, 	// Bw500Cr45Sf128, AGC enabled
// { 0x48,   0x94,    0x04}, 	// Bw31_25Cr48Sf512, AGC enabled
// { 0x78,   0xc4,    0x0c}, 	// Bw125Cr48Sf4096, AGC enabled
// { 0x72,   0xb4,    0x04}, 	// Bw125Cr45Sf2048, AGC enabled
//#define LORA_MODEM_1D 0x72
//#define LORA_MODEM_2E 0x74
//#define LORA_MODEM_26 0x04
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
