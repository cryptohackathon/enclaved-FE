#ifndef ENCRYPTOR_CLIENT
#define ENCRYPTOR_CLIENT

extern "C" {
#include "cifer/innerprod/fullysec/damgard.h"
}

class EncryptorClient
{
private:
    size_t _encryptionVectorLen; // dimension of encryption vector
    mpz_t _bound; // bound of the input values set to 2^10
    size_t _modulusLen; // dimension of encryption vector
    cfe_damgard _scheme;
    cfe_vec _masterPublicKey;

public:
    EncryptorClient(size_t encryptionVectorLen = 5,
                                        size_t modulusLen = 2048);
    ~EncryptorClient();
    void setPublicKey(void *data);
    void encryptData();
};

#endif