# Build cifer-application as follows:
#
# - make               -- create non-SGX no-debug-log manifest
# - make DEBUG=1       -- create non-SGX debug-log manifest
# - make SGX=1         -- create SGX no-debug-log manifest
# - make SGX=1 DEBUG=1 -- create SGX debug-log manifest
#
# Use `make clean` to remove Graphene-generated files.

################################# CONSTANTS ###################################

# Relative path to Graphene root
GRAPHENE_DIR ?= $(THIS_DIR)../..
SGX_SIGNER_KEY ?= $(GRAPHENE_DIR)/Pal/src/host/Linux-SGX/signer/enclave-key.pem

THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
FE_SGX_DEMO_CLIENT_SRC_DIR ?= $(THIS_DIR)src

ifeq ($(DEBUG),1)
GRAPHENE_DEBUG = inline
CIFER_BUILD = Debug
else
GRAPHENE_DEBUG = none
CIFER_BUILD = Release
endif

.PHONY: all
all: fe_sgx_demo_client fe_sgx_demo_client.manifest pal_loader
ifeq ($(SGX),1)
all: fe_sgx_demo_client.token
endif

include $(GRAPHENE_DIR)/Scripts/Makefile.configs

######################## Build EXECUTABLE #####################################

# create a symbolic link in current directory
$(FE_SGX_DEMO_CLIENT_SRC_DIR)/fe_sgx_demo_client:
	@cd $(FE_SGX_DEMO_CLIENT_SRC_DIR) && mkdir -p build
	@cd $(FE_SGX_DEMO_CLIENT_SRC_DIR)/build && cmake ..
	$(MAKE) -C $(FE_SGX_DEMO_CLIENT_SRC_DIR)/build

##############################  MANIFEST ######################################

# CiFEr app dependencies (generate from ldd):
#
# For SGX, the manifest needs to list all the libraries loaded during the
# execution, so that the signer can include the file checksums.
#
# The dependencies are generated from the ldd results.

## We need to replace Glibc dependencies with Graphene-specific Glibc. The Glibc binaries are
# already listed in the manifest template, so we can skip them from the ldd results.
GLIBC_DEPS = linux-vdso.so.1 /lib64/ld-linux-x86-64.so.2 libc.so.6 \
                libm.so.6 librt.so.1 libdl.so.2 libpthread.so.0 \
				libutil.so.1 libresolv.so.2 libnss_dns.so.2

# Listing all the application dependencies, besides Glibc libraries
.INTERMEDIATE: fe_sgx_demo_client_ldd
fe_sgx_demo_client_ldd: fe_sgx_demo_client
		@ldd $< >> $@ || exit 1

.INTERMEDIATE: fe_sgx_demo_client_deps
fe_sgx_demo_client_deps: fe_sgx_demo_client_ldd
	@cat $< | awk '{if ($$2 =="=>") {print $$1}}' \
		| sort | uniq | grep -v -x $(patsubst %,-e %,$(GLIBC_DEPS)) > $@

# Generating manifest rules for fe_sgx_demo_client dependencies
.INTERMEDIATE: fe_sgx_demo_client_trusted_libs
fe_sgx_demo_client_trusted_libs: fe_sgx_demo_client_deps
	@for F in `cat fe_sgx_demo_client_deps`; do \
		P=`ldd fe_sgx_demo_client | grep $$F | awk '{print $$3; exit}'`; \
		N=`echo $$F | tr --delete '.' | tr --delete '-'`; \
		echo -n "sgx.trusted_files.\"$$N\" = \\\"file:$$P\\\"\\\\n"; \
	done > $@

# The template file contains almost all necessary information to run cifer 
# application under Graphene / Graphene-SGX. We create fe_sgx_demo_client.manifest
# (to be run under non-SGX Graphene) by simply replacing variables in the 
# template file via sed.

fe_sgx_demo_client.manifest: fe_sgx_demo_client.manifest.template fe_sgx_demo_client_trusted_libs
	sed -e 's|$$(GRAPHENE_DIR)|'"$(GRAPHENE_DIR)"'|g' \
		-e 's|$$(GRAPHENE_DEBUG)|'"$(GRAPHENE_DEBUG)"'|g' \
		-e 's|$$(FE_SGX_DEMO_CLIENT_TRUSTED_LIBS)|'"`cat fe_sgx_demo_client_trusted_libs`"'|g' \
	    -e 's|$$(ARCH_LIBDIR)|'"$(ARCH_LIBDIR)"'|g' \
	    $< > $@

# Manifest for Graphene-SGX requires special "pal-sgx-sign" procedure. This
# procedure measures all application dependencies (shared libraries and trusted
# files), measures application code/data pages, adds measurements in the
# resulting manifest.sgx file (among other, less important SGX options) and
# creates memcached.sig (SIGSTRUCT object).
#
# Graphene-SGX requires EINITTOKEN and SIGSTRUCT objects (see SGX hardware ABI,
# in particular EINIT instruction). The "pal-sgx-get-token" script generates
# EINITTOKEN based on a SIGSTRUCT and puts it in .token file. Note that
# filenames must be the same as the executable/manifest name (i.e., "fe_sgx_demo_client").
# EINITTOKEN must be generated on the machine where the application will run, not
# where it was built.

fe_sgx_demo_client.manifest.sgx: fe_sgx_demo_client.manifest fe_sgx_demo_client
	$(GRAPHENE_DIR)/Pal/src/host/Linux-SGX/signer/pal-sgx-sign \
		-libpal $(GRAPHENE_DIR)/Runtime/libpal-Linux-SGX.so \
		-key $(SGX_SIGNER_KEY) \
		-manifest $< -output $@ \
		-exec fe_sgx_demo_client

fe_sgx_demo_client.sig: fe_sgx_demo_client.manifest.sgx

fe_sgx_demo_client.token: fe_sgx_demo_client.sig
	$(GRAPHENE_DIR)/Pal/src/host/Linux-SGX/signer/pal-sgx-get-token \
		 -output $@ -sig $<

########################### COPIES OF EXECUTABLES #############################

fe_sgx_demo_client: $(FE_SGX_DEMO_CLIENT_SRC_DIR)/fe_sgx_demo_client
	@cp $(FE_SGX_DEMO_CLIENT_SRC_DIR)/build/fe_sgx_demo_client $@

pal_loader:
	@ln -s $(GRAPHENE_DIR)/Runtime/pal_loader $@

############################## RUNNING APP ####################################

.PHONY: run-native
run-native: all
	./fe_sgx_demo_client

.PHONY: run-graphene
run-graphene: all
	./pal_loader ./fe_sgx_demo_client

	.PHONY: run-graphene-sgx
run-graphene-sgx: all
	SGX=1 ./pal_loader ./fe_sgx_demo_client

################################## CLEANUP ####################################

.PHONY: clean
clean:
	$(RM) -r *.token *.sig *.manifest.sgx *.manifest pal_loader .lck \
		*.o fe_sgx_demo_client $(FE_SGX_DEMO_CLIENT_SRC_DIR)/build

