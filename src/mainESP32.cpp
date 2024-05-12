#include <AccountKeysGeneration.h>
#include <NetworkConnection.h>
#include <ContractInteraction.h>
#include <CryptoUtils.h>

#include <Web3.h>
#include <Crypto.h>
#include <EEPROM.h>

// Stores the last time the code was executed
unsigned long previousMillis = 0;
// Time between SC interactions in milliseconds (0,1 seconds rn)
const long interval = 0;
// Maximum number of transactions to be executed
const long unsigned int txLimit = 5;
unsigned long currentMillis;
unsigned long totalStartTime;
unsigned long totalEndTime;
unsigned long int txCounter = 0;

// unsigned char companyPrK[] = {48, 130, 4, 164, 2, 1, 0, 2, 130, 1, 1, 0, 209, 176, 55, 229, 62, 242, 9, 15, 94, 199, 222, 91, 114, 110, 161, 193, 177, 126, 234, 184, 165, 25, 110, 136, 116, 107, 148, 116, 73, 7, 46, 53, 146, 239, 26, 62, 157, 30, 76, 10, 20, 159, 195, 124, 36, 168, 158, 151, 251, 227, 118, 158, 250, 29, 224, 39, 46, 100, 161, 135, 106, 184, 175, 24, 205, 247, 6, 166, 144, 111, 132, 106, 226, 111, 25, 6, 136, 7, 53, 43, 39, 35, 65, 8, 202, 61, 239, 110, 6, 110, 28, 55, 68, 244, 188, 232, 109, 28, 65, 222, 234, 116, 190, 13, 173, 126, 149, 253, 161, 242, 106, 167, 177, 223, 84, 179, 217, 131, 2, 6, 150, 10, 74, 100, 136, 43, 229, 193, 89, 107, 240, 174, 17, 181, 32, 221, 98, 114, 25, 128, 66, 55, 242, 131, 48, 188, 35, 77, 91, 131, 149, 41, 68, 99, 116, 127, 110, 104, 248, 164, 232, 17, 247, 106, 44, 234, 97, 90, 179, 200, 63, 3, 11, 182, 119, 154, 225, 153, 200, 45, 27, 33, 156, 154, 120, 101, 37, 124, 240, 141, 32, 170, 25, 196, 109, 83, 83, 139, 111, 184, 226, 4, 234, 83, 106, 184, 95, 250, 35, 189, 104, 252, 166, 62, 106, 140, 13, 183, 240, 230, 131, 62, 0, 85, 71, 230, 62, 35, 82, 10, 60, 44, 211, 74, 21, 148, 202, 221, 131, 108, 37, 223, 250, 167, 11, 202, 242, 104, 242, 169, 207, 32, 249, 178, 8, 215, 2, 3, 1, 0, 1, 2, 130, 1, 1, 0, 203, 124, 214, 17, 123, 79, 72, 105, 137, 18, 46, 255, 209, 129, 249, 41, 205, 197, 27, 165, 138, 90, 174, 192, 111, 50, 67, 67, 193, 230, 220, 250, 18, 152, 68, 128, 196, 218, 180, 242, 7, 103, 175, 62, 87, 24, 231, 167, 24, 67, 122, 70, 246, 244, 109, 37, 184, 200, 225, 84, 87, 197, 215, 238, 27, 90, 129, 50, 188, 241, 235, 176, 10, 3, 196, 102, 105, 53, 141, 108, 78, 51, 209, 21, 229, 165, 203, 39, 155, 199, 217, 75, 237, 120, 15, 167, 2, 211, 172, 60, 214, 139, 234, 47, 189, 209, 29, 95, 160, 124, 110, 242, 68, 20, 53, 207, 190, 226, 122, 225, 199, 83, 63, 52, 102, 77, 43, 77, 0, 65, 143, 192, 67, 122, 165, 88, 109, 186, 177, 205, 40, 27, 36, 91, 140, 250, 13, 18, 236, 112, 230, 47, 134, 191, 217, 82, 108, 253, 218, 198, 6, 88, 224, 80, 253, 69, 149, 94, 181, 202, 19, 118, 255, 175, 213, 98, 81, 155, 200, 69, 110, 171, 160, 190, 118, 9, 203, 151, 144, 248, 52, 72, 167, 72, 88, 237, 115, 40, 119, 129, 159, 95, 188, 116, 3, 119, 138, 180, 83, 210, 66, 114, 1, 73, 250, 89, 161, 241, 191, 189, 235, 187, 51, 73, 91, 120, 127, 32, 61, 15, 110, 211, 173, 92, 120, 240, 255, 115, 168, 128, 76, 54, 106, 176, 200, 80, 61, 24, 35, 17, 170, 187, 237, 45, 73, 17, 2, 129, 129, 0, 239, 18, 240, 107, 178, 11, 8, 2, 1, 101, 111, 92, 204, 50, 226, 39, 0, 36, 70, 192, 177, 50, 5, 168, 122, 153, 19, 37, 110, 155, 207, 180, 162, 243, 65, 121, 21, 253, 254, 227, 133, 80, 211, 188, 47, 34, 140, 200, 172, 15, 190, 225, 127, 213, 124, 237, 168, 236, 60, 169, 131, 252, 34, 39, 155, 138, 186, 31, 45, 102, 59, 121, 34, 117, 157, 88, 175, 115, 151, 0, 250, 3, 142, 195, 25, 173, 180, 160, 241, 0, 57, 19, 145, 115, 125, 90, 140, 109, 223, 30, 155, 58, 49, 151, 134, 148, 149, 220, 192, 137, 46, 23, 164, 140, 16, 231, 82, 17, 229, 199, 28, 32, 17, 7, 44, 59, 187, 95, 2, 129, 129, 0, 224, 136, 175, 10, 0, 234, 70, 162, 174, 254, 75, 228, 129, 198, 184, 44, 60, 107, 192, 193, 99, 188, 176, 116, 178, 93, 196, 120, 180, 212, 158, 157, 201, 233, 146, 51, 206, 34, 12, 225, 80, 88, 253, 203, 102, 66, 218, 98, 178, 207, 204, 66, 147, 3, 97, 222, 147, 58, 213, 129, 228, 23, 154, 197, 253, 93, 178, 91, 246, 77, 72, 217, 136, 69, 156, 151, 91, 113, 42, 156, 12, 170, 97, 231, 231, 15, 115, 62, 0, 154, 157, 242, 122, 238, 9, 197, 13, 141, 150, 248, 84, 239, 205, 0, 59, 159, 14, 156, 194, 133, 88, 55, 99, 77, 0, 142, 22, 33, 55, 101, 5, 52, 6, 187, 199, 31, 29, 137, 2, 129, 128, 20, 95, 167, 207, 117, 183, 204, 241, 230, 206, 143, 35, 248, 94, 233, 184, 235, 33, 44, 74, 101, 143, 94, 190, 17, 220, 147, 174, 31, 243, 245, 74, 110, 140, 142, 106, 212, 61, 246, 7, 203, 167, 172, 179, 159, 204, 176, 244, 180, 253, 14, 134, 71, 15, 153, 122, 227, 103, 102, 112, 7, 200, 103, 220, 186, 59, 163, 74, 222, 226, 148, 116, 233, 67, 222, 169, 221, 33, 219, 159, 128, 174, 230, 238, 167, 116, 143, 45, 27, 252, 169, 224, 138, 7, 174, 138, 21, 231, 147, 25, 23, 159, 104, 244, 129, 202, 141, 206, 231, 53, 137, 145, 8, 59, 203, 128, 251, 20, 125, 97, 111, 123, 40, 24, 85, 99, 173, 191, 2, 129, 128, 32, 155, 24, 184, 30, 43, 231, 56, 6, 171, 124, 255, 224, 236, 167, 160, 28, 226, 20, 234, 192, 126, 13, 45, 213, 29, 18, 195, 232, 150, 155, 182, 20, 53, 43, 251, 3, 235, 35, 237, 176, 74, 76, 8, 182, 127, 236, 208, 186, 96, 188, 171, 202, 71, 138, 117, 212, 241, 141, 65, 192, 138, 192, 250, 17, 114, 2, 205, 161, 245, 213, 39, 105, 199, 154, 22, 204, 187, 161, 81, 105, 241, 112, 40, 74, 0, 143, 94, 168, 157, 203, 100, 3, 255, 255, 65, 51, 87, 165, 93, 200, 143, 206, 56, 67, 112, 52, 162, 5, 9, 114, 39, 16, 239, 132, 174, 29, 126, 147, 89, 195, 40, 171, 245, 166, 211, 216, 233, 2, 129, 129, 0, 231, 252, 131, 194, 192, 162, 222, 213, 156, 144, 202, 230, 173, 71, 181, 240, 157, 190, 122, 168, 21, 6, 166, 151, 209, 186, 177, 22, 8, 152, 38, 190, 11, 87, 185, 172, 220, 24, 167, 209, 44, 187, 179, 149, 242, 145, 214, 203, 79, 189, 143, 129, 177, 50, 127, 14, 158, 54, 0, 92, 95, 180, 223, 196, 226, 1, 117, 5, 119, 9, 103, 44, 68, 210, 30, 168, 234, 156, 116, 22, 67, 59, 37, 70, 7, 223, 231, 103, 12, 190, 161, 76, 247, 234, 206, 216, 112, 57, 12, 249, 153, 51, 35, 174, 143, 172, 80, 106, 150, 178, 21, 167, 199, 248, 219, 67, 122, 155, 65, 129, 62, 128, 114, 230, 144, 214, 123, 128};
unsigned char companyPK[] = "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCJVtS827q/ASOr6WEdct8olHVMJxJhoVc4NRZ0sO7f/peGfuMGBm66vRU2oMUtEQOBpvAVw7GCmPJtLMuoSd1s8QJmBrYpYuGKkGUxR0c6KYfLTAW+Clg2YxiWCfIeREgUj57FYzf33TbuKdNC8MWQWdfe8eSH0r2wcClH0g3w1wIDAQAB\n-----END PUBLIC KEY-----";

mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;

// Address of SC to connect to
const char *CONTRACT_ADDRESS = "0x60F27E76A5E0315Bde426B36450355C485E87209";
// Address of Personal Wallet
const char *MY_ADDRESS = "0x627306090abaB3A6e1400e9345bC60c78a8BEf57";

Web3 *web3;
Crypto *crypto;    
Contract energyContract = Contract(web3, CONTRACT_ADDRESS);


void setup() {
    randomSeed(123);
    EEPROM.begin(EEPROM_DIM);
    Serial.begin(9600);

    // clearRom();
    const char *besuPrivateKey = EEPROM.readString(EEPROM_PK_ADDR).c_str();
    if (strlen(besuPrivateKey) == 0) {
        besuPrivateKey = genWritePrivateKeyROM().c_str();
    }
     Serial.printf("\nRead BC Private Key: %s\n", besuPrivateKey);

    // Connect to Network
    setupWifi();
    Serial.println("\n--- Connection Phase Completed ---\n");

    totalStartTime = micros();

    // Define Socket of BC Service
    // The ip address should be that of the server running SSL
    const char * ip_add = "192.168.126.243";
    unsigned short port = 443;
    long long networkId = 1337;

    // Setup WEB3 Connection to BC
    web3 = new Web3(networkId, ip_add, port);

    // Connect to Running SC
    crypto = new Crypto(web3);
    crypto->SetPrivateKey(besuPrivateKey);
    
    // Set Private Key
    energyContract.SetPrivateKey(besuPrivateKey);

    // Init Entropy Context and CRT (Randomization) Function
    mbedtls_entropy_init(&entropy);
    const char *personalization = "dj2K20lxMsl29dj38hc";

    mbedtls_ctr_drbg_init(&ctr_drbg);

    int retCode = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                     (const unsigned char*) personalization,
                     strlen(personalization) );
    if(retCode != 0)
        Serial.println("\nInitialization of Crt Failed...\n");

    esp_wifi_set_max_tx_power(78);
}

void loop() {
    currentMillis = millis();

    if(txCounter == txLimit) {
        totalEndTime = micros();
        Serial.println("\n---------------------- FINISHED ----------------------\n");
        Serial.printf("\nTotal Time Elapsed: %d", totalEndTime - totalStartTime);
        Serial.println();
        txCounter++;
    } else if (txCounter < txLimit){
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            uint8_t randomNumber = random(2, 6) + '0';
            encryptData(companyPK, sizeof(companyPK), &randomNumber, &ctr_drbg);

            sendInstantEnergy(web3, crypto, MY_ADDRESS, CONTRACT_ADDRESS, energyContract, (unsigned char *)encryptData(companyPK, sizeof(companyPK), &randomNumber, &ctr_drbg).c_str());
            txCounter++;
        }
    }
}