COMPONENT_NAME=base64

SRC_FILES = \
  $(VTC_LIB_DIR)/base64.c

TEST_SRC_FILES = \
  $(VTC_TEST_SRC_DIR)/test_base64.cpp \
  $(TEST_FAKE_DIR)/fake_vertices_errors.c

include $(CPPUTEST_MAKFILE_INFRA)