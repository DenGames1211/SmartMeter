// Blockchain ESP32 Account Public and Private Key Generation

#ifndef CONTRACT_INT
#define CONTRACT_INT

#include <CryptoUtils.h>

void getMaxPower(Web3 *web3, const char *myAddress, Contract energyContract);
void sendInstantPower(Web3 *web3, Crypto *crypto, const char *myAddress, const char *contractAddress, Contract energyContract, unsigned char *encryptedPower);

#endif //CONTRACT_INT