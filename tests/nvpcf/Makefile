ROOT := ../..
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror -O0 -g --coverage -fprofile-abs-path \
	-DNVPCF_HOST_TEST -I$(ROOT)/tests/nvpcf
BUILD := $(ROOT)/tests/nvpcf/build

.PHONY: test clean

test: $(BUILD)/os_ut $(BUILD)/acpi_ut $(BUILD)/nv_ut $(BUILD)/acpi_stub_ut
	$(BUILD)/os_ut
	$(BUILD)/acpi_ut
	$(BUILD)/nv_ut
	$(BUILD)/acpi_stub_ut
	python3 $(ROOT)/tests/nvpcf/coverage.py

$(BUILD):
	mkdir -p $(BUILD)/os $(BUILD)/acpi $(BUILD)/nv $(BUILD)/stub

$(BUILD)/os_ut: os_ut.c $(ROOT)/src/nvidia/arch/nvalloc/unix/src/osapi.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(ROOT)/src/nvidia/arch/nvalloc/unix/src -o $(BUILD)/os/os_ut os_ut.c
	cp $(BUILD)/os/os_ut $@

$(BUILD)/acpi_ut: acpi_ut.c $(ROOT)/kernel-open/nvidia/nv-acpi.c | $(BUILD)
	$(CC) $(CFLAGS) -DNVPCF_HOST_ACPI -I$(ROOT)/kernel-open/nvidia -o $(BUILD)/acpi/acpi_ut acpi_ut.c
	cp $(BUILD)/acpi/acpi_ut $@

$(BUILD)/nv_ut: nv_ut.c $(ROOT)/kernel-open/nvidia/nv.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(ROOT)/kernel-open/nvidia -o $(BUILD)/nv/nv_ut nv_ut.c
	cp $(BUILD)/nv/nv_ut $@

$(BUILD)/acpi_stub_ut: acpi_stub_ut.c $(ROOT)/kernel-open/nvidia/nv-acpi.c | $(BUILD)
	$(CC) $(CFLAGS) -Wno-unused-function -Wno-unused-variable \
		-I$(ROOT)/kernel-open/nvidia -o $(BUILD)/stub/acpi_stub_ut acpi_stub_ut.c
	cp $(BUILD)/stub/acpi_stub_ut $@

clean:
	rm -rf $(BUILD)
