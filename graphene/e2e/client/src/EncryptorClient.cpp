#include <cassert>

#include <gmp.h>
extern "C" {
#include "cifer/sample/uniform.h"
}

#include "EncryptorClient.hpp"

using namespace std;

EncryptorClient::EncryptorClient(const string& clientId,
                                    size_t dataVectorLength, 
                                    size_t modulusLen,
                                    int32_t bound)
{
    _clientId = clientId;
    // first choose meta-parameters for the scheme
    _dataVectorLength = dataVectorLength;
    mpz_init(_bound);
    mpz_set_ui(_bound, 2);
    mpz_pow_ui(_bound, _bound, 10);
    _modulusLen = modulusLen;

    // initiate the scheme
    cfe_error err = cfe_ddh_precomp_init(&_scheme, _dataVectorLength, _modulusLen, _bound);
    assert(err == 0);

}

EncryptorClient::~EncryptorClient()
{
    cfe_ddh_free(&_scheme);

}

void EncryptorClient::setPublicKey(const std::string& publicKey)
{
   // _masterPublicKey = data;
}
void EncryptorClient::encryptData()
{
    cfe_ddh encryptor;
    // set the vector (of length l) that you want to encrypt
    // we sample a uniformly random vector x
    cfe_vec x;
    cfe_vec_init(&x, _dataVectorLength);
    cfe_uniform_sample_vec(&x, _bound);

    // encrypt the the vector x
    cfe_vec ciphertext;

    cfe_ddh_copy(&encryptor, &_scheme);
    cfe_ddh_ciphertext_init(&ciphertext, &encryptor);
    cfe_error err = cfe_ddh_encrypt(&ciphertext, &encryptor, 
                                        &x, &_masterPublicKey);
    assert(err == 0);
    cfe_ddh_free(&encryptor);


}

