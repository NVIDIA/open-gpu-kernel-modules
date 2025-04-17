/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* ------------------------ Includes --------------------------------------- */

#include "common_nvswitch.h"
#include "haldef_nvswitch.h"
#include "ls10/ls10.h"
#include "ls10/soe_ls10.h"

#include "nvswitch/ls10/dev_nvlsaw_ip.h"
#include "nvswitch/ls10/dev_nvlsaw_ip_addendum.h"
#include "nvswitch/ls10/dev_ctrl_ip.h"
#include "nvswitch/ls10/dev_ctrl_ip_addendum.h"
#include "nvswitch/ls10/dev_cpr_ip.h"
#include "nvswitch/ls10/dev_npg_ip.h"
#include "nvswitch/ls10/dev_fsp_pri.h"
#include "nvswitch/ls10/dev_soe_ip.h"
#include "nvswitch/ls10/ptop_discovery_ip.h"
#include "nvswitch/ls10/dev_minion_ip.h"

#include <stddef.h>

/* ------------------------ Macros ----------------------------------------- */
#define TNVL_MAX_CERT_CHAIN_SIZE         (0x1000)
#define TNVL_MAX_ENCODED_CERT_CHAIN_SIZE (0x1400)

#define DER_LONG_FORM_LENGTH_FIELD_BIT   (0x80)
#define DER_CERT_SIZE_FIELD_LENGTH       (0x4)

#define TNVL_PEM_BEGIN_CERTIFICATE "-----BEGIN CERTIFICATE-----\n"
#define TNVL_PEM_END_CERTIFICATE   "-----END CERTIFICATE-----\n"

#define TNVL_MAX_MEASUREMENT_SIZE        (0x2000)

#define TNVL_L1_CERTIFICATE_PEM "-----BEGIN CERTIFICATE-----\n"\
                                "MIICCzCCAZCgAwIBAgIQLTZwscoQBBHB/sDoKgZbVDAKBggqhkjOPQQDAzA1MSIw\n"\
                                "IAYDVQQDDBlOVklESUEgRGV2aWNlIElkZW50aXR5IENBMQ8wDQYDVQQKDAZOVklE\n"\
                                "SUEwIBcNMjExMTA1MDAwMDAwWhgPOTk5OTEyMzEyMzU5NTlaMDUxIjAgBgNVBAMM\n"\
                                "GU5WSURJQSBEZXZpY2UgSWRlbnRpdHkgQ0ExDzANBgNVBAoMBk5WSURJQTB2MBAG\n"\
                                "ByqGSM49AgEGBSuBBAAiA2IABA5MFKM7+KViZljbQSlgfky/RRnEQScW9NDZF8SX\n"\
                                "gAW96r6u/Ve8ZggtcYpPi2BS4VFu6KfEIrhN6FcHG7WP05W+oM+hxj7nyA1r1jkB\n"\
                                "2Ry70YfThX3Ba1zOryOP+MJ9vaNjMGEwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8B\n"\
                                "Af8EBAMCAQYwHQYDVR0OBBYEFFeF/4PyY8xlfWi3Olv0jUrL+0lfMB8GA1UdIwQY\n"\
                                "MBaAFFeF/4PyY8xlfWi3Olv0jUrL+0lfMAoGCCqGSM49BAMDA2kAMGYCMQCPeFM3\n"\
                                "TASsKQVaT+8S0sO9u97PVGCpE9d/I42IT7k3UUOLSR/qvJynVOD1vQKVXf0CMQC+\n"\
                                "EY55WYoDBvs2wPAH1Gw4LbcwUN8QCff8bFmV4ZxjCRr4WXTLFHBKjbfneGSBWwA=\n"\
                                "-----END CERTIFICATE-----\n"

#define TNVL_L2_CERTIFICATE_PEM "-----BEGIN CERTIFICATE-----\n"\
                                "MIICijCCAhCgAwIBAgIQTCVe3jvQAb8/SjtgX8qJijAKBggqhkjOPQQDAzA1MSIw\n"\
                                "IAYDVQQDDBlOVklESUEgRGV2aWNlIElkZW50aXR5IENBMQ8wDQYDVQQKDAZOVklE\n"\
                                "SUEwIBcNMjIwMTEyMDAwMDAwWhgPOTk5OTEyMzEyMzU5NTlaMD0xHjAcBgNVBAMM\n"\
                                "FU5WSURJQSBHSDEwMCBJZGVudGl0eTEbMBkGA1UECgwSTlZJRElBIENvcnBvcmF0\n"\
                                "aW9uMHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE+pg+tDUuILlZILk5wg22YEJ9Oh6c\n"\
                                "yPcsv3IvgRWcV4LeZK1pTCoQDIplZ0E4qsLG3G04pxsbMhxbqkiz9pqlTV2rtuVg\n"\
                                "SmIqnSYkU1jWXsPS9oVLCGE8VRLl1JvqyOxUo4HaMIHXMA8GA1UdEwEB/wQFMAMB\n"\
                                "Af8wDgYDVR0PAQH/BAQDAgEGMDsGA1UdHwQ0MDIwMKAuoCyGKmh0dHA6Ly9jcmwu\n"\
                                "bmRpcy5udmlkaWEuY29tL2NybC9sMS1yb290LmNybDA3BggrBgEFBQcBAQQrMCkw\n"\
                                "JwYIKwYBBQUHMAGGG2h0dHA6Ly9vY3NwLm5kaXMubnZpZGlhLmNvbTAdBgNVHQ4E\n"\
                                "FgQUB0Kg6wOcgGB7oUFhmU2uJffCmx4wHwYDVR0jBBgwFoAUV4X/g/JjzGV9aLc6\n"\
                                "W/SNSsv7SV8wCgYIKoZIzj0EAwMDaAAwZQIxAPIQhnveFxYIrPzBqViT2I34SfS4\n"\
                                "JGWFnk/1UcdmgJmp+7l6rH/C4qxwntYSgeYrlQIwdjQuofHnhd1RL09OBO34566J\n"\
                                "C9bYAosT/86cCojiGjhLnal9hJOH0nS/lrbaoc5a\n"\
                                "-----END CERTIFICATE-----\n"

#define TNVL_L3_CERTIFICATE_PEM "-----BEGIN CERTIFICATE-----\n"\
                                "MIICqTCCAi+gAwIBAgIQcidIXMg4KYZ1y7ooFz5gUjAKBggqhkjOPQQDAzA9MR4w\n"\
                                "HAYDVQQDDBVOVklESUEgR0gxMDAgSWRlbnRpdHkxGzAZBgNVBAoMEk5WSURJQSBD\n"\
                                "b3Jwb3JhdGlvbjAgFw0yMjAzMDEwMDAwMDBaGA85OTk5MTIzMTIzNTk1OVowUzEn\n"\
                                "MCUGA1UEAwweTlZJRElBIExTXzEwIFByb3Zpc2lvbmVyIElDQSAxMRswGQYDVQQK\n"\
                                "DBJOVklESUEgQ29ycG9yYXRpb24xCzAJBgNVBAYTAlVTMHYwEAYHKoZIzj0CAQYF\n"\
                                "K4EEACIDYgAEyGbP8B2aF0Zd0V5GhWfcnC8K8BXUJMGPhAWQo88WymU0Az+u2Y7B\n"\
                                "zI9li0TyXhth18dB1yqodYgH3KKU1c0beV1OkAUvnlx3JpNPhx8nfdlhpM9jqsXG\n"\
                                "JXeJixW5+VOlo4HbMIHYMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgEG\n"\
                                "MDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwubmRpcy5udmlkaWEuY29tL2Ny\n"\
                                "bC9sMi1naDEwMC5jcmwwNwYIKwYBBQUHAQEEKzApMCcGCCsGAQUFBzABhhtodHRw\n"\
                                "Oi8vb2NzcC5uZGlzLm52aWRpYS5jb20wHQYDVR0OBBYEFBFA9xSZ0ALwvOeei4fR\n"\
                                "von435VEMB8GA1UdIwQYMBaAFAdCoOsDnIBge6FBYZlNriX3wpseMAoGCCqGSM49\n"\
                                "BAMDA2gAMGUCMQDWLHcBKxi9QVrfMoDcIg3gLBRe5oEo/Q4KR3WaUMz9ABxMHK9Y\n"\
                                "K4xPtjXW4Bup5FwCMBhLpTQqsly8gQ6w1CIyMEC4n/LSjM65TC4pGVokSyjpoyp0\n"\
                                "gWjuEBq1vBNs76Ge8A==\n"\
                                "-----END CERTIFICATE-----\n"

const static NvU8 TNVL_L1_CERTIFICATE_DER[527] =
{
    0x30, 0x82, 0x02, 0x0b, 0x30, 0x82, 0x01, 0x90, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x10, 0x2d,
    0x36, 0x70, 0xb1, 0xca, 0x10, 0x04, 0x11, 0xc1, 0xfe, 0xc0, 0xe8, 0x2a, 0x06, 0x5b, 0x54, 0x30,
    0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30, 0x35, 0x31, 0x22, 0x30,
    0x20, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x19, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x44,
    0x65, 0x76, 0x69, 0x63, 0x65, 0x20, 0x49, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x20, 0x43,
    0x41, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x06, 0x4e, 0x56, 0x49, 0x44,
    0x49, 0x41, 0x30, 0x20, 0x17, 0x0d, 0x32, 0x31, 0x31, 0x31, 0x30, 0x35, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x5a, 0x18, 0x0f, 0x39, 0x39, 0x39, 0x39, 0x31, 0x32, 0x33, 0x31, 0x32, 0x33, 0x35,
    0x39, 0x35, 0x39, 0x5a, 0x30, 0x35, 0x31, 0x22, 0x30, 0x20, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c,
    0x19, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x20, 0x49,
    0x64, 0x65, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x20, 0x43, 0x41, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03,
    0x55, 0x04, 0x0a, 0x0c, 0x06, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x30, 0x76, 0x30, 0x10, 0x06,
    0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0x03,
    0x62, 0x00, 0x04, 0x0e, 0x4c, 0x14, 0xa3, 0x3b, 0xf8, 0xa5, 0x62, 0x66, 0x58, 0xdb, 0x41, 0x29,
    0x60, 0x7e, 0x4c, 0xbf, 0x45, 0x19, 0xc4, 0x41, 0x27, 0x16, 0xf4, 0xd0, 0xd9, 0x17, 0xc4, 0x97,
    0x80, 0x05, 0xbd, 0xea, 0xbe, 0xae, 0xfd, 0x57, 0xbc, 0x66, 0x08, 0x2d, 0x71, 0x8a, 0x4f, 0x8b,
    0x60, 0x52, 0xe1, 0x51, 0x6e, 0xe8, 0xa7, 0xc4, 0x22, 0xb8, 0x4d, 0xe8, 0x57, 0x07, 0x1b, 0xb5,
    0x8f, 0xd3, 0x95, 0xbe, 0xa0, 0xcf, 0xa1, 0xc6, 0x3e, 0xe7, 0xc8, 0x0d, 0x6b, 0xd6, 0x39, 0x01,
    0xd9, 0x1c, 0xbb, 0xd1, 0x87, 0xd3, 0x85, 0x7d, 0xc1, 0x6b, 0x5c, 0xce, 0xaf, 0x23, 0x8f, 0xf8,
    0xc2, 0x7d, 0xbd, 0xa3, 0x63, 0x30, 0x61, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01,
    0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01,
    0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04,
    0x16, 0x04, 0x14, 0x57, 0x85, 0xff, 0x83, 0xf2, 0x63, 0xcc, 0x65, 0x7d, 0x68, 0xb7, 0x3a, 0x5b,
    0xf4, 0x8d, 0x4a, 0xcb, 0xfb, 0x49, 0x5f, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18,
    0x30, 0x16, 0x80, 0x14, 0x57, 0x85, 0xff, 0x83, 0xf2, 0x63, 0xcc, 0x65, 0x7d, 0x68, 0xb7, 0x3a,
    0x5b, 0xf4, 0x8d, 0x4a, 0xcb, 0xfb, 0x49, 0x5f, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
    0x3d, 0x04, 0x03, 0x03, 0x03, 0x69, 0x00, 0x30, 0x66, 0x02, 0x31, 0x00, 0x8f, 0x78, 0x53, 0x37,
    0x4c, 0x04, 0xac, 0x29, 0x05, 0x5a, 0x4f, 0xef, 0x12, 0xd2, 0xc3, 0xbd, 0xbb, 0xde, 0xcf, 0x54,
    0x60, 0xa9, 0x13, 0xd7, 0x7f, 0x23, 0x8d, 0x88, 0x4f, 0xb9, 0x37, 0x51, 0x43, 0x8b, 0x49, 0x1f,
    0xea, 0xbc, 0x9c, 0xa7, 0x54, 0xe0, 0xf5, 0xbd, 0x02, 0x95, 0x5d, 0xfd, 0x02, 0x31, 0x00, 0xbe,
    0x11, 0x8e, 0x79, 0x59, 0x8a, 0x03, 0x06, 0xfb, 0x36, 0xc0, 0xf0, 0x07, 0xd4, 0x6c, 0x38, 0x2d,
    0xb7, 0x30, 0x50, 0xdf, 0x10, 0x09, 0xf7, 0xfc, 0x6c, 0x59, 0x95, 0xe1, 0x9c, 0x63, 0x09, 0x1a,
    0xf8, 0x59, 0x74, 0xcb, 0x14, 0x70, 0x4a, 0x8d, 0xb7, 0xe7, 0x78, 0x64, 0x81, 0x5b, 0x00
};

const static NvU8 TNVL_L2_CERTIFICATE_DER[654] =
{
    0x30, 0x82, 0x02, 0x8a, 0x30, 0x82, 0x02, 0x10, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x10, 0x4c,
    0x25, 0x5e, 0xde, 0x3b, 0xd0, 0x01, 0xbf, 0x3f, 0x4a, 0x3b, 0x60, 0x5f, 0xca, 0x89, 0x8a, 0x30,
    0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30, 0x35, 0x31, 0x22, 0x30,
    0x20, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x19, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x44,
    0x65, 0x76, 0x69, 0x63, 0x65, 0x20, 0x49, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x20, 0x43,
    0x41, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x06, 0x4e, 0x56, 0x49, 0x44,
    0x49, 0x41, 0x30, 0x20, 0x17, 0x0d, 0x32, 0x32, 0x30, 0x31, 0x31, 0x32, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x5a, 0x18, 0x0f, 0x39, 0x39, 0x39, 0x39, 0x31, 0x32, 0x33, 0x31, 0x32, 0x33, 0x35,
    0x39, 0x35, 0x39, 0x5a, 0x30, 0x3d, 0x31, 0x1e, 0x30, 0x1c, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c,
    0x15, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x47, 0x48, 0x31, 0x30, 0x30, 0x20, 0x49, 0x64,
    0x65, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x31, 0x1b, 0x30, 0x19, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c,
    0x12, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x43, 0x6f, 0x72, 0x70, 0x6f, 0x72, 0x61, 0x74,
    0x69, 0x6f, 0x6e, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01,
    0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00, 0x04, 0xfa, 0x98, 0x3e, 0xb4, 0x35,
    0x2e, 0x20, 0xb9, 0x59, 0x20, 0xb9, 0x39, 0xc2, 0x0d, 0xb6, 0x60, 0x42, 0x7d, 0x3a, 0x1e, 0x9c,
    0xc8, 0xf7, 0x2c, 0xbf, 0x72, 0x2f, 0x81, 0x15, 0x9c, 0x57, 0x82, 0xde, 0x64, 0xad, 0x69, 0x4c,
    0x2a, 0x10, 0x0c, 0x8a, 0x65, 0x67, 0x41, 0x38, 0xaa, 0xc2, 0xc6, 0xdc, 0x6d, 0x38, 0xa7, 0x1b,
    0x1b, 0x32, 0x1c, 0x5b, 0xaa, 0x48, 0xb3, 0xf6, 0x9a, 0xa5, 0x4d, 0x5d, 0xab, 0xb6, 0xe5, 0x60,
    0x4a, 0x62, 0x2a, 0x9d, 0x26, 0x24, 0x53, 0x58, 0xd6, 0x5e, 0xc3, 0xd2, 0xf6, 0x85, 0x4b, 0x08,
    0x61, 0x3c, 0x55, 0x12, 0xe5, 0xd4, 0x9b, 0xea, 0xc8, 0xec, 0x54, 0xa3, 0x81, 0xda, 0x30, 0x81,
    0xd7, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01,
    0x01, 0xff, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02,
    0x01, 0x06, 0x30, 0x3b, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x34, 0x30, 0x32, 0x30, 0x30, 0xa0,
    0x2e, 0xa0, 0x2c, 0x86, 0x2a, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x63, 0x72, 0x6c, 0x2e,
    0x6e, 0x64, 0x69, 0x73, 0x2e, 0x6e, 0x76, 0x69, 0x64, 0x69, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f,
    0x63, 0x72, 0x6c, 0x2f, 0x6c, 0x31, 0x2d, 0x72, 0x6f, 0x6f, 0x74, 0x2e, 0x63, 0x72, 0x6c, 0x30,
    0x37, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x01, 0x04, 0x2b, 0x30, 0x29, 0x30,
    0x27, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x01, 0x86, 0x1b, 0x68, 0x74, 0x74,
    0x70, 0x3a, 0x2f, 0x2f, 0x6f, 0x63, 0x73, 0x70, 0x2e, 0x6e, 0x64, 0x69, 0x73, 0x2e, 0x6e, 0x76,
    0x69, 0x64, 0x69, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04,
    0x16, 0x04, 0x14, 0x07, 0x42, 0xa0, 0xeb, 0x03, 0x9c, 0x80, 0x60, 0x7b, 0xa1, 0x41, 0x61, 0x99,
    0x4d, 0xae, 0x25, 0xf7, 0xc2, 0x9b, 0x1e, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18,
    0x30, 0x16, 0x80, 0x14, 0x57, 0x85, 0xff, 0x83, 0xf2, 0x63, 0xcc, 0x65, 0x7d, 0x68, 0xb7, 0x3a,
    0x5b, 0xf4, 0x8d, 0x4a, 0xcb, 0xfb, 0x49, 0x5f, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
    0x3d, 0x04, 0x03, 0x03, 0x03, 0x68, 0x00, 0x30, 0x65, 0x02, 0x31, 0x00, 0xf2, 0x10, 0x86, 0x7b,
    0xde, 0x17, 0x16, 0x08, 0xac, 0xfc, 0xc1, 0xa9, 0x58, 0x93, 0xd8, 0x8d, 0xf8, 0x49, 0xf4, 0xb8,
    0x24, 0x65, 0x85, 0x9e, 0x4f, 0xf5, 0x51, 0xc7, 0x66, 0x80, 0x99, 0xa9, 0xfb, 0xb9, 0x7a, 0xac,
    0x7f, 0xc2, 0xe2, 0xac, 0x70, 0x9e, 0xd6, 0x12, 0x81, 0xe6, 0x2b, 0x95, 0x02, 0x30, 0x76, 0x34,
    0x2e, 0xa1, 0xf1, 0xe7, 0x85, 0xdd, 0x51, 0x2f, 0x4f, 0x4e, 0x04, 0xed, 0xf8, 0xe7, 0xae, 0x89,
    0x0b, 0xd6, 0xd8, 0x02, 0x8b, 0x13, 0xff, 0xce, 0x9c, 0x0a, 0x88, 0xe2, 0x1a, 0x38, 0x4b, 0x9d,
    0xa9, 0x7d, 0x84, 0x93, 0x87, 0xd2, 0x74, 0xbf, 0x96, 0xb6, 0xda, 0xa1, 0xce, 0x5a,
};

const static NvU8 TNVL_L3_CERTIFICATE_DER[686] =
{
    0x30, 0x82, 0x02, 0xaa, 0x30, 0x82, 0x02, 0x2f, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x10, 0x6a,
    0xfe, 0x71, 0x84, 0xf9, 0x22, 0x32, 0xc8, 0xdf, 0x7b, 0x24, 0x22, 0x61, 0x77, 0x6e, 0x56, 0x30,
    0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30, 0x3d, 0x31, 0x1e, 0x30,
    0x1c, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x15, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x47,
    0x48, 0x31, 0x30, 0x30, 0x20, 0x49, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x31, 0x1b, 0x30,
    0x19, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x12, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x43,
    0x6f, 0x72, 0x70, 0x6f, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x30, 0x20, 0x17, 0x0d, 0x32, 0x32,
    0x30, 0x33, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x18, 0x0f, 0x39, 0x39, 0x39,
    0x39, 0x31, 0x32, 0x33, 0x31, 0x32, 0x33, 0x35, 0x39, 0x35, 0x39, 0x5a, 0x30, 0x53, 0x31, 0x27,
    0x30, 0x25, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x1e, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20,
    0x47, 0x48, 0x31, 0x30, 0x30, 0x20, 0x50, 0x72, 0x6f, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x65,
    0x72, 0x20, 0x49, 0x43, 0x41, 0x20, 0x31, 0x31, 0x1b, 0x30, 0x19, 0x06, 0x03, 0x55, 0x04, 0x0a,
    0x0c, 0x12, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x43, 0x6f, 0x72, 0x70, 0x6f, 0x72, 0x61,
    0x74, 0x69, 0x6f, 0x6e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55,
    0x53, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x05,
    0x2b, 0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00, 0x04, 0xcd, 0x47, 0x56, 0xaa, 0x39, 0xf5, 0x3a,
    0x55, 0xe1, 0x2d, 0xf1, 0x4e, 0x28, 0x01, 0x53, 0x82, 0x1a, 0x86, 0xf9, 0x0d, 0xf3, 0x17, 0x8c,
    0x60, 0x48, 0x16, 0xd9, 0xb1, 0x41, 0x32, 0x59, 0x80, 0x82, 0xde, 0xb2, 0xa3, 0x13, 0xd3, 0x67,
    0xa3, 0xcf, 0x19, 0xa4, 0x14, 0x31, 0xf3, 0x93, 0xa8, 0xd0, 0xf9, 0x2a, 0x3f, 0x67, 0x70, 0xa0,
    0xc5, 0x4d, 0x4d, 0x03, 0x2f, 0xe5, 0xcb, 0xf2, 0xf6, 0x32, 0xda, 0x4e, 0xf4, 0x93, 0x5d, 0x9c,
    0x0d, 0xd8, 0x74, 0x07, 0x13, 0xdb, 0xbb, 0xc7, 0xb0, 0x48, 0x62, 0xb5, 0xa2, 0xd2, 0xc1, 0xb7,
    0xe6, 0xad, 0x48, 0x24, 0xd4, 0x8e, 0x18, 0xee, 0x4b, 0xa3, 0x81, 0xdb, 0x30, 0x81, 0xd8, 0x30,
    0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff,
    0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06,
    0x30, 0x3c, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x35, 0x30, 0x33, 0x30, 0x31, 0xa0, 0x2f, 0xa0,
    0x2d, 0x86, 0x2b, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x63, 0x72, 0x6c, 0x2e, 0x6e, 0x64,
    0x69, 0x73, 0x2e, 0x6e, 0x76, 0x69, 0x64, 0x69, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x63, 0x72,
    0x6c, 0x2f, 0x6c, 0x32, 0x2d, 0x67, 0x68, 0x31, 0x30, 0x30, 0x2e, 0x63, 0x72, 0x6c, 0x30, 0x37,
    0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x01, 0x04, 0x2b, 0x30, 0x29, 0x30, 0x27,
    0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x01, 0x86, 0x1b, 0x68, 0x74, 0x74, 0x70,
    0x3a, 0x2f, 0x2f, 0x6f, 0x63, 0x73, 0x70, 0x2e, 0x6e, 0x64, 0x69, 0x73, 0x2e, 0x6e, 0x76, 0x69,
    0x64, 0x69, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16,
    0x04, 0x14, 0x29, 0x68, 0xcb, 0x16, 0x2c, 0xd0, 0x77, 0x95, 0x72, 0xa2, 0x79, 0x10, 0x03, 0xe6,
    0x9e, 0xba, 0x0c, 0xcc, 0x0a, 0x94, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30,
    0x16, 0x80, 0x14, 0x07, 0x42, 0xa0, 0xeb, 0x03, 0x9c, 0x80, 0x60, 0x7b, 0xa1, 0x41, 0x61, 0x99,
    0x4d, 0xae, 0x25, 0xf7, 0xc2, 0x9b, 0x1e, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x04, 0x03, 0x03, 0x03, 0x69, 0x00, 0x30, 0x66, 0x02, 0x31, 0x00, 0xca, 0xd0, 0x10, 0xab, 0xe3,
    0xd0, 0xcd, 0x27, 0x8f, 0x18, 0x87, 0x9c, 0x2e, 0xdf, 0xcf, 0x1b, 0x66, 0x27, 0xc3, 0x17, 0x10,
    0x2c, 0x89, 0x44, 0xb8, 0x53, 0x57, 0xc6, 0x9a, 0x92, 0x59, 0x16, 0x10, 0x9b, 0x38, 0x31, 0xfa,
    0x99, 0x1c, 0x1c, 0x33, 0x1a, 0x7a, 0x10, 0xe6, 0xb2, 0x66, 0xe6, 0x02, 0x31, 0x00, 0xd8, 0x29,
    0x3c, 0x8d, 0x73, 0x61, 0x71, 0x58, 0x5b, 0xa1, 0x1e, 0xd7, 0x42, 0x13, 0x79, 0x6c, 0xe1, 0x2e,
    0xd2, 0x22, 0x77, 0xa1, 0x74, 0x4a, 0xd3, 0x64, 0xdb, 0xa1, 0x17, 0x29, 0x9c, 0xe1, 0x83, 0x8c,
    0xe9, 0x59, 0x4b, 0x59, 0x36, 0x3f, 0x08, 0xcf, 0xab, 0x7a, 0xc8, 0xe3, 0xab, 0x14,
};

/* ------------------------ Static Functions ------------------------------- */
/*!
 @param pCert       : The pointer to certification chain start
 @param bufferEnd   : The pointer to certification chain end
 @parsm pCertLength : The pointer to store return certification size

 @return Return NV-OK if no error.

* Static function that calculates the length of the X509 certificate in DER/TLV
* format. It assumes that the certificate is valid.
*/
static NvlStatus
_calc_x509_cert_size_ls10
(
    nvswitch_device *device,
    NvU8 *pCert,
    NvU8 *bufferEnd,
    NvU32 *pCertLength
)
{
    // The cert is in TLV format.
    NvU32 certSize       = pCert[1];

    // Check to make sure that some data exists after TNVL header, and it is enough to check cert size.
    if (pCert + DER_CERT_SIZE_FIELD_LENGTH >= bufferEnd ||
        pCert + DER_CERT_SIZE_FIELD_LENGTH <= pCert)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: pCert + DER_CERT_SIZE_FIELD_LENGTH(0x%x) is not valid value !! \n",
            __FUNCTION__, DER_CERT_SIZE_FIELD_LENGTH);

       return -NVL_NO_MEM;
    }

    // Check if the length is in DER longform.
    // MSB in the length field is set for long form notation.
    // fields.
    if (certSize & DER_LONG_FORM_LENGTH_FIELD_BIT)
    {
        //
        // The remaining bits in the length field indicate the
        // number of following bytes used to represent the length.
        // in base 256, most significant digit first.
        //
        NvU32 numLenBytes = certSize & 0x3f;
        NvU8 *pStart      = &pCert[2];
        NvU8 *pEnd        = pStart + numLenBytes; // NOTE: Don't need to subtract numLenBytes 1 here.

        // Checking for buffer overflow.
        if (pEnd > bufferEnd)
        {
            return -NVL_NO_MEM;
        }

        certSize = *pStart;
        while (++pStart < pEnd)
        {
            certSize = (certSize << 8) + *pStart ;
        }
        // Total cert length includes the Tag + length
        // Adding it here.
        certSize += 2 + numLenBytes;
    }

    //
    // Check to make sure we have not hit end of buffer, and there is space for AK cert.
    // Check for underflow as well. This makes sure we haven't missed the calculation to
    // go past the end of the buffer
    //
    if (pCert + (certSize - 1) > bufferEnd ||
        pCert + (certSize - 1) <= pCert)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: pCert + (certSize(0x%x) - 1) is not a valid value !! \n",
            __FUNCTION__, certSize);

        return -NVL_NO_MEM;
    }

    *pCertLength = certSize;
    return NVL_SUCCESS;
}

static NvlStatus
_pem_write_buffer_ls10
(
    nvswitch_device *device,
    NvU8 const *der,
    NvU64       derLen,
    NvU8       *buffer,
    NvU64       bufferLen,
    NvU64      *bufferUsed
)
{
    static const NvU8 base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    NvU64 i, tmp, size;
    NvU64 printed = 0;
    NvU8 *ptr = buffer;

    // Base64 encoded size
    size = (derLen + 2) / 3 * 4;

    // Add 1 byte per 64 for newline
    size = size + (size + 63) / 64;

    // Add header excluding the terminating null and footer including the null
    size += sizeof(TNVL_PEM_BEGIN_CERTIFICATE) - 1 +
            sizeof(TNVL_PEM_END_CERTIFICATE);

    if (bufferLen < size)
    {
        return -NVL_NO_MEM;
    }

    nvswitch_os_memcpy(ptr, TNVL_PEM_BEGIN_CERTIFICATE, sizeof(TNVL_PEM_BEGIN_CERTIFICATE) - 1);
    ptr += sizeof(TNVL_PEM_BEGIN_CERTIFICATE) - 1;

    for (i = 0; (i + 2) < derLen; i += 3)
    {
        tmp = (der[i] << 16) | (der[i + 1] << 8) | (der[i + 2]);
        *ptr++ = base64[(tmp >> 18) & 63];
        *ptr++ = base64[(tmp >> 12) & 63];
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[(tmp >> 0) & 63];

        printed += 4;
        if (printed == 64)
        {
            *ptr++ = '\n';
            printed = 0;
        }
    }

    if ((i == derLen) && (printed != 0))
    {
        *ptr++ = '\n';
    }

    // 1 byte extra
    if (i == (derLen - 1))
    {
        tmp = der[i] << 4;
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[(tmp >> 0) & 63];
        *ptr++ = '=';
        *ptr++ = '=';
        *ptr++ = '\n';
    }

    // 2 byte extra
    if (i == (derLen - 2))
    {
        tmp = ((der[i] << 8) | (der[i + 1])) << 2;
        *ptr++ = base64[(tmp >> 12) & 63];
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[(tmp >> 0) & 63];
        *ptr++ = '=';
        *ptr++ = '\n';
    }

     nvswitch_os_memcpy(ptr, TNVL_PEM_END_CERTIFICATE, sizeof(TNVL_PEM_END_CERTIFICATE));
     ptr += sizeof(TNVL_PEM_END_CERTIFICATE);

    *bufferUsed = size;
    return NVL_SUCCESS;
}

/*!
* Static function builds the cert chain in DER format. It is assumed that
* the all the certificates are valid. Also it is assumed that there is a valid
* tnvl session already established.
*/
static NvlStatus
_tnvl_build_cert_chain_der_ls10
(
    nvswitch_device *device,
    NvU8   *pFirstCert,
    NvU32   firstCertSize,
    NvU8   *pSecondCert,
    NvU32   secondCertSize,
    NvU8   *pOutBuffer,
    size_t *outBufferSize
)
{
    NvU64      remainingOutBufferSize = 0;
    NvU64      totalSize              = 0;
    void      *pPortMemCopyStatus     = NULL;

    if (pFirstCert == NULL || pSecondCert == NULL || pOutBuffer == NULL || outBufferSize == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    // Calculate the total size of the certificate chain (in DER).
    totalSize = sizeof(TNVL_L1_CERTIFICATE_DER) +
                sizeof(TNVL_L2_CERTIFICATE_DER) +
                sizeof(TNVL_L3_CERTIFICATE_DER) +
                secondCertSize                  +
                firstCertSize;

    remainingOutBufferSize = *outBufferSize;
    if (remainingOutBufferSize < totalSize)
    {
        return -NVL_NO_MEM;
    }

    //
    // Write the L1 DER certificate to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     TNVL_L1_CERTIFICATE_DER,
                                     sizeof(TNVL_L1_CERTIFICATE_DER));
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }

    remainingOutBufferSize -= sizeof(TNVL_L1_CERTIFICATE_DER);
    pOutBuffer             += sizeof(TNVL_L1_CERTIFICATE_DER);

    //
    // Write the L2 DER certificate to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     TNVL_L2_CERTIFICATE_DER,
                                     sizeof(TNVL_L2_CERTIFICATE_DER));
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }

    remainingOutBufferSize -= sizeof(TNVL_L2_CERTIFICATE_DER);
    pOutBuffer             += sizeof(TNVL_L2_CERTIFICATE_DER);

    //
    // Write the L3 DER certificate to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     TNVL_L3_CERTIFICATE_DER,
                                     sizeof(TNVL_L3_CERTIFICATE_DER));
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }

    remainingOutBufferSize -= sizeof(TNVL_L3_CERTIFICATE_DER);
    pOutBuffer             += sizeof(TNVL_L3_CERTIFICATE_DER);

    //
    // Write the IK certificate in DER to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     pSecondCert,
                                     secondCertSize);
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }

    remainingOutBufferSize -= secondCertSize;
    pOutBuffer             += secondCertSize;

    //
    // Write the AK certificate in DER to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     pFirstCert,
                                     firstCertSize);
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }

    remainingOutBufferSize -= firstCertSize;
    pOutBuffer             += firstCertSize;

    // Output the total certificate chain size
    *outBufferSize = totalSize;

    return NVL_SUCCESS;
}

/*!
* Static function that first converts the IK and AK certificates from DER to
* PEM format. Then it builds the cert chain in PEM format. It is assumed that
* the all the certificates are valid. Also it is assumed that there is a valid
* tnvl session already established.
*/
static NvlStatus
_tnvl_build_cert_chain_pem_ls10
(
    nvswitch_device *device,
    NvU8   *pFirstCert,
    NvU32   firstCertSize,
    NvU8   *pSecondCert,
    NvU32   secondCertSize,
    NvU8   *pOutBuffer,
    size_t *outBufferSize
)
{
    NvU64              firstCertOutputSize      = 0;
    NvU64              secondCertOutputSize     = 0;
    NvU64              remainingOutBufferSize   = 0;
    void              *pPortMemCopyStatus       = NULL;
    NvlStatus          status;

    if (pFirstCert == NULL || pSecondCert == NULL || pOutBuffer == NULL || outBufferSize == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    remainingOutBufferSize = *outBufferSize;

    //
    // Write the AK certificate to the output buffer
    //
    status = _pem_write_buffer_ls10(device, pFirstCert, firstCertSize, pOutBuffer,
                              remainingOutBufferSize, &firstCertOutputSize);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    //
    // Keep track how much space we have left in the output buffer
    // and where the next certificate should start.
    // Clear the last byte (NULL).
    //
    remainingOutBufferSize -= firstCertOutputSize - 1;
    pOutBuffer             += firstCertOutputSize - 1;

    //
    // Write the IK certificate to the output buffer
    //
    status = _pem_write_buffer_ls10(device, pSecondCert, secondCertSize, pOutBuffer,
                              remainingOutBufferSize, &secondCertOutputSize);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    remainingOutBufferSize -= secondCertOutputSize - 1;
    pOutBuffer             += secondCertOutputSize - 1;

    // Checking if the available size of buffer is enough to keep the whole
    // certificate chain otherwise raise error.
    if (remainingOutBufferSize < sizeof(TNVL_L1_CERTIFICATE_PEM)
                               + sizeof(TNVL_L2_CERTIFICATE_PEM)
                               + sizeof(TNVL_L3_CERTIFICATE_PEM))
    {
        return -NVL_NO_MEM;
    }

    //
    // Write the L3 certificate to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     TNVL_L3_CERTIFICATE_PEM,
                                     sizeof(TNVL_L3_CERTIFICATE_PEM) - 1);
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }

    remainingOutBufferSize -= sizeof(TNVL_L3_CERTIFICATE_PEM) - 1;
    pOutBuffer             += sizeof(TNVL_L3_CERTIFICATE_PEM) - 1;

    //
    // Write the L2 certificate to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     TNVL_L2_CERTIFICATE_PEM,
                                     sizeof(TNVL_L2_CERTIFICATE_PEM) - 1);
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }
    remainingOutBufferSize -= sizeof(TNVL_L2_CERTIFICATE_PEM) - 1;
    pOutBuffer             += sizeof(TNVL_L2_CERTIFICATE_PEM) - 1;

    //
    // Write the L1 certificate to the output buffer
    //
    pPortMemCopyStatus = nvswitch_os_memcpy(pOutBuffer,
                                     TNVL_L1_CERTIFICATE_PEM,
                                     sizeof(TNVL_L1_CERTIFICATE_PEM) - 1);
    if (pPortMemCopyStatus == NULL)
    {
        return -NVL_NO_MEM;
    }

    //
    // Output the total certificate chain size
    // Do not count the NULL bytes.
    //
    *outBufferSize = firstCertOutputSize - 1 +
                     secondCertOutputSize - 1 +
                     sizeof(TNVL_L3_CERTIFICATE_PEM) - 1 +
                     sizeof(TNVL_L2_CERTIFICATE_PEM) - 1 +
                     sizeof(TNVL_L1_CERTIFICATE_PEM) - 1;

    return NVL_SUCCESS;
}

/* ------------------------ Public Functions ------------------------------- */

static NvlStatus
_nvswitch_tnvl_get_cert_chain_from_fsp_ls10
(
    nvswitch_device *device,
    NvU8 *pCertChain,
    size_t *pCertChainLength
)
{
    NvlStatus status;
    TNVL_GET_ATT_CERTS_CMD_PAYLOAD *pCmdPayload   = nvswitch_os_malloc(sizeof(TNVL_GET_ATT_CERTS_CMD_PAYLOAD));
    TNVL_GET_ATT_CERTS_RSP_PAYLOAD *pRspPayload   = nvswitch_os_malloc(sizeof(TNVL_GET_ATT_CERTS_RSP_PAYLOAD));
    NVSWITCH_TIMEOUT timeout;

    if (pCmdPayload == NULL || pRspPayload == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to allocate memory\n",
            __FUNCTION__);
        status = -NVL_NO_MEM;
        goto ErrorExit;
    }

    pCmdPayload->subMessageId = TNVL_GET_ATT_CERTS_SUBMESSAGE_ID;
    pCmdPayload->minorVersion = 0;
    pCmdPayload->majorVersion = 1;

    nvswitch_timeout_create(5 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    status = nvswitch_fsp_send_and_read_message(device,
        (NvU8*) pCmdPayload, sizeof(TNVL_GET_ATT_CERTS_CMD_PAYLOAD), NVDM_TYPE_TNVL,
        (NvU8*) pRspPayload, sizeof(TNVL_GET_ATT_CERTS_RSP_PAYLOAD), &timeout);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: RPC failed for FSP cert query\n",
            __FUNCTION__);
        status = -NVL_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    // Validate response
    if (pRspPayload->subMessageId != TNVL_GET_ATT_CERTS_SUBMESSAGE_ID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unknown submessage Id %d, Expected %d\n",
            __FUNCTION__,
            pRspPayload->subMessageId, TNVL_GET_ATT_CERTS_SUBMESSAGE_ID);
        status = -NVL_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    nvswitch_os_memcpy(pCertChain, pRspPayload->certChain, pRspPayload->certChainLength);
    *pCertChainLength = pRspPayload->certChainLength;

ErrorExit:
    nvswitch_os_free(pCmdPayload);
    nvswitch_os_free(pRspPayload);

    return status;
}

/*!
 * This function retrieves nvswitch certificates.
 *
 * @param  device    nvswitch device pointer
 * @param  params   certificate params
 *
 * @return Returns NVL_SUCCESS on success. Anything else should be considered a failure.
 */
NvlStatus
nvswitch_tnvl_get_attestation_certificate_chain_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS *params
)
{
    NvlStatus          status = NVL_SUCCESS;
    NvU8              *pIkCertificate           = NULL;
    NvU32              ikCertificateSize        = 0;
    NvU8              *pAkCertificate           = NULL;
    NvU32              akCertificateSize        = 0;
    NvU8              *pCertChain               = NULL;
    size_t             certChainLength          = 0;
    NvU8              *pDerCertChain            = NULL;
    size_t             derCertChainSize         = 0;
    NvU8              *pCertChainBufferEnd      = NULL;
    NvU8              *pAttestationCertChain    = NULL;
    size_t             attestationCertChainSize = 0;
    NvU32              totalSize                = 0;

    if (params == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Bad args!\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        NVSWITCH_PRINT(device, ERROR,
           "%s: TNVL is not enabled\n",
           __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    // Allocate buffer for certificates.
    certChainLength                 = TNVL_MAX_CERT_CHAIN_SIZE;
    pCertChain                      = nvswitch_os_malloc(certChainLength);
    derCertChainSize                = TNVL_MAX_ENCODED_CERT_CHAIN_SIZE;
    pDerCertChain                   = nvswitch_os_malloc(derCertChainSize);
    attestationCertChainSize        = TNVL_MAX_ENCODED_CERT_CHAIN_SIZE;
    pAttestationCertChain           = nvswitch_os_malloc(attestationCertChainSize);

    if (pCertChain == NULL || pDerCertChain == NULL || pAttestationCertChain == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to allocate memory\n",
            __FUNCTION__);
        status = -NVL_NO_MEM;
        goto ErrorExit;
    }

    nvswitch_os_memset(pCertChain, 0, certChainLength);
    nvswitch_os_memset(pDerCertChain, 0, derCertChainSize);
    nvswitch_os_memset(pAttestationCertChain, 0, attestationCertChainSize);

    // Fetch Attestation cert chain from FSP
    status = _nvswitch_tnvl_get_cert_chain_from_fsp_ls10(device, pCertChain, &certChainLength);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get nvswitch certs from FSP\n",
            __FUNCTION__);
        goto ErrorExit;
    }

    certChainLength = certChainLength -
                      NVSWITCH_IK_HASH_LENGTH -
                      NVSWITCH_ATT_CERT_SIZE_FIELD_LENGTH -
                      NVSWITCH_ATT_RSVD1_FIELD_LENGTH;
    //
    // pCertChainBufferEnd represents last valid byte for cert buffer.
    //
    pCertChainBufferEnd = pCertChain + certChainLength - 1;
    pIkCertificate      = (NvU8 *)pCertChain;

    // Calculate the size of the IK cert, and skip past it to get the AK cert.
    status = _calc_x509_cert_size_ls10(device, pIkCertificate, pCertChainBufferEnd, &ikCertificateSize);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to calculate IK size\n",
            __FUNCTION__);
        goto ErrorExit;
    }

    pAkCertificate = (NvU8 *)pIkCertificate + ikCertificateSize;

    // Calculate the size of the AK certificate.
    status = _calc_x509_cert_size_ls10(device, pAkCertificate, pCertChainBufferEnd, &akCertificateSize);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to calculate AK size\n",
            __FUNCTION__);
        goto ErrorExit;
    }

    // Make sure we have calculated the size correctly.
    if ((pAkCertificate + akCertificateSize - 1) != pCertChainBufferEnd)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error in calculating AK size\n",
            __FUNCTION__);
        status = -NVL_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    // Retrieve the entire certificate chain in DER format in order to validate it.
    status = _tnvl_build_cert_chain_der_ls10(device,
                                    pAkCertificate, akCertificateSize,
                                    pIkCertificate, ikCertificateSize,
                                    pDerCertChain,
                                    &derCertChainSize);
    if (status != NVL_SUCCESS)
    {
        goto ErrorExit;
    }

    totalSize = sizeof(TNVL_L1_CERTIFICATE_DER)  +
                sizeof(TNVL_L2_CERTIFICATE_DER)  +
                sizeof(TNVL_L3_CERTIFICATE_DER)  +
                akCertificateSize                +
                ikCertificateSize;

    if (derCertChainSize != totalSize)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: derCertChainSize(%llu) != totalSize(0x%x) !! \n",
            __FUNCTION__, ((NvU64)derCertChainSize), totalSize);
        status = -NVL_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    //
    // Now that the cert chain is valid, retrieve the cert chain in PEM format,
    // as the Verifier can only parse PEM format.
    //
    status = _tnvl_build_cert_chain_pem_ls10(device,
                                    pAkCertificate, akCertificateSize,
                                    pIkCertificate, ikCertificateSize,
                                    pAttestationCertChain,
                                    &attestationCertChainSize);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to build PEM cert chain\n",
            __FUNCTION__);
        goto ErrorExit;
    }

    nvswitch_os_memcpy(params->attestationCertChain, pAttestationCertChain, attestationCertChainSize);
    params->attestationCertChainSize = attestationCertChainSize;

ErrorExit:
    nvswitch_os_free(pCertChain);
    nvswitch_os_free(pDerCertChain);
    nvswitch_os_free(pAttestationCertChain);

    return status;
}

NvlStatus
nvswitch_tnvl_get_attestation_report_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_ATTESTATION_REPORT_PARAMS *params
)
{
    NvlStatus status;
    TNVL_GET_ATT_REPORT_CMD_PAYLOAD *pCmdPayload;
    TNVL_GET_ATT_REPORT_RSP_PAYLOAD *pRspPayload;
    NVSWITCH_TIMEOUT timeout;

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        NVSWITCH_PRINT(device, ERROR,
           "%s: TNVL is not enabled\n",
           __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pCmdPayload  = nvswitch_os_malloc(sizeof(TNVL_GET_ATT_REPORT_CMD_PAYLOAD));
    pRspPayload  = nvswitch_os_malloc(sizeof(TNVL_GET_ATT_REPORT_RSP_PAYLOAD));

    if (pCmdPayload == NULL || pRspPayload == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to allocate memory\n",
            __FUNCTION__);
        status = -NVL_NO_MEM;
        goto ErrorExit;
    }

    // Request the Attestation Report using the provided nonce, signed by the AK cert.
    pCmdPayload->subMessageId = TNVL_GET_ATT_REPORT_SUBMESSAGE_ID;
    pCmdPayload->minorVersion = 0;
    pCmdPayload->majorVersion = 1;
    nvswitch_os_memcpy(pCmdPayload->nonce, params->nonce, NVSWITCH_NONCE_SIZE);

    nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    status = nvswitch_fsp_send_and_read_message(device,
        (NvU8*) pCmdPayload, sizeof(TNVL_GET_ATT_REPORT_CMD_PAYLOAD), NVDM_TYPE_TNVL,
        (NvU8*) pRspPayload, sizeof(TNVL_GET_ATT_REPORT_RSP_PAYLOAD), &timeout);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get nvswitch measurements from FSP\n",
            __FUNCTION__);
        goto ErrorExit;
    }

    // Validate response
    if (pRspPayload->subMessageId != TNVL_GET_ATT_REPORT_SUBMESSAGE_ID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unknown submessage Id %d, Expected %d\n",
            __FUNCTION__,
            pRspPayload->subMessageId, TNVL_GET_ATT_REPORT_SUBMESSAGE_ID);
        status = -NVL_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    nvswitch_os_memcpy(params->attestationReport,
        pRspPayload->measurementBuffer, pRspPayload->measurementSize);
    params->attestationReportSize = pRspPayload->measurementSize;

ErrorExit:
    nvswitch_os_free(pCmdPayload);
    nvswitch_os_free(pRspPayload);

    return status;
}

NvlStatus
nvswitch_detect_tnvl_mode_ls10
(
    nvswitch_device *device
)
{
    NvU32 val;

    // Detect for TNVL mode
    val = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _TNVL_MODE);
    if (FLD_TEST_DRF(_NVLSAW, _TNVL_MODE, _STATUS, _ENABLED, val))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: TNVL Mode Detected\n",
            __FUNCTION__);
        device->tnvl_mode = NVSWITCH_DEVICE_TNVL_MODE_ENABLED;
    }

    return NVL_SUCCESS;
}

NvBool
nvswitch_is_tnvl_mode_enabled_ls10
(
    nvswitch_device *device
)
{
    return (device->tnvl_mode != NVSWITCH_DEVICE_TNVL_MODE_DISABLED);
}

NvBool
nvswitch_is_tnvl_mode_locked_ls10
(
    nvswitch_device *device
)
{
    return (device->tnvl_mode == NVSWITCH_DEVICE_TNVL_MODE_LOCKED);
}

NvlStatus
nvswitch_tnvl_send_fsp_lock_config_ls10
(
    nvswitch_device *device
)
{
    NvlStatus status;
    TNVL_LOCK_CONFIG_CMD_PAYLOAD *pCmdPayload;
    TNVL_LOCK_CONFIG_RSP_PAYLOAD *pRspPayload;
    NVSWITCH_TIMEOUT timeout;

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        NVSWITCH_PRINT(device, ERROR,
           "%s: TNVL is not enabled\n",
           __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pCmdPayload   = nvswitch_os_malloc(sizeof(TNVL_LOCK_CONFIG_CMD_PAYLOAD));
    pRspPayload   = nvswitch_os_malloc(sizeof(TNVL_LOCK_CONFIG_RSP_PAYLOAD));

    if (pCmdPayload == NULL || pRspPayload == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to allocate memory\n",
            __FUNCTION__);
        status = -NVL_NO_MEM;
        goto ErrorExit;
    }

    pCmdPayload->subMessageId = TNVL_LOCK_CONFIG_SUBMESSAGE_ID;
    pCmdPayload->minorVersion = 0;
    pCmdPayload->majorVersion = 1;

    nvswitch_timeout_create(5 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    status = nvswitch_fsp_send_and_read_message(device,
        (NvU8*) pCmdPayload, sizeof(TNVL_LOCK_CONFIG_CMD_PAYLOAD), NVDM_TYPE_TNVL,
        (NvU8*) pRspPayload, sizeof(TNVL_LOCK_CONFIG_RSP_PAYLOAD), &timeout);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: RPC failed for FSP lock config command\n",
            __FUNCTION__);
        status = -NVL_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    // Validate response
    if (pRspPayload->subMessageId != TNVL_LOCK_CONFIG_SUBMESSAGE_ID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unknown submessage Id %d, Expected %d\n",
            __FUNCTION__,
            pRspPayload->subMessageId, TNVL_LOCK_CONFIG_SUBMESSAGE_ID);
        status = -NVL_ERR_INVALID_STATE;
        goto ErrorExit;
    }

ErrorExit:
    nvswitch_os_free(pCmdPayload);
    nvswitch_os_free(pRspPayload);

    return status;
}

NvlStatus
nvswitch_tnvl_get_status_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_TNVL_STATUS_PARAMS *params
)
{
    params->status = device->tnvl_mode;
    return NVL_SUCCESS;
}

static NvBool
_nvswitch_tnvl_eng_wr_cpu_allow_list_ls10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 offset
)
{
    switch (eng_id)
    {
        case NVSWITCH_ENGINE_ID_SOE:
        case NVSWITCH_ENGINE_ID_GIN:
        case NVSWITCH_ENGINE_ID_FSP:
            return NV_TRUE;
        case NVSWITCH_ENGINE_ID_SAW:
        {
            if (offset == NV_NVLSAW_DRIVER_ATTACH_DETACH)
                return NV_TRUE;
            break;
        }
        case NVSWITCH_ENGINE_ID_NPG:
        {
            if ((offset == NV_NPG_INTR_RETRIGGER(0)) ||
                (offset == NV_NPG_INTR_RETRIGGER(1)))
                   return NV_TRUE;
            break;
        }
        case NVSWITCH_ENGINE_ID_CPR:
        {
            if ((offset == NV_CPR_SYS_INTR_RETRIGGER(0)) ||
                (offset == NV_CPR_SYS_INTR_RETRIGGER(1)))
                   return NV_TRUE;
            break;
        }
        case NVSWITCH_ENGINE_ID_MINION:
        {
            if ((offset == NV_MINION_NVLINK_DL_STAT(0)) ||
                (offset == NV_MINION_NVLINK_DL_STAT(1)) ||
                (offset == NV_MINION_NVLINK_DL_STAT(2)) ||
                (offset == NV_MINION_NVLINK_DL_STAT(3)))
                    return NV_TRUE;
            break;
        }
        default :
            return NV_FALSE;
    }

    return NV_FALSE;
}

void
nvswitch_tnvl_eng_wr_32_ls10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast,
    NvU32 eng_instance,
    NvU32 base_addr,
    NvU32 offset,
    NvU32 data
)
{
    if (device->nvlink_device->pciInfo.bars[0].pBar == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: register write failed at offset 0x%x\n",
            __FUNCTION__, offset);
        return;
    }

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "ENG reg-write failed. TNVL mode is not enabled\n");
        return;
    }

    if (_nvswitch_tnvl_eng_wr_cpu_allow_list_ls10(device, eng_id, offset))
    {
        nvswitch_os_mem_write32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + base_addr + offset, data);
        return;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "TNVL ENG_WR failure - 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
            eng_id, eng_instance, eng_bcast, base_addr, offset, data);

        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "TNVL mode is locked\n");
        return;
    }

    if (nvswitch_soe_eng_wr_32_ls10(device, eng_id, eng_bcast, eng_instance, base_addr, offset, data) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "TNVL ENG_WR failure - 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
            eng_id, eng_instance, eng_bcast, base_addr, offset, data);

        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE ENG_WR failed for 0x%x[%d] %s @0x%08x+0x%06x = 0x%08x\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ),
            base_addr, offset, data);
    }
}

static NvBool
_nvswitch_tnvl_reg_wr_cpu_allow_list_ls10
(
    nvswitch_device *device,
    NvU32 offset
)
{
    if ((offset >= DRF_BASE(NV_PFSP)) && 
       (offset <= DRF_EXTENT(NV_PFSP)))
    {
        return NV_TRUE;
    }

    if ((offset >= NV_PTOP_UNICAST_SW_DEVICE_BASE_SOE_0 + DRF_BASE(NV_SOE)) && 
       (offset <=  NV_PTOP_UNICAST_SW_DEVICE_BASE_SOE_0 + DRF_EXTENT(NV_SOE)))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

void
nvswitch_tnvl_reg_wr_32_ls10
(
    nvswitch_device *device,
    NvU32 offset,
    NvU32 data
)
{
    if (device->nvlink_device->pciInfo.bars[0].pBar == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: register write failed at offset 0x%x\n",
            __FUNCTION__, offset);
        NVSWITCH_ASSERT(0);
        return;
    }

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "Reg-write failed. TNVL mode is not enabled\n");
        return;
    }

    if (_nvswitch_tnvl_reg_wr_cpu_allow_list_ls10(device, offset))
    {
        nvswitch_os_mem_write32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + offset, data);
        return;
    }

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "TNVL REG_WR failure - 0x%08x, 0x%08x\n", offset, data);

        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "TNVL mode is locked\n");
        return;
    }

    if (nvswitch_soe_reg_wr_32_ls10(device, offset, data) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_TNVL_ERROR,
           "TNVL REG_WR failure - 0x%08x, 0x%08x\n", offset, data);
    }
}

void
nvswitch_tnvl_disable_interrupts_ls10
(
    nvswitch_device *device
)
{
    //
    // In TNVL locked disable non-fatal NVLW, NPG, and legacy interrupt,
    // disable additional non-fatals on those partitions.
    //
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NVLW_NON_FATAL_IDX),
        0xFFFF);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
        0xFFFF);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_UNITS_IDX),
        0xFFFFFFFF);
}

