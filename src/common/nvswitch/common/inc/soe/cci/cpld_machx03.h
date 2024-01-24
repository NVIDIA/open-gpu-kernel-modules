/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _CPLD_MACHX03_H_
#define _CPLD_MACHX03_H_

// Register definitions
#define CPLD_MACHXO3_VERSION_MAJOR                 0x0
#define CPLD_MACHXO3_VERSION_MINOR                 0x1
#define CPLD_MACHXO3_PART_NUMBER_MSB               0x2
#define CPLD_MACHXO3_PART_NUMBER_LSB               0x3
#define CPLD_MACHXO3_PORTS_RESET_REG1              0x4
#define CPLD_MACHXO3_PORTS_RESET_REG2              0x5
#define CPLD_MACHXO3_PORTS_INT_REG1                0x6
#define CPLD_MACHXO3_PORTS_INT_REG2                0x7
#define CPLD_MACHXO3_PORTS_PRSNT_REG1              0x8
#define CPLD_MACHXO3_PORTS_PRSNT_REG2              0x9
#define CPLD_MACHXO3_PORTS_LPMODE_REG1             0xA
#define CPLD_MACHXO3_PORTS_LPMODE_REG2             0xB
#define CPLD_MACHXO3_LD_SW_EN_REG1                 0xC
#define CPLD_MACHXO3_LD_SW_EN_REG2                 0xD
#define CPLD_MACHXO3_LD_SW_PG_REG1                 0xE
#define CPLD_MACHXO3_LD_SW_PG_REG2                 0xF
#define CPLD_MACHXO3_LIN_EQ_EN_REG1                0x10
#define CPLD_MACHXO3_LIN_EQ_EN_REG2                0x11
#define CPLD_MACHXO3_LED_STATE_REG(cageNum)        (0x12 + cageNum)
#define CPLD_MACHXO3_LED_STATE_REG_LED_A           3:0 
#define CPLD_MACHXO3_LED_STATE_REG_LED_B           7:4     
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_OFF                0x0
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_AMBER              0x1
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_GREEN              0x2
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_3HZ_AMBER          0x3
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_3HZ_GREEN          0x4
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_6HZ_AMBER          0x5
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_6HZ_GREEN          0x6
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_3HZ_AMBER_GREEN    0x7
#define CPLD_MACHXO3_LED_STATE_REG_LED_STATE_6HZ_AMBER_GREEN    0x8
#define CPLD_MACHXO3_PORTS_UNPLUG_REG1             0x23
#define CPLD_MACHXO3_PORTS_UNPLUG_REG2             0x24
#define CPLD_MACHXO3_CPLD_CTL_REG1                 0x25
#define CPLD_MACHXO3_CPLD_CTL_REG1_I2CC_MUX_RESET   2:2
#define CPLD_MACHXO3_CPLD_CTL_REG1_I2CB_MUX_RESET   1:1
#define CPLD_MACHXO3_CPLD_CTL_REG1_WATCHDOG_ENABLE  0:0
#define CPLD_MACHXO3_CPLD_STATUS_REG               0x26
#define CPLD_MACHXO3_LD_SW_FAULT_REG1              0x29
#define CPLD_MACHXO3_LD_SW_FAULT_REG2              0x2A
#define CPLD_MACHXO3_PORTS_PRSNT_STATE_CHANGE_REG1 0x2B
#define CPLD_MACHXO3_PORTS_PRSNT_STATE_CHANGE_REG2 0x2C
#define CPLD_MACHXO3_CPLD_ID                       0x2D
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL                                         0x31
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED                           3:0 
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_OFF             0x0
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_AMBER           0x1
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_GREEN           0x2
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_3HZ_AMBER       0x3
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_3HZ_GREEN       0x4
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_6HZ_AMBER       0x5
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_6HZ_GREEN       0x6
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_3HZ_AMBER_GREEN 0x7
#define CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED_REG_STATE_6HZ_AMBER_GREEN 0x8


#endif //_CPLD_MACHX03_H_
