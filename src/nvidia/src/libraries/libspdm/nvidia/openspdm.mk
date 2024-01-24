# SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

#
# Description: This makefile allows for the compilation of the libspdm
# open-source library, in order to implement an SPDM Requester in RM.
#

LIBSPDM_SOURCES  :=
LIBSPDM_INCLUDES :=
LIBSPDM_DEFINES  :=
NV_USE_LIBSPDM   := 0

# Set some common path defines for readability.
LIBSPDM_SOURCE_DIR := src/libraries/libspdm
LIBSPDM_VERSION    := 3.1.1

# List all files which depend on libspdm headers under LIBSPDM_SOURCES.

# NVIDIA-authored module code which depends on libspdm headers.
LIBSPDM_SOURCES += src/kernel/gpu/spdm/spdm.c
LIBSPDM_SOURCES += src/kernel/gpu/spdm/arch/hopper/spdm_gh100.c
LIBSPDM_SOURCES += src/kernel/gpu/spdm/arch/hopper/spdm_certs_gh100.c
LIBSPDM_SOURCES += src/kernel/gpu/conf_compute/ccsl.c
LIBSPDM_SOURCES += src/kernel/gpu/conf_compute/arch/hopper/conf_compute_keystore_gh100.c

# NVIDIA-authored platform code
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/nvidia/nvspdm_crypt_null.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/nvidia/nvspdm_debuglib.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/nvidia/nvspdm_malloclib.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/nvidia/nvspdm_rng.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/nvidia/nvspdm_memlib.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/nvidia/nvspdm_rsa.c

# libspdm source files
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_common_lib/libspdm_com_context_data.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_common_lib/libspdm_com_context_data_session.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_common_lib/libspdm_com_crypto_service.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_common_lib/libspdm_com_crypto_service_session.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_common_lib/libspdm_com_msg_log.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_common_lib/libspdm_com_opaque_data.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_common_lib/libspdm_com_support.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_aead.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_asym.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_cert.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_dhe.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_hash.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_hkdf.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_hmac.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_crypt_lib/libspdm_crypt_rng.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_common.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_communication.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_end_session.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_finish.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_get_capabilities.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_get_certificate.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_get_digests.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_get_measurements.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_get_version.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_handle_error_response.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_key_exchange.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_encap_certificate.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_encap_challenge_auth.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_encap_digests.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_encap_error.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_encap_key_update.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_encap_request.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_heartbeat.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_key_update.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_negotiate_algorithms.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_requester_lib/libspdm_req_send_receive.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_secured_message_lib/libspdm_secmes_context_data.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_secured_message_lib/libspdm_secmes_encode_decode.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_secured_message_lib/libspdm_secmes_key_exchange.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/library/spdm_secured_message_lib/libspdm_secmes_session.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/os_stub/memlib/compare_mem.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/os_stub/spdm_device_secret_lib_null/lib.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/os_stub/platform_lib_null/watchdog.c
LIBSPDM_SOURCES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/os_stub/platform_lib_null/time_linux.c

# Add all libspdm include paths required for the above LIBSPDM_SOURCES to LIBSPDM_INCLUDES.
LIBSPDM_INCLUDES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/include
LIBSPDM_INCLUDES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/include/hal
LIBSPDM_INCLUDES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/os_stub/include
LIBSPDM_INCLUDES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/os_stub
LIBSPDM_INCLUDES += $(LIBSPDM_SOURCE_DIR)/$(LIBSPDM_VERSION)/os_stub/cryptlib_null
LIBSPDM_INCLUDES += $(LIBSPDM_SOURCE_DIR)/nvidia

# Override the default libspdm configuration with our own.
LIBSPDM_DEFINES  += LIBSPDM_CONFIG=\"nvspdm_rmconfig.h\"
