# Enclaved-FE : Run Functional Encryption in Intel SGX

This project aims to enable applications using Fentec **Functional Encryption** libraries [CiFEr](https://github.com/fentec-project/CiFEr) or [Go version named GoFE](https://github.com/fentec-project/gofe) library to run in Intel SGX with little or even no modifications of source code. We leverage library OS's such as [Graphene](https://github.com/oscarlab/graphene) or [Occlum](https://github.com/occlum/occlum) which are purposely built for Intel SGX. This project primarily contains various build and run configurations and sample applications. 

<!-- toc -->
- [Enclaved-FE : Run Functional Encryption in Intel SGX](#enclaved-fe--run-functional-encryption-in-intel-sgx)
  - [Running CiFEr in Graphene-SGX](#running-cifer-in-graphene-sgx)
    - [Prerequisite](#prerequisite)
    - [1. Example Application From CiFEr Poject](#1-example-application-from-cifer-poject)
      - [Building for Linux](#building-for-linux)
      - [Building for SGX](#building-for-sgx)
      - [Run Application](#run-application)
    - [2. End-To-End Application](#2-end-to-end-application)
<!-- tocstop -->

## Running CiFEr in Graphene-SGX
[graphene directory](graphene) includes the Makefile and a template for generating the manifest. There are two applications one simple and one end-to-end, both are tested on Ubuntu 20.04, with both normal Linux and SGX platforms.

### Prerequisite
- Follow [Graphene Document](https://graphene.readthedocs.io/en/latest/quickstart.html) to install Graphene and Intel SDK and tools.
- Also make sure [graphene-sgx-helloworld](https://graphene.readthedocs.io/en/latest/quickstart.html#quick-start-with-sgx-support) works on your machine.

- Install CiFEr dependencies:
    ```sh
    sudo apt install -y build-essential libgmp-dev libsodium-dev
    ```

### 1. Example Application From CiFEr Poject
[graphene/simple](graphene/simple) downloads code from official CiFEr github repository and builds *libcifer* and the example application in CiFEr project.

Clone this repo and go to *graphene/simple* directory
  ```
  cd graphene/simple
  ```

#### Building for Linux

Run `GRAPHENE_DIR=<graphene-root-dir> make` (non-debug) or `GRAPHENE_DIR=<graphene-root-dir> make DEBUG=1` (debug) in the directory.

#### Building for SGX

Run `GRAPHENE_DIR=<graphene-root-dir> make SGX=1` (non-debug) or `GRAPHENE_DIR=<graphene-root-dir> make SGX=1 DEBUG=1` (debug) in the directory.

#### Run Application

Without SGX:
  ```
  ./pal_loader ./cifer_app
  ```

With SGX:
  ```
  SGX=1 ./pal_loader ./cifer_app
  ```
### 2. End-To-End Application
[graphene/e2e](graphene/e2e) contains source code for a more real world example implemented by us. For this we also need to install *libcifer* in */usr/local* path. Follow official [CiFEr documentation](https://github.com/fentec-project/CiFEr) for installation steps for *libcifer*.

Clone this repo and go to *graphene/e2e* directory
  ```
  cd graphene/e2e
  ```

