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

// Change instantEnergy with encrypted Data
void sendInstantEnergy(Web3 *web3, Crypto *crypto, const char *myAddress, const char *contractAddress, Contract energyContract, unsigned char *encryptedEnergy) {
    unsigned long startTime = micros(), endTime;
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

    // Define Data, hash of an invoked method signature and its encoded parameters
    // Hash and Sign encrypted power data 
    crypto->Keccak256(encryptedEnergy, 32, hashedEncryptedData);
    crypto->Sign(hashedEncryptedData, signatureEncryptedData);

    uint8_t r[32], s[32];
    unsigned int v;
    memcpy(r, signatureEncryptedData, 32);
    memcpy(s, &signatureEncryptedData[32], 32);
    v = signatureEncryptedData[64];
    string strHash = "0x" + getHexStringFromUint8(hashedEncryptedData, 32);
    string strR = "0x" + getHexStringFromUint8(r, 32);
    string strS = "0x" + getHexStringFromUint8(s, 32);
    
    string dataStr = energyContract.SetupContractData(
        "addEnergyEntry(uint256,bytes,bytes32,bytes32,bytes32)",
        (uint256_t)v,
        encryptedEnergy,
        strHash,
        strR,
        strS
        );

    // Send Transaction to BC (JSON-RPC API to eth_sendRawTransaction)
    string result = energyContract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, &caddress, &valueStr, &dataStr);
    // Serial.printf("\nEnergy Notarization - Send POW Value: %d - Result: %s\n", instantPower, formattedResult.c_str());
}