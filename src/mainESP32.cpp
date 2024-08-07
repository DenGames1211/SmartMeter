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
const long unsigned int txLimit = 20;
unsigned long currentMillis;
unsigned long totalStartTime;
unsigned long totalEndTime;
unsigned long int txCounter = 0;
unsigned long paramTime[txLimit];
unsigned long cypherTime[txLimit];
unsigned long hashTime[txLimit];
unsigned long signTime[txLimit];
unsigned long sendTime[txLimit];


unsigned char companyPK[] = "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCJVtS827q/ASOr6WEdct8olHVMJxJhoVc4NRZ0sO7f/peGfuMGBm66vRU2oMUtEQOBpvAVw7GCmPJtLMuoSd1s8QJmBrYpYuGKkGUxR0c6KYfLTAW+Clg2YxiWCfIeREgUj57FYzf33TbuKdNC8MWQWdfe8eSH0r2wcClH0g3w1wIDAQAB\n-----END PUBLIC KEY-----";

mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;

// Address of SC to connect to
const char *CONTRACT_ADDRESS = "0xcD9B4112C0E0e9bF8592137Ca2Ff788982B3Bc80";
// Address of Personal Wallet
const char *MY_ADDRESS = "0x627306090abaB3A6e1400e9345bC60c78a8BEf57";

Web3 *web3;
Crypto *crypto;    
Contract *energyContract;


void setup() {
    randomSeed(123);
    EEPROM.begin(EEPROM_DIM);
    Serial.begin(9600);

    // clearRom();
    int valid = true;
    String besuKey;
    besuKey = EEPROM.readString(EEPROM_PK_ADDR);
    // Check if there is already a private key in EEPROM having String HEX Format (it needs to be completely printable)
    for (int i = 0; i < ETHERS_PRIVATEKEY_LENGTH * 2; i++) if (isprint(besuKey.c_str()[i]) == 0) valid = false;
    // Serial.printf("\nRead Before Write BC Private Key %d: %s\n", strlen(besuKey.c_str()), besuKey.c_str());
    if (!valid) {
        Serial.println("Writing key on EEPROM...");
        besuKey = genWritePrivateKeyROM();
    }
    // Serial.printf("\nRead BC Private Key: %s\n", besuKey.c_str());

    // Connect to Network
    setupWifi();
    Serial.println("\n--- Connection Phase Completed ---\n");

    totalStartTime = micros();

    // Define Socket of BC Service
    // The ip address should be that of the server running SSL
    const char * ip_add = "192.168.50.243";
    unsigned short port = 443;
    long long networkId = 1337;

    // Setup WEB3 Connection to BC
    web3 = new Web3(networkId, ip_add, port);

    // Connect to Running SC
    crypto = new Crypto(web3);
    crypto->SetPrivateKey("c87509a1c067bbde78beb793e6fa76530b6382a4c0241e5e4a9ec0a0f44dc0d3");
    
    // Set Private Key
    energyContract = new Contract(web3, CONTRACT_ADDRESS);
    energyContract->SetPrivateKey("c87509a1c067bbde78beb793e6fa76530b6382a4c0241e5e4a9ec0a0f44dc0d3");

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
        Serial.printf("ParamTime:\n");
        printArray(paramTime, txLimit);
        Serial.printf("CypherTime:\n");
        printArray(cypherTime, txLimit);
        Serial.printf("HashTime:\n");
        printArray(hashTime, txLimit);
        Serial.printf("SignTime:\n");
        printArray(signTime, txLimit);
        Serial.printf("SendTime:\n");
        printArray(sendTime, txLimit);
        txCounter++;
    } else if (txCounter < txLimit){
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            uint8_t randomNumber = random(2, 6) + '0';

            sendInstantEnergy(txCounter, paramTime, hashTime, signTime, sendTime, web3, crypto, MY_ADDRESS, CONTRACT_ADDRESS, *energyContract, (unsigned char *)encryptData(txCounter, cypherTime, companyPK, sizeof(companyPK), &randomNumber, &ctr_drbg).c_str());
            txCounter++;
        }
    }
}