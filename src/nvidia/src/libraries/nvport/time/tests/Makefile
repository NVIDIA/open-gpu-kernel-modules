# Run from the repository root:
# make -C src/nvidia/src/libraries/nvport/time/tests check

TEST_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
REPO_ROOT := $(abspath $(TEST_DIR)/../../../../../../..)
TIME_SOURCE ?= $(REPO_ROOT)/src/nvidia/src/libraries/nvport/time/time_generic.c
OUTPUT_DIR ?= $(REPO_ROOT)/_out/nvport-time-tests
CFLAGS ?= -O2 -g

.PHONY: check
check:
	mkdir -p "$(OUTPUT_DIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c99 -Wall -Wextra -Werror \
	    -D_POSIX_C_SOURCE=200809L \
	    -DPORT_IS_KERNEL_BUILD=0 -DPORT_IS_CHECKED_BUILD=0 \
	    -DPORT_MODULE_time=1 \
	    -I"$(REPO_ROOT)/src/nvidia/inc/libraries" \
	    -I"$(REPO_ROOT)/src/common/sdk/nvidia/inc" \
	    "$(TIME_SOURCE)" "$(TEST_DIR)/test_time.c" \
	    $(LDFLAGS) -o "$(OUTPUT_DIR)/test_time"
	"$(OUTPUT_DIR)/test_time"
