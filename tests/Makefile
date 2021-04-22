# Collects all the Makefile_*.mk in this directory and then invokes them using
# recursive make (sorry). This let's us have unit tests for our different C SDK files!

TEST_MAKEFILE_ROOT := makefiles
TEST_MAKEFILE_FILTER ?= *

TEST_MAKEFILES := $(wildcard $(TEST_MAKEFILE_ROOT)/Makefile_$(TEST_MAKEFILE_FILTER))

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(dir $(MKFILE_PATH))

# Convenience defines that can be used for individual tests in makefiles/
export BUILD_DIR=build
export PROJECT_DIR := $(abspath $(CURRENT_DIR)/..)
export TEST_ROOT := $(CURRENT_DIR)
export PROJECT_ROOT_DIR := $(PROJECT_DIR)
export VTC_LIB_DIR := $(PROJECT_DIR)/src

export VTC_TEST_SRC_DIR ?= $(TEST_ROOT)/src
export TEST_COMMON_SRC_DIR := $(TEST_ROOT)/src
export TEST_FAKE_DIR := $(TEST_ROOT)/fakes
export TEST_MOCK_DIR := $(TEST_ROOT)/mocks
export CPPUTEST_MAKFILE_INFRA := $(TEST_ROOT)/MakefileWorkerOverrides.mk

VTC_ALL_INCLUDE_DIRS := \
  $(VTC_LIB_DIR)/include \
  $(PROJECT_DIR)/include \
  $(PROJECT_DIR)/mdk \
  $(PROJECT_DIR)/external/mbedtls/include

VTC_ALL_SRC_DIRS := \
	$(wildcard $(VTC_LIB_DIR)/*) \
	$(VTC_LIB_DIR)

VTC_ALL_INCLUDE_FLAGS := \
  $(foreach COMPONENT_INCLUDE, $(VTC_ALL_INCLUDE_DIRS), -I$(COMPONENT_INCLUDE))

export VERTICES_EXTRA_INC_PATHS += \
  $(VTC_ALL_INCLUDE_FLAGS)

all: $(TEST_MAKEFILES)

compile: CPPUTEST_BUILD_RULE=start
compile: $(TEST_MAKEFILES)

LCOV_INFO_FILE = $(BUILD_DIR)/lcov.info
lcov: $(TEST_MAKEFILES)
	lcov --base-directory . --directory . -c -o $(LCOV_INFO_FILE) --exclude "*cpputest/*" --exclude "*tests/*"
	genhtml -o test_coverage -t "coverage" --num-spaces 4 $(LCOV_INFO_FILE) -o $(BUILD_DIR)/test_coverage/

mbded_library:
	make -C $(PROJECT_ROOT_DIR)/external/mbedtls/library static

$(TEST_MAKEFILES): mbded_library
	$(MAKE) -f $@ $(CPPUTEST_BUILD_RULE)

base64:
	$(MAKE) -f makefiles/Makefile_$@.mk $(CPPUTEST_BUILD_RULE)

base32:
	$(MAKE) -f makefiles/Makefile_$@.mk $(CPPUTEST_BUILD_RULE)

sha512_256: mbded_library
	$(MAKE) -f makefiles/Makefile_$@.mk $(CPPUTEST_BUILD_RULE)

clean:
	rm -rf $(BUILD_DIR)
	make -C $(PROJECT_ROOT_DIR)/external/mbedtls/library clean

.PHONY: all clean $(TEST_MAKEFILES)