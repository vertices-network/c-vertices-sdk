#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# embed files from the "certs" directory as binary data symbols
# in the app
COMPONENT_EMBED_TXTFILES := algoexplorer_root_cert.pem

# this path is created relative to the component build directory
SIGNATURE_KEYS := $(abspath private_key.bin)
COMPONENT_EXTRA_CLEAN += $(SIGNATURE_KEYS)
COMPONENT_EMBED_FILES := $(SIGNATURE_KEYS)