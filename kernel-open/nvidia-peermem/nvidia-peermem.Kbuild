###########################################################################
# Kbuild fragment for nvidia-peermem.ko
###########################################################################

#
# Define NVIDIA_PEERMEM_{SOURCES,OBJECTS}
#

NVIDIA_PEERMEM_SOURCES =
NVIDIA_PEERMEM_SOURCES += nvidia-peermem/nvidia-peermem.c

NVIDIA_PEERMEM_OBJECTS = $(patsubst %.c,%.o,$(NVIDIA_PEERMEM_SOURCES))

obj-m += nvidia-peermem.o
nvidia-peermem-y := $(NVIDIA_PEERMEM_OBJECTS)

NVIDIA_PEERMEM_KO = nvidia-peermem/nvidia-peermem.ko

NV_KERNEL_MODULE_TARGETS += $(NVIDIA_PEERMEM_KO)

#
# Define nvidia-peermem.ko-specific CFLAGS.
#
NVIDIA_PEERMEM_CFLAGS += -I$(src)/nvidia-peermem
NVIDIA_PEERMEM_CFLAGS += -UDEBUG -U_DEBUG -DNDEBUG -DNV_BUILD_MODULE_INSTANCES=0

#
# In case of MOFED installation, nvidia-peermem compilation
# needs paths to the MOFED headers in CFLAGS.
# MOFED's Module.symvers is needed for the build
# to find the additional ib_* symbols.
#
# Also, MOFED doesn't use kbuild ARCH names.
# So adapt OFA_ARCH to match MOFED's conventions.
#
ifeq ($(ARCH), arm64)
    OFA_ARCH := aarch64
else ifeq ($(ARCH), powerpc)
    OFA_ARCH := ppc64le
else
    OFA_ARCH := $(ARCH)
endif
OFA_DIR := /usr/src/ofa_kernel
OFA_CANDIDATES = $(OFA_DIR)/$(OFA_ARCH)/$(KERNELRELEASE) $(OFA_DIR)/$(KERNELRELEASE) $(OFA_DIR)/default /var/lib/dkms/mlnx-ofed-kernel
MLNX_OFED_KERNEL := $(shell for d in $(OFA_CANDIDATES); do \
                              if [ -d "$$d" ]; then \
                                echo "$$d"; \
                                exit 0; \
                              fi; \
                            done; \
                            echo $(OFA_DIR) \
                     )

ifneq ($(shell test -d $(MLNX_OFED_KERNEL) && echo "true" || echo "" ),)
    NVIDIA_PEERMEM_CFLAGS += -I$(MLNX_OFED_KERNEL)/include -I$(MLNX_OFED_KERNEL)/include/rdma
    KBUILD_EXTRA_SYMBOLS := $(MLNX_OFED_KERNEL)/Module.symvers
endif

$(call ASSIGN_PER_OBJ_CFLAGS, $(NVIDIA_PEERMEM_OBJECTS), $(NVIDIA_PEERMEM_CFLAGS))

#
# Register the conftests needed by nvidia-peermem.ko
#

NV_OBJECTS_DEPEND_ON_CONFTEST += $(NVIDIA_PEERMEM_OBJECTS)

NV_CONFTEST_GENERIC_COMPILE_TESTS += ib_peer_memory_symbols

NV_CONFTEST_FUNCTION_COMPILE_TESTS +=

NV_CONFTEST_TYPE_COMPILE_TESTS +=
