COMPONENT_NAME=sha512_256

SRC_FILES = \
  $(VTC_LIB_DIR)/sha512_256.c

TEST_SRC_FILES = \
  $(VTC_TEST_SRC_DIR)/test_sha512_256.cpp \
  $(TEST_FAKE_DIR)/fake_vertices_errors.c

LD_LIBRARIES += -L$(PROJECT_ROOT_DIR)/external/mbedtls/library -lmbedcrypto

include $(CPPUTEST_MAKFILE_INFRA)