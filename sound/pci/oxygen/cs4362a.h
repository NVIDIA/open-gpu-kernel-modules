/* SPDX-License-Identifier: GPL-2.0 */
/* register 01h */
#define CS4362A_PDN		0x01
#define CS4362A_DAC1_DIS	0x02
#define CS4362A_DAC2_DIS	0x04
#define CS4362A_DAC3_DIS	0x08
#define CS4362A_MCLKDIV		0x20
#define CS4362A_FREEZE		0x40
#define CS4362A_CPEN		0x80
/* register 02h */
#define CS4362A_DIF_MASK	0x70
#define CS4362A_DIF_LJUST	0x00
#define CS4362A_DIF_I2S		0x10
#define CS4362A_DIF_RJUST_16	0x20
#define CS4362A_DIF_RJUST_24	0x30
#define CS4362A_DIF_RJUST_20	0x40
#define CS4362A_DIF_RJUST_18	0x50
/* register 03h */
#define CS4362A_MUTEC_MASK	0x03
#define CS4362A_MUTEC_6		0x00
#define CS4362A_MUTEC_1		0x01
#define CS4362A_MUTEC_3		0x03
#define CS4362A_AMUTE		0x04
#define CS4362A_MUTEC_POL	0x08
#define CS4362A_RMP_UP		0x10
#define CS4362A_SNGLVOL		0x20
#define CS4362A_ZERO_CROSS	0x40
#define CS4362A_SOFT_RAMP	0x80
/* register 04h */
#define CS4362A_RMP_DN		0x01
#define CS4362A_DEM_MASK	0x06
#define CS4362A_DEM_NONE	0x00
#define CS4362A_DEM_44100	0x02
#define CS4362A_DEM_48000	0x04
#define CS4362A_DEM_32000	0x06
#define CS4362A_FILT_SEL	0x10
/* register 05h */
#define CS4362A_INV_A1		0x01
#define CS4362A_INV_B1		0x02
#define CS4362A_INV_A2		0x04
#define CS4362A_INV_B2		0x08
#define CS4362A_INV_A3		0x10
#define CS4362A_INV_B3		0x20
/* register 06h */
#define CS4362A_FM_MASK		0x03
#define CS4362A_FM_SINGLE	0x00
#define CS4362A_FM_DOUBLE	0x01
#define CS4362A_FM_QUAD		0x02
#define CS4362A_FM_DSD		0x03
#define CS4362A_ATAPI_MASK	0x7c
#define CS4362A_ATAPI_B_MUTE	0x00
#define CS4362A_ATAPI_B_R	0x04
#define CS4362A_ATAPI_B_L	0x08
#define CS4362A_ATAPI_B_LR	0x0c
#define CS4362A_ATAPI_A_MUTE	0x00
#define CS4362A_ATAPI_A_R	0x10
#define CS4362A_ATAPI_A_L	0x20
#define CS4362A_ATAPI_A_LR	0x30
#define CS4362A_ATAPI_MIX_LR_VOL 0x40
#define CS4362A_A_EQ_B		0x80
/* register 07h */
#define CS4362A_VOL_MASK		0x7f
#define CS4362A_MUTE			0x80
/* register 08h: like 07h */
/* registers 09h..0Bh: like 06h..08h */
/* registers 0Ch..0Eh: like 06h..08h */
/* register 12h */
#define CS4362A_REV_MASK	0x07
#define CS4362A_PART_MASK	0xf8
#define CS4362A_PART_CS4362A	0x50
