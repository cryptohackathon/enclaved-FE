# Enclaved-FE : Run Functional Encryption in Intel SGX

This project aims to enable applications using Fentec **Functional Encryption** libraries [CiFEr](https://github.com/fentec-project/CiFEr) or [Go version named GoFE](https://github.com/fentec-project/gofe) library to run in Intel SGX with little or even no modifications of source code. We leverage library OS's such as [Graphene](https://github.com/oscarlab/graphene) or [Occlum](https://github.com/occlum/occlum) which are purposely built for Intel SGX. This project primarily contains various build and run configurations and sample applications. 

<!-- toc -->
- [Enclaved-FE : Run Functional Encryption in Intel SGX](#enclaved-fe--run-functional-encryption-in-intel-sgx)
  - [Running CiFEr in Graphene-SGX](#running-cifer-in-graphene-sgx)
    - [Prerequisite](#prerequisite)
    - [Build Example Application](#build-example-application)
      - [Building for Linux](#building-for-linux)
      - [Building for SGX](#building-for-sgx)
    - [Run Example Application](#run-example-application)
<!-- tocstop -->

## Running CiFEr in Graphene-SGX
[graphene directory](graphene) includes the Makefile and a template for generating the manifest. The application is tested on Ubuntu 20.04, with both normal Linux and SGX platforms.

### Prerequisite
- Follow [Graphene Document](https://graphene.readthedocs.io/en/latest/quickstart.html) to install Graphene and Intel SDK and tools.
- Also make sure [graphene-sx-helloworld](https://graphene.readthedocs.io/en/latest/quickstart.html#quick-start-with-sgx-support) works on your machine.

- Install CiFEr dependencies:
    ```sh
    sudo apt install -y build-essential libgmp-dev libsodium-dev
    ```

### Build Example Application

#### Building for Linux

Run `GRAPHENE_DIR=<graphene-root-dir> make` (non-debug) or `GRAPHENE_DIR=<graphene-root-dir> make DEBUG=1` (debug) in the directory.

#### Building for SGX

Run `GRAPHENE_DIR=<graphene-root-dir> make SGX=1` (non-debug) or `GRAPHENE_DIR=<graphene-root-dir> make SGX=1 DEBUG=1` (debug) in the directory.

### Run Example Application

Without SGX:
  ```
  ./pal_loader ./cifer_app
  ```

With SGX:
  ```
  SGX=1 ./pal_loader ./cifer_app
  ```