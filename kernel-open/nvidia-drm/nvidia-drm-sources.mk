###########################################################################
# Kbuild fragment for nvidia-drm.ko
###########################################################################

#
# Define NVIDIA_DRM_SOURCES
#

NVIDIA_DRM_SOURCES =
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-drv.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-utils.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-crtc.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-encoder.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-connector.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-gem.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-fb.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-modeset.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-fence.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-helper.c
NVIDIA_DRM_SOURCES += nvidia-drm/nv-kthread-q.c
NVIDIA_DRM_SOURCES += nvidia-drm/nv-pci-table.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-gem-nvkms-memory.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-gem-user-memory.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-gem-dma-buf.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-format.c
NVIDIA_DRM_SOURCES += nvidia-drm/nvidia-drm-os-interface.c

#
# Register the conftests needed by nvidia-drm.ko
#

NV_CONFTEST_GENERIC_COMPILE_TESTS += drm_available
NV_CONFTEST_GENERIC_COMPILE_TESTS += drm_atomic_available
NV_CONFTEST_GENERIC_COMPILE_TESTS += is_export_symbol_gpl_refcount_inc
NV_CONFTEST_GENERIC_COMPILE_TESTS += is_export_symbol_gpl_refcount_dec_and_test
NV_CONFTEST_GENERIC_COMPILE_TESTS += drm_alpha_blending_available
NV_CONFTEST_GENERIC_COMPILE_TESTS += is_export_symbol_present_drm_gem_prime_fd_to_handle
NV_CONFTEST_GENERIC_COMPILE_TESTS += is_export_symbol_present_drm_gem_prime_handle_to_fd
NV_CONFTEST_GENERIC_COMPILE_TESTS += is_export_symbol_present_timer_delete_sync
NV_CONFTEST_GENERIC_COMPILE_TESTS += is_export_symbol_gpl___vma_start_write

NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_dev_unref
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_reinit_primary_mode_group
NV_CONFTEST_FUNCTION_COMPILE_TESTS += get_user_pages_remote
NV_CONFTEST_FUNCTION_COMPILE_TESTS += get_user_pages
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pin_user_pages_remote
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pin_user_pages
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_gem_object_lookup
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_atomic_state_ref_counting
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_driver_has_gem_prime_res_obj
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_atomic_helper_connector_dpms
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_connector_funcs_have_mode_in_name
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_connector_has_vrr_capable_property
NV_CONFTEST_FUNCTION_COMPILE_TESTS += vmf_insert_pfn
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_framebuffer_get
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_gem_object_get
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_dev_put
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_format_num_planes
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_connector_for_each_possible_encoder
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_rotation_available
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_vma_offset_exact_lookup_locked
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_gem_object_put_unlocked
NV_CONFTEST_FUNCTION_COMPILE_TESTS += nvhost_dma_fence_unpack
NV_CONFTEST_FUNCTION_COMPILE_TESTS += list_is_first
NV_CONFTEST_FUNCTION_COMPILE_TESTS += timer_setup
NV_CONFTEST_FUNCTION_COMPILE_TESTS += dma_fence_set_error
NV_CONFTEST_FUNCTION_COMPILE_TESTS += fence_set_error
NV_CONFTEST_FUNCTION_COMPILE_TESTS += sync_file_get_fence
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_aperture_remove_conflicting_framebuffers
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_aperture_remove_conflicting_pci_framebuffers
NV_CONFTEST_FUNCTION_COMPILE_TESTS += aperture_remove_conflicting_devices
NV_CONFTEST_FUNCTION_COMPILE_TESTS += aperture_remove_conflicting_pci_devices
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_fbdev_generic_setup
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_fbdev_ttm_setup
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_client_setup
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_connector_attach_hdr_output_metadata_property
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_helper_crtc_enable_color_mgmt
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_crtc_enable_color_mgmt
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_plane_create_color_properties
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_atomic_helper_legacy_gamma_set
NV_CONFTEST_FUNCTION_COMPILE_TESTS += vmf_insert_mixed
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pfn_to_pfn_t
NV_CONFTEST_FUNCTION_COMPILE_TESTS += drm_gem_prime_mmap

NV_CONFTEST_TYPE_COMPILE_TESTS += drm_bus_present
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_bus_has_bus_type
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_bus_has_get_irq
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_bus_has_get_name
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_device_list
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_legacy_dev_list
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_set_busid
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_crtc_state_has_connectors_changed
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_init_function_args
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_helper_mode_fill_fb_struct
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_master_drop_has_from_release_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_unload_has_int_return_type
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_fault_has_address
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_ops_fault_removed_vma_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_atomic_helper_crtc_destroy_state_has_crtc_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_atomic_helper_plane_destroy_state_has_plane_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_mode_object_find_has_file_priv_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += dma_buf_owner
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_connector_list_iter
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_atomic_helper_swap_state_has_stall_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_prime_flag_present
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_fault_t
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_gem_object_has_resv
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_crtc_state_has_async_flip
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_crtc_state_has_pageflip_flags
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_crtc_state_has_vrr_enabled
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_format_modifiers_present
NV_CONFTEST_TYPE_COMPILE_TESTS += mm_has_mmap_lock
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_vma_node_is_allowed_has_tag_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_vma_offset_node_has_readonly
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_display_mode_has_vrefresh
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_master_set_has_int_return_type
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_gem_free_object
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_prime_pages_to_sg_has_drm_device_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_gem_prime_callbacks
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_crtc_atomic_check_has_atomic_state_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_gem_object_vmap_has_map_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_plane_atomic_check_has_atomic_state_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_device_has_pdev
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_crtc_state_has_no_vblank
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_mode_config_has_allow_fb_modifiers
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_has_hdr_output_metadata
NV_CONFTEST_TYPE_COMPILE_TESTS += dma_resv_add_fence
NV_CONFTEST_TYPE_COMPILE_TESTS += dma_resv_reserve_fences
NV_CONFTEST_TYPE_COMPILE_TESTS += reservation_object_reserve_shared_has_num_fences_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_connector_has_override_edid
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_master_has_leases
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_file_get_master
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_modeset_lock_all_end
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_connector_lookup
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_connector_put
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_area_struct_has_const_vm_flags
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_dumb_destroy
NV_CONFTEST_TYPE_COMPILE_TESTS += fence_ops_use_64bit_seqno
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_aperture_remove_conflicting_framebuffers_has_driver_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_aperture_remove_conflicting_framebuffers_has_no_primary_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_aperture_remove_conflicting_pci_framebuffers_has_driver_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_mode_create_dp_colorspace_property_has_supported_colorspaces_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_syncobj_features_present
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_unlocked_ioctl_flag_present
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_framebuffer_obj_present
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_color_ctm_3x4_present
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_color_lut
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_format_info_has_is_yuv
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_property_blob_put
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_gem_prime_mmap
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_output_poll_changed
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_driver_has_date
NV_CONFTEST_TYPE_COMPILE_TESTS += drm_connector_helper_funcs_mode_valid_has_const_mode_arg
