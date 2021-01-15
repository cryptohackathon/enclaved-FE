FROM ya0guang/sgx-u20:graphene-bwa-1.1

LABEL maintainer="gaurav1@tutamail.com"

# install dependencies and build tools
RUN apt update --fix-missing
RUN apt install -y build-essential libgmp-dev libsodium-dev git python python3-pip libcurl4-openssl-dev vim
RUN pip3 install cmake==3.18

WORKDIR /app
RUN git clone https://github.com/fentec-project/CiFEr.git

# install amcl dependency
WORKDIR /app/CiFEr/external/amcl
RUN ./setup_amcl.sh
# build install cifer
WORKDIR /app/CiFEr
RUN mkdir build_files
WORKDIR /app/CiFEr/build_files
RUN cmake ..
RUN make
RUN make install

WORKDIR /app
RUN git clone https://github.com/cryptohackathon/enclaved-FE.git

# Generate siging key
WORKDIR /root/graphene/Pal/src/host/Linux-SGX/signer/
RUN openssl genrsa -3 -out enclave-key.pem 3072

# buid client app
WORKDIR /app/enclaved-FE/pp_analytics_app/client
#RUN GRAPHENE_DIR=/root/graphene SGX=1 DEBUG=1 make

# run CiFEr-example-app
WORKDIR /app/enclaved-FE/pp_analytics_app/client
#CMD SGX=1 DEBUG=1 ./pal_loader ./fe_sgx_demo_client
