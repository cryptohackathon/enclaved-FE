#ifndef ENCRYPTOR_CLIENT
#define ENCRYPTOR_CLIENT

#include <cstdint>
#include <string>

#include <gmp.h>
extern "C" {
#include "cifer/innerprod/simple/ddh.h"
}

using namespace std;

class EncryptorClient
{
private:
    string _clientId;
    size_t _dataVectorLength; // dimension of encryption vector
    mpz_t _bound; // bound of the input values set to 2^10
    size_t _modulusLen; // dimension of encryption vector
    cfe_ddh _scheme;
    cfe_vec _masterPublicKey;

public:
    EncryptorClient(const string& clientId, size_t dataVectorLength,
                                        size_t modulusLen, int32_t bound);
    ~EncryptorClient();
    void setPublicKey(const string& publicKey);
    void encryptData();
};

#endif