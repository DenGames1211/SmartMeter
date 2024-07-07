#include <AccountKeysGeneration.h>

void clearRom() {
    for (int i = 0 ; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
    }
}

void generatePrivateKey(uint8_t* privateKey, size_t length) {
    // Generate random bytes for private key
    for (size_t i = 0; i < length; i += 4) {
        // Use ESP32's hardware random number generator to generate 4 bytes of random data
        uint32_t randomValue = esp_random();
        memcpy(privateKey + i, &randomValue, (4 < length - i) ? 4 : length - i);
    }
}

void derivePublicKey(const uint8_t privateKey[32], uint8_t publicKey[16]) {
    // Use the private key to compute the public key
    uECC_compute_public_key(privateKey, publicKey, uECC_secp256r1()); // sub with 192 r1 if not working
}

String genWritePrivateKeyROM() {
    EEPROM.begin(EEPROM_DIM);
    uint8_t privateKey[32];
    uint8_t publicKey[16];

    // Generate private key
    generatePrivateKey(privateKey, 32);

    // Derive public key
    derivePublicKey(privateKey, publicKey);

    // Print private key (in hexadecimal format)
    // printHexFromUInt8(privateKey, 32);

    // Print public key (in uncompressed format)
    Serial.print("\nESP32 Blockchain Connection Public Key: ");
    printHexFromUInt8(publicKey, 16);

    String privateKeyString = "";
    const int arraySize = sizeof(privateKey) / sizeof(privateKey[0]);
    char buffer[3];
    for (int i = 0; i < 32; i++) {
        snprintf(buffer, sizeof(buffer), "%02x", privateKey[i]);
        privateKeyString += buffer;
    }
    
    EEPROM.writeString(EEPROM_PK_ADDR, privateKeyString);
    EEPROM.commit();

    Serial.printf("From GEN: %s\n", privateKeyString.c_str());

    return privateKeyString;
}