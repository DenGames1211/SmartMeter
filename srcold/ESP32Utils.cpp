#include <ESP32Utils.h>

string getHexStringFromUint8(uint8_t uintArr[], int len) {
    string hexString = "";
    char buffer[3];

    for (int i = 0; i < len; i++) {
        snprintf(buffer, sizeof(buffer), "%02x", uintArr[i]);
        hexString += buffer;
    }

    return hexString;
}

void printHexFromUInt8(uint8_t uintArray[], int len) {
    for (int i = 0; i < 16; i++) {
        Serial.printf("%02x", uintArray[i]);
    }
    Serial.println();
}

void getOperationTime(string timeLabel, unsigned long *startTime, unsigned long *endTime) {
    *endTime = micros();
    Serial.printf("Time needed to Encode Data String: %d\n", *endTime - *startTime);
    *startTime = micros();
}

string encryptData(unsigned char *publicKey, const unsigned int publicKeyLen, const unsigned char *toEncrypt, mbedtls_ctr_drbg_context *ctr_drbg) {
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    string dataToSend = "0x0";

    // Parse the public key
    int retCode = mbedtls_pk_parse_public_key(&pk, publicKey, publicKeyLen);

    if (retCode != 0) {
        Serial.printf("\nPublic Key Parsing Failed...");
        exit;
    }
    
    unsigned char encrypted[MBEDTLS_MPI_MAX_SIZE];
    size_t olenp = 0;
    
    printf("\nGenerating the encrypted value...\n");
    fflush(stdout);

    if((retCode = mbedtls_pk_encrypt(
                                    &pk, toEncrypt, 1,
                                    encrypted, &olenp, sizeof(encrypted),
                                    mbedtls_ctr_drbg_random, ctr_drbg
                                    )) != 0 ) {
        Serial.printf("Data Encryption Failed\nmbedtls_pk_encrypt returned -0x%04x\n", -retCode );
        exit;
    } else {
        dataToSend = "0x" + getHexStringFromUint8(encrypted, olenp);
        Serial.printf("\nEncrypted Data: %s\n", dataToSend.c_str());
    }

    return dataToSend;
}