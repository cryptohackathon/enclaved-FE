# Privacy preserving analytics demo using Functional Encryption and SGX

This application has at least three different parties:
1. Client - the data owner
2. Trusted Server - can be a government or trustee which acts as a key/IAM server and stores clients encrypted data
3. Analytics Engine or its Clients - who are authorized by data owner to get some insight from data owner's data

Our goal is to run application using Fentec-libraries in Intel SGX enclave.
- [Client - Data Owner](clent) C++ REST client uses CiFEr and runs with graphene-SGX
- [Trusted Entity](trustedentity) Go REST server uses GoFE
- [Analytics Engine](analyticsengine) Go REST server uses GoFE


## Build and Run Trusted Entity
  ```sh
  cd trustedentity

  go run main.go
  ```
## Build and Run Analytics Engine
  ```sh
  cd analyticsengine

  go run main.go
  ```

## Build and Run data owner Client

Clone this repo and go to *[simple_app](simple_app)* directory
  ```sh
  cd client
  ```


#### Building for SGX
Install libcifer followings steps [here](https://github.com/fentec-project/CiFEr).
Install libcurl-dev
    ```sh
    sudo apt-get install libcurl4-openssl-dev
    ```
Run `GRAPHENE_DIR=<graphene-root-dir> make SGX=1` (non-debug) or `GRAPHENE_DIR=<graphene-root-dir> make SGX=1 DEBUG=1` (debug) in the directory.

#### Run Application

With SGX:
  ```
  SGX=1 ./pal_loader ./fe_sgx_demo_client
  ```
