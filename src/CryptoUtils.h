// ESP32 Generic Utils for Crypto & Logging

#ifndef ESP_UTILS
#define ESP_UTILS

#include <string>
#include <Arduino.h>
#include <Web3.h>
#include <Crypto.h>
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

string getHexStringFromUint8(uint8_t uintArray[], int len);
void printHexFromUInt8(uint8_t uintArray[], int len);
void printArray(unsigned long *array, unsigned long int len);
void getOperationTime(string timeLabel, unsigned long *startTime, unsigned long *endTime);
string encryptData(unsigned long int i, unsigned long *cypherTime, unsigned char *publicKey, const unsigned int publicKeyLen, const unsigned char *toEncrypt, mbedtls_ctr_drbg_context *ctr_drbg);

#endif //ESP_UTILS