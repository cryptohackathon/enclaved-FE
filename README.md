# Enclaved-FE : Run Functional Encryption in Intel SGX

This project aims to enable applications using Fentec **Functional Encryption** libraries [CiFEr](https://github.com/fentec-project/CiFEr) or [Go version named GoFE](https://github.com/fentec-project/gofe) library to run in Intel SGX with little or even no modifications of source code. We leverage library OS's such as [Graphene](https://github.com/oscarlab/graphene) or [Occlum](https://github.com/occlum/occlum) which are purposely built for Intel SGX. This project primarily contains various build and run configurations and sample applications. 

<!-- toc -->
- [Enclaved-FE : Run Functional Encryption in Intel SGX](#enclaved-fe--run-functional-encryption-in-intel-sgx)
  - [Running CiFEr in Graphene-SGX](#running-cifer-in-graphene-sgx)
    - [Prerequisite](#prerequisite)
    - [1. simple_app From CiFEr Poject](#1-simple_app-from-cifer-poject)
      - [Building for Linux](#building-for-linux)
      - [Building for SGX](#building-for-sgx)
      - [Run Application](#run-application)
    - [2. pp_analytics_app End-To-End Application](#2-pp_analytics_app-end-to-end-application)
<!-- tocstop -->

## Running CiFEr in Graphene-SGX

There are two applications:

- [simple_app](simple_app) doesn't have any source code, it only contains Makefile script which downloads CiFEr github repo, builds library and example applications. Directory also a template file for generating the manifest used by Graphene to run in SGX. The example application runs locally without any client-server type of communication scenario. This project can be used as starter project for more complex SGX ready application.

- [pp_analytics_app](pp_analytics_app) contains source code for a client-server type of example implemented by us where multiple entities utilizing functional encryption run in SGX.

Both application are tested on Ubuntu 20.04, with both normal Linux and in SGX hardware mode container.

### Prerequisite
- Follow [Graphene Document](https://graphene.readthedocs.io/en/latest/quickstart.html) to install Graphene and Intel SDK and tools.
- Also make sure [graphene-sgx-helloworld](https://graphene.readthedocs.io/en/latest/quickstart.html#quick-start-with-sgx-support) works on your machine.

- Install CiFEr dependencies:
    ```sh
    sudo apt install -y build-essential libgmp-dev libsodium-dev
    ```

### 1. [simple_app](simple_app) From CiFEr Poject

Clone this repo and go to *[simple_app](simple_app)* directory
  ```
  cd simple_app
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
### 2. [pp_analytics_app](pp_analytics_app) End-To-End Application
Please check [READEME](pp_analytics_app/README.md) for build and run instructions.
