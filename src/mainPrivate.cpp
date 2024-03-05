#include "esp_wifi.h"
#include <Arduino.h>
#include <Web3.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <functional>
#include <string>
#include <Trezor/rand.h>
#include <Crypto.h>
#include <vector>
#include "pkey.h"

// To take into consideration if ECDSA of mbedtls is too slow:
// https://github.com/kmackay/micro-ecc/blob/master/examples/ecc_test/ecc_test.ino

// Custom Type to store Wifi Networks Credentials
typedef struct {
  const char *ssid;
  const char *password;
} WiFiCredentials;

// Vector of Credential Pairs (SSID, Password)
std::vector<WiFiCredentials> wiFiCredentials{
    {"Wifi1211", ""},
    {"FASTWEB-Q4EGAH", "T9UEAUHC4U"},
    {"WebCube4-89F4", "XMZSGT7X"},
};

// Blockchain Pubbliche: meglio sidechain come Polygon (permette di risparmiare sul costo delle transazioni)

// Address of SC to connect to
const char *CONTRACT_ADDRESS = "0xc7F472ae1c408c30d5E13286C3Db6dDc3f00Ce84";
// Address of Personal Wallet
const char *MY_ADDRESS = "0x627306090abaB3A6e1400e9345bC60c78a8BEf57";

// Web3 Object to manage the connection
Web3 *web3;
Crypto *crypto;

// Stores the last time the code was executed
unsigned long previousMillis = 0;
// Time between SC interactions in milliseconds (10 seconds rn)
const long interval = 10000;

uint8_t secureESPID[8] = {'e', 's', 'i', '3', 'p', 'm', 't', '@'};
uint8_t hashedESPID[32];
uint8_t signatureESPID[65];

string uint8ArrayToHexString(uint8_t arr[], size_t len) {
  string hexStr = "";
  for(size_t i = 0; i < len; i++) {
    char buf[3];
    sprintf(buf, "%02X", arr[i]);
    hexStr += buf;
  }
  return hexStr;
}

/**
 * Initialize connection to a specific network
 * 
 * @param ssid     The SSID of the network to connect to.
 * @param password The password of the network to connect to.
 * @return         True if successfully connected, false otherwise.
 */
bool wifiConnect(const char *ssid, const char *password)
{
    // If already connected, no need to execute connection operations
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }

    Serial.println();
    Serial.print("\nConnecting to ");
    Serial.println(ssid);

    // If not connected, set WiFi Module Power to max, and start connection to Network
    if (WiFi.status() != WL_CONNECTED)
    {
        esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
        WiFi.begin(ssid, password);
    }

    delay(3000);

    // If successfully connected, return true; if not, return false
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Error: the SSID or the password may be wrong.");
        return false;
    }
    else
    {
        return true;
    }
}


// Initialize WiFi module and invoke wifiConnect() to initialize connection
void setupWifi()
{
    // If already connected, return
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    // Wait 100 milliseconds
    delay(100);

    // Enable and set Station Mode on Module (Client Mode, connection to other networks)
    WiFi.enableSTA(true);
    WiFi.mode(WIFI_STA);

    bool connected = false;
    int index = 0;
    while (!connected)
    {
        // Try to connect to all networks in defined vector
        connected = wifiConnect(wiFiCredentials[index].ssid, wiFiCredentials[index].password);
        // If reached networks' number limit, break
        if (++index > wiFiCredentials.size())
        {
            break;
        }
    }

    // Restart the MCU if not connected to a network
    if (!connected)
    {
        Serial.println("Restarting ...");
        ESP.restart();
    }

    // Set WiFi Module Transmission Power to max
    esp_wifi_set_max_tx_power(78);
    delay(10);

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void getPowerHistory(Contract energyContract) {
    string waddress = (string) MY_ADDRESS;

    string func = "getPowerHistory(address)";
    string param = energyContract.SetupContractData(func.c_str(), waddress);
    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.ViewCall(&param);

    Serial.printf("Energy Notarization - Get History POW - Result: %d\n", web3->getInt(&result));
}

void getMaxPower(Contract energyContract) {
    string waddress = (string) MY_ADDRESS;

    string func = "getAllTimeMax(address)";
    string param = energyContract.SetupContractData(func.c_str(), waddress);
    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.ViewCall(&param);

    Serial.printf("Energy Notarization - Get AllTimeMax POW - Result: %d\n", web3->getInt(&result));
}

void authenticate(Contract energyContract) {
    string waddress = (string) MY_ADDRESS;
    string caddress = (string) CONTRACT_ADDRESS;

    // Extracting r, s, and v
    BYTE r[32], s[32];
    BYTE v;

    memcpy(r, signatureESPID, 32); // Copy first 32 bytes to r
    memcpy(s, signatureESPID + 32, 32); // Copy next 32 bytes to s
    v = signatureESPID[64];

    // Define Nonce, int value associated to transaction
    uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(&waddress);
    // Define gasPrice, int of gas used for transaction
    unsigned long long gasPriceVal = 10000LL;
    // Define gasLimit, max gas provived for transaction
    uint32_t  gasLimitVal = 3000000;
    // Define Amount sent in transaction
    uint256_t valueStr = 0;
    // Define address of contract to send transaction to
    string *toStr = (string *)CONTRACT_ADDRESS;
    // Define Data, hash of an invoked method signature and its encoded parameters
    string dataStr = energyContract.SetupContractData("verifySignature(bytes32, uint8, bytes32, bytes32)", hashedESPID, v, r, s);

    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, &caddress, &valueStr, &dataStr);
    string formattedResult = web3->getResult(&result);
    Serial.printf("\nEnergy Notarization - Authenticate - Result: %s\n", formattedResult.c_str());

}

// Questa funzione provoca il crash dell'ESP32 (Guru Meditation Error/ abort())
// Per maggiori informazioni sul contratto è possibile aprire il file EnergyNotarization.sol
void sendInstantPower(Contract energyContract, int instantPower) {
    string waddress = (string) MY_ADDRESS;
    string caddress = (string) CONTRACT_ADDRESS;

    // Define Nonce, int value associated to transaction
    uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(&waddress);
    // Define gasPrice, int of gas used for transaction
    unsigned long long gasPriceVal = 10000LL;
    // Define gasLimit, max gas provived for transaction
    uint32_t  gasLimitVal = 3000000;
    // Define Amount sent in transaction
    uint256_t valueStr = 0;
    // Define address of contract to send transaction to
    string *toStr = (string *)CONTRACT_ADDRESS;
    // Define Data, hash of an invoked method signature and its encoded parameters
    string func = "addPowerEntry(uint256, bytes32, bytes32, bytes32, uint8)";
    uint8_t r[32], s[32];
    uint8_t v;
    memcpy(r, signatureESPID, 32);
    memcpy(s, &signatureESPID[32], 32);
    v = signatureESPID[64];

    string strR = "0x" + uint8ArrayToHexString(r, 32);
    string strS = "0x" + uint8ArrayToHexString(s, 32);
    string strHash = "0x" + uint8ArrayToHexString(hashedESPID, 32);
    Serial.printf("hash: %s\n", strHash.c_str());
    Serial.printf("r: %s\n", strR.c_str());
    Serial.printf("s: %s\n", strS.c_str());

    Serial.println("Until setup");
    // Il crash avviente in questa esecuzione di funzione presente nel file Contract.cpp
    // di Web3E alla posizione .pio/libdeps/esp32dev/Web3E/src/Contract.cpp
    // Presenta differentemente alla funzione in echoBytes, l'invio anche di due valori interi
    string dataStr = energyContract.SetupContractData(
        "addPowerEntry(uint256, bytes, bytes, bytes, uint)",
        (uint256_t)instantPower, 
        strHash,
        strR,
        strS,
        (uint)v);
    Serial.println("Until send");

    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, &caddress, &valueStr, &dataStr);
    string formattedResult = web3->getResult(&result);
    Serial.printf("\nEnergy Notarization - Send POW Value: %d - Result: %s\n", instantPower, formattedResult.c_str());
}

// Questa funzione non provoca crash all'ESP32 ma la sua transazione fallisce
// Per maggiori informazioni sul contratto è possibile aprire il file EnergyNotarization.sol
void echoBytes(Contract energyContract) {
    string waddress = (string) MY_ADDRESS;
    string caddress = (string) CONTRACT_ADDRESS;

    // Define Nonce, int value associated to transaction
    uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(&waddress);
    // Define gasPrice, int of gas used for transaction
    unsigned long long gasPriceVal = 10000LL;
    // Define gasLimit, max gas provived for transaction
    uint32_t  gasLimitVal = 3000000;
    // Define Amount sent in transaction
    uint256_t valueStr = 0;
    // Define address of contract to send transaction to
    string *toStr = (string *)CONTRACT_ADDRESS;
    // Define Data, hash of an invoked method signature and its encoded parameters
    uint8_t r[32], s[32];
    uint8_t v;
    memcpy(r, signatureESPID, 32);
    memcpy(s, &signatureESPID[32], 32);
    v = signatureESPID[64];
    string strHash = "0x" + uint8ArrayToHexString(hashedESPID, 32);
    string strR = "0x" + uint8ArrayToHexString(r, 32);
    string strS = "0x" + uint8ArrayToHexString(s, 32);
    Serial.printf("hash: %s\n", strHash.c_str());

    Serial.println("Until setup");
    string dataStr = energyContract.SetupContractData(
        "echoBytes(bytes,bytes,bytes)",
        strHash,
        strR,
        strS);
    Serial.println("Until send");
    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, &caddress, &valueStr, &dataStr);
    string formattedResult = web3->getResult(&result);
    Serial.printf("\nEnergy Notarization - Echo Bytes Value - Result: %s\n", formattedResult.c_str());
}

void setup() {
    randomSeed(123);
    Serial.begin(9600);

    // Connect to Network
    setupWifi();
    Serial.println("\n--- Connection Phase Completed ---\n");
    
    // Define Socket of BC Service
    // The ip address should be that of the server running SSL
    const char * ip_add = "192.168.126.243";
    unsigned short port = 443;
    long long networkId = 1337;

    // Setup WEB3 Connection to BC
    web3 = new Web3(networkId, ip_add, port);

    // Operazioni di generazione di hash e firma
    crypto = new Crypto(web3);
    crypto->SetPrivateKey(PRIVATE_KEY);
    crypto->Keccak256(secureESPID, 32, hashedESPID);
    crypto->Sign(hashedESPID, signatureESPID);

    esp_wifi_set_max_tx_power(78);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Connect to Running SC
        Contract energyContract = Contract(web3, CONTRACT_ADDRESS);
        // Set Private Key
        energyContract.SetPrivateKey(PRIVATE_KEY);

        int randomInstantPower = random(2, 7);
        echoBytes(energyContract);
        // La funzione sottostante genera un segentation fault
        // sendInstantPower(energyContract, randomInstantPower);
        getMaxPower(energyContract);
    }
}