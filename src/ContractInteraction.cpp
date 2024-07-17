#include <ContractInteraction.h>

/* 
Only available for not encrypted data
void getMaxPower(Web3 *web3, const char *myAddress, Contract energyContract) {
    string waddress = (string) myAddress;

    string func = "getAllTimeMax(address)";
    string param = energyContract.SetupContractData(func.c_str(), waddress);
    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.ViewCall(&param);

    Serial.printf("Energy Notarization - Get AllTimeMax POW - Result: %d\n", web3->getInt(&result));
}
*/

void sendInstantEnergy(unsigned long int i, unsigned long *paramTime, unsigned long *hashTime, unsigned long *signTime, unsigned long *sendTime, 
                        Web3 *web3, Crypto *crypto, const char *myAddress, const char *contractAddress, Contract energyContract, unsigned char *encryptedEnergy) {
    unsigned long startTime = micros();
    string waddress = (string) myAddress;
    string caddress = (string) contractAddress;
    uint8_t hashedEncryptedData[32], signatureEncryptedData[65];

    // Define Nonce, int value associated to transaction
    uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(&waddress);
    // Define gasPrice, int of gas used for transaction
    unsigned long long gasPriceVal = 10000LL;
    // Define gasLimit, max gas provived for transaction
    uint32_t  gasLimitVal = 3000000;
    // Define Amount sent in transaction
    uint256_t valueStr = 0;
    // Define address of contract to send transaction to
    string *toStr = (string *)contractAddress;
    paramTime[i] = micros() - startTime;
    startTime = micros();

    // Define Data, hash of an invoked method signature and its encoded parameters
    // Hash and Sign encrypted power data 
    crypto->Keccak256(encryptedEnergy, 32, hashedEncryptedData);
    hashTime[i] = micros() - startTime;
    startTime = micros();

    crypto->Sign(hashedEncryptedData, signatureEncryptedData);
    signTime[i] = micros() - startTime;
    startTime = micros();

    uint8_t r[32], s[32];
    unsigned int v;
    memcpy(r, signatureEncryptedData, 32);
    memcpy(s, &signatureEncryptedData[32], 32);
    v = signatureEncryptedData[64];
    string strHash = "0x" + getHexStringFromUint8(hashedEncryptedData, 32);
    string strR = "0x" + getHexStringFromUint8(r, 32);
    string strS = "0x" + getHexStringFromUint8(s, 32);
    std::string strEncEnergy(reinterpret_cast<char*>(encryptedEnergy));

    Serial.printf("v: %d\n", v);
    Serial.printf("energy: %s\n", strEncEnergy.c_str());
    Serial.printf("hash: %s\n", strHash.c_str());
    Serial.printf("r: %s\n", strR.c_str()); 
    Serial.printf("s: %s\n", strS.c_str());

    string dataStr = energyContract.SetupContractData(
        "addEnergyEntry(bytes,bytes32,bytes32,bytes32,uint256)",
        strEncEnergy,
        strHash,
        strR,
        strS,
        (uint256_t)v
        );

    Serial.printf("\nData String: %s\n", dataStr.c_str());

    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, &caddress, &valueStr, &dataStr);
    sendTime[i] = micros() - startTime;
    // Serial.printf("\nEnergy Notarization - Send POW Value: %d - Result: %s\n", instantPower, formattedResult.c_str());
}