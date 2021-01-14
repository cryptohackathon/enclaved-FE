#include <cassert>

#include <gmp.h>
extern "C" {
#include "cifer/sample/uniform.h"
}

#include "EncryptorClient.hpp"

using namespace std;

EncryptorClient::EncryptorClient(int64_t clientId, size_t dataVectorLength,
                                 size_t modulusLen, int32_t bound) {
  _clientId = clientId;
  // first choose meta-parameters for the scheme
  _dataVectorLength = dataVectorLength;
  mpz_init(_bound);
  mpz_set_ui(_bound, 2);
  mpz_pow_ui(_bound, _bound, bound);
  _modulusLen = modulusLen;

  // initiate the scheme
  cfe_error err =
      cfe_ddh_precomp_init(&_scheme, _dataVectorLength, _modulusLen, _bound);
  assert(err == 0);
  cfe_vec_init(&_masterPublicKey, _dataVectorLength);
}

EncryptorClient::~EncryptorClient() {
  mpz_clear(_bound);
  cfe_vec_free(&_masterPublicKey);
  cfe_ddh_free(&_scheme);
}

void EncryptorClient::setPublicKey(const vector<string> &publicKey) {
  assert(publicKey.size() == _dataVectorLength);
  for (size_t i = 0; i < _dataVectorLength; i++) {
    mpz_set_str(_masterPublicKey.vec[i], publicKey[0].c_str(), 10);
  }
}

vector<string> EncryptorClient::encryptData(void *data) {
  vector<string> encryptedData;
  mpz_t bound_neg;
  cfe_ddh encryptor;

  // Generate and encrypt some dummy data
  // set the vector (of length l) that you want to encrypt
  // we sample a uniformly random vector x
  cfe_vec x;

  mpz_init(bound_neg);
  mpz_neg(bound_neg, _bound);
  cfe_vec_init(&x, _dataVectorLength);
  cfe_uniform_sample_range_vec(&x, bound_neg, _bound);

  // encrypt the the vector x
  cfe_vec ciphertext;

  cfe_ddh_copy(&encryptor, &_scheme);
  cfe_ddh_ciphertext_init(&ciphertext, &encryptor);
  cfe_error err =
      cfe_ddh_encrypt(&ciphertext, &encryptor, &x, &_masterPublicKey);
  assert(err == 0);

  for (size_t i = 0; i < ciphertext.size; i++) {
    encryptedData.push_back(mpz_get_str(NULL, 0, ciphertext.vec[i]));
  }
  mpz_clear(bound_neg);
  cfe_ddh_free(&encryptor);
  return encryptedData;
}
