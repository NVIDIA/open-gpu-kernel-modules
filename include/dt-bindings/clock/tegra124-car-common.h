/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This header provides constants for binding nvidia,tegra124-car or
 * nvidia,tegra132-car.
 *
 * The first 192 clocks are numbered to match the bits in the CAR's CLK_OUT_ENB
 * registers. These IDs often match those in the CAR's RST_DEVICES registers,
 * but not in all cases. Some bits in CLK_OUT_ENB affect multiple clocks. In
 * this case, those clocks are assigned IDs above 185 in order to highlight
 * this issue. Implementations that interpret these clock IDs as bit values
 * within the CLK_OUT_ENB or RST_DEVICES registers should be careful to
 * explicitly handle these special cases.
 *
 * The balance of the clocks controlled by the CAR are assigned IDs of 185 and
 * above.
 */

#ifndef _DT_BINDINGS_CLOCK_TEGRA124_CAR_COMMON_H
#define _DT_BINDINGS_CLOCK_TEGRA124_CAR_COMMON_H

/* 0 */
/* 1 */
/* 2 */
#define TEGRA124_CLK_ISPB 3
#define TEGRA124_CLK_RTC 4
#define TEGRA124_CLK_TIMER 5
#define TEGRA124_CLK_UARTA 6
/* 7 (register bit affects uartb and vfir) */
/* 8 */
#define TEGRA124_CLK_SDMMC2 9
/* 10 (register bit affects spdif_in and spdif_out) */
#define TEGRA124_CLK_I2S1 11
#define TEGRA124_CLK_I2C1 12
/* 13 */
#define TEGRA124_CLK_SDMMC1 14
#define TEGRA124_CLK_SDMMC4 15
/* 16 */
#define TEGRA124_CLK_PWM 17
#define TEGRA124_CLK_I2S2 18
/* 20 (register bit affects vi and vi_sensor) */
/* 21 */
#define TEGRA124_CLK_USBD 22
#define TEGRA124_CLK_ISP 23
/* 26 */
/* 25 */
#define TEGRA124_CLK_DISP2 26
#define TEGRA124_CLK_DISP1 27
#define TEGRA124_CLK_HOST1X 28
#define TEGRA124_CLK_VCP 29
#define TEGRA124_CLK_I2S0 30
/* 31 */

#define TEGRA124_CLK_MC 32
/* 33 */
#define TEGRA124_CLK_APBDMA 34
/* 35 */
#define TEGRA124_CLK_KBC 36
/* 37 */
/* 38 */
/* 39 (register bit affects fuse and fuse_burn) */
#define TEGRA124_CLK_KFUSE 40
#define TEGRA124_CLK_SBC1 41
#define TEGRA124_CLK_NOR 42
/* 43 */
#define TEGRA124_CLK_SBC2 44
/* 45 */
#define TEGRA124_CLK_SBC3 46
#define TEGRA124_CLK_I2C5 47
#define TEGRA124_CLK_DSIA 48
/* 49 */
#define TEGRA124_CLK_MIPI 50
#define TEGRA124_CLK_HDMI 51
#define TEGRA124_CLK_CSI 52
/* 53 */
#define TEGRA124_CLK_I2C2 54
#define TEGRA124_CLK_UARTC 55
#define TEGRA124_CLK_MIPI_CAL 56
#define TEGRA124_CLK_EMC 57
#define TEGRA124_CLK_USB2 58
#define TEGRA124_CLK_USB3 59
/* 60 */
#define TEGRA124_CLK_VDE 61
#define TEGRA124_CLK_BSEA 62
#define TEGRA124_CLK_BSEV 63

/* 64 */
#define TEGRA124_CLK_UARTD 65
/* 66 */
#define TEGRA124_CLK_I2C3 67
#define TEGRA124_CLK_SBC4 68
#define TEGRA124_CLK_SDMMC3 69
#define TEGRA124_CLK_PCIE 70
#define TEGRA124_CLK_OWR 71
#define TEGRA124_CLK_AFI 72
#define TEGRA124_CLK_CSITE 73
/* 74 */
/* 75 */
#define TEGRA124_CLK_LA 76
#define TEGRA124_CLK_TRACE 77
#define TEGRA124_CLK_SOC_THERM 78
#define TEGRA124_CLK_DTV 79
/* 80 */
#define TEGRA124_CLK_I2CSLOW 81
#define TEGRA124_CLK_DSIB 82
#define TEGRA124_CLK_TSEC 83
/* 84 */
/* 85 */
/* 86 */
/* 87 */
/* 88 */
#define TEGRA124_CLK_XUSB_HOST 89
/* 90 */
#define TEGRA124_CLK_MSENC 91
#define TEGRA124_CLK_CSUS 92
/* 93 */
/* 94 */
/* 95 (bit affects xusb_dev and xusb_dev_src) */

/* 96 */
/* 97 */
/* 98 */
#define TEGRA124_CLK_MSELECT 99
#define TEGRA124_CLK_TSENSOR 100
#define TEGRA124_CLK_I2S3 101
#define TEGRA124_CLK_I2S4 102
#define TEGRA124_CLK_I2C4 103
#define TEGRA124_CLK_SBC5 104
#define TEGRA124_CLK_SBC6 105
#define TEGRA124_CLK_D_AUDIO 106
#define TEGRA124_CLK_APBIF 107
#define TEGRA124_CLK_DAM0 108
#define TEGRA124_CLK_DAM1 109
#define TEGRA124_CLK_DAM2 110
#define TEGRA124_CLK_HDA2CODEC_2X 111
/* 112 */
#define TEGRA124_CLK_AUDIO0_2X 113
#define TEGRA124_CLK_AUDIO1_2X 114
#define TEGRA124_CLK_AUDIO2_2X 115
#define TEGRA124_CLK_AUDIO3_2X 116
#define TEGRA124_CLK_AUDIO4_2X 117
#define TEGRA124_CLK_SPDIF_2X 118
#define TEGRA124_CLK_ACTMON 119
#define TEGRA124_CLK_EXTERN1 120
#define TEGRA124_CLK_EXTERN2 121
#define TEGRA124_CLK_EXTERN3 122
#define TEGRA124_CLK_SATA_OOB 123
#define TEGRA124_CLK_SATA 124
#define TEGRA124_CLK_HDA 125
/* 126 */
#define TEGRA124_CLK_SE 127

#define TEGRA124_CLK_HDA2HDMI 128
#define TEGRA124_CLK_SATA_COLD 129
/* 130 */
/* 131 */
/* 132 */
/* 133 */
/* 134 */
/* 135 */
#define TEGRA124_CLK_CEC 136
/* 137 */
/* 138 */
/* 139 */
/* 140 */
/* 141 */
/* 142 */
/* 143 (bit affects xusb_falcon_src, xusb_fs_src, */
/*      xusb_host_src and xusb_ss_src) */
#define TEGRA124_CLK_CILAB 144
#define TEGRA124_CLK_CILCD 145
#define TEGRA124_CLK_CILE 146
#define TEGRA124_CLK_DSIALP 147
#define TEGRA124_CLK_DSIBLP 148
#define TEGRA124_CLK_ENTROPY 149
#define TEGRA124_CLK_DDS 150
/* 151 */
#define TEGRA124_CLK_DP2 152
#define TEGRA124_CLK_AMX 153
#define TEGRA124_CLK_ADX 154
/* 155 (bit affects dfll_ref and dfll_soc) */
#define TEGRA124_CLK_XUSB_SS 156
/* 157 */
/* 158 */
/* 159 */

/* 160 */
/* 161 */
/* 162 */
/* 163 */
/* 164 */
/* 165 */
#define TEGRA124_CLK_I2C6 166
/* 167 */
/* 168 */
/* 169 */
/* 170 */
#define TEGRA124_CLK_VIM2_CLK 171
/* 172 */
/* 173 */
/* 174 */
/* 175 */
#define TEGRA124_CLK_HDMI_AUDIO 176
#define TEGRA124_CLK_CLK72MHZ 177
#define TEGRA124_CLK_VIC03 178
/* 179 */
#define TEGRA124_CLK_ADX1 180
#define TEGRA124_CLK_DPAUX 181
#define TEGRA124_CLK_SOR0 182
/* 183 */
#define TEGRA124_CLK_GPU 184
#define TEGRA124_CLK_AMX1 185
/* 186 */
/* 187 */
/* 188 */
/* 189 */
/* 190 */
/* 191 */
#define TEGRA124_CLK_UARTB 192
#define TEGRA124_CLK_VFIR 193
#define TEGRA124_CLK_SPDIF_IN 194
#define TEGRA124_CLK_SPDIF_OUT 195
#define TEGRA124_CLK_VI 196
#define TEGRA124_CLK_VI_SENSOR 197
#define TEGRA124_CLK_FUSE 198
#define TEGRA124_CLK_FUSE_BURN 199
#define TEGRA124_CLK_CLK_32K 200
#define TEGRA124_CLK_CLK_M 201
#define TEGRA124_CLK_CLK_M_DIV2 202
#define TEGRA124_CLK_CLK_M_DIV4 203
#define TEGRA124_CLK_OSC_DIV2 202
#define TEGRA124_CLK_OSC_DIV4 203
#define TEGRA124_CLK_PLL_REF 204
#define TEGRA124_CLK_PLL_C 205
#define TEGRA124_CLK_PLL_C_OUT1 206
#define TEGRA124_CLK_PLL_C2 207
#define TEGRA124_CLK_PLL_C3 208
#define TEGRA124_CLK_PLL_M 209
#define TEGRA124_CLK_PLL_M_OUT1 210
#define TEGRA124_CLK_PLL_P 211
#define TEGRA124_CLK_PLL_P_OUT1 212
#define TEGRA124_CLK_PLL_P_OUT2 213
#define TEGRA124_CLK_PLL_P_OUT3 214
#define TEGRA124_CLK_PLL_P_OUT4 215
#define TEGRA124_CLK_PLL_A 216
#define TEGRA124_CLK_PLL_A_OUT0 217
#define TEGRA124_CLK_PLL_D 218
#define TEGRA124_CLK_PLL_D_OUT0 219
#define TEGRA124_CLK_PLL_D2 220
#define TEGRA124_CLK_PLL_D2_OUT0 221
#define TEGRA124_CLK_PLL_U 222
#define TEGRA124_CLK_PLL_U_480M 223

#define TEGRA124_CLK_PLL_U_60M 224
#define TEGRA124_CLK_PLL_U_48M 225
#define TEGRA124_CLK_PLL_U_12M 226
/* 227 */
/* 228 */
#define TEGRA124_CLK_PLL_RE_VCO 229
#define TEGRA124_CLK_PLL_RE_OUT 230
#define TEGRA124_CLK_PLL_E 231
#define TEGRA124_CLK_SPDIF_IN_SYNC 232
#define TEGRA124_CLK_I2S0_SYNC 233
#define TEGRA124_CLK_I2S1_SYNC 234
#define TEGRA124_CLK_I2S2_SYNC 235
#define TEGRA124_CLK_I2S3_SYNC 236
#define TEGRA124_CLK_I2S4_SYNC 237
#define TEGRA124_CLK_VIMCLK_SYNC 238
#define TEGRA124_CLK_AUDIO0 239
#define TEGRA124_CLK_AUDIO1 240
#define TEGRA124_CLK_AUDIO2 241
#define TEGRA124_CLK_AUDIO3 242
#define TEGRA124_CLK_AUDIO4 243
#define TEGRA124_CLK_SPDIF 244
/* 245 */
/* 246 */
/* 247 */
/* 248 */
#define TEGRA124_CLK_OSC 249
/* 250 */
/* 251 */
#define TEGRA124_CLK_XUSB_HOST_SRC 252
#define TEGRA124_CLK_XUSB_FALCON_SRC 253
#define TEGRA124_CLK_XUSB_FS_SRC 254
#define TEGRA124_CLK_XUSB_SS_SRC 255

#define TEGRA124_CLK_XUSB_DEV_SRC 256
#define TEGRA124_CLK_XUSB_DEV 257
#define TEGRA124_CLK_XUSB_HS_SRC 258
#define TEGRA124_CLK_SCLK 259
#define TEGRA124_CLK_HCLK 260
#define TEGRA124_CLK_PCLK 261
/* 262 */
/* 263 */
#define TEGRA124_CLK_DFLL_REF 264
#define TEGRA124_CLK_DFLL_SOC 265
#define TEGRA124_CLK_VI_SENSOR2 266
#define TEGRA124_CLK_PLL_P_OUT5 267
#define TEGRA124_CLK_CML0 268
#define TEGRA124_CLK_CML1 269
#define TEGRA124_CLK_PLL_C4 270
#define TEGRA124_CLK_PLL_DP 271
#define TEGRA124_CLK_PLL_E_MUX 272
#define TEGRA124_CLK_PLL_D_DSI_OUT 273
/* 274 */
/* 275 */
/* 276 */
/* 277 */
/* 278 */
/* 279 */
/* 280 */
/* 281 */
/* 282 */
/* 283 */
/* 284 */
/* 285 */
/* 286 */
/* 287 */

/* 288 */
/* 289 */
/* 290 */
/* 291 */
/* 292 */
/* 293 */
/* 294 */
/* 295 */
/* 296 */
/* 297 */
/* 298 */
/* 299 */
#define TEGRA124_CLK_AUDIO0_MUX 300
#define TEGRA124_CLK_AUDIO1_MUX 301
#define TEGRA124_CLK_AUDIO2_MUX 302
#define TEGRA124_CLK_AUDIO3_MUX 303
#define TEGRA124_CLK_AUDIO4_MUX 304
#define TEGRA124_CLK_SPDIF_MUX 305
/* 306 */
/* 307 */
/* 308 */
/* 309 */
/* 310 */
#define TEGRA124_CLK_SOR0_LVDS 311 /* deprecated */
#define TEGRA124_CLK_SOR0_OUT 311
#define TEGRA124_CLK_XUSB_SS_DIV2 312

#define TEGRA124_CLK_PLL_M_UD 313
#define TEGRA124_CLK_PLL_C_UD 314

#endif	/* _DT_BINDINGS_CLOCK_TEGRA124_CAR_COMMON_H */
