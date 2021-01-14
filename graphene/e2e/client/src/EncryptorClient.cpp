#include "cassert"

#include <gmp.h>
extern "C" {
#include "cifer/sample/uniform.h"
}

#include "EncryptorClient.hpp"

using namespace std;

EncryptorClient::EncryptorClient(size_t encryptionVectorLen, size_t modulusLen)
{
    // first choose meta-parameters for the scheme
    _encryptionVectorLen = encryptionVectorLen;
    mpz_init(_bound);
    mpz_set_ui(_bound, 2);
    mpz_pow_ui(_bound, _bound, 10);
    _modulusLen = modulusLen;

    // initiate the scheme
    cfe_error err = cfe_damgard_precomp_init(&_scheme, _encryptionVectorLen, _modulusLen, _bound);
    assert(err == 0);

}

EncryptorClient::~EncryptorClient()
{
        cfe_damgard_free(&_scheme);

}

void EncryptorClient::setPublicKey(void *data)
{
   // _masterPublicKey = data;
}
void EncryptorClient::encryptData()
{
    // set the vector (of length l) that you want to encrypt
    // we sample a uniformly random vector x
    cfe_vec x;
    cfe_vec_init(&x, _encryptionVectorLen);
    cfe_uniform_sample_vec(&x, _bound);

    // encrypt the the vector x
    cfe_vec ciphertext;
    cfe_damgard_ciphertext_init(&ciphertext, &_scheme);
    cfe_error err = cfe_damgard_encrypt(&ciphertext, &_scheme, 
                                            &x, &_masterPublicKey);
    assert(err == 0);

}

