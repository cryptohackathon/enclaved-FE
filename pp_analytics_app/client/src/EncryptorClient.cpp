#include <cassert>

#include <gmp.h>
extern "C" {
#include "cifer/sample/uniform.h"
}

#include "logfault/logfault.h"

#include "EncryptorClient.hpp"

using namespace std;

EncryptorClient::EncryptorClient(int64_t clientId, size_t dataVectorLength,
                                 size_t modulusLen, const string &bound) {
  _clientId = clientId;
  // first choose meta-parameters for the scheme
  _dataVectorLength = dataVectorLength;
  mpz_init(_bound);
  mpz_set_str(_bound, bound.c_str(), 10);

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
    mpz_set_str(_masterPublicKey.vec[i], publicKey[i].c_str(), 10);
  }
}

vector<string> EncryptorClient::encryptData(void *data) {
  vector<string> encryptedData;
  cfe_ddh encryptor;

  // Generate and encrypt some dummy data
  // set the vector (of length l) that you want to encrypt
  // we sample a uniformly random vector x
  cfe_vec x;

  cfe_vec_init(&x, _dataVectorLength);
  cfe_uniform_sample_vec(&x, _bound);

  /*   LFLOG_INFO << "plain text data(x):";
    for (size_t i = 0; i < x.size; i++) {
      LFLOG_INFO << "x[" << i << "]: " << mpz_get_str(NULL, 10, x.vec[i]);
    } */
  LFLOG_INFO << "plain text data data" << endl;
  cfe_vec_print(&x);
  LFLOG_INFO << endl;

  // encrypt the the vector x
  cfe_vec ciphertext;

  cfe_ddh_copy(&encryptor, &_scheme);
  cfe_ddh_ciphertext_init(&ciphertext, &encryptor);
  cfe_error err =
      cfe_ddh_encrypt(&ciphertext, &encryptor, &x, &_masterPublicKey);
  assert(err == 0);

  for (size_t i = 0; i < ciphertext.size; i++) {
    encryptedData.push_back(mpz_get_str(NULL, 10, ciphertext.vec[i]));
  }

  cfe_ddh_free(&encryptor);
  return encryptedData;
}
