// Blockchain ESP32 Account Public and Private Key Generation

#ifndef KEYS_GEN
#define KEYS_GEN

#include <string>
#include <EEPROM.h>
#include <Arduino.h>
#include <uECC.h>
#include <CryptoUtils.h>

#define EEPROM_DIM 64
#define EEPROM_PK_ADDR 0

void clearRom();
void generatePrivateKey(uint8_t* privateKey, size_t length);
void derivePublicKey(const uint8_t privateKey[32], uint8_t publicKey[16]);
String genWritePrivateKeyROM();

#endif //KEYS_GEN