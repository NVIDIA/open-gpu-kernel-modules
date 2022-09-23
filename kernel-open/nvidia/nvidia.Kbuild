###########################################################################
# Kbuild fragment for nvidia.ko
###########################################################################

#
# Define NVIDIA_{SOURCES,OBJECTS}
#

include $(src)/nvidia/nvidia-sources.Kbuild
NVIDIA_OBJECTS = $(patsubst %.c,%.o,$(NVIDIA_SOURCES))

obj-m += nvidia.o
nvidia-y := $(NVIDIA_OBJECTS)

NVIDIA_KO = nvidia/nvidia.ko


#
# nv-kernel.o_binary is the core binary component of nvidia.ko, shared
# across all UNIX platforms. Create a symlink, "nv-kernel.o" that
# points to nv-kernel.o_binary, and add nv-kernel.o to the list of
# objects to link into nvidia.ko.
#
# Note that:
# - The kbuild "clean" rule will delete all objects in nvidia-y (which
# is why we use a symlink instead of just adding nv-kernel.o_binary
# to nvidia-y).
# - kbuild normally uses the naming convention of ".o_shipped" for
# binary files. That is not used here, because the kbuild rule to
# create the "normal" object file from ".o_shipped" does a copy, not
# a symlink. This file is quite large, so a symlink is preferred.
# - The file added to nvidia-y should be relative to gmake's cwd.
# But, the target for the symlink rule should be prepended with $(obj).
# - The "symlink" command is called using kbuild's if_changed macro to
# generate an .nv-kernel.o.cmd file which can be used on subsequent
# runs to determine if the command line to create the symlink changed
# and needs to be re-executed.
#

NVIDIA_BINARY_OBJECT := $(src)/nvidia/nv-kernel.o_binary
NVIDIA_BINARY_OBJECT_O := nvidia/nv-kernel.o

quiet_cmd_symlink = SYMLINK $@
 cmd_symlink = ln -sf $< $@

targets += $(NVIDIA_BINARY_OBJECT_O)

$(obj)/$(NVIDIA_BINARY_OBJECT_O): $(NVIDIA_BINARY_OBJECT) FORCE
	$(call if_changed,symlink)

nvidia-y += $(NVIDIA_BINARY_OBJECT_O)


#
# Define nvidia.ko-specific CFLAGS.
#

NVIDIA_CFLAGS += -I$(src)/nvidia
NVIDIA_CFLAGS += -DNVIDIA_UNDEF_LEGACY_BIT_MACROS

ifeq ($(NV_BUILD_TYPE),release)
 NVIDIA_CFLAGS += -UDEBUG -U_DEBUG -DNDEBUG
endif

ifeq ($(NV_BUILD_TYPE),develop)
 NVIDIA_CFLAGS += -UDEBUG -U_DEBUG -DNDEBUG -DNV_MEM_LOGGER
endif

ifeq ($(NV_BUILD_TYPE),debug)
 NVIDIA_CFLAGS += -DDEBUG -D_DEBUG -UNDEBUG -DNV_MEM_LOGGER
endif

$(call ASSIGN_PER_OBJ_CFLAGS, $(NVIDIA_OBJECTS), $(NVIDIA_CFLAGS))


#
# nv-procfs.c requires nv-compiler.h
#

NV_COMPILER_VERSION_HEADER = $(obj)/nv_compiler.h

$(NV_COMPILER_VERSION_HEADER):
	@echo \#define NV_COMPILER \"`$(CC) -v 2>&1 | tail -n 1`\" > $@

$(obj)/nvidia/nv-procfs.o: $(NV_COMPILER_VERSION_HEADER)

clean-files += $(NV_COMPILER_VERSION_HEADER)


#
# Build nv-interface.o from the kernel interface layer objects, suitable
# for further processing by the top-level makefile to produce a precompiled
# kernel interface file.
#

NVIDIA_INTERFACE := nvidia/nv-interface.o

# Linux kernel v5.12 and later looks at "always-y", Linux kernel versions 
# before v5.6 looks at "always"; kernel versions between v5.12 and v5.6
# look at both.

always += $(NVIDIA_INTERFACE)
always-y += $(NVIDIA_INTERFACE)

$(obj)/$(NVIDIA_INTERFACE): $(addprefix $(obj)/,$(NVIDIA_OBJECTS))
	$(LD) -r -o $@ $^


#
# Register the conftests needed by nvidia.ko
#

NV_OBJECTS_DEPEND_ON_CONFTEST += $(NVIDIA_OBJECTS)

NV_CONFTEST_FUNCTION_COMPILE_TESTS += hash__remap_4k_pfn
NV_CONFTEST_FUNCTION_COMPILE_TESTS += set_pages_uc
NV_CONFTEST_FUNCTION_COMPILE_TESTS += list_is_first
NV_CONFTEST_FUNCTION_COMPILE_TESTS += set_memory_uc
NV_CONFTEST_FUNCTION_COMPILE_TESTS += set_memory_array_uc
NV_CONFTEST_FUNCTION_COMPILE_TESTS += set_pages_array_uc
NV_CONFTEST_FUNCTION_COMPILE_TESTS += ioremap_cache
NV_CONFTEST_FUNCTION_COMPILE_TESTS += ioremap_wc
NV_CONFTEST_FUNCTION_COMPILE_TESTS += sg_alloc_table
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pci_get_domain_bus_and_slot
NV_CONFTEST_FUNCTION_COMPILE_TESTS += get_num_physpages
NV_CONFTEST_FUNCTION_COMPILE_TESTS += efi_enabled
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pde_data
NV_CONFTEST_FUNCTION_COMPILE_TESTS += proc_remove
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pm_vt_switch_required
NV_CONFTEST_FUNCTION_COMPILE_TESTS += xen_ioemu_inject_msi
NV_CONFTEST_FUNCTION_COMPILE_TESTS += phys_to_dma
NV_CONFTEST_FUNCTION_COMPILE_TESTS += get_dma_ops
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_attr_macros
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_map_page_attrs
NV_CONFTEST_FUNCTION_COMPILE_TESTS += write_cr4
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_get_property
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_find_node_by_phandle
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_node_to_nid
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pnv_pci_get_npu_dev
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_get_ibm_chip_id
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pci_bus_address
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pci_stop_and_remove_bus_device
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pci_remove_bus_device
NV_CONFTEST_FUNCTION_COMPILE_TESTS += register_cpu_notifier
NV_CONFTEST_FUNCTION_COMPILE_TESTS += cpuhp_setup_state
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_map_resource
NV_CONFTEST_FUNCTION_COMPILE_TESTS += get_backlight_device_by_name
NV_CONFTEST_FUNCTION_COMPILE_TESTS += timer_setup
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pci_enable_msix_range
NV_CONFTEST_FUNCTION_COMPILE_TESTS += kernel_read_has_pointer_pos_arg
NV_CONFTEST_FUNCTION_COMPILE_TESTS += kernel_write
NV_CONFTEST_FUNCTION_COMPILE_TESTS += kthread_create_on_node
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_find_matching_node
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dev_is_pci
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_direct_map_resource
NV_CONFTEST_FUNCTION_COMPILE_TESTS += tegra_get_platform
NV_CONFTEST_FUNCTION_COMPILE_TESTS += tegra_bpmp_send_receive
NV_CONFTEST_FUNCTION_COMPILE_TESTS += flush_cache_all
NV_CONFTEST_FUNCTION_COMPILE_TESTS += vmf_insert_pfn
NV_CONFTEST_FUNCTION_COMPILE_TESTS += jiffies_to_timespec
NV_CONFTEST_FUNCTION_COMPILE_TESTS += ktime_get_raw_ts64
NV_CONFTEST_FUNCTION_COMPILE_TESTS += ktime_get_real_ts64
NV_CONFTEST_FUNCTION_COMPILE_TESTS += full_name_hash
NV_CONFTEST_FUNCTION_COMPILE_TESTS += hlist_for_each_entry
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pci_enable_atomic_ops_to_root
NV_CONFTEST_FUNCTION_COMPILE_TESTS += vga_tryget
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pgprot_decrypted
NV_CONFTEST_FUNCTION_COMPILE_TESTS += cc_mkdec
NV_CONFTEST_FUNCTION_COMPILE_TESTS += iterate_fd
NV_CONFTEST_FUNCTION_COMPILE_TESTS += seq_read_iter
NV_CONFTEST_FUNCTION_COMPILE_TESTS += sg_page_iter_page
NV_CONFTEST_FUNCTION_COMPILE_TESTS += unsafe_follow_pfn
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_gem_object_get
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_gem_object_put_unlocked
NV_CONFTEST_FUNCTION_COMPILE_TESTS += set_close_on_exec
NV_CONFTEST_FUNCTION_COMPILE_TESTS += add_memory_driver_managed
NV_CONFTEST_FUNCTION_COMPILE_TESTS += device_property_read_u64
NV_CONFTEST_FUNCTION_COMPILE_TESTS += devm_of_platform_populate
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_dma_configure
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_property_count_elems_of_size
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_property_read_variable_u8_array
NV_CONFTEST_FUNCTION_COMPILE_TESTS += i2c_new_client_device
NV_CONFTEST_FUNCTION_COMPILE_TESTS += i2c_unregister_device
NV_CONFTEST_FUNCTION_COMPILE_TESTS += of_get_named_gpio
NV_CONFTEST_FUNCTION_COMPILE_TESTS += devm_gpio_request_one
NV_CONFTEST_FUNCTION_COMPILE_TESTS += gpio_direction_input
NV_CONFTEST_FUNCTION_COMPILE_TESTS += gpio_direction_output
NV_CONFTEST_FUNCTION_COMPILE_TESTS += gpio_get_value
NV_CONFTEST_FUNCTION_COMPILE_TESTS += gpio_set_value
NV_CONFTEST_FUNCTION_COMPILE_TESTS += gpio_to_irq
NV_CONFTEST_FUNCTION_COMPILE_TESTS += icc_get
NV_CONFTEST_FUNCTION_COMPILE_TESTS += icc_put
NV_CONFTEST_FUNCTION_COMPILE_TESTS += icc_set_bw
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_buf_export_args
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_buf_ops_has_kmap
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_buf_ops_has_kmap_atomic
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_buf_ops_has_map
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_buf_ops_has_map_atomic
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_buf_has_dynamic_attachment
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_buf_attachment_has_peer2peer
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_set_mask_and_coherent
NV_CONFTEST_FUNCTION_COMPILE_TESTS += get_task_ioprio

NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_gpl_of_node_to_nid
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_gpl_sme_active
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_swiotlb_map_sg_attrs
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_swiotlb_dma_ops
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present___close_fd
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_close_fd
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_get_unused_fd
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_get_unused_fd_flags
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_nvhost_get_default_device
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_nvhost_syncpt_unit_interface_get_byte_offset
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_nvhost_syncpt_unit_interface_get_aperture
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_tegra_dce_register_ipc_client
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_tegra_dce_unregister_ipc_client
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_tegra_dce_client_ipc_send_recv
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_dram_clk_to_mc_clk
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_get_dram_num_channels
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_tegra_dram_types
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_pxm_to_node
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_screen_info

NV_CONFTEST_TYPE_COMPILE_TESTS += file_operations
NV_CONFTEST_TYPE_COMPILE_TESTS += kuid_t
NV_CONFTEST_TYPE_COMPILE_TESTS += dma_ops
NV_CONFTEST_TYPE_COMPILE_TESTS += swiotlb_dma_ops
NV_CONFTEST_TYPE_COMPILE_TESTS += noncoherent_swiotlb_dma_ops
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_fault_has_address
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_insert_pfn_prot
NV_CONFTEST_TYPE_COMPILE_TESTS += vmf_insert_pfn_prot
NV_CONFTEST_TYPE_COMPILE_TESTS += address_space_init_once
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_ops_fault_removed_vma_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += vmbus_channel_has_ringbuffer_page
NV_CONFTEST_TYPE_COMPILE_TESTS += device_driver_of_match_table
NV_CONFTEST_TYPE_COMPILE_TESTS += device_of_node
NV_CONFTEST_TYPE_COMPILE_TESTS += node_states_n_memory
NV_CONFTEST_TYPE_COMPILE_TESTS += kmem_cache_has_kobj_remove_work
NV_CONFTEST_TYPE_COMPILE_TESTS += sysfs_slab_unlink
NV_CONFTEST_TYPE_COMPILE_TESTS += proc_ops
NV_CONFTEST_TYPE_COMPILE_TESTS += timespec64
NV_CONFTEST_TYPE_COMPILE_TESTS += vmalloc_has_pgprot_t_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += mm_has_mmap_lock
NV_CONFTEST_TYPE_COMPILE_TESTS += pci_channel_state
NV_CONFTEST_TYPE_COMPILE_TESTS += pci_dev_has_ats_enabled
NV_CONFTEST_TYPE_COMPILE_TESTS += remove_memory_has_nid_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += add_memory_driver_managed_has_mhp_flags_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += num_registered_fb

NV_CONFTEST_GENERIC_COMPILE_TESTS += dom0_kernel_present
NV_CONFTEST_GENERIC_COMPILE_TESTS += nvidia_vgpu_kvm_build
NV_CONFTEST_GENERIC_COMPILE_TESTS += nvidia_grid_build
NV_CONFTEST_GENERIC_COMPILE_TESTS += nvidia_grid_csp_build
NV_CONFTEST_GENERIC_COMPILE_TESTS += get_user_pages
NV_CONFTEST_GENERIC_COMPILE_TESTS += get_user_pages_remote
NV_CONFTEST_GENERIC_COMPILE_TESTS += pm_runtime_available
NV_CONFTEST_GENERIC_COMPILE_TESTS += vm_fault_t
NV_CONFTEST_GENERIC_COMPILE_TESTS += pci_class_multimedia_hd_audio
NV_CONFTEST_GENERIC_COMPILE_TESTS += drm_available
