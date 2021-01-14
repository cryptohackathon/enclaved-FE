# Build cifer-application as follows:
#
# - make               -- create non-SGX no-debug-log manifest
# - make DEBUG=1       -- create non-SGX debug-log manifest
# - make SGX=1         -- create SGX no-debug-log manifest
# - make SGX=1 DEBUG=1 -- create SGX debug-log manifest
#
# Any of these invocations downloads cifer and builds cifer in default
# configuration.
#
# Use `make clean` to remove Graphene-generated files and `make distclean` to
# additionally remove the cifer source and installation.

################################# CONSTANTS ###################################

# Relative path to Graphene root
GRAPHENE_DIR ?= $(THIS_DIR)../..
SGX_SIGNER_KEY ?= $(GRAPHENE_DIR)/Pal/src/host/Linux-SGX/signer/enclave-key.pem

THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
CIFER_LIB_SRC_DIR ?= $(THIS_DIR)cifer

# Git repo ro download CiFEr library
CIFER_LIB_REPO ?= https://github.com/dev0x1/CiFEr.git
CIFER_LIB_REPO_BRANCH ?= multi_defs_build_issue_fix

ifeq ($(DEBUG),1)
GRAPHENE_DEBUG = inline
CIFER_BUILD = Debug
else
GRAPHENE_DEBUG = none
CIFER_BUILD = Release
endif

.PHONY: all
all: cifer_app cifer_app.manifest pal_loader
ifeq ($(SGX),1)
all: cifer_app.token
endif

include $(GRAPHENE_DIR)/Scripts/Makefile.configs

######################## cifer LIB and EXECUTABLE #########################

# clone master branch cifer github repository
$(CIFER_LIB_SRC_DIR)/git_download:
	@git clone $(CIFER_LIB_REPO) -b $(CIFER_LIB_REPO_BRANCH) $(CIFER_LIB_SRC_DIR)

# cifer librry is built as usual, without any changes to the build process.
$(CIFER_LIB_SRC_DIR)/cifer_lib_build: $(CIFER_LIB_SRC_DIR)/git_download
	@cd $(CIFER_LIB_SRC_DIR) && mkdir -p build
	@cd $(CIFER_LIB_SRC_DIR)/build && cmake ..
	$(MAKE) -C $(CIFER_LIB_SRC_DIR)/build
	sudo $(MAKE) -C $(CIFER_LIB_SRC_DIR)/build install


# build application
cifer_app: $(CIFER_LIB_SRC_DIR)/cifer_lib_build
	$(CC) $(CIFER_LIB_SRC_DIR)/example/example.c -o cifer_app -lgmp -lcifer


############################## cifer MANIFEST #############################

# CiFEr app dependencies (generate from ldd):
#
# For SGX, the manifest needs to list all the libraries loaded during the
# execution, so that the signer can include the file checksums.
#
# The dependencies are generated from the ldd results.

# We need to replace Glibc dependencies with Graphene-specific Glibc. The Glibc
# binaries are already listed in the manifest template, so we can skip them
# from the ldd results
GLIBC_DEPS = linux-vdso /lib64/ld-linux-x86-64 libc libm librt libdl libutil libpthread

# Listing all the cifer application dependencies, besides Glibc libraries
.INTERMEDIATE: cifer_app_ldd
cifer_app_ldd: cifer_app
		@ldd $< >> $@ || exit 1

.INTERMEDIATE: cifer_app_deps
cifer_app_deps: cifer_app_ldd
	@cat $< | awk '{if ($$2 =="=>") {split($$1,s,/\./); print s[1]}}' \
		| sort | uniq | grep -v -x $(patsubst %,-e %,$(GLIBC_DEPS)) > $@

# Generating manifest rules for cifer_app dependencies
.INTERMEDIATE: cifer_app_trusted_libs
cifer_app_trusted_libs: cifer_app_deps
	@CIFER_APP="cifer_app" && \
	for F in `cat cifer_app_deps`; do \
		P=`ldd $$CIFER_APP | grep $$F | awk '{print $$3; exit}'`; \
		N=`echo $$F | tr --delete '-'`; \
		echo -n "sgx.trusted_files.$$N = \\\"file:$$P\\\"\\\\n"; \
	done > $@

# The template file contains almost all necessary information to run cifer 
# application under Graphene / Graphene-SGX. We create cifer_app.manifest
# (to be run under non-SGX Graphene) by simply replacing variables in the 
# template file via sed.

cifer_app.manifest: cifer.manifest.template cifer_app_trusted_libs
	sed -e 's|$$(GRAPHENE_DIR)|'"$(GRAPHENE_DIR)"'|g' \
		-e 's|$$(GRAPHENE_DEBUG)|'"$(GRAPHENE_DEBUG)"'|g' \
		-e 's|$$(CIFER_TRUSTED_LIBS)|'"`cat cifer_app_trusted_libs`"'|g' \
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
# filenames must be the same as the executable/manifest name (i.e., "cifer_app").
# EINITTOKEN must be generated on the machine where the application will run, not
# where it was built.

cifer_app.manifest.sgx: cifer_app.manifest cifer_app
	$(GRAPHENE_DIR)/Pal/src/host/Linux-SGX/signer/pal-sgx-sign \
		-libpal $(GRAPHENE_DIR)/Runtime/libpal-Linux-SGX.so \
		-key $(SGX_SIGNER_KEY) \
		-manifest $< -output $@ \
		-exec cifer_app

cifer_app.sig: cifer_app.manifest.sgx

cifer_app.token: cifer_app.sig
	$(GRAPHENE_DIR)/Pal/src/host/Linux-SGX/signer/pal-sgx-get-token \
		 -output $@ -sig $<

########################### COPIES OF EXECUTABLES #############################

pal_loader:
	@ln -s $(GRAPHENE_DIR)/Runtime/pal_loader $@

############################## RUNNING APP ####################################

.PHONY: run-native
run-native: all
	./cifer_app

.PHONY: run-graphene
run-graphene: all
	./pal_loader ./cifer_app

	.PHONY: run-graphene-sgx
run-graphene-sgx: all
	SGX=1 ./pal_loader ./cifer_app

################################## CLEANUP ####################################

.PHONY: clean
clean:
	$(RM) -r *.token *.sig *.manifest.sgx *.manifest pal_loader .lck \
		*.o cifer_app $(CIFER_LIB_SRC_DIR)/build

.PHONY: distclean
distclean: clean
	$(RM) -r $(CIFER_LIB_SRC_DIR)
