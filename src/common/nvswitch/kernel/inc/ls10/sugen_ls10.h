/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _SUGEN_LS10_H_
#define _SUGEN_LS10_H_


#include "common_nvswitch.h"
#include "ls10/ls10.h"

#include "nvswitch/ls10/dev_nvs_top.h"
#include "nvswitch/ls10/dev_pri_masterstation_ip.h"
#include "nvswitch/ls10/dev_pri_ringstation_sys_ip.h"
#include "nvswitch/ls10/dev_pri_ringstation_sysb_ip.h"
#include "nvswitch/ls10/dev_pri_ringstation_prt_ip.h"
#include "nvswitch/ls10/dev_pri_hub_sys_ip.h"
#include "nvswitch/ls10/dev_pri_hub_sysb_ip.h"
#include "nvswitch/ls10/dev_pri_hub_prt_ip.h"
#include "nvswitch/ls10/dev_nvlsaw_ip.h"
#include "nvswitch/ls10/dev_ctrl_ip.h"
#include "nvswitch/ls10/dev_timer_ip.h"
#include "nvswitch/ls10/dev_trim.h"
#include "nvswitch/ls10/dev_nv_xal_ep.h"
#include "nvswitch/ls10/dev_nv_xpl.h"
#include "nvswitch/ls10/dev_xtl_ep_pri.h"
#include "nvswitch/ls10/dev_soe_ip.h"
#include "nvswitch/ls10/dev_se_pri.h"
#include "nvswitch/ls10/dev_perf.h"
#include "nvswitch/ls10/dev_pmgr.h"
#include "nvswitch/ls10/dev_therm.h"

// NVLW
#include "nvswitch/ls10/dev_nvlw_ip.h"
#include "nvswitch/ls10/dev_cpr_ip.h"
#include "nvswitch/ls10/dev_nvlipt_ip.h"
#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/ls10/dev_nvltlc_ip.h"
#include "nvswitch/ls10/dev_nvldl_ip.h"
#include "nvswitch/ls10/dev_minion_ip.h"

// NPG/NPORT
#include "nvswitch/ls10/dev_npg_ip.h"
#include "nvswitch/ls10/dev_npgperf_ip.h"
#include "nvswitch/ls10/dev_nport_ip.h"
#include "nvswitch/ls10/dev_route_ip.h"
#include "nvswitch/ls10/dev_tstate_ip.h"
#include "nvswitch/ls10/dev_egress_ip.h"
#include "nvswitch/ls10/dev_ingress_ip.h"
#include "nvswitch/ls10/dev_sourcetrack_ip.h"
#include "nvswitch/ls10/dev_multicasttstate_ip.h"
#include "nvswitch/ls10/dev_reductiontstate_ip.h"

// NXBAR
#include "nvswitch/ls10/dev_nxbar_tile_ip.h"
#include "nvswitch/ls10/dev_nxbar_tileout_ip.h"

#endif //_SUGEN_LS10_H_
