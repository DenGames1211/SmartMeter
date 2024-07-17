// Blockchain ESP32 Account Public and Private Key Generation

#ifndef CONTRACT_INT
#define CONTRACT_INT

#include <CryptoUtils.h>

void sendInstantEnergy(unsigned long int i, unsigned long *paramTime, unsigned long *hashTime, unsigned long *signTime, unsigned long *sendTime, Web3 *web3, Crypto *crypto, const char *myAddress, const char *contractAddress, Contract energyContract, unsigned char *encryptedEnergy);

#endif //CONTRACT_INT