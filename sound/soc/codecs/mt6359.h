/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 MediaTek Inc.
 * Author: Argus Lin <argus.lin@mediatek.com>
 */

#ifndef _MT6359_H_
#define _MT6359_H_

/*************Register Bit Define*************/
#define MT6359_TOP0_ID                       0x0
#define MT6359_SMT_CON1                      0x32
#define MT6359_DRV_CON2                      0x3c
#define MT6359_DRV_CON3                      0x3e
#define MT6359_DRV_CON4                      0x40
#define MT6359_TOP_CKPDN_CON0                0x10c
#define MT6359_TOP_CKPDN_CON0_SET            0x10e
#define MT6359_TOP_CKPDN_CON0_CLR            0x110
#define MT6359_AUXADC_RQST0                  0x1108
#define MT6359_AUXADC_CON10                  0x11a0
#define MT6359_AUXADC_ACCDET                 0x11ba
#define MT6359_LDO_VUSB_OP_EN                0x1d0c
#define MT6359_LDO_VUSB_OP_EN_SET            0x1d0e
#define MT6359_LDO_VUSB_OP_EN_CLR            0x1d10
#define MT6359_AUD_TOP_CKPDN_CON0            0x230c
#define MT6359_AUD_TOP_CKPDN_CON0_SET        0x230e
#define MT6359_AUD_TOP_CKPDN_CON0_CLR        0x2310
#define MT6359_AUD_TOP_RST_CON0              0x2320
#define MT6359_AUD_TOP_RST_CON0_SET          0x2322
#define MT6359_AUD_TOP_RST_CON0_CLR          0x2324
#define MT6359_AUD_TOP_INT_CON0              0x2328
#define MT6359_AUD_TOP_INT_CON0_SET          0x232a
#define MT6359_AUD_TOP_INT_CON0_CLR          0x232c
#define MT6359_AUD_TOP_INT_MASK_CON0         0x232e
#define MT6359_AUD_TOP_INT_MASK_CON0_SET     0x2330
#define MT6359_AUD_TOP_INT_MASK_CON0_CLR     0x2332
#define MT6359_AUD_TOP_INT_STATUS0           0x2334
#define MT6359_AFE_NCP_CFG2                  0x24e2
#define MT6359_AUDENC_DSN_ID                 0x2500
#define MT6359_AUDENC_DSN_REV0               0x2502
#define MT6359_AUDENC_DSN_DBI                0x2504
#define MT6359_AUDENC_DSN_FPI                0x2506
#define MT6359_AUDENC_ANA_CON0               0x2508
#define MT6359_AUDENC_ANA_CON1               0x250a
#define MT6359_AUDENC_ANA_CON2               0x250c
#define MT6359_AUDENC_ANA_CON3               0x250e
#define MT6359_AUDENC_ANA_CON4               0x2510
#define MT6359_AUDENC_ANA_CON5               0x2512
#define MT6359_AUDENC_ANA_CON6               0x2514
#define MT6359_AUDENC_ANA_CON7               0x2516
#define MT6359_AUDENC_ANA_CON8               0x2518
#define MT6359_AUDENC_ANA_CON9               0x251a
#define MT6359_AUDENC_ANA_CON10              0x251c
#define MT6359_AUDENC_ANA_CON11              0x251e
#define MT6359_AUDENC_ANA_CON12              0x2520
#define MT6359_AUDENC_ANA_CON13              0x2522
#define MT6359_AUDENC_ANA_CON14              0x2524
#define MT6359_AUDENC_ANA_CON15              0x2526
#define MT6359_AUDENC_ANA_CON16              0x2528
#define MT6359_AUDENC_ANA_CON17              0x252a
#define MT6359_AUDENC_ANA_CON18              0x252c
#define MT6359_AUDENC_ANA_CON19              0x252e
#define MT6359_AUDENC_ANA_CON20              0x2530
#define MT6359_AUDENC_ANA_CON21              0x2532
#define MT6359_AUDENC_ANA_CON22              0x2534
#define MT6359_AUDENC_ANA_CON23              0x2536
#define MT6359_AUDDEC_DSN_ID                 0x2580
#define MT6359_AUDDEC_DSN_REV0               0x2582
#define MT6359_AUDDEC_DSN_DBI                0x2584
#define MT6359_AUDDEC_DSN_FPI                0x2586
#define MT6359_AUDDEC_ANA_CON0               0x2588
#define MT6359_AUDDEC_ANA_CON1               0x258a
#define MT6359_AUDDEC_ANA_CON2               0x258c
#define MT6359_AUDDEC_ANA_CON3               0x258e
#define MT6359_AUDDEC_ANA_CON4               0x2590
#define MT6359_AUDDEC_ANA_CON5               0x2592
#define MT6359_AUDDEC_ANA_CON6               0x2594
#define MT6359_AUDDEC_ANA_CON7               0x2596
#define MT6359_AUDDEC_ANA_CON8               0x2598
#define MT6359_AUDDEC_ANA_CON9               0x259a
#define MT6359_AUDDEC_ANA_CON10              0x259c
#define MT6359_AUDDEC_ANA_CON11              0x259e
#define MT6359_AUDDEC_ANA_CON12              0x25a0
#define MT6359_AUDDEC_ANA_CON13              0x25a2
#define MT6359_AUDDEC_ANA_CON14              0x25a4
#define MT6359_ACCDET_DSN_DIG_ID             0x2680
#define MT6359_ACCDET_DSN_DIG_REV0           0x2682
#define MT6359_ACCDET_DSN_DBI                0x2684
#define MT6359_ACCDET_DSN_FPI                0x2686
#define MT6359_ACCDET_CON0                   0x2688
#define MT6359_ACCDET_CON1                   0x268a
#define MT6359_ACCDET_CON2                   0x268c
#define MT6359_ACCDET_CON3                   0x268e
#define MT6359_ACCDET_CON4                   0x2690
#define MT6359_ACCDET_CON5                   0x2692
#define MT6359_ACCDET_CON6                   0x2694
#define MT6359_ACCDET_CON7                   0x2696
#define MT6359_ACCDET_CON8                   0x2698
#define MT6359_ACCDET_CON9                   0x269a
#define MT6359_ACCDET_CON10                  0x269c
#define MT6359_ACCDET_CON11                  0x269e
#define MT6359_ACCDET_CON12                  0x26a0
#define MT6359_ACCDET_CON13                  0x26a2
#define MT6359_ACCDET_CON14                  0x26a4
#define MT6359_ACCDET_CON15                  0x26a6
#define MT6359_ACCDET_CON16                  0x26a8
#define MT6359_ACCDET_CON17                  0x26aa
#define MT6359_ACCDET_CON18                  0x26ac
#define MT6359_ACCDET_CON19                  0x26ae
#define MT6359_ACCDET_CON20                  0x26b0
#define MT6359_ACCDET_CON21                  0x26b2
#define MT6359_ACCDET_CON22                  0x26b4
#define MT6359_ACCDET_CON23                  0x26b6
#define MT6359_ACCDET_CON24                  0x26b8
#define MT6359_ACCDET_CON25                  0x26ba
#define MT6359_ACCDET_CON26                  0x26bc
#define MT6359_ACCDET_CON27                  0x26be
#define MT6359_ACCDET_CON28                  0x26c0
#define MT6359_ACCDET_CON29                  0x26c2
#define MT6359_ACCDET_CON30                  0x26c4
#define MT6359_ACCDET_CON31                  0x26c6
#define MT6359_ACCDET_CON32                  0x26c8
#define MT6359_ACCDET_CON33                  0x26ca
#define MT6359_ACCDET_CON34                  0x26cc
#define MT6359_ACCDET_CON35                  0x26ce
#define MT6359_ACCDET_CON36                  0x26d0
#define MT6359_ACCDET_CON37                  0x26d2
#define MT6359_ACCDET_CON38                  0x26d4
#define MT6359_ACCDET_CON39                  0x26d6
#define MT6359_ACCDET_CON40                  0x26d8

#define TOP0_ANA_ID_ADDR                               \
	MT6359_TOP0_ID
#define TOP0_ANA_ID_SFT                                0
#define TOP0_ANA_ID_MASK                               0xFF
#define TOP0_ANA_ID_MASK_SFT                           (0xFF << 0)
#define AUXADC_RQST_CH0_ADDR                           \
	MT6359_AUXADC_RQST0
#define AUXADC_RQST_CH0_SFT                            0
#define AUXADC_RQST_CH0_MASK                           0x1
#define AUXADC_RQST_CH0_MASK_SFT                       (0x1 << 0)
#define AUXADC_ACCDET_ANASWCTRL_EN_ADDR                \
	MT6359_AUXADC_CON15
#define AUXADC_ACCDET_ANASWCTRL_EN_SFT                 6
#define AUXADC_ACCDET_ANASWCTRL_EN_MASK                0x1
#define AUXADC_ACCDET_ANASWCTRL_EN_MASK_SFT            (0x1 << 6)

#define AUXADC_ACCDET_AUTO_SPL_ADDR                    \
	MT6359_AUXADC_ACCDET
#define AUXADC_ACCDET_AUTO_SPL_SFT                     0
#define AUXADC_ACCDET_AUTO_SPL_MASK                    0x1
#define AUXADC_ACCDET_AUTO_SPL_MASK_SFT                (0x1 << 0)
#define AUXADC_ACCDET_AUTO_RQST_CLR_ADDR               \
	MT6359_AUXADC_ACCDET
#define AUXADC_ACCDET_AUTO_RQST_CLR_SFT                1
#define AUXADC_ACCDET_AUTO_RQST_CLR_MASK               0x1
#define AUXADC_ACCDET_AUTO_RQST_CLR_MASK_SFT           (0x1 << 1)
#define AUXADC_ACCDET_DIG1_RSV0_ADDR                   \
	MT6359_AUXADC_ACCDET
#define AUXADC_ACCDET_DIG1_RSV0_SFT                    2
#define AUXADC_ACCDET_DIG1_RSV0_MASK                   0x3F
#define AUXADC_ACCDET_DIG1_RSV0_MASK_SFT               (0x3F << 2)
#define AUXADC_ACCDET_DIG0_RSV0_ADDR                   \
	MT6359_AUXADC_ACCDET
#define AUXADC_ACCDET_DIG0_RSV0_SFT                    8
#define AUXADC_ACCDET_DIG0_RSV0_MASK                   0xFF
#define AUXADC_ACCDET_DIG0_RSV0_MASK_SFT               (0xFF << 8)

#define RG_ACCDET_CK_PDN_ADDR                          \
	MT6359_AUD_TOP_CKPDN_CON0
#define RG_ACCDET_CK_PDN_SFT                           0
#define RG_ACCDET_CK_PDN_MASK                          0x1
#define RG_ACCDET_CK_PDN_MASK_SFT                      (0x1 << 0)

#define RG_ACCDET_RST_ADDR                             \
	MT6359_AUD_TOP_RST_CON0
#define RG_ACCDET_RST_SFT                              1
#define RG_ACCDET_RST_MASK                             0x1
#define RG_ACCDET_RST_MASK_SFT                         (0x1 << 1)
#define BANK_ACCDET_SWRST_ADDR                         \
	MT6359_AUD_TOP_RST_BANK_CON0
#define BANK_ACCDET_SWRST_SFT                          0
#define BANK_ACCDET_SWRST_MASK                         0x1
#define BANK_ACCDET_SWRST_MASK_SFT                     (0x1 << 0)

#define RG_INT_EN_ACCDET_ADDR                          \
	MT6359_AUD_TOP_INT_CON0
#define RG_INT_EN_ACCDET_SFT                           5
#define RG_INT_EN_ACCDET_MASK                          0x1
#define RG_INT_EN_ACCDET_MASK_SFT                      (0x1 << 5)
#define RG_INT_EN_ACCDET_EINT0_ADDR                    \
	MT6359_AUD_TOP_INT_CON0
#define RG_INT_EN_ACCDET_EINT0_SFT                     6
#define RG_INT_EN_ACCDET_EINT0_MASK                    0x1
#define RG_INT_EN_ACCDET_EINT0_MASK_SFT                (0x1 << 6)
#define RG_INT_EN_ACCDET_EINT1_ADDR                    \
	MT6359_AUD_TOP_INT_CON0
#define RG_INT_EN_ACCDET_EINT1_SFT                     7
#define RG_INT_EN_ACCDET_EINT1_MASK                    0x1
#define RG_INT_EN_ACCDET_EINT1_MASK_SFT                (0x1 << 7)

#define RG_INT_MASK_ACCDET_ADDR                        \
	MT6359_AUD_TOP_INT_MASK_CON0
#define RG_INT_MASK_ACCDET_SFT                         5
#define RG_INT_MASK_ACCDET_MASK                        0x1
#define RG_INT_MASK_ACCDET_MASK_SFT                    (0x1 << 5)
#define RG_INT_MASK_ACCDET_EINT0_ADDR                  \
	MT6359_AUD_TOP_INT_MASK_CON0
#define RG_INT_MASK_ACCDET_EINT0_SFT                   6
#define RG_INT_MASK_ACCDET_EINT0_MASK                  0x1
#define RG_INT_MASK_ACCDET_EINT0_MASK_SFT              (0x1 << 6)
#define RG_INT_MASK_ACCDET_EINT1_ADDR                  \
	MT6359_AUD_TOP_INT_MASK_CON0
#define RG_INT_MASK_ACCDET_EINT1_SFT                   7
#define RG_INT_MASK_ACCDET_EINT1_MASK                  0x1
#define RG_INT_MASK_ACCDET_EINT1_MASK_SFT              (0x1 << 7)

#define RG_INT_STATUS_ACCDET_ADDR                      \
	MT6359_AUD_TOP_INT_STATUS0
#define RG_INT_STATUS_ACCDET_SFT                       5
#define RG_INT_STATUS_ACCDET_MASK                      0x1
#define RG_INT_STATUS_ACCDET_MASK_SFT                  (0x1 << 5)
#define RG_INT_STATUS_ACCDET_EINT0_ADDR                \
	MT6359_AUD_TOP_INT_STATUS0
#define RG_INT_STATUS_ACCDET_EINT0_SFT                 6
#define RG_INT_STATUS_ACCDET_EINT0_MASK                0x1
#define RG_INT_STATUS_ACCDET_EINT0_MASK_SFT            (0x1 << 6)
#define RG_INT_STATUS_ACCDET_EINT1_ADDR                \
	MT6359_AUD_TOP_INT_STATUS0
#define RG_INT_STATUS_ACCDET_EINT1_SFT                 7
#define RG_INT_STATUS_ACCDET_EINT1_MASK                0x1
#define RG_INT_STATUS_ACCDET_EINT1_MASK_SFT            (0x1 << 7)

#define RG_INT_RAW_STATUS_ACCDET_ADDR                  \
	MT6359_AUD_TOP_INT_RAW_STATUS0
#define RG_INT_RAW_STATUS_ACCDET_SFT                   5
#define RG_INT_RAW_STATUS_ACCDET_MASK                  0x1
#define RG_INT_RAW_STATUS_ACCDET_MASK_SFT              (0x1 << 5)
#define RG_INT_RAW_STATUS_ACCDET_EINT0_ADDR            \
	MT6359_AUD_TOP_INT_RAW_STATUS0
#define RG_INT_RAW_STATUS_ACCDET_EINT0_SFT             6
#define RG_INT_RAW_STATUS_ACCDET_EINT0_MASK            0x1
#define RG_INT_RAW_STATUS_ACCDET_EINT0_MASK_SFT        (0x1 << 6)
#define RG_INT_RAW_STATUS_ACCDET_EINT1_ADDR            \
	MT6359_AUD_TOP_INT_RAW_STATUS0
#define RG_INT_RAW_STATUS_ACCDET_EINT1_SFT             7
#define RG_INT_RAW_STATUS_ACCDET_EINT1_MASK            0x1
#define RG_INT_RAW_STATUS_ACCDET_EINT1_MASK_SFT        (0x1 << 7)

#define RG_AUDACCDETMICBIAS0PULLLOW_ADDR               \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETMICBIAS0PULLLOW_SFT                0
#define RG_AUDACCDETMICBIAS0PULLLOW_MASK               0x1
#define RG_AUDACCDETMICBIAS0PULLLOW_MASK_SFT           (0x1 << 0)
#define RG_AUDACCDETMICBIAS1PULLLOW_ADDR               \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETMICBIAS1PULLLOW_SFT                1
#define RG_AUDACCDETMICBIAS1PULLLOW_MASK               0x1
#define RG_AUDACCDETMICBIAS1PULLLOW_MASK_SFT           (0x1 << 1)
#define RG_AUDACCDETMICBIAS2PULLLOW_ADDR               \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETMICBIAS2PULLLOW_SFT                2
#define RG_AUDACCDETMICBIAS2PULLLOW_MASK               0x1
#define RG_AUDACCDETMICBIAS2PULLLOW_MASK_SFT           (0x1 << 2)
#define RG_AUDACCDETVIN1PULLLOW_ADDR                   \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETVIN1PULLLOW_SFT                    3
#define RG_AUDACCDETVIN1PULLLOW_MASK                   0x1
#define RG_AUDACCDETVIN1PULLLOW_MASK_SFT               (0x1 << 3)
#define RG_AUDACCDETVTHACAL_ADDR                       \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETVTHACAL_SFT                        4
#define RG_AUDACCDETVTHACAL_MASK                       0x1
#define RG_AUDACCDETVTHACAL_MASK_SFT                   (0x1 << 4)
#define RG_AUDACCDETVTHBCAL_ADDR                       \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETVTHBCAL_SFT                        5
#define RG_AUDACCDETVTHBCAL_MASK                       0x1
#define RG_AUDACCDETVTHBCAL_MASK_SFT                   (0x1 << 5)
#define RG_AUDACCDETTVDET_ADDR                         \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETTVDET_SFT                          6
#define RG_AUDACCDETTVDET_MASK                         0x1
#define RG_AUDACCDETTVDET_MASK_SFT                     (0x1 << 6)
#define RG_ACCDETSEL_ADDR                              \
	MT6359_AUDENC_ANA_CON18
#define RG_ACCDETSEL_SFT                               7
#define RG_ACCDETSEL_MASK                              0x1
#define RG_ACCDETSEL_MASK_SFT                          (0x1 << 7)

#define RG_AUDPWDBMICBIAS1_ADDR                        \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDPWDBMICBIAS1_SFT                         0
#define RG_AUDPWDBMICBIAS1_MASK                        0x1
#define RG_AUDPWDBMICBIAS1_MASK_SFT                    (0x1 << 0)
#define RG_AUDMICBIAS1BYPASSEN_ADDR                    \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDMICBIAS1BYPASSEN_SFT                     1
#define RG_AUDMICBIAS1BYPASSEN_MASK                    0x1
#define RG_AUDMICBIAS1BYPASSEN_MASK_SFT                (0x1 << 1)
#define RG_AUDMICBIAS1LOWPEN_ADDR                      \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDMICBIAS1LOWPEN_SFT                       2
#define RG_AUDMICBIAS1LOWPEN_MASK                      0x1
#define RG_AUDMICBIAS1LOWPEN_MASK_SFT                  (0x1 << 2)
#define RG_AUDMICBIAS1VREF_ADDR                        \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDMICBIAS1VREF_SFT                         4
#define RG_AUDMICBIAS1VREF_MASK                        0x7
#define RG_AUDMICBIAS1VREF_MASK_SFT                    (0x7 << 4)
#define RG_AUDMICBIAS1DCSW1PEN_ADDR                    \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDMICBIAS1DCSW1PEN_SFT                     8
#define RG_AUDMICBIAS1DCSW1PEN_MASK                    0x1
#define RG_AUDMICBIAS1DCSW1PEN_MASK_SFT                (0x1 << 8)
#define RG_AUDMICBIAS1DCSW1NEN_ADDR                    \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDMICBIAS1DCSW1NEN_SFT                     9
#define RG_AUDMICBIAS1DCSW1NEN_MASK                    0x1
#define RG_AUDMICBIAS1DCSW1NEN_MASK_SFT                (0x1 << 9)
#define RG_BANDGAPGEN_ADDR                             \
	MT6359_AUDENC_ANA_CON16
#define RG_BANDGAPGEN_SFT                              10
#define RG_BANDGAPGEN_MASK                             0x1
#define RG_BANDGAPGEN_MASK_SFT                         (0x1 << 10)
#define RG_AUDMICBIAS1HVEN_ADDR                        \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDMICBIAS1HVEN_SFT                         12
#define RG_AUDMICBIAS1HVEN_MASK                        0x1
#define RG_AUDMICBIAS1HVEN_MASK_SFT                    (0x1 << 12)
#define RG_AUDMICBIAS1HVVREF_ADDR                      \
	MT6359_AUDENC_ANA_CON16
#define RG_AUDMICBIAS1HVVREF_SFT                       13
#define RG_AUDMICBIAS1HVVREF_MASK                      0x1
#define RG_AUDMICBIAS1HVVREF_MASK_SFT                  (0x1 << 13)

#define RG_EINT0NOHYS_ADDR                             \
	MT6359_AUDENC_ANA_CON18
#define RG_EINT0NOHYS_SFT                              10
#define RG_EINT0NOHYS_MASK                             0x1
#define RG_EINT0NOHYS_MASK_SFT                         (0x1 << 10)
#define RG_EINT0CONFIGACCDET_ADDR                      \
	MT6359_AUDENC_ANA_CON18
#define RG_EINT0CONFIGACCDET_SFT                       11
#define RG_EINT0CONFIGACCDET_MASK                      0x1
#define RG_EINT0CONFIGACCDET_MASK_SFT                  (0x1 << 11)
#define RG_EINT0HIRENB_ADDR                            \
	MT6359_AUDENC_ANA_CON18
#define RG_EINT0HIRENB_SFT                             12
#define RG_EINT0HIRENB_MASK                            0x1
#define RG_EINT0HIRENB_MASK_SFT                        (0x1 << 12)
#define RG_ACCDET2AUXRESBYPASS_ADDR                    \
	MT6359_AUDENC_ANA_CON18
#define RG_ACCDET2AUXRESBYPASS_SFT                     13
#define RG_ACCDET2AUXRESBYPASS_MASK                    0x1
#define RG_ACCDET2AUXRESBYPASS_MASK_SFT                (0x1 << 13)
#define RG_ACCDET2AUXSWEN_ADDR                         \
	MT6359_AUDENC_ANA_CON18
#define RG_ACCDET2AUXSWEN_SFT                          14
#define RG_ACCDET2AUXSWEN_MASK                         0x1
#define RG_ACCDET2AUXSWEN_MASK_SFT                     (0x1 << 14)
#define RG_AUDACCDETMICBIAS3PULLLOW_ADDR               \
	MT6359_AUDENC_ANA_CON18
#define RG_AUDACCDETMICBIAS3PULLLOW_SFT                15
#define RG_AUDACCDETMICBIAS3PULLLOW_MASK               0x1
#define RG_AUDACCDETMICBIAS3PULLLOW_MASK_SFT           (0x1 << 15)
#define RG_EINT1CONFIGACCDET_ADDR                      \
	MT6359_AUDENC_ANA_CON19
#define RG_EINT1CONFIGACCDET_SFT                       0
#define RG_EINT1CONFIGACCDET_MASK                      0x1
#define RG_EINT1CONFIGACCDET_MASK_SFT                  (0x1 << 0)
#define RG_EINT1HIRENB_ADDR                            \
	MT6359_AUDENC_ANA_CON19
#define RG_EINT1HIRENB_SFT                             1
#define RG_EINT1HIRENB_MASK                            0x1
#define RG_EINT1HIRENB_MASK_SFT                        (0x1 << 1)
#define RG_EINT1NOHYS_ADDR                             \
	MT6359_AUDENC_ANA_CON19
#define RG_EINT1NOHYS_SFT                              2
#define RG_EINT1NOHYS_MASK                             0x1
#define RG_EINT1NOHYS_MASK_SFT                         (0x1 << 2)
#define RG_EINTCOMPVTH_ADDR                            \
	MT6359_AUDENC_ANA_CON19
#define RG_MTEST_EN_ADDR                               \
	MT6359_AUDENC_ANA_CON19
#define RG_MTEST_EN_SFT                                8
#define RG_MTEST_EN_MASK                               0x1
#define RG_MTEST_EN_MASK_SFT                           (0x1 << 8)
#define RG_MTEST_SEL_ADDR                              \
	MT6359_AUDENC_ANA_CON19
#define RG_MTEST_SEL_SFT                               9
#define RG_MTEST_SEL_MASK                              0x1
#define RG_MTEST_SEL_MASK_SFT                          (0x1 << 9)
#define RG_MTEST_CURRENT_ADDR                          \
	MT6359_AUDENC_ANA_CON19
#define RG_MTEST_CURRENT_SFT                           10
#define RG_MTEST_CURRENT_MASK                          0x1
#define RG_MTEST_CURRENT_MASK_SFT                      (0x1 << 10)
#define RG_ANALOGFDEN_ADDR                             \
	MT6359_AUDENC_ANA_CON19
#define RG_ANALOGFDEN_SFT                              12
#define RG_ANALOGFDEN_MASK                             0x1
#define RG_ANALOGFDEN_MASK_SFT                         (0x1 << 12)
#define RG_FDVIN1PPULLLOW_ADDR                         \
	MT6359_AUDENC_ANA_CON19
#define RG_FDVIN1PPULLLOW_SFT                          13
#define RG_FDVIN1PPULLLOW_MASK                         0x1
#define RG_FDVIN1PPULLLOW_MASK_SFT                     (0x1 << 13)
#define RG_FDEINT0TYPE_ADDR                            \
	MT6359_AUDENC_ANA_CON19
#define RG_FDEINT0TYPE_SFT                             14
#define RG_FDEINT0TYPE_MASK                            0x1
#define RG_FDEINT0TYPE_MASK_SFT                        (0x1 << 14)
#define RG_FDEINT1TYPE_ADDR                            \
	MT6359_AUDENC_ANA_CON19
#define RG_FDEINT1TYPE_SFT                             15
#define RG_FDEINT1TYPE_MASK                            0x1
#define RG_FDEINT1TYPE_MASK_SFT                        (0x1 << 15)
#define RG_EINT0CMPEN_ADDR                             \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT0CMPEN_SFT                              0
#define RG_EINT0CMPEN_MASK                             0x1
#define RG_EINT0CMPEN_MASK_SFT                         (0x1 << 0)
#define RG_EINT0CMPMEN_ADDR                            \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT0CMPMEN_SFT                             1
#define RG_EINT0CMPMEN_MASK                            0x1
#define RG_EINT0CMPMEN_MASK_SFT                        (0x1 << 1)
#define RG_EINT0EN_ADDR                                \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT0EN_SFT                                 2
#define RG_EINT0EN_MASK                                0x1
#define RG_EINT0EN_MASK_SFT                            (0x1 << 2)
#define RG_EINT0CEN_ADDR                               \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT0CEN_SFT                                3
#define RG_EINT0CEN_MASK                               0x1
#define RG_EINT0CEN_MASK_SFT                           (0x1 << 3)
#define RG_EINT0INVEN_ADDR                             \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT0INVEN_SFT                              4
#define RG_EINT0INVEN_MASK                             0x1
#define RG_EINT0INVEN_MASK_SFT                         (0x1 << 4)
#define RG_EINT0CTURBO_ADDR                            \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT0CTURBO_SFT                             5
#define RG_EINT0CTURBO_MASK                            0x7
#define RG_EINT0CTURBO_MASK_SFT                        (0x7 << 5)
#define RG_EINT1CMPEN_ADDR                             \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT1CMPEN_SFT                              8
#define RG_EINT1CMPEN_MASK                             0x1
#define RG_EINT1CMPEN_MASK_SFT                         (0x1 << 8)
#define RG_EINT1CMPMEN_ADDR                            \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT1CMPMEN_SFT                             9
#define RG_EINT1CMPMEN_MASK                            0x1
#define RG_EINT1CMPMEN_MASK_SFT                        (0x1 << 9)
#define RG_EINT1EN_ADDR                                \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT1EN_SFT                                 10
#define RG_EINT1EN_MASK                                0x1
#define RG_EINT1EN_MASK_SFT                            (0x1 << 10)
#define RG_EINT1CEN_ADDR                               \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT1CEN_SFT                                11
#define RG_EINT1CEN_MASK                               0x1
#define RG_EINT1CEN_MASK_SFT                           (0x1 << 11)
#define RG_EINT1INVEN_ADDR                             \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT1INVEN_SFT                              12
#define RG_EINT1INVEN_MASK                             0x1
#define RG_EINT1INVEN_MASK_SFT                         (0x1 << 12)
#define RG_EINT1CTURBO_ADDR                            \
	MT6359_AUDENC_ANA_CON20
#define RG_EINT1CTURBO_SFT                             13
#define RG_EINT1CTURBO_MASK                            0x7
#define RG_EINT1CTURBO_MASK_SFT                        (0x7 << 13)
#define RG_ACCDETSPARE_ADDR                            \
	MT6359_AUDENC_ANA_CON21

#define ACCDET_ANA_ID_ADDR                             \
	MT6359_ACCDET_DSN_DIG_ID
#define ACCDET_ANA_ID_SFT                              0
#define ACCDET_ANA_ID_MASK                             0xFF
#define ACCDET_ANA_ID_MASK_SFT                         (0xFF << 0)
#define ACCDET_DIG_ID_ADDR                             \
	MT6359_ACCDET_DSN_DIG_ID
#define ACCDET_DIG_ID_SFT                              8
#define ACCDET_DIG_ID_MASK                             0xFF
#define ACCDET_DIG_ID_MASK_SFT                         (0xFF << 8)
#define ACCDET_ANA_MINOR_REV_ADDR                      \
	MT6359_ACCDET_DSN_DIG_REV0
#define ACCDET_ANA_MINOR_REV_SFT                       0
#define ACCDET_ANA_MINOR_REV_MASK                      0xF
#define ACCDET_ANA_MINOR_REV_MASK_SFT                  (0xF << 0)
#define ACCDET_ANA_MAJOR_REV_ADDR                      \
	MT6359_ACCDET_DSN_DIG_REV0
#define ACCDET_ANA_MAJOR_REV_SFT                       4
#define ACCDET_ANA_MAJOR_REV_MASK                      0xF
#define ACCDET_ANA_MAJOR_REV_MASK_SFT                  (0xF << 4)
#define ACCDET_DIG_MINOR_REV_ADDR                      \
	MT6359_ACCDET_DSN_DIG_REV0
#define ACCDET_DIG_MINOR_REV_SFT                       8
#define ACCDET_DIG_MINOR_REV_MASK                      0xF
#define ACCDET_DIG_MINOR_REV_MASK_SFT                  (0xF << 8)
#define ACCDET_DIG_MAJOR_REV_ADDR                      \
	MT6359_ACCDET_DSN_DIG_REV0
#define ACCDET_DIG_MAJOR_REV_SFT                       12
#define ACCDET_DIG_MAJOR_REV_MASK                      0xF
#define ACCDET_DIG_MAJOR_REV_MASK_SFT                  (0xF << 12)
#define ACCDET_DSN_CBS_ADDR                            \
	MT6359_ACCDET_DSN_DBI
#define ACCDET_DSN_CBS_SFT                             0
#define ACCDET_DSN_CBS_MASK                            0x3
#define ACCDET_DSN_CBS_MASK_SFT                        (0x3 << 0)
#define ACCDET_DSN_BIX_ADDR                            \
	MT6359_ACCDET_DSN_DBI
#define ACCDET_DSN_BIX_SFT                             2
#define ACCDET_DSN_BIX_MASK                            0x3
#define ACCDET_DSN_BIX_MASK_SFT                        (0x3 << 2)
#define ACCDET_ESP_ADDR                                \
	MT6359_ACCDET_DSN_DBI
#define ACCDET_ESP_SFT                                 8
#define ACCDET_ESP_MASK                                0xFF
#define ACCDET_ESP_MASK_SFT                            (0xFF << 8)
#define ACCDET_DSN_FPI_ADDR                            \
	MT6359_ACCDET_DSN_FPI
#define ACCDET_DSN_FPI_SFT                             0
#define ACCDET_DSN_FPI_MASK                            0xFF
#define ACCDET_DSN_FPI_MASK_SFT                        (0xFF << 0)
#define ACCDET_AUXADC_SEL_ADDR                         \
	MT6359_ACCDET_CON0
#define ACCDET_AUXADC_SEL_SFT                          0
#define ACCDET_AUXADC_SEL_MASK                         0x1
#define ACCDET_AUXADC_SEL_MASK_SFT                     (0x1 << 0)
#define ACCDET_AUXADC_SW_ADDR                          \
	MT6359_ACCDET_CON0
#define ACCDET_AUXADC_SW_SFT                           1
#define ACCDET_AUXADC_SW_MASK                          0x1
#define ACCDET_AUXADC_SW_MASK_SFT                      (0x1 << 1)
#define ACCDET_TEST_AUXADC_ADDR                        \
	MT6359_ACCDET_CON0
#define ACCDET_TEST_AUXADC_SFT                         2
#define ACCDET_TEST_AUXADC_MASK                        0x1
#define ACCDET_TEST_AUXADC_MASK_SFT                    (0x1 << 2)
#define ACCDET_AUXADC_ANASWCTRL_SEL_ADDR               \
	MT6359_ACCDET_CON0
#define ACCDET_AUXADC_ANASWCTRL_SEL_SFT                8
#define ACCDET_AUXADC_ANASWCTRL_SEL_MASK               0x1
#define ACCDET_AUXADC_ANASWCTRL_SEL_MASK_SFT           (0x1 << 8)
#define AUDACCDETAUXADCSWCTRL_SEL_ADDR                 \
	MT6359_ACCDET_CON0
#define AUDACCDETAUXADCSWCTRL_SEL_SFT                  9
#define AUDACCDETAUXADCSWCTRL_SEL_MASK                 0x1
#define AUDACCDETAUXADCSWCTRL_SEL_MASK_SFT             (0x1 << 9)
#define AUDACCDETAUXADCSWCTRL_SW_ADDR                  \
	MT6359_ACCDET_CON0
#define AUDACCDETAUXADCSWCTRL_SW_SFT                   10
#define AUDACCDETAUXADCSWCTRL_SW_MASK                  0x1
#define AUDACCDETAUXADCSWCTRL_SW_MASK_SFT              (0x1 << 10)
#define ACCDET_TEST_ANA_ADDR                           \
	MT6359_ACCDET_CON0
#define ACCDET_TEST_ANA_SFT                            11
#define ACCDET_TEST_ANA_MASK                           0x1
#define ACCDET_TEST_ANA_MASK_SFT                       (0x1 << 11)
#define RG_AUDACCDETRSV_ADDR                           \
	MT6359_ACCDET_CON0
#define RG_AUDACCDETRSV_SFT                            13
#define RG_AUDACCDETRSV_MASK                           0x3
#define RG_AUDACCDETRSV_MASK_SFT                       (0x3 << 13)
#define ACCDET_SW_EN_ADDR                              \
	MT6359_ACCDET_CON1
#define ACCDET_SW_EN_SFT                               0
#define ACCDET_SW_EN_MASK                              0x1
#define ACCDET_SW_EN_MASK_SFT                          (0x1 << 0)
#define ACCDET_SEQ_INIT_ADDR                           \
	MT6359_ACCDET_CON1
#define ACCDET_SEQ_INIT_SFT                            1
#define ACCDET_SEQ_INIT_MASK                           0x1
#define ACCDET_SEQ_INIT_MASK_SFT                       (0x1 << 1)
#define ACCDET_EINT0_SW_EN_ADDR                        \
	MT6359_ACCDET_CON1
#define ACCDET_EINT0_SW_EN_SFT                         2
#define ACCDET_EINT0_SW_EN_MASK                        0x1
#define ACCDET_EINT0_SW_EN_MASK_SFT                    (0x1 << 2)
#define ACCDET_EINT0_SEQ_INIT_ADDR                     \
	MT6359_ACCDET_CON1
#define ACCDET_EINT0_SEQ_INIT_SFT                      3
#define ACCDET_EINT0_SEQ_INIT_MASK                     0x1
#define ACCDET_EINT0_SEQ_INIT_MASK_SFT                 (0x1 << 3)
#define ACCDET_EINT1_SW_EN_ADDR                        \
	MT6359_ACCDET_CON1
#define ACCDET_EINT1_SW_EN_SFT                         4
#define ACCDET_EINT1_SW_EN_MASK                        0x1
#define ACCDET_EINT1_SW_EN_MASK_SFT                    (0x1 << 4)
#define ACCDET_EINT1_SEQ_INIT_ADDR                     \
	MT6359_ACCDET_CON1
#define ACCDET_EINT1_SEQ_INIT_SFT                      5
#define ACCDET_EINT1_SEQ_INIT_MASK                     0x1
#define ACCDET_EINT1_SEQ_INIT_MASK_SFT                 (0x1 << 5)
#define ACCDET_EINT0_INVERTER_SW_EN_ADDR               \
	MT6359_ACCDET_CON1
#define ACCDET_EINT0_INVERTER_SW_EN_SFT                6
#define ACCDET_EINT0_INVERTER_SW_EN_MASK               0x1
#define ACCDET_EINT0_INVERTER_SW_EN_MASK_SFT           (0x1 << 6)
#define ACCDET_EINT0_INVERTER_SEQ_INIT_ADDR            \
	MT6359_ACCDET_CON1
#define ACCDET_EINT0_INVERTER_SEQ_INIT_SFT             7
#define ACCDET_EINT0_INVERTER_SEQ_INIT_MASK            0x1
#define ACCDET_EINT0_INVERTER_SEQ_INIT_MASK_SFT        (0x1 << 7)
#define ACCDET_EINT1_INVERTER_SW_EN_ADDR               \
	MT6359_ACCDET_CON1
#define ACCDET_EINT1_INVERTER_SW_EN_SFT                8
#define ACCDET_EINT1_INVERTER_SW_EN_MASK               0x1
#define ACCDET_EINT1_INVERTER_SW_EN_MASK_SFT           (0x1 << 8)
#define ACCDET_EINT1_INVERTER_SEQ_INIT_ADDR            \
	MT6359_ACCDET_CON1
#define ACCDET_EINT1_INVERTER_SEQ_INIT_SFT             9
#define ACCDET_EINT1_INVERTER_SEQ_INIT_MASK            0x1
#define ACCDET_EINT1_INVERTER_SEQ_INIT_MASK_SFT        (0x1 << 9)
#define ACCDET_EINT0_M_SW_EN_ADDR                      \
	MT6359_ACCDET_CON1
#define ACCDET_EINT0_M_SW_EN_SFT                       10
#define ACCDET_EINT0_M_SW_EN_MASK                      0x1
#define ACCDET_EINT0_M_SW_EN_MASK_SFT                  (0x1 << 10)
#define ACCDET_EINT1_M_SW_EN_ADDR                      \
	MT6359_ACCDET_CON1
#define ACCDET_EINT1_M_SW_EN_SFT                       11
#define ACCDET_EINT1_M_SW_EN_MASK                      0x1
#define ACCDET_EINT1_M_SW_EN_MASK_SFT                  (0x1 << 11)
#define ACCDET_EINT_M_DETECT_EN_ADDR                   \
	MT6359_ACCDET_CON1
#define ACCDET_EINT_M_DETECT_EN_SFT                    12
#define ACCDET_EINT_M_DETECT_EN_MASK                   0x1
#define ACCDET_EINT_M_DETECT_EN_MASK_SFT               (0x1 << 12)
#define ACCDET_CMP_PWM_EN_ADDR                         \
	MT6359_ACCDET_CON2
#define ACCDET_CMP_PWM_EN_SFT                          0
#define ACCDET_CMP_PWM_EN_MASK                         0x1
#define ACCDET_CMP_PWM_EN_MASK_SFT                     (0x1 << 0)
#define ACCDET_VTH_PWM_EN_ADDR                         \
	MT6359_ACCDET_CON2
#define ACCDET_VTH_PWM_EN_SFT                          1
#define ACCDET_VTH_PWM_EN_MASK                         0x1
#define ACCDET_VTH_PWM_EN_MASK_SFT                     (0x1 << 1)
#define ACCDET_MBIAS_PWM_EN_ADDR                       \
	MT6359_ACCDET_CON2
#define ACCDET_MBIAS_PWM_EN_SFT                        2
#define ACCDET_MBIAS_PWM_EN_MASK                       0x1
#define ACCDET_MBIAS_PWM_EN_MASK_SFT                   (0x1 << 2)
#define ACCDET_EINT_EN_PWM_EN_ADDR                     \
	MT6359_ACCDET_CON2
#define ACCDET_EINT_EN_PWM_EN_SFT                      3
#define ACCDET_EINT_EN_PWM_EN_MASK                     0x1
#define ACCDET_EINT_EN_PWM_EN_MASK_SFT                 (0x1 << 3)
#define ACCDET_EINT_CMPEN_PWM_EN_ADDR                  \
	MT6359_ACCDET_CON2
#define ACCDET_EINT_CMPEN_PWM_EN_SFT                   4
#define ACCDET_EINT_CMPEN_PWM_EN_MASK                  0x1
#define ACCDET_EINT_CMPEN_PWM_EN_MASK_SFT              (0x1 << 4)
#define ACCDET_EINT_CMPMEN_PWM_EN_ADDR                 \
	MT6359_ACCDET_CON2
#define ACCDET_EINT_CMPMEN_PWM_EN_SFT                  5
#define ACCDET_EINT_CMPMEN_PWM_EN_MASK                 0x1
#define ACCDET_EINT_CMPMEN_PWM_EN_MASK_SFT             (0x1 << 5)
#define ACCDET_EINT_CTURBO_PWM_EN_ADDR                 \
	MT6359_ACCDET_CON2
#define ACCDET_EINT_CTURBO_PWM_EN_SFT                  6
#define ACCDET_EINT_CTURBO_PWM_EN_MASK                 0x1
#define ACCDET_EINT_CTURBO_PWM_EN_MASK_SFT             (0x1 << 6)
#define ACCDET_CMP_PWM_IDLE_ADDR                       \
	MT6359_ACCDET_CON2
#define ACCDET_CMP_PWM_IDLE_SFT                        8
#define ACCDET_CMP_PWM_IDLE_MASK                       0x1
#define ACCDET_CMP_PWM_IDLE_MASK_SFT                   (0x1 << 8)
#define ACCDET_VTH_PWM_IDLE_ADDR                       \
	MT6359_ACCDET_CON2
#define ACCDET_VTH_PWM_IDLE_SFT                        9
#define ACCDET_VTH_PWM_IDLE_MASK                       0x1
#define ACCDET_VTH_PWM_IDLE_MASK_SFT                   (0x1 << 9)
#define ACCDET_MBIAS_PWM_IDLE_ADDR                     \
	MT6359_ACCDET_CON2
#define ACCDET_MBIAS_PWM_IDLE_SFT                      10
#define ACCDET_MBIAS_PWM_IDLE_MASK                     0x1
#define ACCDET_MBIAS_PWM_IDLE_MASK_SFT                 (0x1 << 10)
#define ACCDET_EINT0_CMPEN_PWM_IDLE_ADDR               \
	MT6359_ACCDET_CON2
#define ACCDET_EINT0_CMPEN_PWM_IDLE_SFT                11
#define ACCDET_EINT0_CMPEN_PWM_IDLE_MASK               0x1
#define ACCDET_EINT0_CMPEN_PWM_IDLE_MASK_SFT           (0x1 << 11)
#define ACCDET_EINT1_CMPEN_PWM_IDLE_ADDR               \
	MT6359_ACCDET_CON2
#define ACCDET_EINT1_CMPEN_PWM_IDLE_SFT                12
#define ACCDET_EINT1_CMPEN_PWM_IDLE_MASK               0x1
#define ACCDET_EINT1_CMPEN_PWM_IDLE_MASK_SFT           (0x1 << 12)
#define ACCDET_PWM_EN_SW_ADDR                          \
	MT6359_ACCDET_CON2
#define ACCDET_PWM_EN_SW_SFT                           13
#define ACCDET_PWM_EN_SW_MASK                          0x1
#define ACCDET_PWM_EN_SW_MASK_SFT                      (0x1 << 13)
#define ACCDET_PWM_EN_SEL_ADDR                         \
	MT6359_ACCDET_CON2
#define ACCDET_PWM_EN_SEL_SFT                          14
#define ACCDET_PWM_EN_SEL_MASK                         0x3
#define ACCDET_PWM_EN_SEL_MASK_SFT                     (0x3 << 14)
#define ACCDET_PWM_WIDTH_ADDR                          \
	MT6359_ACCDET_CON3
#define ACCDET_PWM_WIDTH_SFT                           0
#define ACCDET_PWM_WIDTH_MASK                          0xFFFF
#define ACCDET_PWM_WIDTH_MASK_SFT                      (0xFFFF << 0)
#define ACCDET_PWM_THRESH_ADDR                         \
	MT6359_ACCDET_CON4
#define ACCDET_PWM_THRESH_SFT                          0
#define ACCDET_PWM_THRESH_MASK                         0xFFFF
#define ACCDET_PWM_THRESH_MASK_SFT                     (0xFFFF << 0)
#define ACCDET_RISE_DELAY_ADDR                         \
	MT6359_ACCDET_CON5
#define ACCDET_RISE_DELAY_SFT                          0
#define ACCDET_RISE_DELAY_MASK                         0x7FFF
#define ACCDET_RISE_DELAY_MASK_SFT                     (0x7FFF << 0)
#define ACCDET_FALL_DELAY_ADDR                         \
	MT6359_ACCDET_CON5
#define ACCDET_FALL_DELAY_SFT                          15
#define ACCDET_FALL_DELAY_MASK                         0x1
#define ACCDET_FALL_DELAY_MASK_SFT                     (0x1 << 15)
#define ACCDET_EINT_CMPMEN_PWM_THRESH_ADDR             \
	MT6359_ACCDET_CON6
#define ACCDET_EINT_CMPMEN_PWM_THRESH_SFT              0
#define ACCDET_EINT_CMPMEN_PWM_THRESH_MASK             0x7
#define ACCDET_EINT_CMPMEN_PWM_THRESH_MASK_SFT         (0x7 << 0)
#define ACCDET_EINT_CMPMEN_PWM_WIDTH_ADDR              \
	MT6359_ACCDET_CON6
#define ACCDET_EINT_CMPMEN_PWM_WIDTH_SFT               4
#define ACCDET_EINT_CMPMEN_PWM_WIDTH_MASK              0x7
#define ACCDET_EINT_CMPMEN_PWM_WIDTH_MASK_SFT          (0x7 << 4)
#define ACCDET_EINT_EN_PWM_THRESH_ADDR                 \
	MT6359_ACCDET_CON7
#define ACCDET_EINT_EN_PWM_THRESH_SFT                  0
#define ACCDET_EINT_EN_PWM_THRESH_MASK                 0x7
#define ACCDET_EINT_EN_PWM_THRESH_MASK_SFT             (0x7 << 0)
#define ACCDET_EINT_EN_PWM_WIDTH_ADDR                  \
	MT6359_ACCDET_CON7
#define ACCDET_EINT_EN_PWM_WIDTH_SFT                   4
#define ACCDET_EINT_EN_PWM_WIDTH_MASK                  0x3
#define ACCDET_EINT_EN_PWM_WIDTH_MASK_SFT              (0x3 << 4)
#define ACCDET_EINT_CMPEN_PWM_THRESH_ADDR              \
	MT6359_ACCDET_CON7
#define ACCDET_EINT_CMPEN_PWM_THRESH_SFT               8
#define ACCDET_EINT_CMPEN_PWM_THRESH_MASK              0x7
#define ACCDET_EINT_CMPEN_PWM_THRESH_MASK_SFT          (0x7 << 8)
#define ACCDET_EINT_CMPEN_PWM_WIDTH_ADDR               \
	MT6359_ACCDET_CON7
#define ACCDET_EINT_CMPEN_PWM_WIDTH_SFT                12
#define ACCDET_EINT_CMPEN_PWM_WIDTH_MASK               0x3
#define ACCDET_EINT_CMPEN_PWM_WIDTH_MASK_SFT           (0x3 << 12)
#define ACCDET_DEBOUNCE0_ADDR                          \
	MT6359_ACCDET_CON8
#define ACCDET_DEBOUNCE0_SFT                           0
#define ACCDET_DEBOUNCE0_MASK                          0xFFFF
#define ACCDET_DEBOUNCE0_MASK_SFT                      (0xFFFF << 0)
#define ACCDET_DEBOUNCE1_ADDR                          \
	MT6359_ACCDET_CON9
#define ACCDET_DEBOUNCE1_SFT                           0
#define ACCDET_DEBOUNCE1_MASK                          0xFFFF
#define ACCDET_DEBOUNCE1_MASK_SFT                      (0xFFFF << 0)
#define ACCDET_DEBOUNCE2_ADDR                          \
	MT6359_ACCDET_CON10
#define ACCDET_DEBOUNCE2_SFT                           0
#define ACCDET_DEBOUNCE2_MASK                          0xFFFF
#define ACCDET_DEBOUNCE2_MASK_SFT                      (0xFFFF << 0)
#define ACCDET_DEBOUNCE3_ADDR                          \
	MT6359_ACCDET_CON11
#define ACCDET_DEBOUNCE3_SFT                           0
#define ACCDET_DEBOUNCE3_MASK                          0xFFFF
#define ACCDET_DEBOUNCE3_MASK_SFT                      (0xFFFF << 0)
#define ACCDET_CONNECT_AUXADC_TIME_DIG_ADDR            \
	MT6359_ACCDET_CON12
#define ACCDET_CONNECT_AUXADC_TIME_DIG_SFT             0
#define ACCDET_CONNECT_AUXADC_TIME_DIG_MASK            0xFFFF
#define ACCDET_CONNECT_AUXADC_TIME_DIG_MASK_SFT        (0xFFFF << 0)
#define ACCDET_CONNECT_AUXADC_TIME_ANA_ADDR            \
	MT6359_ACCDET_CON13
#define ACCDET_CONNECT_AUXADC_TIME_ANA_SFT             0
#define ACCDET_CONNECT_AUXADC_TIME_ANA_MASK            0xFFFF
#define ACCDET_CONNECT_AUXADC_TIME_ANA_MASK_SFT        (0xFFFF << 0)
#define ACCDET_EINT_DEBOUNCE0_ADDR                     \
	MT6359_ACCDET_CON14
#define ACCDET_EINT_DEBOUNCE0_SFT                      0
#define ACCDET_EINT_DEBOUNCE0_MASK                     0xF
#define ACCDET_EINT_DEBOUNCE0_MASK_SFT                 (0xF << 0)
#define ACCDET_EINT_DEBOUNCE1_ADDR                     \
	MT6359_ACCDET_CON14
#define ACCDET_EINT_DEBOUNCE1_SFT                      4
#define ACCDET_EINT_DEBOUNCE1_MASK                     0xF
#define ACCDET_EINT_DEBOUNCE1_MASK_SFT                 (0xF << 4)
#define ACCDET_EINT_DEBOUNCE2_ADDR                     \
	MT6359_ACCDET_CON14
#define ACCDET_EINT_DEBOUNCE2_SFT                      8
#define ACCDET_EINT_DEBOUNCE2_MASK                     0xF
#define ACCDET_EINT_DEBOUNCE2_MASK_SFT                 (0xF << 8)
#define ACCDET_EINT_DEBOUNCE3_ADDR                     \
	MT6359_ACCDET_CON14
#define ACCDET_EINT_DEBOUNCE3_SFT                      12
#define ACCDET_EINT_DEBOUNCE3_MASK                     0xF
#define ACCDET_EINT_DEBOUNCE3_MASK_SFT                 (0xF << 12)
#define ACCDET_EINT_INVERTER_DEBOUNCE_ADDR             \
	MT6359_ACCDET_CON15
#define ACCDET_EINT_INVERTER_DEBOUNCE_SFT              0
#define ACCDET_EINT_INVERTER_DEBOUNCE_MASK             0xF
#define ACCDET_EINT_INVERTER_DEBOUNCE_MASK_SFT         (0xF << 0)
#define ACCDET_IVAL_CUR_IN_ADDR                        \
	MT6359_ACCDET_CON16
#define ACCDET_IVAL_CUR_IN_SFT                         0
#define ACCDET_IVAL_CUR_IN_MASK                        0x3
#define ACCDET_IVAL_CUR_IN_MASK_SFT                    (0x3 << 0)
#define ACCDET_IVAL_SAM_IN_ADDR                        \
	MT6359_ACCDET_CON16
#define ACCDET_IVAL_SAM_IN_SFT                         2
#define ACCDET_IVAL_SAM_IN_MASK                        0x3
#define ACCDET_IVAL_SAM_IN_MASK_SFT                    (0x3 << 2)
#define ACCDET_IVAL_MEM_IN_ADDR                        \
	MT6359_ACCDET_CON16
#define ACCDET_IVAL_MEM_IN_SFT                         4
#define ACCDET_IVAL_MEM_IN_MASK                        0x3
#define ACCDET_IVAL_MEM_IN_MASK_SFT                    (0x3 << 4)
#define ACCDET_EINT_IVAL_CUR_IN_ADDR                   \
	MT6359_ACCDET_CON16
#define ACCDET_EINT_IVAL_CUR_IN_SFT                    6
#define ACCDET_EINT_IVAL_CUR_IN_MASK                   0x3
#define ACCDET_EINT_IVAL_CUR_IN_MASK_SFT               (0x3 << 6)
#define ACCDET_EINT_IVAL_SAM_IN_ADDR                   \
	MT6359_ACCDET_CON16
#define ACCDET_EINT_IVAL_SAM_IN_SFT                    8
#define ACCDET_EINT_IVAL_SAM_IN_MASK                   0x3
#define ACCDET_EINT_IVAL_SAM_IN_MASK_SFT               (0x3 << 8)
#define ACCDET_EINT_IVAL_MEM_IN_ADDR                   \
	MT6359_ACCDET_CON16
#define ACCDET_EINT_IVAL_MEM_IN_SFT                    10
#define ACCDET_EINT_IVAL_MEM_IN_MASK                   0x3
#define ACCDET_EINT_IVAL_MEM_IN_MASK_SFT               (0x3 << 10)
#define ACCDET_IVAL_SEL_ADDR                           \
	MT6359_ACCDET_CON16
#define ACCDET_IVAL_SEL_SFT                            12
#define ACCDET_IVAL_SEL_MASK                           0x1
#define ACCDET_IVAL_SEL_MASK_SFT                       (0x1 << 12)
#define ACCDET_EINT_IVAL_SEL_ADDR                      \
	MT6359_ACCDET_CON16
#define ACCDET_EINT_IVAL_SEL_SFT                       13
#define ACCDET_EINT_IVAL_SEL_MASK                      0x1
#define ACCDET_EINT_IVAL_SEL_MASK_SFT                  (0x1 << 13)
#define ACCDET_EINT_INVERTER_IVAL_CUR_IN_ADDR          \
	MT6359_ACCDET_CON17
#define ACCDET_EINT_INVERTER_IVAL_CUR_IN_SFT           0
#define ACCDET_EINT_INVERTER_IVAL_CUR_IN_MASK          0x1
#define ACCDET_EINT_INVERTER_IVAL_CUR_IN_MASK_SFT      (0x1 << 0)
#define ACCDET_EINT_INVERTER_IVAL_SAM_IN_ADDR          \
	MT6359_ACCDET_CON17
#define ACCDET_EINT_INVERTER_IVAL_SAM_IN_SFT           1
#define ACCDET_EINT_INVERTER_IVAL_SAM_IN_MASK          0x1
#define ACCDET_EINT_INVERTER_IVAL_SAM_IN_MASK_SFT      (0x1 << 1)
#define ACCDET_EINT_INVERTER_IVAL_MEM_IN_ADDR          \
	MT6359_ACCDET_CON17
#define ACCDET_EINT_INVERTER_IVAL_MEM_IN_SFT           2
#define ACCDET_EINT_INVERTER_IVAL_MEM_IN_MASK          0x1
#define ACCDET_EINT_INVERTER_IVAL_MEM_IN_MASK_SFT      (0x1 << 2)
#define ACCDET_EINT_INVERTER_IVAL_SEL_ADDR             \
	MT6359_ACCDET_CON17
#define ACCDET_EINT_INVERTER_IVAL_SEL_SFT              3
#define ACCDET_EINT_INVERTER_IVAL_SEL_MASK             0x1
#define ACCDET_EINT_INVERTER_IVAL_SEL_MASK_SFT         (0x1 << 3)
#define ACCDET_IRQ_ADDR                                \
	MT6359_ACCDET_CON18
#define ACCDET_IRQ_SFT                                 0
#define ACCDET_IRQ_MASK                                0x1
#define ACCDET_IRQ_MASK_SFT                            (0x1 << 0)
#define ACCDET_EINT0_IRQ_ADDR                          \
	MT6359_ACCDET_CON18
#define ACCDET_EINT0_IRQ_SFT                           2
#define ACCDET_EINT0_IRQ_MASK                          0x1
#define ACCDET_EINT0_IRQ_MASK_SFT                      (0x1 << 2)
#define ACCDET_EINT1_IRQ_ADDR                          \
	MT6359_ACCDET_CON18
#define ACCDET_EINT1_IRQ_SFT                           3
#define ACCDET_EINT1_IRQ_MASK                          0x1
#define ACCDET_EINT1_IRQ_MASK_SFT                      (0x1 << 3)
#define ACCDET_EINT_IN_INVERSE_ADDR                    \
	MT6359_ACCDET_CON18
#define ACCDET_EINT_IN_INVERSE_SFT                     4
#define ACCDET_EINT_IN_INVERSE_MASK                    0x1
#define ACCDET_EINT_IN_INVERSE_MASK_SFT                (0x1 << 4)
#define ACCDET_IRQ_CLR_ADDR                            \
	MT6359_ACCDET_CON18
#define ACCDET_IRQ_CLR_SFT                             8
#define ACCDET_IRQ_CLR_MASK                            0x1
#define ACCDET_IRQ_CLR_MASK_SFT                        (0x1 << 8)
#define ACCDET_EINT0_IRQ_CLR_ADDR                      \
	MT6359_ACCDET_CON18
#define ACCDET_EINT0_IRQ_CLR_SFT                       10
#define ACCDET_EINT0_IRQ_CLR_MASK                      0x1
#define ACCDET_EINT0_IRQ_CLR_MASK_SFT                  (0x1 << 10)
#define ACCDET_EINT1_IRQ_CLR_ADDR                      \
	MT6359_ACCDET_CON18
#define ACCDET_EINT1_IRQ_CLR_SFT                       11
#define ACCDET_EINT1_IRQ_CLR_MASK                      0x1
#define ACCDET_EINT1_IRQ_CLR_MASK_SFT                  (0x1 << 11)
#define ACCDET_EINT_M_PLUG_IN_NUM_ADDR                 \
	MT6359_ACCDET_CON18
#define ACCDET_EINT_M_PLUG_IN_NUM_SFT                  12
#define ACCDET_EINT_M_PLUG_IN_NUM_MASK                 0x7
#define ACCDET_EINT_M_PLUG_IN_NUM_MASK_SFT             (0x7 << 12)
#define ACCDET_DA_STABLE_ADDR                          \
	MT6359_ACCDET_CON19
#define ACCDET_DA_STABLE_SFT                           0
#define ACCDET_DA_STABLE_MASK                          0x1
#define ACCDET_DA_STABLE_MASK_SFT                      (0x1 << 0)
#define ACCDET_EINT0_EN_STABLE_ADDR                    \
	MT6359_ACCDET_CON19
#define ACCDET_EINT0_EN_STABLE_SFT                     1
#define ACCDET_EINT0_EN_STABLE_MASK                    0x1
#define ACCDET_EINT0_EN_STABLE_MASK_SFT                (0x1 << 1)
#define ACCDET_EINT0_CMPEN_STABLE_ADDR                 \
	MT6359_ACCDET_CON19
#define ACCDET_EINT0_CMPEN_STABLE_SFT                  2
#define ACCDET_EINT0_CMPEN_STABLE_MASK                 0x1
#define ACCDET_EINT0_CMPEN_STABLE_MASK_SFT             (0x1 << 2)
#define ACCDET_EINT0_CMPMEN_STABLE_ADDR                \
	MT6359_ACCDET_CON19
#define ACCDET_EINT0_CMPMEN_STABLE_SFT                 3
#define ACCDET_EINT0_CMPMEN_STABLE_MASK                0x1
#define ACCDET_EINT0_CMPMEN_STABLE_MASK_SFT            (0x1 << 3)
#define ACCDET_EINT0_CTURBO_STABLE_ADDR                \
	MT6359_ACCDET_CON19
#define ACCDET_EINT0_CTURBO_STABLE_SFT                 4
#define ACCDET_EINT0_CTURBO_STABLE_MASK                0x1
#define ACCDET_EINT0_CTURBO_STABLE_MASK_SFT            (0x1 << 4)
#define ACCDET_EINT0_CEN_STABLE_ADDR                   \
	MT6359_ACCDET_CON19
#define ACCDET_EINT0_CEN_STABLE_SFT                    5
#define ACCDET_EINT0_CEN_STABLE_MASK                   0x1
#define ACCDET_EINT0_CEN_STABLE_MASK_SFT               (0x1 << 5)
#define ACCDET_EINT1_EN_STABLE_ADDR                    \
	MT6359_ACCDET_CON19
#define ACCDET_EINT1_EN_STABLE_SFT                     6
#define ACCDET_EINT1_EN_STABLE_MASK                    0x1
#define ACCDET_EINT1_EN_STABLE_MASK_SFT                (0x1 << 6)
#define ACCDET_EINT1_CMPEN_STABLE_ADDR                 \
	MT6359_ACCDET_CON19
#define ACCDET_EINT1_CMPEN_STABLE_SFT                  7
#define ACCDET_EINT1_CMPEN_STABLE_MASK                 0x1
#define ACCDET_EINT1_CMPEN_STABLE_MASK_SFT             (0x1 << 7)
#define ACCDET_EINT1_CMPMEN_STABLE_ADDR                \
	MT6359_ACCDET_CON19
#define ACCDET_EINT1_CMPMEN_STABLE_SFT                 8
#define ACCDET_EINT1_CMPMEN_STABLE_MASK                0x1
#define ACCDET_EINT1_CMPMEN_STABLE_MASK_SFT            (0x1 << 8)
#define ACCDET_EINT1_CTURBO_STABLE_ADDR                \
	MT6359_ACCDET_CON19
#define ACCDET_EINT1_CTURBO_STABLE_SFT                 9
#define ACCDET_EINT1_CTURBO_STABLE_MASK                0x1
#define ACCDET_EINT1_CTURBO_STABLE_MASK_SFT            (0x1 << 9)
#define ACCDET_EINT1_CEN_STABLE_ADDR                   \
	MT6359_ACCDET_CON19
#define ACCDET_EINT1_CEN_STABLE_SFT                    10
#define ACCDET_EINT1_CEN_STABLE_MASK                   0x1
#define ACCDET_EINT1_CEN_STABLE_MASK_SFT               (0x1 << 10)
#define ACCDET_HWMODE_EN_ADDR                          \
	MT6359_ACCDET_CON20
#define ACCDET_HWMODE_EN_SFT                           0
#define ACCDET_HWMODE_EN_MASK                          0x1
#define ACCDET_HWMODE_EN_MASK_SFT                      (0x1 << 0)
#define ACCDET_HWMODE_SEL_ADDR                         \
	MT6359_ACCDET_CON20
#define ACCDET_HWMODE_SEL_SFT                          1
#define ACCDET_HWMODE_SEL_MASK                         0x3
#define ACCDET_HWMODE_SEL_MASK_SFT                     (0x3 << 1)
#define ACCDET_PLUG_OUT_DETECT_ADDR                    \
	MT6359_ACCDET_CON20
#define ACCDET_PLUG_OUT_DETECT_SFT                     3
#define ACCDET_PLUG_OUT_DETECT_MASK                    0x1
#define ACCDET_PLUG_OUT_DETECT_MASK_SFT                (0x1 << 3)
#define ACCDET_EINT0_REVERSE_ADDR                      \
	MT6359_ACCDET_CON20
#define ACCDET_EINT0_REVERSE_SFT                       4
#define ACCDET_EINT0_REVERSE_MASK                      0x1
#define ACCDET_EINT0_REVERSE_MASK_SFT                  (0x1 << 4)
#define ACCDET_EINT1_REVERSE_ADDR                      \
	MT6359_ACCDET_CON20
#define ACCDET_EINT1_REVERSE_SFT                       5
#define ACCDET_EINT1_REVERSE_MASK                      0x1
#define ACCDET_EINT1_REVERSE_MASK_SFT                  (0x1 << 5)
#define ACCDET_EINT_HWMODE_EN_ADDR                     \
	MT6359_ACCDET_CON20
#define ACCDET_EINT_HWMODE_EN_SFT                      8
#define ACCDET_EINT_HWMODE_EN_MASK                     0x1
#define ACCDET_EINT_HWMODE_EN_MASK_SFT                 (0x1 << 8)
#define ACCDET_EINT_PLUG_OUT_BYPASS_DEB_ADDR           \
	MT6359_ACCDET_CON20
#define ACCDET_EINT_PLUG_OUT_BYPASS_DEB_SFT            9
#define ACCDET_EINT_PLUG_OUT_BYPASS_DEB_MASK           0x1
#define ACCDET_EINT_PLUG_OUT_BYPASS_DEB_MASK_SFT       (0x1 << 9)
#define ACCDET_EINT_M_PLUG_IN_EN_ADDR                  \
	MT6359_ACCDET_CON20
#define ACCDET_EINT_M_PLUG_IN_EN_SFT                   10
#define ACCDET_EINT_M_PLUG_IN_EN_MASK                  0x1
#define ACCDET_EINT_M_PLUG_IN_EN_MASK_SFT              (0x1 << 10)
#define ACCDET_EINT_M_HWMODE_EN_ADDR                   \
	MT6359_ACCDET_CON20
#define ACCDET_EINT_M_HWMODE_EN_SFT                    11
#define ACCDET_EINT_M_HWMODE_EN_MASK                   0x1
#define ACCDET_EINT_M_HWMODE_EN_MASK_SFT               (0x1 << 11)
#define ACCDET_TEST_CMPEN_ADDR                         \
	MT6359_ACCDET_CON21
#define ACCDET_TEST_CMPEN_SFT                          0
#define ACCDET_TEST_CMPEN_MASK                         0x1
#define ACCDET_TEST_CMPEN_MASK_SFT                     (0x1 << 0)
#define ACCDET_TEST_VTHEN_ADDR                         \
	MT6359_ACCDET_CON21
#define ACCDET_TEST_VTHEN_SFT                          1
#define ACCDET_TEST_VTHEN_MASK                         0x1
#define ACCDET_TEST_VTHEN_MASK_SFT                     (0x1 << 1)
#define ACCDET_TEST_MBIASEN_ADDR                       \
	MT6359_ACCDET_CON21
#define ACCDET_TEST_MBIASEN_SFT                        2
#define ACCDET_TEST_MBIASEN_MASK                       0x1
#define ACCDET_TEST_MBIASEN_MASK_SFT                   (0x1 << 2)
#define ACCDET_EINT_TEST_EN_ADDR                       \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_EN_SFT                        3
#define ACCDET_EINT_TEST_EN_MASK                       0x1
#define ACCDET_EINT_TEST_EN_MASK_SFT                   (0x1 << 3)
#define ACCDET_EINT_TEST_INVEN_ADDR                    \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_INVEN_SFT                     4
#define ACCDET_EINT_TEST_INVEN_MASK                    0x1
#define ACCDET_EINT_TEST_INVEN_MASK_SFT                (0x1 << 4)
#define ACCDET_EINT_TEST_CMPEN_ADDR                    \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_CMPEN_SFT                     5
#define ACCDET_EINT_TEST_CMPEN_MASK                    0x1
#define ACCDET_EINT_TEST_CMPEN_MASK_SFT                (0x1 << 5)
#define ACCDET_EINT_TEST_CMPMEN_ADDR                   \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_CMPMEN_SFT                    6
#define ACCDET_EINT_TEST_CMPMEN_MASK                   0x1
#define ACCDET_EINT_TEST_CMPMEN_MASK_SFT               (0x1 << 6)
#define ACCDET_EINT_TEST_CTURBO_ADDR                   \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_CTURBO_SFT                    7
#define ACCDET_EINT_TEST_CTURBO_MASK                   0x1
#define ACCDET_EINT_TEST_CTURBO_MASK_SFT               (0x1 << 7)
#define ACCDET_EINT_TEST_CEN_ADDR                      \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_CEN_SFT                       8
#define ACCDET_EINT_TEST_CEN_MASK                      0x1
#define ACCDET_EINT_TEST_CEN_MASK_SFT                  (0x1 << 8)
#define ACCDET_TEST_B_ADDR                             \
	MT6359_ACCDET_CON21
#define ACCDET_TEST_B_SFT                              9
#define ACCDET_TEST_B_MASK                             0x1
#define ACCDET_TEST_B_MASK_SFT                         (0x1 << 9)
#define ACCDET_TEST_A_ADDR                             \
	MT6359_ACCDET_CON21
#define ACCDET_TEST_A_SFT                              10
#define ACCDET_TEST_A_MASK                             0x1
#define ACCDET_TEST_A_MASK_SFT                         (0x1 << 10)
#define ACCDET_EINT_TEST_CMPOUT_ADDR                   \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_CMPOUT_SFT                    11
#define ACCDET_EINT_TEST_CMPOUT_MASK                   0x1
#define ACCDET_EINT_TEST_CMPOUT_MASK_SFT               (0x1 << 11)
#define ACCDET_EINT_TEST_CMPMOUT_ADDR                  \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_CMPMOUT_SFT                   12
#define ACCDET_EINT_TEST_CMPMOUT_MASK                  0x1
#define ACCDET_EINT_TEST_CMPMOUT_MASK_SFT              (0x1 << 12)
#define ACCDET_EINT_TEST_INVOUT_ADDR                   \
	MT6359_ACCDET_CON21
#define ACCDET_EINT_TEST_INVOUT_SFT                    13
#define ACCDET_EINT_TEST_INVOUT_MASK                   0x1
#define ACCDET_EINT_TEST_INVOUT_MASK_SFT               (0x1 << 13)
#define ACCDET_CMPEN_SEL_ADDR                          \
	MT6359_ACCDET_CON22
#define ACCDET_CMPEN_SEL_SFT                           0
#define ACCDET_CMPEN_SEL_MASK                          0x1
#define ACCDET_CMPEN_SEL_MASK_SFT                      (0x1 << 0)
#define ACCDET_VTHEN_SEL_ADDR                          \
	MT6359_ACCDET_CON22
#define ACCDET_VTHEN_SEL_SFT                           1
#define ACCDET_VTHEN_SEL_MASK                          0x1
#define ACCDET_VTHEN_SEL_MASK_SFT                      (0x1 << 1)
#define ACCDET_MBIASEN_SEL_ADDR                        \
	MT6359_ACCDET_CON22
#define ACCDET_MBIASEN_SEL_SFT                         2
#define ACCDET_MBIASEN_SEL_MASK                        0x1
#define ACCDET_MBIASEN_SEL_MASK_SFT                    (0x1 << 2)
#define ACCDET_EINT_EN_SEL_ADDR                        \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_EN_SEL_SFT                         3
#define ACCDET_EINT_EN_SEL_MASK                        0x1
#define ACCDET_EINT_EN_SEL_MASK_SFT                    (0x1 << 3)
#define ACCDET_EINT_INVEN_SEL_ADDR                     \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_INVEN_SEL_SFT                      4
#define ACCDET_EINT_INVEN_SEL_MASK                     0x1
#define ACCDET_EINT_INVEN_SEL_MASK_SFT                 (0x1 << 4)
#define ACCDET_EINT_CMPEN_SEL_ADDR                     \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_CMPEN_SEL_SFT                      5
#define ACCDET_EINT_CMPEN_SEL_MASK                     0x1
#define ACCDET_EINT_CMPEN_SEL_MASK_SFT                 (0x1 << 5)
#define ACCDET_EINT_CMPMEN_SEL_ADDR                    \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_CMPMEN_SEL_SFT                     6
#define ACCDET_EINT_CMPMEN_SEL_MASK                    0x1
#define ACCDET_EINT_CMPMEN_SEL_MASK_SFT                (0x1 << 6)
#define ACCDET_EINT_CTURBO_SEL_ADDR                    \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_CTURBO_SEL_SFT                     7
#define ACCDET_EINT_CTURBO_SEL_MASK                    0x1
#define ACCDET_EINT_CTURBO_SEL_MASK_SFT                (0x1 << 7)
#define ACCDET_B_SEL_ADDR                              \
	MT6359_ACCDET_CON22
#define ACCDET_B_SEL_SFT                               9
#define ACCDET_B_SEL_MASK                              0x1
#define ACCDET_B_SEL_MASK_SFT                          (0x1 << 9)
#define ACCDET_A_SEL_ADDR                              \
	MT6359_ACCDET_CON22
#define ACCDET_A_SEL_SFT                               10
#define ACCDET_A_SEL_MASK                              0x1
#define ACCDET_A_SEL_MASK_SFT                          (0x1 << 10)
#define ACCDET_EINT_CMPOUT_SEL_ADDR                    \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_CMPOUT_SEL_SFT                     11
#define ACCDET_EINT_CMPOUT_SEL_MASK                    0x1
#define ACCDET_EINT_CMPOUT_SEL_MASK_SFT                (0x1 << 11)
#define ACCDET_EINT_CMPMOUT_SEL_ADDR                   \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_CMPMOUT_SEL_SFT                    12
#define ACCDET_EINT_CMPMOUT_SEL_MASK                   0x1
#define ACCDET_EINT_CMPMOUT_SEL_MASK_SFT               (0x1 << 12)
#define ACCDET_EINT_INVOUT_SEL_ADDR                    \
	MT6359_ACCDET_CON22
#define ACCDET_EINT_INVOUT_SEL_SFT                     13
#define ACCDET_EINT_INVOUT_SEL_MASK                    0x1
#define ACCDET_EINT_INVOUT_SEL_MASK_SFT                (0x1 << 13)
#define ACCDET_CMPEN_SW_ADDR                           \
	MT6359_ACCDET_CON23
#define ACCDET_CMPEN_SW_SFT                            0
#define ACCDET_CMPEN_SW_MASK                           0x1
#define ACCDET_CMPEN_SW_MASK_SFT                       (0x1 << 0)
#define ACCDET_VTHEN_SW_ADDR                           \
	MT6359_ACCDET_CON23
#define ACCDET_VTHEN_SW_SFT                            1
#define ACCDET_VTHEN_SW_MASK                           0x1
#define ACCDET_VTHEN_SW_MASK_SFT                       (0x1 << 1)
#define ACCDET_MBIASEN_SW_ADDR                         \
	MT6359_ACCDET_CON23
#define ACCDET_MBIASEN_SW_SFT                          2
#define ACCDET_MBIASEN_SW_MASK                         0x1
#define ACCDET_MBIASEN_SW_MASK_SFT                     (0x1 << 2)
#define ACCDET_EINT0_EN_SW_ADDR                        \
	MT6359_ACCDET_CON23
#define ACCDET_EINT0_EN_SW_SFT                         3
#define ACCDET_EINT0_EN_SW_MASK                        0x1
#define ACCDET_EINT0_EN_SW_MASK_SFT                    (0x1 << 3)
#define ACCDET_EINT0_INVEN_SW_ADDR                     \
	MT6359_ACCDET_CON23
#define ACCDET_EINT0_INVEN_SW_SFT                      4
#define ACCDET_EINT0_INVEN_SW_MASK                     0x1
#define ACCDET_EINT0_INVEN_SW_MASK_SFT                 (0x1 << 4)
#define ACCDET_EINT0_CMPEN_SW_ADDR                     \
	MT6359_ACCDET_CON23
#define ACCDET_EINT0_CMPEN_SW_SFT                      5
#define ACCDET_EINT0_CMPEN_SW_MASK                     0x1
#define ACCDET_EINT0_CMPEN_SW_MASK_SFT                 (0x1 << 5)
#define ACCDET_EINT0_CMPMEN_SW_ADDR                    \
	MT6359_ACCDET_CON23
#define ACCDET_EINT0_CMPMEN_SW_SFT                     6
#define ACCDET_EINT0_CMPMEN_SW_MASK                    0x1
#define ACCDET_EINT0_CMPMEN_SW_MASK_SFT                (0x1 << 6)
#define ACCDET_EINT0_CTURBO_SW_ADDR                    \
	MT6359_ACCDET_CON23
#define ACCDET_EINT0_CTURBO_SW_SFT                     7
#define ACCDET_EINT0_CTURBO_SW_MASK                    0x1
#define ACCDET_EINT0_CTURBO_SW_MASK_SFT                (0x1 << 7)
#define ACCDET_EINT1_EN_SW_ADDR                        \
	MT6359_ACCDET_CON23
#define ACCDET_EINT1_EN_SW_SFT                         8
#define ACCDET_EINT1_EN_SW_MASK                        0x1
#define ACCDET_EINT1_EN_SW_MASK_SFT                    (0x1 << 8)
#define ACCDET_EINT1_INVEN_SW_ADDR                     \
	MT6359_ACCDET_CON23
#define ACCDET_EINT1_INVEN_SW_SFT                      9
#define ACCDET_EINT1_INVEN_SW_MASK                     0x1
#define ACCDET_EINT1_INVEN_SW_MASK_SFT                 (0x1 << 9)
#define ACCDET_EINT1_CMPEN_SW_ADDR                     \
	MT6359_ACCDET_CON23
#define ACCDET_EINT1_CMPEN_SW_SFT                      10
#define ACCDET_EINT1_CMPEN_SW_MASK                     0x1
#define ACCDET_EINT1_CMPEN_SW_MASK_SFT                 (0x1 << 10)
#define ACCDET_EINT1_CMPMEN_SW_ADDR                    \
	MT6359_ACCDET_CON23
#define ACCDET_EINT1_CMPMEN_SW_SFT                     11
#define ACCDET_EINT1_CMPMEN_SW_MASK                    0x1
#define ACCDET_EINT1_CMPMEN_SW_MASK_SFT                (0x1 << 11)
#define ACCDET_EINT1_CTURBO_SW_ADDR                    \
	MT6359_ACCDET_CON23
#define ACCDET_EINT1_CTURBO_SW_SFT                     12
#define ACCDET_EINT1_CTURBO_SW_MASK                    0x1
#define ACCDET_EINT1_CTURBO_SW_MASK_SFT                (0x1 << 12)
#define ACCDET_B_SW_ADDR                               \
	MT6359_ACCDET_CON24
#define ACCDET_B_SW_SFT                                0
#define ACCDET_B_SW_MASK                               0x1
#define ACCDET_B_SW_MASK_SFT                           (0x1 << 0)
#define ACCDET_A_SW_ADDR                               \
	MT6359_ACCDET_CON24
#define ACCDET_A_SW_SFT                                1
#define ACCDET_A_SW_MASK                               0x1
#define ACCDET_A_SW_MASK_SFT                           (0x1 << 1)
#define ACCDET_EINT0_CMPOUT_SW_ADDR                    \
	MT6359_ACCDET_CON24
#define ACCDET_EINT0_CMPOUT_SW_SFT                     2
#define ACCDET_EINT0_CMPOUT_SW_MASK                    0x1
#define ACCDET_EINT0_CMPOUT_SW_MASK_SFT                (0x1 << 2)
#define ACCDET_EINT0_CMPMOUT_SW_ADDR                   \
	MT6359_ACCDET_CON24
#define ACCDET_EINT0_CMPMOUT_SW_SFT                    3
#define ACCDET_EINT0_CMPMOUT_SW_MASK                   0x1
#define ACCDET_EINT0_CMPMOUT_SW_MASK_SFT               (0x1 << 3)
#define ACCDET_EINT0_INVOUT_SW_ADDR                    \
	MT6359_ACCDET_CON24
#define ACCDET_EINT0_INVOUT_SW_SFT                     4
#define ACCDET_EINT0_INVOUT_SW_MASK                    0x1
#define ACCDET_EINT0_INVOUT_SW_MASK_SFT                (0x1 << 4)
#define ACCDET_EINT1_CMPOUT_SW_ADDR                    \
	MT6359_ACCDET_CON24
#define ACCDET_EINT1_CMPOUT_SW_SFT                     5
#define ACCDET_EINT1_CMPOUT_SW_MASK                    0x1
#define ACCDET_EINT1_CMPOUT_SW_MASK_SFT                (0x1 << 5)
#define ACCDET_EINT1_CMPMOUT_SW_ADDR                   \
	MT6359_ACCDET_CON24
#define ACCDET_EINT1_CMPMOUT_SW_SFT                    6
#define ACCDET_EINT1_CMPMOUT_SW_MASK                   0x1
#define ACCDET_EINT1_CMPMOUT_SW_MASK_SFT               (0x1 << 6)
#define ACCDET_EINT1_INVOUT_SW_ADDR                    \
	MT6359_ACCDET_CON24
#define ACCDET_EINT1_INVOUT_SW_SFT                     7
#define ACCDET_EINT1_INVOUT_SW_MASK                    0x1
#define ACCDET_EINT1_INVOUT_SW_MASK_SFT                (0x1 << 7)
#define AD_AUDACCDETCMPOB_ADDR                         \
	MT6359_ACCDET_CON25
#define AD_AUDACCDETCMPOB_SFT                          0
#define AD_AUDACCDETCMPOB_MASK                         0x1
#define AD_AUDACCDETCMPOB_MASK_SFT                     (0x1 << 0)
#define AD_AUDACCDETCMPOA_ADDR                         \
	MT6359_ACCDET_CON25
#define AD_AUDACCDETCMPOA_SFT                          1
#define AD_AUDACCDETCMPOA_MASK                         0x1
#define AD_AUDACCDETCMPOA_MASK_SFT                     (0x1 << 1)
#define ACCDET_CUR_IN_ADDR                             \
	MT6359_ACCDET_CON25
#define ACCDET_CUR_IN_SFT                              2
#define ACCDET_CUR_IN_MASK                             0x3
#define ACCDET_CUR_IN_MASK_SFT                         (0x3 << 2)
#define ACCDET_SAM_IN_ADDR                             \
	MT6359_ACCDET_CON25
#define ACCDET_SAM_IN_SFT                              4
#define ACCDET_SAM_IN_MASK                             0x3
#define ACCDET_SAM_IN_MASK_SFT                         (0x3 << 4)
#define ACCDET_MEM_IN_ADDR                             \
	MT6359_ACCDET_CON25
#define ACCDET_MEM_IN_SFT                              6
#define ACCDET_MEM_IN_MASK                             0x3
#define ACCDET_MEM_IN_MASK_SFT                         (0x3 << 6)
#define ACCDET_STATE_ADDR                              \
	MT6359_ACCDET_CON25
#define ACCDET_STATE_SFT                               8
#define ACCDET_STATE_MASK                              0x7
#define ACCDET_STATE_MASK_SFT                          (0x7 << 8)
#define DA_AUDACCDETMBIASCLK_ADDR                      \
	MT6359_ACCDET_CON25
#define DA_AUDACCDETMBIASCLK_SFT                       12
#define DA_AUDACCDETMBIASCLK_MASK                      0x1
#define DA_AUDACCDETMBIASCLK_MASK_SFT                  (0x1 << 12)
#define DA_AUDACCDETVTHCLK_ADDR                        \
	MT6359_ACCDET_CON25
#define DA_AUDACCDETVTHCLK_SFT                         13
#define DA_AUDACCDETVTHCLK_MASK                        0x1
#define DA_AUDACCDETVTHCLK_MASK_SFT                    (0x1 << 13)
#define DA_AUDACCDETCMPCLK_ADDR                        \
	MT6359_ACCDET_CON25
#define DA_AUDACCDETCMPCLK_SFT                         14
#define DA_AUDACCDETCMPCLK_MASK                        0x1
#define DA_AUDACCDETCMPCLK_MASK_SFT                    (0x1 << 14)
#define DA_AUDACCDETAUXADCSWCTRL_ADDR                  \
	MT6359_ACCDET_CON25
#define DA_AUDACCDETAUXADCSWCTRL_SFT                   15
#define DA_AUDACCDETAUXADCSWCTRL_MASK                  0x1
#define DA_AUDACCDETAUXADCSWCTRL_MASK_SFT              (0x1 << 15)
#define AD_EINT0CMPMOUT_ADDR                           \
	MT6359_ACCDET_CON26
#define AD_EINT0CMPMOUT_SFT                            0
#define AD_EINT0CMPMOUT_MASK                           0x1
#define AD_EINT0CMPMOUT_MASK_SFT                       (0x1 << 0)
#define AD_EINT0CMPOUT_ADDR                            \
	MT6359_ACCDET_CON26
#define AD_EINT0CMPOUT_SFT                             1
#define AD_EINT0CMPOUT_MASK                            0x1
#define AD_EINT0CMPOUT_MASK_SFT                        (0x1 << 1)
#define ACCDET_EINT0_CUR_IN_ADDR                       \
	MT6359_ACCDET_CON26
#define ACCDET_EINT0_CUR_IN_SFT                        2
#define ACCDET_EINT0_CUR_IN_MASK                       0x3
#define ACCDET_EINT0_CUR_IN_MASK_SFT                   (0x3 << 2)
#define ACCDET_EINT0_SAM_IN_ADDR                       \
	MT6359_ACCDET_CON26
#define ACCDET_EINT0_SAM_IN_SFT                        4
#define ACCDET_EINT0_SAM_IN_MASK                       0x3
#define ACCDET_EINT0_SAM_IN_MASK_SFT                   (0x3 << 4)
#define ACCDET_EINT0_MEM_IN_ADDR                       \
	MT6359_ACCDET_CON26
#define ACCDET_EINT0_MEM_IN_SFT                        6
#define ACCDET_EINT0_MEM_IN_MASK                       0x3
#define ACCDET_EINT0_MEM_IN_MASK_SFT                   (0x3 << 6)
#define ACCDET_EINT0_STATE_ADDR                        \
	MT6359_ACCDET_CON26
#define ACCDET_EINT0_STATE_SFT                         8
#define ACCDET_EINT0_STATE_MASK                        0x7
#define ACCDET_EINT0_STATE_MASK_SFT                    (0x7 << 8)
#define DA_EINT0CMPEN_ADDR                             \
	MT6359_ACCDET_CON26
#define DA_EINT0CMPEN_SFT                              13
#define DA_EINT0CMPEN_MASK                             0x1
#define DA_EINT0CMPEN_MASK_SFT                         (0x1 << 13)
#define DA_EINT0CMPMEN_ADDR                            \
	MT6359_ACCDET_CON26
#define DA_EINT0CMPMEN_SFT                             14
#define DA_EINT0CMPMEN_MASK                            0x1
#define DA_EINT0CMPMEN_MASK_SFT                        (0x1 << 14)
#define DA_EINT0CTURBO_ADDR                            \
	MT6359_ACCDET_CON26
#define DA_EINT0CTURBO_SFT                             15
#define DA_EINT0CTURBO_MASK                            0x1
#define DA_EINT0CTURBO_MASK_SFT                        (0x1 << 15)
#define AD_EINT1CMPMOUT_ADDR                           \
	MT6359_ACCDET_CON27
#define AD_EINT1CMPMOUT_SFT                            0
#define AD_EINT1CMPMOUT_MASK                           0x1
#define AD_EINT1CMPMOUT_MASK_SFT                       (0x1 << 0)
#define AD_EINT1CMPOUT_ADDR                            \
	MT6359_ACCDET_CON27
#define AD_EINT1CMPOUT_SFT                             1
#define AD_EINT1CMPOUT_MASK                            0x1
#define AD_EINT1CMPOUT_MASK_SFT                        (0x1 << 1)
#define ACCDET_EINT1_CUR_IN_ADDR                       \
	MT6359_ACCDET_CON27
#define ACCDET_EINT1_CUR_IN_SFT                        2
#define ACCDET_EINT1_CUR_IN_MASK                       0x3
#define ACCDET_EINT1_CUR_IN_MASK_SFT                   (0x3 << 2)
#define ACCDET_EINT1_SAM_IN_ADDR                       \
	MT6359_ACCDET_CON27
#define ACCDET_EINT1_SAM_IN_SFT                        4
#define ACCDET_EINT1_SAM_IN_MASK                       0x3
#define ACCDET_EINT1_SAM_IN_MASK_SFT                   (0x3 << 4)
#define ACCDET_EINT1_MEM_IN_ADDR                       \
	MT6359_ACCDET_CON27
#define ACCDET_EINT1_MEM_IN_SFT                        6
#define ACCDET_EINT1_MEM_IN_MASK                       0x3
#define ACCDET_EINT1_MEM_IN_MASK_SFT                   (0x3 << 6)
#define ACCDET_EINT1_STATE_ADDR                        \
	MT6359_ACCDET_CON27
#define ACCDET_EINT1_STATE_SFT                         8
#define ACCDET_EINT1_STATE_MASK                        0x7
#define ACCDET_EINT1_STATE_MASK_SFT                    (0x7 << 8)
#define DA_EINT1CMPEN_ADDR                             \
	MT6359_ACCDET_CON27
#define DA_EINT1CMPEN_SFT                              13
#define DA_EINT1CMPEN_MASK                             0x1
#define DA_EINT1CMPEN_MASK_SFT                         (0x1 << 13)
#define DA_EINT1CMPMEN_ADDR                            \
	MT6359_ACCDET_CON27
#define DA_EINT1CMPMEN_SFT                             14
#define DA_EINT1CMPMEN_MASK                            0x1
#define DA_EINT1CMPMEN_MASK_SFT                        (0x1 << 14)
#define DA_EINT1CTURBO_ADDR                            \
	MT6359_ACCDET_CON27
#define DA_EINT1CTURBO_SFT                             15
#define DA_EINT1CTURBO_MASK                            0x1
#define DA_EINT1CTURBO_MASK_SFT                        (0x1 << 15)
#define AD_EINT0INVOUT_ADDR                            \
	MT6359_ACCDET_CON28
#define AD_EINT0INVOUT_SFT                             0
#define AD_EINT0INVOUT_MASK                            0x1
#define AD_EINT0INVOUT_MASK_SFT                        (0x1 << 0)
#define ACCDET_EINT0_INVERTER_CUR_IN_ADDR              \
	MT6359_ACCDET_CON28
#define ACCDET_EINT0_INVERTER_CUR_IN_SFT               1
#define ACCDET_EINT0_INVERTER_CUR_IN_MASK              0x1
#define ACCDET_EINT0_INVERTER_CUR_IN_MASK_SFT          (0x1 << 1)
#define ACCDET_EINT0_INVERTER_SAM_IN_ADDR              \
	MT6359_ACCDET_CON28
#define ACCDET_EINT0_INVERTER_SAM_IN_SFT               2
#define ACCDET_EINT0_INVERTER_SAM_IN_MASK              0x1
#define ACCDET_EINT0_INVERTER_SAM_IN_MASK_SFT          (0x1 << 2)
#define ACCDET_EINT0_INVERTER_MEM_IN_ADDR              \
	MT6359_ACCDET_CON28
#define ACCDET_EINT0_INVERTER_MEM_IN_SFT               3
#define ACCDET_EINT0_INVERTER_MEM_IN_MASK              0x1
#define ACCDET_EINT0_INVERTER_MEM_IN_MASK_SFT          (0x1 << 3)
#define ACCDET_EINT0_INVERTER_STATE_ADDR               \
	MT6359_ACCDET_CON28
#define ACCDET_EINT0_INVERTER_STATE_SFT                8
#define ACCDET_EINT0_INVERTER_STATE_MASK               0x7
#define ACCDET_EINT0_INVERTER_STATE_MASK_SFT           (0x7 << 8)
#define DA_EINT0EN_ADDR                                \
	MT6359_ACCDET_CON28
#define DA_EINT0EN_SFT                                 12
#define DA_EINT0EN_MASK                                0x1
#define DA_EINT0EN_MASK_SFT                            (0x1 << 12)
#define DA_EINT0INVEN_ADDR                             \
	MT6359_ACCDET_CON28
#define DA_EINT0INVEN_SFT                              13
#define DA_EINT0INVEN_MASK                             0x1
#define DA_EINT0INVEN_MASK_SFT                         (0x1 << 13)
#define DA_EINT0CEN_ADDR                               \
	MT6359_ACCDET_CON28
#define DA_EINT0CEN_SFT                                14
#define DA_EINT0CEN_MASK                               0x1
#define DA_EINT0CEN_MASK_SFT                           (0x1 << 14)
#define AD_EINT1INVOUT_ADDR                            \
	MT6359_ACCDET_CON29
#define AD_EINT1INVOUT_SFT                             0
#define AD_EINT1INVOUT_MASK                            0x1
#define AD_EINT1INVOUT_MASK_SFT                        (0x1 << 0)
#define ACCDET_EINT1_INVERTER_CUR_IN_ADDR              \
	MT6359_ACCDET_CON29
#define ACCDET_EINT1_INVERTER_CUR_IN_SFT               1
#define ACCDET_EINT1_INVERTER_CUR_IN_MASK              0x1
#define ACCDET_EINT1_INVERTER_CUR_IN_MASK_SFT          (0x1 << 1)
#define ACCDET_EINT1_INVERTER_SAM_IN_ADDR              \
	MT6359_ACCDET_CON29
#define ACCDET_EINT1_INVERTER_SAM_IN_SFT               2
#define ACCDET_EINT1_INVERTER_SAM_IN_MASK              0x1
#define ACCDET_EINT1_INVERTER_SAM_IN_MASK_SFT          (0x1 << 2)
#define ACCDET_EINT1_INVERTER_MEM_IN_ADDR              \
	MT6359_ACCDET_CON29
#define ACCDET_EINT1_INVERTER_MEM_IN_SFT               3
#define ACCDET_EINT1_INVERTER_MEM_IN_MASK              0x1
#define ACCDET_EINT1_INVERTER_MEM_IN_MASK_SFT          (0x1 << 3)
#define ACCDET_EINT1_INVERTER_STATE_ADDR               \
	MT6359_ACCDET_CON29
#define ACCDET_EINT1_INVERTER_STATE_SFT                8
#define ACCDET_EINT1_INVERTER_STATE_MASK               0x7
#define ACCDET_EINT1_INVERTER_STATE_MASK_SFT           (0x7 << 8)
#define DA_EINT1EN_ADDR                                \
	MT6359_ACCDET_CON29
#define DA_EINT1EN_SFT                                 12
#define DA_EINT1EN_MASK                                0x1
#define DA_EINT1EN_MASK_SFT                            (0x1 << 12)
#define DA_EINT1INVEN_ADDR                             \
	MT6359_ACCDET_CON29
#define DA_EINT1INVEN_SFT                              13
#define DA_EINT1INVEN_MASK                             0x1
#define DA_EINT1INVEN_MASK_SFT                         (0x1 << 13)
#define DA_EINT1CEN_ADDR                               \
	MT6359_ACCDET_CON29
#define DA_EINT1CEN_SFT                                14
#define DA_EINT1CEN_MASK                               0x1
#define DA_EINT1CEN_MASK_SFT                           (0x1 << 14)
#define ACCDET_EN_ADDR                                 \
	MT6359_ACCDET_CON30
#define ACCDET_EN_SFT                                  0
#define ACCDET_EN_MASK                                 0x1
#define ACCDET_EN_MASK_SFT                             (0x1 << 0)
#define ACCDET_EINT0_EN_ADDR                           \
	MT6359_ACCDET_CON30
#define ACCDET_EINT0_EN_SFT                            1
#define ACCDET_EINT0_EN_MASK                           0x1
#define ACCDET_EINT0_EN_MASK_SFT                       (0x1 << 1)
#define ACCDET_EINT1_EN_ADDR                           \
	MT6359_ACCDET_CON30
#define ACCDET_EINT1_EN_SFT                            2
#define ACCDET_EINT1_EN_MASK                           0x1
#define ACCDET_EINT1_EN_MASK_SFT                       (0x1 << 2)
#define ACCDET_EINT0_M_EN_ADDR                         \
	MT6359_ACCDET_CON30
#define ACCDET_EINT0_M_EN_SFT                          3
#define ACCDET_EINT0_M_EN_MASK                         0x1
#define ACCDET_EINT0_M_EN_MASK_SFT                     (0x1 << 3)
#define ACCDET_EINT0_DETECT_MOISTURE_ADDR              \
	MT6359_ACCDET_CON30
#define ACCDET_EINT0_DETECT_MOISTURE_SFT               4
#define ACCDET_EINT0_DETECT_MOISTURE_MASK              0x1
#define ACCDET_EINT0_DETECT_MOISTURE_MASK_SFT          (0x1 << 4)
#define ACCDET_EINT0_PLUG_IN_ADDR                      \
	MT6359_ACCDET_CON30
#define ACCDET_EINT0_PLUG_IN_SFT                       5
#define ACCDET_EINT0_PLUG_IN_MASK                      0x1
#define ACCDET_EINT0_PLUG_IN_MASK_SFT                  (0x1 << 5)
#define ACCDET_EINT0_M_PLUG_IN_ADDR                    \
	MT6359_ACCDET_CON30
#define ACCDET_EINT0_M_PLUG_IN_SFT                     6
#define ACCDET_EINT0_M_PLUG_IN_MASK                    0x1
#define ACCDET_EINT0_M_PLUG_IN_MASK_SFT                (0x1 << 6)
#define ACCDET_EINT1_M_EN_ADDR                         \
	MT6359_ACCDET_CON30
#define ACCDET_EINT1_M_EN_SFT                          7
#define ACCDET_EINT1_M_EN_MASK                         0x1
#define ACCDET_EINT1_M_EN_MASK_SFT                     (0x1 << 7)
#define ACCDET_EINT1_DETECT_MOISTURE_ADDR              \
	MT6359_ACCDET_CON30
#define ACCDET_EINT1_DETECT_MOISTURE_SFT               8
#define ACCDET_EINT1_DETECT_MOISTURE_MASK              0x1
#define ACCDET_EINT1_DETECT_MOISTURE_MASK_SFT          (0x1 << 8)
#define ACCDET_EINT1_PLUG_IN_ADDR                      \
	MT6359_ACCDET_CON30
#define ACCDET_EINT1_PLUG_IN_SFT                       9
#define ACCDET_EINT1_PLUG_IN_MASK                      0x1
#define ACCDET_EINT1_PLUG_IN_MASK_SFT                  (0x1 << 9)
#define ACCDET_EINT1_M_PLUG_IN_ADDR                    \
	MT6359_ACCDET_CON30
#define ACCDET_EINT1_M_PLUG_IN_SFT                     10
#define ACCDET_EINT1_M_PLUG_IN_MASK                    0x1
#define ACCDET_EINT1_M_PLUG_IN_MASK_SFT                (0x1 << 10)
#define ACCDET_CUR_DEB_ADDR                            \
	MT6359_ACCDET_CON31
#define ACCDET_CUR_DEB_SFT                             0
#define ACCDET_CUR_DEB_MASK                            0xFFFF
#define ACCDET_CUR_DEB_MASK_SFT                        (0xFFFF << 0)
#define ACCDET_EINT0_CUR_DEB_ADDR                      \
	MT6359_ACCDET_CON32
#define ACCDET_EINT0_CUR_DEB_SFT                       0
#define ACCDET_EINT0_CUR_DEB_MASK                      0x7FFF
#define ACCDET_EINT0_CUR_DEB_MASK_SFT                  (0x7FFF << 0)
#define ACCDET_EINT1_CUR_DEB_ADDR                      \
	MT6359_ACCDET_CON33
#define ACCDET_EINT1_CUR_DEB_SFT                       0
#define ACCDET_EINT1_CUR_DEB_MASK                      0x7FFF
#define ACCDET_EINT1_CUR_DEB_MASK_SFT                  (0x7FFF << 0)
#define ACCDET_EINT0_INVERTER_CUR_DEB_ADDR             \
	MT6359_ACCDET_CON34
#define ACCDET_EINT0_INVERTER_CUR_DEB_SFT              0
#define ACCDET_EINT0_INVERTER_CUR_DEB_MASK             0x7FFF
#define ACCDET_EINT0_INVERTER_CUR_DEB_MASK_SFT         (0x7FFF << 0)
#define ACCDET_EINT1_INVERTER_CUR_DEB_ADDR             \
	MT6359_ACCDET_CON35
#define ACCDET_EINT1_INVERTER_CUR_DEB_SFT              0
#define ACCDET_EINT1_INVERTER_CUR_DEB_MASK             0x7FFF
#define ACCDET_EINT1_INVERTER_CUR_DEB_MASK_SFT         (0x7FFF << 0)
#define AD_AUDACCDETCMPOB_MON_ADDR                     \
	MT6359_ACCDET_CON36
#define AD_AUDACCDETCMPOB_MON_SFT                      0
#define AD_AUDACCDETCMPOB_MON_MASK                     0x1
#define AD_AUDACCDETCMPOB_MON_MASK_SFT                 (0x1 << 0)
#define AD_AUDACCDETCMPOA_MON_ADDR                     \
	MT6359_ACCDET_CON36
#define AD_AUDACCDETCMPOA_MON_SFT                      1
#define AD_AUDACCDETCMPOA_MON_MASK                     0x1
#define AD_AUDACCDETCMPOA_MON_MASK_SFT                 (0x1 << 1)
#define AD_EINT0CMPMOUT_MON_ADDR                       \
	MT6359_ACCDET_CON36
#define AD_EINT0CMPMOUT_MON_SFT                        2
#define AD_EINT0CMPMOUT_MON_MASK                       0x1
#define AD_EINT0CMPMOUT_MON_MASK_SFT                   (0x1 << 2)
#define AD_EINT0CMPOUT_MON_ADDR                        \
	MT6359_ACCDET_CON36
#define AD_EINT0CMPOUT_MON_SFT                         3
#define AD_EINT0CMPOUT_MON_MASK                        0x1
#define AD_EINT0CMPOUT_MON_MASK_SFT                    (0x1 << 3)
#define AD_EINT0INVOUT_MON_ADDR                        \
	MT6359_ACCDET_CON36
#define AD_EINT0INVOUT_MON_SFT                         4
#define AD_EINT0INVOUT_MON_MASK                        0x1
#define AD_EINT0INVOUT_MON_MASK_SFT                    (0x1 << 4)
#define AD_EINT1CMPMOUT_MON_ADDR                       \
	MT6359_ACCDET_CON36
#define AD_EINT1CMPMOUT_MON_SFT                        5
#define AD_EINT1CMPMOUT_MON_MASK                       0x1
#define AD_EINT1CMPMOUT_MON_MASK_SFT                   (0x1 << 5)
#define AD_EINT1CMPOUT_MON_ADDR                        \
	MT6359_ACCDET_CON36
#define AD_EINT1CMPOUT_MON_SFT                         6
#define AD_EINT1CMPOUT_MON_MASK                        0x1
#define AD_EINT1CMPOUT_MON_MASK_SFT                    (0x1 << 6)
#define AD_EINT1INVOUT_MON_ADDR                        \
	MT6359_ACCDET_CON36
#define AD_EINT1INVOUT_MON_SFT                         7
#define AD_EINT1INVOUT_MON_MASK                        0x1
#define AD_EINT1INVOUT_MON_MASK_SFT                    (0x1 << 7)
#define DA_AUDACCDETCMPCLK_MON_ADDR                    \
	MT6359_ACCDET_CON37
#define DA_AUDACCDETCMPCLK_MON_SFT                     0
#define DA_AUDACCDETCMPCLK_MON_MASK                    0x1
#define DA_AUDACCDETCMPCLK_MON_MASK_SFT                (0x1 << 0)
#define DA_AUDACCDETVTHCLK_MON_ADDR                    \
	MT6359_ACCDET_CON37
#define DA_AUDACCDETVTHCLK_MON_SFT                     1
#define DA_AUDACCDETVTHCLK_MON_MASK                    0x1
#define DA_AUDACCDETVTHCLK_MON_MASK_SFT                (0x1 << 1)
#define DA_AUDACCDETMBIASCLK_MON_ADDR                  \
	MT6359_ACCDET_CON37
#define DA_AUDACCDETMBIASCLK_MON_SFT                   2
#define DA_AUDACCDETMBIASCLK_MON_MASK                  0x1
#define DA_AUDACCDETMBIASCLK_MON_MASK_SFT              (0x1 << 2)
#define DA_AUDACCDETAUXADCSWCTRL_MON_ADDR              \
	MT6359_ACCDET_CON37
#define DA_AUDACCDETAUXADCSWCTRL_MON_SFT               3
#define DA_AUDACCDETAUXADCSWCTRL_MON_MASK              0x1
#define DA_AUDACCDETAUXADCSWCTRL_MON_MASK_SFT          (0x1 << 3)
#define DA_EINT0CTURBO_MON_ADDR                        \
	MT6359_ACCDET_CON38
#define DA_EINT0CTURBO_MON_SFT                         0
#define DA_EINT0CTURBO_MON_MASK                        0x1
#define DA_EINT0CTURBO_MON_MASK_SFT                    (0x1 << 0)
#define DA_EINT0CMPMEN_MON_ADDR                        \
	MT6359_ACCDET_CON38
#define DA_EINT0CMPMEN_MON_SFT                         1
#define DA_EINT0CMPMEN_MON_MASK                        0x1
#define DA_EINT0CMPMEN_MON_MASK_SFT                    (0x1 << 1)
#define DA_EINT0CMPEN_MON_ADDR                         \
	MT6359_ACCDET_CON38
#define DA_EINT0CMPEN_MON_SFT                          2
#define DA_EINT0CMPEN_MON_MASK                         0x1
#define DA_EINT0CMPEN_MON_MASK_SFT                     (0x1 << 2)
#define DA_EINT0INVEN_MON_ADDR                         \
	MT6359_ACCDET_CON38
#define DA_EINT0INVEN_MON_SFT                          3
#define DA_EINT0INVEN_MON_MASK                         0x1
#define DA_EINT0INVEN_MON_MASK_SFT                     (0x1 << 3)
#define DA_EINT0CEN_MON_ADDR                           \
	MT6359_ACCDET_CON38
#define DA_EINT0CEN_MON_SFT                            4
#define DA_EINT0CEN_MON_MASK                           0x1
#define DA_EINT0CEN_MON_MASK_SFT                       (0x1 << 4)
#define DA_EINT0EN_MON_ADDR                            \
	MT6359_ACCDET_CON38
#define DA_EINT0EN_MON_SFT                             5
#define DA_EINT0EN_MON_MASK                            0x1
#define DA_EINT0EN_MON_MASK_SFT                        (0x1 << 5)
#define DA_EINT1CTURBO_MON_ADDR                        \
	MT6359_ACCDET_CON38
#define DA_EINT1CTURBO_MON_SFT                         8
#define DA_EINT1CTURBO_MON_MASK                        0x1
#define DA_EINT1CTURBO_MON_MASK_SFT                    (0x1 << 8)
#define DA_EINT1CMPMEN_MON_ADDR                        \
	MT6359_ACCDET_CON38
#define DA_EINT1CMPMEN_MON_SFT                         9
#define DA_EINT1CMPMEN_MON_MASK                        0x1
#define DA_EINT1CMPMEN_MON_MASK_SFT                    (0x1 << 9)
#define DA_EINT1CMPEN_MON_ADDR                         \
	MT6359_ACCDET_CON38
#define DA_EINT1CMPEN_MON_SFT                          10
#define DA_EINT1CMPEN_MON_MASK                         0x1
#define DA_EINT1CMPEN_MON_MASK_SFT                     (0x1 << 10)
#define DA_EINT1INVEN_MON_ADDR                         \
	MT6359_ACCDET_CON38
#define DA_EINT1INVEN_MON_SFT                          11
#define DA_EINT1INVEN_MON_MASK                         0x1
#define DA_EINT1INVEN_MON_MASK_SFT                     (0x1 << 11)
#define DA_EINT1CEN_MON_ADDR                           \
	MT6359_ACCDET_CON38
#define DA_EINT1CEN_MON_SFT                            12
#define DA_EINT1CEN_MON_MASK                           0x1
#define DA_EINT1CEN_MON_MASK_SFT                       (0x1 << 12)
#define DA_EINT1EN_MON_ADDR                            \
	MT6359_ACCDET_CON38
#define DA_EINT1EN_MON_SFT                             13
#define DA_EINT1EN_MON_MASK                            0x1
#define DA_EINT1EN_MON_MASK_SFT                        (0x1 << 13)
#define ACCDET_EINT0_M_PLUG_IN_COUNT_ADDR              \
	MT6359_ACCDET_CON39
#define ACCDET_EINT0_M_PLUG_IN_COUNT_SFT               0
#define ACCDET_EINT0_M_PLUG_IN_COUNT_MASK              0x7
#define ACCDET_EINT0_M_PLUG_IN_COUNT_MASK_SFT          (0x7 << 0)
#define ACCDET_EINT1_M_PLUG_IN_COUNT_ADDR              \
	MT6359_ACCDET_CON39
#define ACCDET_EINT1_M_PLUG_IN_COUNT_SFT               4
#define ACCDET_EINT1_M_PLUG_IN_COUNT_MASK              0x7
#define ACCDET_EINT1_M_PLUG_IN_COUNT_MASK_SFT          (0x7 << 4)
#define ACCDET_MON_FLAG_EN_ADDR                        \
	MT6359_ACCDET_CON40
#define ACCDET_MON_FLAG_EN_SFT                         0
#define ACCDET_MON_FLAG_EN_MASK                        0x1
#define ACCDET_MON_FLAG_EN_MASK_SFT                    (0x1 << 0)
#define ACCDET_MON_FLAG_SEL_ADDR                       \
	MT6359_ACCDET_CON40
#define ACCDET_MON_FLAG_SEL_SFT                        4
#define ACCDET_MON_FLAG_SEL_MASK                       0xF
#define ACCDET_MON_FLAG_SEL_MASK_SFT                   (0xF << 4)

#define RG_AUDPWDBMICBIAS0_ADDR                        \
	MT6359_AUDENC_ANA_CON15
#define RG_AUDPWDBMICBIAS0_SFT                         0
#define RG_AUDPWDBMICBIAS0_MASK                        0x1
#define RG_AUDPWDBMICBIAS0_MASK_SFT                    (0x1 << 0)
#define RG_AUDPREAMPLON_ADDR                           \
	MT6359_AUDENC_ANA_CON0
#define RG_AUDPREAMPLON_SFT                            0
#define RG_AUDPREAMPLON_MASK                           0x1
#define RG_AUDPREAMPLON_MASK_SFT                       (0x1 << 0)
#define RG_CLKSQ_EN_ADDR                               \
	MT6359_AUDENC_ANA_CON23
#define RG_CLKSQ_EN_SFT                                0
#define RG_CLKSQ_EN_MASK                               0x1
#define RG_CLKSQ_EN_MASK_SFT                           (0x1 << 0)
#define RG_RTC32K_CK_PDN_ADDR                          \
	MT6359_TOP_CKPDN_CON0
#define RG_RTC32K_CK_PDN_SFT                           15
#define RG_RTC32K_CK_PDN_MASK                          0x1
#define RG_RTC32K_CK_PDN_MASK_SFT                      (0x1 << 15)
#define RG_HPLOUTPUTSTBENH_VAUDP32_ADDR                \
	MT6359_AUDDEC_ANA_CON2
#define RG_HPLOUTPUTSTBENH_VAUDP32_SFT                 0
#define RG_HPLOUTPUTSTBENH_VAUDP32_MASK                0x7
#define RG_HPLOUTPUTSTBENH_VAUDP32_MASK_SFT            (0x7 << 0)
#define AUXADC_RQST_CH5_ADDR                           \
	MT6359_AUXADC_RQST0
#define AUXADC_RQST_CH5_SFT                            5
#define AUXADC_RQST_CH5_MASK                           0x1
#define AUXADC_RQST_CH5_MASK_SFT                       (0x1 << 5)
#define RG_LDO_VUSB_HW0_OP_EN_ADDR                     \
	MT6359_LDO_VUSB_OP_EN
#define RG_LDO_VUSB_HW0_OP_EN_SFT                      0
#define RG_LDO_VUSB_HW0_OP_EN_MASK                     0x1
#define RG_LDO_VUSB_HW0_OP_EN_MASK_SFT                 (0x1 << 0)
#define RG_HPROUTPUTSTBENH_VAUDP32_ADDR                \
	MT6359_AUDDEC_ANA_CON2
#define RG_HPROUTPUTSTBENH_VAUDP32_SFT                 4
#define RG_HPROUTPUTSTBENH_VAUDP32_MASK                0x7
#define RG_HPROUTPUTSTBENH_VAUDP32_MASK_SFT            (0x7 << 4)
#define RG_NCP_PDDIS_EN_ADDR                           \
	MT6359_AFE_NCP_CFG2
#define RG_NCP_PDDIS_EN_SFT                            0
#define RG_NCP_PDDIS_EN_MASK                           0x1
#define RG_NCP_PDDIS_EN_MASK_SFT                       (0x1 << 0)
#define RG_SCK32K_CK_PDN_ADDR                          \
	MT6359_TOP_CKPDN_CON0
#define RG_SCK32K_CK_PDN_SFT                           0
#define RG_SCK32K_CK_PDN_MASK                          0x1
#define RG_SCK32K_CK_PDN_MASK_SFT                      (0x1 << 0)
/* AUDENC_ANA_CON18: */
#define RG_ACCDET_MODE_ANA11_MODE1	(0x000F)
#define RG_ACCDET_MODE_ANA11_MODE2	(0x008F)
#define RG_ACCDET_MODE_ANA11_MODE6	(0x008F)

/* AUXADC_ADC5:  Auxadc CH5 read data */
#define AUXADC_DATA_RDY_CH5		BIT(15)
#define AUXADC_DATA_PROCEED_CH5		BIT(15)
#define AUXADC_DATA_MASK		(0x0FFF)

/* AUXADC_RQST0_SET:  Auxadc CH5 request, relevant 0x07EC */
#define AUXADC_RQST_CH5_SET		BIT(5)
/* AUXADC_RQST0_CLR:  Auxadc CH5 request, relevant 0x07EC */
#define AUXADC_RQST_CH5_CLR		BIT(5)

#define ACCDET_CALI_MASK0		(0xFF)
#define ACCDET_CALI_MASK1		(0xFF << 8)
#define ACCDET_CALI_MASK2		(0xFF)
#define ACCDET_CALI_MASK3		(0xFF << 8)
#define ACCDET_CALI_MASK4		(0xFF)

#define ACCDET_EINT_IRQ_B2_B3		(0x03 << ACCDET_EINT0_IRQ_SFT)

/* ACCDET_CON25: RO, accdet FSM state,etc.*/
#define ACCDET_STATE_MEM_IN_OFFSET	(ACCDET_MEM_IN_SFT)
#define ACCDET_STATE_AB_MASK		(0x03)
#define ACCDET_STATE_AB_00		(0x00)
#define ACCDET_STATE_AB_01		(0x01)
#define ACCDET_STATE_AB_10		(0x02)
#define ACCDET_STATE_AB_11		(0x03)

/* ACCDET_CON19 */
#define ACCDET_EINT0_STABLE_VAL ((ACCDET_DA_STABLE_MASK_SFT) | \
				(ACCDET_EINT0_EN_STABLE_MASK_SFT) | \
				(ACCDET_EINT0_CMPEN_STABLE_MASK_SFT) | \
				(ACCDET_EINT0_CEN_STABLE_MASK_SFT))

#define ACCDET_EINT1_STABLE_VAL ((ACCDET_DA_STABLE_MASK_SFT) | \
				(ACCDET_EINT1_EN_STABLE_MASK_SFT) | \
				(ACCDET_EINT1_CMPEN_STABLE_MASK_SFT) | \
				(ACCDET_EINT1_CEN_STABLE_MASK_SFT))
/* The following are used for mt6359.c */
/* MT6359_DCXO_CW12 */
#define RG_XO_AUDIO_EN_M_SFT				13

/* AUD_TOP_CKPDN_CON0 */
#define RG_VOW13M_CK_PDN_SFT				13
#define RG_VOW13M_CK_PDN_MASK				0x1
#define RG_VOW13M_CK_PDN_MASK_SFT			(0x1 << 13)
#define RG_VOW32K_CK_PDN_SFT				12
#define RG_VOW32K_CK_PDN_MASK				0x1
#define RG_VOW32K_CK_PDN_MASK_SFT			(0x1 << 12)
#define RG_AUD_INTRP_CK_PDN_SFT				8
#define RG_AUD_INTRP_CK_PDN_MASK			0x1
#define RG_AUD_INTRP_CK_PDN_MASK_SFT			(0x1 << 8)
#define RG_PAD_AUD_CLK_MISO_CK_PDN_SFT			7
#define RG_PAD_AUD_CLK_MISO_CK_PDN_MASK			0x1
#define RG_PAD_AUD_CLK_MISO_CK_PDN_MASK_SFT		(0x1 << 7)
#define RG_AUDNCP_CK_PDN_SFT				6
#define RG_AUDNCP_CK_PDN_MASK				0x1
#define RG_AUDNCP_CK_PDN_MASK_SFT			(0x1 << 6)
#define RG_ZCD13M_CK_PDN_SFT				5
#define RG_ZCD13M_CK_PDN_MASK				0x1
#define RG_ZCD13M_CK_PDN_MASK_SFT			(0x1 << 5)
#define RG_AUDIF_CK_PDN_SFT				2
#define RG_AUDIF_CK_PDN_MASK				0x1
#define RG_AUDIF_CK_PDN_MASK_SFT			(0x1 << 2)
#define RG_AUD_CK_PDN_SFT				1
#define RG_AUD_CK_PDN_MASK				0x1
#define RG_AUD_CK_PDN_MASK_SFT				(0x1 << 1)
#define RG_ACCDET_CK_PDN_SFT				0
#define RG_ACCDET_CK_PDN_MASK				0x1
#define RG_ACCDET_CK_PDN_MASK_SFT			(0x1 << 0)

/* AUD_TOP_CKPDN_CON0_SET */
#define RG_AUD_TOP_CKPDN_CON0_SET_SFT			0
#define RG_AUD_TOP_CKPDN_CON0_SET_MASK			0x3fff
#define RG_AUD_TOP_CKPDN_CON0_SET_MASK_SFT		(0x3fff << 0)

/* AUD_TOP_CKPDN_CON0_CLR */
#define RG_AUD_TOP_CKPDN_CON0_CLR_SFT			0
#define RG_AUD_TOP_CKPDN_CON0_CLR_MASK			0x3fff
#define RG_AUD_TOP_CKPDN_CON0_CLR_MASK_SFT		(0x3fff << 0)

/* AUD_TOP_CKSEL_CON0 */
#define RG_AUDIF_CK_CKSEL_SFT				3
#define RG_AUDIF_CK_CKSEL_MASK				0x1
#define RG_AUDIF_CK_CKSEL_MASK_SFT			(0x1 << 3)
#define RG_AUD_CK_CKSEL_SFT				2
#define RG_AUD_CK_CKSEL_MASK				0x1
#define RG_AUD_CK_CKSEL_MASK_SFT			(0x1 << 2)

/* AUD_TOP_CKSEL_CON0_SET */
#define RG_AUD_TOP_CKSEL_CON0_SET_SFT			0
#define RG_AUD_TOP_CKSEL_CON0_SET_MASK			0xf
#define RG_AUD_TOP_CKSEL_CON0_SET_MASK_SFT		(0xf << 0)

/* AUD_TOP_CKSEL_CON0_CLR */
#define RG_AUD_TOP_CKSEL_CON0_CLR_SFT			0
#define RG_AUD_TOP_CKSEL_CON0_CLR_MASK			0xf
#define RG_AUD_TOP_CKSEL_CON0_CLR_MASK_SFT		(0xf << 0)

/* AUD_TOP_CKTST_CON0 */
#define RG_VOW13M_CK_TSTSEL_SFT				9
#define RG_VOW13M_CK_TSTSEL_MASK			0x1
#define RG_VOW13M_CK_TSTSEL_MASK_SFT			(0x1 << 9)
#define RG_VOW13M_CK_TST_DIS_SFT			8
#define RG_VOW13M_CK_TST_DIS_MASK			0x1
#define RG_VOW13M_CK_TST_DIS_MASK_SFT			(0x1 << 8)
#define RG_AUD26M_CK_TSTSEL_SFT				4
#define RG_AUD26M_CK_TSTSEL_MASK			0x1
#define RG_AUD26M_CK_TSTSEL_MASK_SFT			(0x1 << 4)
#define RG_AUDIF_CK_TSTSEL_SFT				3
#define RG_AUDIF_CK_TSTSEL_MASK				0x1
#define RG_AUDIF_CK_TSTSEL_MASK_SFT			(0x1 << 3)
#define RG_AUD_CK_TSTSEL_SFT				2
#define RG_AUD_CK_TSTSEL_MASK				0x1
#define RG_AUD_CK_TSTSEL_MASK_SFT			(0x1 << 2)
#define RG_AUD26M_CK_TST_DIS_SFT			0
#define RG_AUD26M_CK_TST_DIS_MASK			0x1
#define RG_AUD26M_CK_TST_DIS_MASK_SFT			(0x1 << 0)

/* AUD_TOP_CLK_HWEN_CON0 */
#define RG_AUD_INTRP_CK_PDN_HWEN_SFT			0
#define RG_AUD_INTRP_CK_PDN_HWEN_MASK			0x1
#define RG_AUD_INTRP_CK_PDN_HWEN_MASK_SFT		(0x1 << 0)

/* AUD_TOP_CLK_HWEN_CON0_SET */
#define RG_AUD_INTRP_CK_PND_HWEN_CON0_SET_SFT		0
#define RG_AUD_INTRP_CK_PND_HWEN_CON0_SET_MASK		0xffff
#define RG_AUD_INTRP_CK_PND_HWEN_CON0_SET_MASK_SFT	(0xffff << 0)

/* AUD_TOP_CLK_HWEN_CON0_CLR */
#define RG_AUD_INTRP_CLK_PDN_HWEN_CON0_CLR_SFT		0
#define RG_AUD_INTRP_CLK_PDN_HWEN_CON0_CLR_MASK		0xffff
#define RG_AUD_INTRP_CLK_PDN_HWEN_CON0_CLR_MASK_SFT	(0xffff << 0)

/* AUD_TOP_RST_CON0 */
#define RG_AUDNCP_RST_SFT				3
#define RG_AUDNCP_RST_MASK				0x1
#define RG_AUDNCP_RST_MASK_SFT				(0x1 << 3)
#define RG_ZCD_RST_SFT					2
#define RG_ZCD_RST_MASK					0x1
#define RG_ZCD_RST_MASK_SFT				(0x1 << 2)
#define RG_ACCDET_RST_SFT				1
#define RG_ACCDET_RST_MASK				0x1
#define RG_ACCDET_RST_MASK_SFT				(0x1 << 1)
#define RG_AUDIO_RST_SFT				0
#define RG_AUDIO_RST_MASK				0x1
#define RG_AUDIO_RST_MASK_SFT				(0x1 << 0)

/* AUD_TOP_RST_CON0_SET */
#define RG_AUD_TOP_RST_CON0_SET_SFT			0
#define RG_AUD_TOP_RST_CON0_SET_MASK			0xf
#define RG_AUD_TOP_RST_CON0_SET_MASK_SFT		(0xf << 0)

/* AUD_TOP_RST_CON0_CLR */
#define RG_AUD_TOP_RST_CON0_CLR_SFT			0
#define RG_AUD_TOP_RST_CON0_CLR_MASK			0xf
#define RG_AUD_TOP_RST_CON0_CLR_MASK_SFT		(0xf << 0)

/* AUD_TOP_RST_BANK_CON0 */
#define BANK_AUDZCD_SWRST_SFT				2
#define BANK_AUDZCD_SWRST_MASK				0x1
#define BANK_AUDZCD_SWRST_MASK_SFT			(0x1 << 2)
#define BANK_AUDIO_SWRST_SFT				1
#define BANK_AUDIO_SWRST_MASK				0x1
#define BANK_AUDIO_SWRST_MASK_SFT			(0x1 << 1)
#define BANK_ACCDET_SWRST_SFT				0
#define BANK_ACCDET_SWRST_MASK				0x1
#define BANK_ACCDET_SWRST_MASK_SFT			(0x1 << 0)

/* AFE_UL_DL_CON0 */
#define AFE_UL_LR_SWAP_SFT				15
#define AFE_UL_LR_SWAP_MASK				0x1
#define AFE_UL_LR_SWAP_MASK_SFT				(0x1 << 15)
#define AFE_DL_LR_SWAP_SFT				14
#define AFE_DL_LR_SWAP_MASK				0x1
#define AFE_DL_LR_SWAP_MASK_SFT				(0x1 << 14)
#define AFE_ON_SFT					0
#define AFE_ON_MASK					0x1
#define AFE_ON_MASK_SFT					(0x1 << 0)

/* AFE_DL_SRC2_CON0_L */
#define DL_2_SRC_ON_TMP_CTL_PRE_SFT			0
#define DL_2_SRC_ON_TMP_CTL_PRE_MASK			0x1
#define DL_2_SRC_ON_TMP_CTL_PRE_MASK_SFT		(0x1 << 0)

/* AFE_UL_SRC_CON0_H */
#define C_DIGMIC_PHASE_SEL_CH1_CTL_SFT			11
#define C_DIGMIC_PHASE_SEL_CH1_CTL_MASK			0x7
#define C_DIGMIC_PHASE_SEL_CH1_CTL_MASK_SFT		(0x7 << 11)
#define C_DIGMIC_PHASE_SEL_CH2_CTL_SFT			8
#define C_DIGMIC_PHASE_SEL_CH2_CTL_MASK			0x7
#define C_DIGMIC_PHASE_SEL_CH2_CTL_MASK_SFT		(0x7 << 8)
#define C_TWO_DIGITAL_MIC_CTL_SFT			7
#define C_TWO_DIGITAL_MIC_CTL_MASK			0x1
#define C_TWO_DIGITAL_MIC_CTL_MASK_SFT			(0x1 << 7)

/* AFE_UL_SRC_CON0_L */
#define DMIC_LOW_POWER_MODE_CTL_SFT			14
#define DMIC_LOW_POWER_MODE_CTL_MASK			0x3
#define DMIC_LOW_POWER_MODE_CTL_MASK_SFT		(0x3 << 14)
#define DIGMIC_4P33M_SEL_CTL_SFT			6
#define DIGMIC_4P33M_SEL_CTL_MASK			0x1
#define DIGMIC_4P33M_SEL_CTL_MASK_SFT			(0x1 << 6)
#define DIGMIC_3P25M_1P625M_SEL_CTL_SFT			5
#define DIGMIC_3P25M_1P625M_SEL_CTL_MASK		0x1
#define DIGMIC_3P25M_1P625M_SEL_CTL_MASK_SFT		(0x1 << 5)
#define UL_LOOP_BACK_MODE_CTL_SFT			2
#define UL_LOOP_BACK_MODE_CTL_MASK			0x1
#define UL_LOOP_BACK_MODE_CTL_MASK_SFT			(0x1 << 2)
#define UL_SDM_3_LEVEL_CTL_SFT				1
#define UL_SDM_3_LEVEL_CTL_MASK				0x1
#define UL_SDM_3_LEVEL_CTL_MASK_SFT			(0x1 << 1)
#define UL_SRC_ON_TMP_CTL_SFT				0
#define UL_SRC_ON_TMP_CTL_MASK				0x1
#define UL_SRC_ON_TMP_CTL_MASK_SFT			(0x1 << 0)

/* AFE_ADDA6_L_SRC_CON0_H */
#define ADDA6_C_DIGMIC_PHASE_SEL_CH1_CTL_SFT		11
#define ADDA6_C_DIGMIC_PHASE_SEL_CH1_CTL_MASK		0x7
#define ADDA6_C_DIGMIC_PHASE_SEL_CH1_CTL_MASK_SFT	(0x7 << 11)
#define ADDA6_C_DIGMIC_PHASE_SEL_CH2_CTL_SFT		8
#define ADDA6_C_DIGMIC_PHASE_SEL_CH2_CTL_MASK		0x7
#define ADDA6_C_DIGMIC_PHASE_SEL_CH2_CTL_MASK_SFT	(0x7 << 8)
#define ADDA6_C_TWO_DIGITAL_MIC_CTL_SFT			7
#define ADDA6_C_TWO_DIGITAL_MIC_CTL_MASK		0x1
#define ADDA6_C_TWO_DIGITAL_MIC_CTL_MASK_SFT		(0x1 << 7)

/* AFE_ADDA6_UL_SRC_CON0_L */
#define ADDA6_DMIC_LOW_POWER_MODE_CTL_SFT		14
#define ADDA6_DMIC_LOW_POWER_MODE_CTL_MASK		0x3
#define ADDA6_DMIC_LOW_POWER_MODE_CTL_MASK_SFT		(0x3 << 14)
#define ADDA6_DIGMIC_4P33M_SEL_CTL_SFT			6
#define ADDA6_DIGMIC_4P33M_SEL_CTL_MASK			0x1
#define ADDA6_DIGMIC_4P33M_SEL_CTL_MASK_SFT		(0x1 << 6)
#define ADDA6_DIGMIC_3P25M_1P625M_SEL_CTL_SFT		5
#define ADDA6_DIGMIC_3P25M_1P625M_SEL_CTL_MASK		0x1
#define ADDA6_DIGMIC_3P25M_1P625M_SEL_CTL_MASK_SFT	(0x1 << 5)
#define ADDA6_UL_LOOP_BACK_MODE_CTL_SFT			2
#define ADDA6_UL_LOOP_BACK_MODE_CTL_MASK		0x1
#define ADDA6_UL_LOOP_BACK_MODE_CTL_MASK_SFT		(0x1 << 2)
#define ADDA6_UL_SDM_3_LEVEL_CTL_SFT			1
#define ADDA6_UL_SDM_3_LEVEL_CTL_MASK			0x1
#define ADDA6_UL_SDM_3_LEVEL_CTL_MASK_SFT		(0x1 << 1)
#define ADDA6_UL_SRC_ON_TMP_CTL_SFT			0
#define ADDA6_UL_SRC_ON_TMP_CTL_MASK			0x1
#define ADDA6_UL_SRC_ON_TMP_CTL_MASK_SFT		(0x1 << 0)

/* AFE_TOP_CON0 */
#define ADDA6_MTKAIF_SINE_ON_SFT			4
#define ADDA6_MTKAIF_SINE_ON_MASK			0x1
#define ADDA6_MTKAIF_SINE_ON_MASK_SFT			(0x1 << 4)
#define ADDA6_UL_SINE_ON_SFT				3
#define ADDA6_UL_SINE_ON_MASK				0x1
#define ADDA6_UL_SINE_ON_MASK_SFT			(0x1 << 3)
#define MTKAIF_SINE_ON_SFT				2
#define MTKAIF_SINE_ON_MASK				0x1
#define MTKAIF_SINE_ON_MASK_SFT				(0x1 << 2)
#define UL_SINE_ON_SFT					1
#define UL_SINE_ON_MASK					0x1
#define UL_SINE_ON_MASK_SFT				(0x1 << 1)
#define DL_SINE_ON_SFT					0
#define DL_SINE_ON_MASK					0x1
#define DL_SINE_ON_MASK_SFT				(0x1 << 0)

/* AUDIO_TOP_CON0 */
#define PDN_AFE_CTL_SFT					7
#define PDN_AFE_CTL_MASK				0x1
#define PDN_AFE_CTL_MASK_SFT				(0x1 << 7)
#define PDN_DAC_CTL_SFT					6
#define PDN_DAC_CTL_MASK				0x1
#define PDN_DAC_CTL_MASK_SFT				(0x1 << 6)
#define PDN_ADC_CTL_SFT					5
#define PDN_ADC_CTL_MASK				0x1
#define PDN_ADC_CTL_MASK_SFT				(0x1 << 5)
#define PDN_ADDA6_ADC_CTL_SFT				4
#define PDN_ADDA6_ADC_CTL_MASK				0x1
#define PDN_ADDA6_ADC_CTL_MASK_SFT			(0x1 << 4)
#define PDN_I2S_DL_CTL_SFT				3
#define PDN_I2S_DL_CTL_MASK				0x1
#define PDN_I2S_DL_CTL_MASK_SFT				(0x1 << 3)
#define PWR_CLK_DIS_CTL_SFT				2
#define PWR_CLK_DIS_CTL_MASK				0x1
#define PWR_CLK_DIS_CTL_MASK_SFT			(0x1 << 2)
#define PDN_AFE_TESTMODEL_CTL_SFT			1
#define PDN_AFE_TESTMODEL_CTL_MASK			0x1
#define PDN_AFE_TESTMODEL_CTL_MASK_SFT			(0x1 << 1)
#define PDN_RESERVED_SFT				0
#define PDN_RESERVED_MASK				0x1
#define PDN_RESERVED_MASK_SFT				(0x1 << 0)

/* AFE_MON_DEBUG0 */
#define AUDIO_SYS_TOP_MON_SWAP_SFT			14
#define AUDIO_SYS_TOP_MON_SWAP_MASK			0x3
#define AUDIO_SYS_TOP_MON_SWAP_MASK_SFT			(0x3 << 14)
#define AUDIO_SYS_TOP_MON_SEL_SFT			8
#define AUDIO_SYS_TOP_MON_SEL_MASK			0x1f
#define AUDIO_SYS_TOP_MON_SEL_MASK_SFT			(0x1f << 8)
#define AFE_MON_SEL_SFT					0
#define AFE_MON_SEL_MASK				0xff
#define AFE_MON_SEL_MASK_SFT				(0xff << 0)

/* AFUNC_AUD_CON0 */
#define CCI_AUD_ANACK_SEL_SFT				15
#define CCI_AUD_ANACK_SEL_MASK				0x1
#define CCI_AUD_ANACK_SEL_MASK_SFT			(0x1 << 15)
#define CCI_AUDIO_FIFO_WPTR_SFT				12
#define CCI_AUDIO_FIFO_WPTR_MASK			0x7
#define CCI_AUDIO_FIFO_WPTR_MASK_SFT			(0x7 << 12)
#define CCI_SCRAMBLER_CG_EN_SFT				11
#define CCI_SCRAMBLER_CG_EN_MASK			0x1
#define CCI_SCRAMBLER_CG_EN_MASK_SFT			(0x1 << 11)
#define CCI_LCH_INV_SFT					10
#define CCI_LCH_INV_MASK				0x1
#define CCI_LCH_INV_MASK_SFT				(0x1 << 10)
#define CCI_RAND_EN_SFT					9
#define CCI_RAND_EN_MASK				0x1
#define CCI_RAND_EN_MASK_SFT				(0x1 << 9)
#define CCI_SPLT_SCRMB_CLK_ON_SFT			8
#define CCI_SPLT_SCRMB_CLK_ON_MASK			0x1
#define CCI_SPLT_SCRMB_CLK_ON_MASK_SFT			(0x1 << 8)
#define CCI_SPLT_SCRMB_ON_SFT				7
#define CCI_SPLT_SCRMB_ON_MASK				0x1
#define CCI_SPLT_SCRMB_ON_MASK_SFT			(0x1 << 7)
#define CCI_AUD_IDAC_TEST_EN_SFT			6
#define CCI_AUD_IDAC_TEST_EN_MASK			0x1
#define CCI_AUD_IDAC_TEST_EN_MASK_SFT			(0x1 << 6)
#define CCI_ZERO_PAD_DISABLE_SFT			5
#define CCI_ZERO_PAD_DISABLE_MASK			0x1
#define CCI_ZERO_PAD_DISABLE_MASK_SFT			(0x1 << 5)
#define CCI_AUD_SPLIT_TEST_EN_SFT			4
#define CCI_AUD_SPLIT_TEST_EN_MASK			0x1
#define CCI_AUD_SPLIT_TEST_EN_MASK_SFT			(0x1 << 4)
#define CCI_AUD_SDM_MUTEL_SFT				3
#define CCI_AUD_SDM_MUTEL_MASK				0x1
#define CCI_AUD_SDM_MUTEL_MASK_SFT			(0x1 << 3)
#define CCI_AUD_SDM_MUTER_SFT				2
#define CCI_AUD_SDM_MUTER_MASK				0x1
#define CCI_AUD_SDM_MUTER_MASK_SFT			(0x1 << 2)
#define CCI_AUD_SDM_7BIT_SEL_SFT			1
#define CCI_AUD_SDM_7BIT_SEL_MASK			0x1
#define CCI_AUD_SDM_7BIT_SEL_MASK_SFT			(0x1 << 1)
#define CCI_SCRAMBLER_EN_SFT				0
#define CCI_SCRAMBLER_EN_MASK				0x1
#define CCI_SCRAMBLER_EN_MASK_SFT			(0x1 << 0)

/* AFUNC_AUD_CON1 */
#define AUD_SDM_TEST_L_SFT				8
#define AUD_SDM_TEST_L_MASK				0xff
#define AUD_SDM_TEST_L_MASK_SFT				(0xff << 8)
#define AUD_SDM_TEST_R_SFT				0
#define AUD_SDM_TEST_R_MASK				0xff
#define AUD_SDM_TEST_R_MASK_SFT				(0xff << 0)

/* AFUNC_AUD_CON2 */
#define CCI_AUD_DAC_ANA_MUTE_SFT			7
#define CCI_AUD_DAC_ANA_MUTE_MASK			0x1
#define CCI_AUD_DAC_ANA_MUTE_MASK_SFT			(0x1 << 7)
#define CCI_AUD_DAC_ANA_RSTB_SEL_SFT			6
#define CCI_AUD_DAC_ANA_RSTB_SEL_MASK			0x1
#define CCI_AUD_DAC_ANA_RSTB_SEL_MASK_SFT		(0x1 << 6)
#define CCI_AUDIO_FIFO_CLKIN_INV_SFT			4
#define CCI_AUDIO_FIFO_CLKIN_INV_MASK			0x1
#define CCI_AUDIO_FIFO_CLKIN_INV_MASK_SFT		(0x1 << 4)
#define CCI_AUDIO_FIFO_ENABLE_SFT			3
#define CCI_AUDIO_FIFO_ENABLE_MASK			0x1
#define CCI_AUDIO_FIFO_ENABLE_MASK_SFT			(0x1 << 3)
#define CCI_ACD_MODE_SFT				2
#define CCI_ACD_MODE_MASK				0x1
#define CCI_ACD_MODE_MASK_SFT				(0x1 << 2)
#define CCI_AFIFO_CLK_PWDB_SFT				1
#define CCI_AFIFO_CLK_PWDB_MASK				0x1
#define CCI_AFIFO_CLK_PWDB_MASK_SFT			(0x1 << 1)
#define CCI_ACD_FUNC_RSTB_SFT				0
#define CCI_ACD_FUNC_RSTB_MASK				0x1
#define CCI_ACD_FUNC_RSTB_MASK_SFT			(0x1 << 0)

/* AFUNC_AUD_CON3 */
#define SDM_ANA13M_TESTCK_SEL_SFT			15
#define SDM_ANA13M_TESTCK_SEL_MASK			0x1
#define SDM_ANA13M_TESTCK_SEL_MASK_SFT			(0x1 << 15)
#define SDM_ANA13M_TESTCK_SRC_SEL_SFT			12
#define SDM_ANA13M_TESTCK_SRC_SEL_MASK			0x7
#define SDM_ANA13M_TESTCK_SRC_SEL_MASK_SFT		(0x7 << 12)
#define SDM_TESTCK_SRC_SEL_SFT				8
#define SDM_TESTCK_SRC_SEL_MASK				0x7
#define SDM_TESTCK_SRC_SEL_MASK_SFT			(0x7 << 8)
#define DIGMIC_TESTCK_SRC_SEL_SFT			4
#define DIGMIC_TESTCK_SRC_SEL_MASK			0x7
#define DIGMIC_TESTCK_SRC_SEL_MASK_SFT			(0x7 << 4)
#define DIGMIC_TESTCK_SEL_SFT				0
#define DIGMIC_TESTCK_SEL_MASK				0x1
#define DIGMIC_TESTCK_SEL_MASK_SFT			(0x1 << 0)

/* AFUNC_AUD_CON4 */
#define UL_FIFO_WCLK_INV_SFT				8
#define UL_FIFO_WCLK_INV_MASK				0x1
#define UL_FIFO_WCLK_INV_MASK_SFT			(0x1 << 8)
#define UL_FIFO_DIGMIC_WDATA_TESTSRC_SEL_SFT		6
#define UL_FIFO_DIGMIC_WDATA_TESTSRC_SEL_MASK		0x1
#define UL_FIFO_DIGMIC_WDATA_TESTSRC_SEL_MASK_SFT	(0x1 << 6)
#define UL_FIFO_WDATA_TESTEN_SFT			5
#define UL_FIFO_WDATA_TESTEN_MASK			0x1
#define UL_FIFO_WDATA_TESTEN_MASK_SFT			(0x1 << 5)
#define UL_FIFO_WDATA_TESTSRC_SEL_SFT			4
#define UL_FIFO_WDATA_TESTSRC_SEL_MASK			0x1
#define UL_FIFO_WDATA_TESTSRC_SEL_MASK_SFT		(0x1 << 4)
#define UL_FIFO_WCLK_6P5M_TESTCK_SEL_SFT		 3
#define UL_FIFO_WCLK_6P5M_TESTCK_SEL_MASK		0x1
#define UL_FIFO_WCLK_6P5M_TESTCK_SEL_MASK_SFT		(0x1 << 3)
#define UL_FIFO_WCLK_6P5M_TESTCK_SRC_SEL_SFT		0
#define UL_FIFO_WCLK_6P5M_TESTCK_SRC_SEL_MASK		0x7
#define UL_FIFO_WCLK_6P5M_TESTCK_SRC_SEL_MASK_SFT	(0x7 << 0)

/* AFUNC_AUD_CON5 */
#define R_AUD_DAC_POS_LARGE_MONO_SFT			8
#define R_AUD_DAC_POS_LARGE_MONO_MASK			0xff
#define R_AUD_DAC_POS_LARGE_MONO_MASK_SFT		(0xff << 8)
#define R_AUD_DAC_NEG_LARGE_MONO_SFT			0
#define R_AUD_DAC_NEG_LARGE_MONO_MASK			0xff
#define R_AUD_DAC_NEG_LARGE_MONO_MASK_SFT		(0xff << 0)

/* AFUNC_AUD_CON6 */
#define R_AUD_DAC_POS_SMALL_MONO_SFT			12
#define R_AUD_DAC_POS_SMALL_MONO_MASK			0xf
#define R_AUD_DAC_POS_SMALL_MONO_MASK_SFT		(0xf << 12)
#define R_AUD_DAC_NEG_SMALL_MONO_SFT			8
#define R_AUD_DAC_NEG_SMALL_MONO_MASK			0xf
#define R_AUD_DAC_NEG_SMALL_MONO_MASK_SFT		(0xf << 8)
#define R_AUD_DAC_POS_TINY_MONO_SFT			6
#define R_AUD_DAC_POS_TINY_MONO_MASK			0x3
#define R_AUD_DAC_POS_TINY_MONO_MASK_SFT		(0x3 << 6)
#define R_AUD_DAC_NEG_TINY_MONO_SFT			4
#define R_AUD_DAC_NEG_TINY_MONO_MASK			0x3
#define R_AUD_DAC_NEG_TINY_MONO_MASK_SFT		(0x3 << 4)
#define R_AUD_DAC_MONO_SEL_SFT				3
#define R_AUD_DAC_MONO_SEL_MASK				0x1
#define R_AUD_DAC_MONO_SEL_MASK_SFT			(0x1 << 3)
#define R_AUD_DAC_3TH_SEL_SFT				1
#define R_AUD_DAC_3TH_SEL_MASK				0x1
#define R_AUD_DAC_3TH_SEL_MASK_SFT			(0x1 << 1)
#define R_AUD_DAC_SW_RSTB_SFT				0
#define R_AUD_DAC_SW_RSTB_MASK				0x1
#define R_AUD_DAC_SW_RSTB_MASK_SFT			(0x1 << 0)

/* AFUNC_AUD_CON7 */
#define UL2_DIGMIC_TESTCK_SRC_SEL_SFT			10
#define UL2_DIGMIC_TESTCK_SRC_SEL_MASK			0x7
#define UL2_DIGMIC_TESTCK_SRC_SEL_MASK_SFT		(0x7 << 10)
#define UL2_DIGMIC_TESTCK_SEL_SFT			9
#define UL2_DIGMIC_TESTCK_SEL_MASK			0x1
#define UL2_DIGMIC_TESTCK_SEL_MASK_SFT			(0x1 << 9)
#define UL2_FIFO_WCLK_INV_SFT				8
#define UL2_FIFO_WCLK_INV_MASK				0x1
#define UL2_FIFO_WCLK_INV_MASK_SFT			(0x1 << 8)
#define UL2_FIFO_DIGMIC_WDATA_TESTSRC_SEL_SFT		6
#define UL2_FIFO_DIGMIC_WDATA_TESTSRC_SEL_MASK		0x1
#define UL2_FIFO_DIGMIC_WDATA_TESTSRC_SEL_MASK_SFT	(0x1 << 6)
#define UL2_FIFO_WDATA_TESTEN_SFT			5
#define UL2_FIFO_WDATA_TESTEN_MASK			0x1
#define UL2_FIFO_WDATA_TESTEN_MASK_SFT			(0x1 << 5)
#define UL2_FIFO_WDATA_TESTSRC_SEL_SFT			4
#define UL2_FIFO_WDATA_TESTSRC_SEL_MASK			0x1
#define UL2_FIFO_WDATA_TESTSRC_SEL_MASK_SFT		(0x1 << 4)
#define UL2_FIFO_WCLK_6P5M_TESTCK_SEL_SFT		3
#define UL2_FIFO_WCLK_6P5M_TESTCK_SEL_MASK		0x1
#define UL2_FIFO_WCLK_6P5M_TESTCK_SEL_MASK_SFT		(0x1 << 3)
#define UL2_FIFO_WCLK_6P5M_TESTCK_SRC_SEL_SFT		0
#define UL2_FIFO_WCLK_6P5M_TESTCK_SRC_SEL_MASK		0x7
#define UL2_FIFO_WCLK_6P5M_TESTCK_SRC_SEL_MASK_SFT	(0x7 << 0)

/* AFUNC_AUD_CON8 */
#define SPLITTER2_DITHER_EN_SFT				9
#define SPLITTER2_DITHER_EN_MASK			0x1
#define SPLITTER2_DITHER_EN_MASK_SFT			(0x1 << 9)
#define SPLITTER1_DITHER_EN_SFT				8
#define SPLITTER1_DITHER_EN_MASK			0x1
#define SPLITTER1_DITHER_EN_MASK_SFT			(0x1 << 8)
#define SPLITTER2_DITHER_GAIN_SFT			4
#define SPLITTER2_DITHER_GAIN_MASK			0xf
#define SPLITTER2_DITHER_GAIN_MASK_SFT			(0xf << 4)
#define SPLITTER1_DITHER_GAIN_SFT			0
#define SPLITTER1_DITHER_GAIN_MASK			0xf
#define SPLITTER1_DITHER_GAIN_MASK_SFT			(0xf << 0)

/* AFUNC_AUD_CON9 */
#define CCI_AUD_ANACK_SEL_2ND_SFT			15
#define CCI_AUD_ANACK_SEL_2ND_MASK			0x1
#define CCI_AUD_ANACK_SEL_2ND_MASK_SFT			(0x1 << 15)
#define CCI_AUDIO_FIFO_WPTR_2ND_SFT			12
#define CCI_AUDIO_FIFO_WPTR_2ND_MASK			0x7
#define CCI_AUDIO_FIFO_WPTR_2ND_MASK_SFT		(0x7 << 12)
#define CCI_SCRAMBLER_CG_EN_2ND_SFT			11
#define CCI_SCRAMBLER_CG_EN_2ND_MASK			0x1
#define CCI_SCRAMBLER_CG_EN_2ND_MASK_SFT		 (0x1 << 11)
#define CCI_LCH_INV_2ND_SFT				10
#define CCI_LCH_INV_2ND_MASK				0x1
#define CCI_LCH_INV_2ND_MASK_SFT			(0x1 << 10)
#define CCI_RAND_EN_2ND_SFT				9
#define CCI_RAND_EN_2ND_MASK				0x1
#define CCI_RAND_EN_2ND_MASK_SFT			(0x1 << 9)
#define CCI_SPLT_SCRMB_CLK_ON_2ND_SFT			8
#define CCI_SPLT_SCRMB_CLK_ON_2ND_MASK			0x1
#define CCI_SPLT_SCRMB_CLK_ON_2ND_MASK_SFT		(0x1 << 8)
#define CCI_SPLT_SCRMB_ON_2ND_SFT			7
#define CCI_SPLT_SCRMB_ON_2ND_MASK			0x1
#define CCI_SPLT_SCRMB_ON_2ND_MASK_SFT			(0x1 << 7)
#define CCI_AUD_IDAC_TEST_EN_2ND_SFT			6
#define CCI_AUD_IDAC_TEST_EN_2ND_MASK			0x1
#define CCI_AUD_IDAC_TEST_EN_2ND_MASK_SFT		(0x1 << 6)
#define CCI_ZERO_PAD_DISABLE_2ND_SFT			5
#define CCI_ZERO_PAD_DISABLE_2ND_MASK			0x1
#define CCI_ZERO_PAD_DISABLE_2ND_MASK_SFT		(0x1 << 5)
#define CCI_AUD_SPLIT_TEST_EN_2ND_SFT			4
#define CCI_AUD_SPLIT_TEST_EN_2ND_MASK			0x1
#define CCI_AUD_SPLIT_TEST_EN_2ND_MASK_SFT		(0x1 << 4)
#define CCI_AUD_SDM_MUTEL_2ND_SFT			3
#define CCI_AUD_SDM_MUTEL_2ND_MASK			0x1
#define CCI_AUD_SDM_MUTEL_2ND_MASK_SFT			(0x1 << 3)
#define CCI_AUD_SDM_MUTER_2ND_SFT			2
#define CCI_AUD_SDM_MUTER_2ND_MASK			0x1
#define CCI_AUD_SDM_MUTER_2ND_MASK_SFT			(0x1 << 2)
#define CCI_AUD_SDM_7BIT_SEL_2ND_SFT			1
#define CCI_AUD_SDM_7BIT_SEL_2ND_MASK			0x1
#define CCI_AUD_SDM_7BIT_SEL_2ND_MASK_SFT		(0x1 << 1)
#define CCI_SCRAMBLER_EN_2ND_SFT			0
#define CCI_SCRAMBLER_EN_2ND_MASK			0x1
#define CCI_SCRAMBLER_EN_2ND_MASK_SFT			(0x1 << 0)

/* AFUNC_AUD_CON10 */
#define AUD_SDM_TEST_L_2ND_SFT				8
#define AUD_SDM_TEST_L_2ND_MASK				0xff
#define AUD_SDM_TEST_L_2ND_MASK_SFT			(0xff << 8)
#define AUD_SDM_TEST_R_2ND_SFT				0
#define AUD_SDM_TEST_R_2ND_MASK				0xff
#define AUD_SDM_TEST_R_2ND_MASK_SFT			(0xff << 0)

/* AFUNC_AUD_CON11 */
#define CCI_AUD_DAC_ANA_MUTE_2ND_SFT			7
#define CCI_AUD_DAC_ANA_MUTE_2ND_MASK			0x1
#define CCI_AUD_DAC_ANA_MUTE_2ND_MASK_SFT		(0x1 << 7)
#define CCI_AUD_DAC_ANA_RSTB_SEL_2ND_SFT		6
#define CCI_AUD_DAC_ANA_RSTB_SEL_2ND_MASK		0x1
#define CCI_AUD_DAC_ANA_RSTB_SEL_2ND_MASK_SFT		(0x1 << 6)
#define CCI_AUDIO_FIFO_CLKIN_INV_2ND_SFT		4
#define CCI_AUDIO_FIFO_CLKIN_INV_2ND_MASK		0x1
#define CCI_AUDIO_FIFO_CLKIN_INV_2ND_MASK_SFT		(0x1 << 4)
#define CCI_AUDIO_FIFO_ENABLE_2ND_SFT			3
#define CCI_AUDIO_FIFO_ENABLE_2ND_MASK			0x1
#define CCI_AUDIO_FIFO_ENABLE_2ND_MASK_SFT		(0x1 << 3)
#define CCI_ACD_MODE_2ND_SFT				2
#define CCI_ACD_MODE_2ND_MASK				0x1
#define CCI_ACD_MODE_2ND_MASK_SFT			(0x1 << 2)
#define CCI_AFIFO_CLK_PWDB_2ND_SFT			1
#define CCI_AFIFO_CLK_PWDB_2ND_MASK			0x1
#define CCI_AFIFO_CLK_PWDB_2ND_MASK_SFT			(0x1 << 1)
#define CCI_ACD_FUNC_RSTB_2ND_SFT			0
#define CCI_ACD_FUNC_RSTB_2ND_MASK			0x1
#define CCI_ACD_FUNC_RSTB_2ND_MASK_SFT			(0x1 << 0)

/* AFUNC_AUD_CON12 */
#define SPLITTER2_DITHER_EN_2ND_SFT			9
#define SPLITTER2_DITHER_EN_2ND_MASK			0x1
#define SPLITTER2_DITHER_EN_2ND_MASK_SFT		(0x1 << 9)
#define SPLITTER1_DITHER_EN_2ND_SFT			8
#define SPLITTER1_DITHER_EN_2ND_MASK			0x1
#define SPLITTER1_DITHER_EN_2ND_MASK_SFT		(0x1 << 8)
#define SPLITTER2_DITHER_GAIN_2ND_SFT			4
#define SPLITTER2_DITHER_GAIN_2ND_MASK			0xf
#define SPLITTER2_DITHER_GAIN_2ND_MASK_SFT		(0xf << 4)
#define SPLITTER1_DITHER_GAIN_2ND_SFT			0
#define SPLITTER1_DITHER_GAIN_2ND_MASK			0xf
#define SPLITTER1_DITHER_GAIN_2ND_MASK_SFT		(0xf << 0)

/* AFUNC_AUD_MON0 */
#define AUD_SCR_OUT_L_SFT				8
#define AUD_SCR_OUT_L_MASK				0xff
#define AUD_SCR_OUT_L_MASK_SFT				(0xff << 8)
#define AUD_SCR_OUT_R_SFT				0
#define AUD_SCR_OUT_R_MASK				0xff
#define AUD_SCR_OUT_R_MASK_SFT				(0xff << 0)

/* AFUNC_AUD_MON1 */
#define AUD_SCR_OUT_L_2ND_SFT				8
#define AUD_SCR_OUT_L_2ND_MASK				0xff
#define AUD_SCR_OUT_L_2ND_MASK_SFT			(0xff << 8)
#define AUD_SCR_OUT_R_2ND_SFT				0
#define AUD_SCR_OUT_R_2ND_MASK				0xff
#define AUD_SCR_OUT_R_2ND_MASK_SFT			(0xff << 0)

/* AUDRC_TUNE_MON0 */
#define ASYNC_TEST_OUT_BCK_SFT				15
#define ASYNC_TEST_OUT_BCK_MASK				0x1
#define ASYNC_TEST_OUT_BCK_MASK_SFT			(0x1 << 15)
#define RGS_AUDRCTUNE1READ_SFT				8
#define RGS_AUDRCTUNE1READ_MASK				0x1f
#define RGS_AUDRCTUNE1READ_MASK_SFT			(0x1f << 8)
#define RGS_AUDRCTUNE0READ_SFT				0
#define RGS_AUDRCTUNE0READ_MASK				0x1f
#define RGS_AUDRCTUNE0READ_MASK_SFT			(0x1f << 0)

/* AFE_ADDA_MTKAIF_FIFO_CFG0 */
#define AFE_RESERVED_SFT				1
#define AFE_RESERVED_MASK				0x7fff
#define AFE_RESERVED_MASK_SFT				(0x7fff << 1)
#define RG_MTKAIF_RXIF_FIFO_INTEN_SFT			0
#define RG_MTKAIF_RXIF_FIFO_INTEN_MASK			0x1
#define RG_MTKAIF_RXIF_FIFO_INTEN_MASK_SFT		(0x1 << 0)

/* AFE_ADDA_MTKAIF_FIFO_LOG_MON1 */
#define MTKAIF_RXIF_WR_FULL_STATUS_SFT			1
#define MTKAIF_RXIF_WR_FULL_STATUS_MASK			0x1
#define MTKAIF_RXIF_WR_FULL_STATUS_MASK_SFT		(0x1 << 1)
#define MTKAIF_RXIF_RD_EMPTY_STATUS_SFT			0
#define MTKAIF_RXIF_RD_EMPTY_STATUS_MASK		0x1
#define MTKAIF_RXIF_RD_EMPTY_STATUS_MASK_SFT		(0x1 << 0)

/* AFE_ADDA_MTKAIF_MON0 */
#define MTKAIFTX_V3_SYNC_OUT_SFT			15
#define MTKAIFTX_V3_SYNC_OUT_MASK			0x1
#define MTKAIFTX_V3_SYNC_OUT_MASK_SFT			(0x1 << 15)
#define MTKAIFTX_V3_SDATA_OUT3_SFT			14
#define MTKAIFTX_V3_SDATA_OUT3_MASK			0x1
#define MTKAIFTX_V3_SDATA_OUT3_MASK_SFT			(0x1 << 14)
#define MTKAIFTX_V3_SDATA_OUT2_SFT			13
#define MTKAIFTX_V3_SDATA_OUT2_MASK			0x1
#define MTKAIFTX_V3_SDATA_OUT2_MASK_SFT			(0x1 << 13)
#define MTKAIFTX_V3_SDATA_OUT1_SFT			12
#define MTKAIFTX_V3_SDATA_OUT1_MASK			0x1
#define MTKAIFTX_V3_SDATA_OUT1_MASK_SFT			(0x1 << 12)
#define MTKAIF_RXIF_FIFO_STATUS_SFT			0
#define MTKAIF_RXIF_FIFO_STATUS_MASK			0xfff
#define MTKAIF_RXIF_FIFO_STATUS_MASK_SFT		(0xfff << 0)

/* AFE_ADDA_MTKAIF_MON1 */
#define MTKAIFRX_V3_SYNC_IN_SFT				15
#define MTKAIFRX_V3_SYNC_IN_MASK			0x1
#define MTKAIFRX_V3_SYNC_IN_MASK_SFT			(0x1 << 15)
#define MTKAIFRX_V3_SDATA_IN3_SFT			14
#define MTKAIFRX_V3_SDATA_IN3_MASK			0x1
#define MTKAIFRX_V3_SDATA_IN3_MASK_SFT			(0x1 << 14)
#define MTKAIFRX_V3_SDATA_IN2_SFT			13
#define MTKAIFRX_V3_SDATA_IN2_MASK			0x1
#define MTKAIFRX_V3_SDATA_IN2_MASK_SFT			(0x1 << 13)
#define MTKAIFRX_V3_SDATA_IN1_SFT			12
#define MTKAIFRX_V3_SDATA_IN1_MASK			0x1
#define MTKAIFRX_V3_SDATA_IN1_MASK_SFT			(0x1 << 12)
#define MTKAIF_RXIF_SEARCH_FAIL_FLAG_SFT		11
#define MTKAIF_RXIF_SEARCH_FAIL_FLAG_MASK		0x1
#define MTKAIF_RXIF_SEARCH_FAIL_FLAG_MASK_SFT		(0x1 << 11)
#define MTKAIF_RXIF_INVALID_FLAG_SFT			8
#define MTKAIF_RXIF_INVALID_FLAG_MASK			0x1
#define MTKAIF_RXIF_INVALID_FLAG_MASK_SFT		(0x1 << 8)
#define MTKAIF_RXIF_INVALID_CYCLE_SFT			0
#define MTKAIF_RXIF_INVALID_CYCLE_MASK			0xff
#define MTKAIF_RXIF_INVALID_CYCLE_MASK_SFT		(0xff << 0)

/* AFE_ADDA_MTKAIF_MON2 */
#define MTKAIF_TXIF_IN_CH2_SFT				8
#define MTKAIF_TXIF_IN_CH2_MASK				0xff
#define MTKAIF_TXIF_IN_CH2_MASK_SFT			(0xff << 8)
#define MTKAIF_TXIF_IN_CH1_SFT				0
#define MTKAIF_TXIF_IN_CH1_MASK				0xff
#define MTKAIF_TXIF_IN_CH1_MASK_SFT			(0xff << 0)

/* AFE_ADDA6_MTKAIF_MON3 */
#define ADDA6_MTKAIF_TXIF_IN_CH2_SFT			8
#define ADDA6_MTKAIF_TXIF_IN_CH2_MASK			0xff
#define ADDA6_MTKAIF_TXIF_IN_CH2_MASK_SFT		(0xff << 8)
#define ADDA6_MTKAIF_TXIF_IN_CH1_SFT			0
#define ADDA6_MTKAIF_TXIF_IN_CH1_MASK			0xff
#define ADDA6_MTKAIF_TXIF_IN_CH1_MASK_SFT		(0xff << 0)

/* AFE_ADDA_MTKAIF_MON4 */
#define MTKAIF_RXIF_OUT_CH2_SFT				8
#define MTKAIF_RXIF_OUT_CH2_MASK			0xff
#define MTKAIF_RXIF_OUT_CH2_MASK_SFT			(0xff << 8)
#define MTKAIF_RXIF_OUT_CH1_SFT				0
#define MTKAIF_RXIF_OUT_CH1_MASK			0xff
#define MTKAIF_RXIF_OUT_CH1_MASK_SFT			(0xff << 0)

/* AFE_ADDA_MTKAIF_MON5 */
#define MTKAIF_RXIF_OUT_CH3_SFT				0
#define MTKAIF_RXIF_OUT_CH3_MASK			0xff
#define MTKAIF_RXIF_OUT_CH3_MASK_SFT			(0xff << 0)

/* AFE_ADDA_MTKAIF_CFG0 */
#define RG_MTKAIF_RXIF_CLKINV_SFT			15
#define RG_MTKAIF_RXIF_CLKINV_MASK			0x1
#define RG_MTKAIF_RXIF_CLKINV_MASK_SFT			(0x1 << 15)
#define RG_ADDA6_MTKAIF_TXIF_PROTOCOL2_SFT		9
#define RG_ADDA6_MTKAIF_TXIF_PROTOCOL2_MASK		0x1
#define RG_ADDA6_MTKAIF_TXIF_PROTOCOL2_MASK_SFT		(0x1 << 9)
#define RG_MTKAIF_RXIF_PROTOCOL2_SFT			8
#define RG_MTKAIF_RXIF_PROTOCOL2_MASK			0x1
#define RG_MTKAIF_RXIF_PROTOCOL2_MASK_SFT		(0x1 << 8)
#define RG_MTKAIF_BYPASS_SRC_MODE_SFT			6
#define RG_MTKAIF_BYPASS_SRC_MODE_MASK			0x3
#define RG_MTKAIF_BYPASS_SRC_MODE_MASK_SFT		(0x3 << 6)
#define RG_MTKAIF_BYPASS_SRC_TEST_SFT			5
#define RG_MTKAIF_BYPASS_SRC_TEST_MASK			0x1
#define RG_MTKAIF_BYPASS_SRC_TEST_MASK_SFT		(0x1 << 5)
#define RG_MTKAIF_TXIF_PROTOCOL2_SFT			4
#define RG_MTKAIF_TXIF_PROTOCOL2_MASK			0x1
#define RG_MTKAIF_TXIF_PROTOCOL2_MASK_SFT		(0x1 << 4)
#define RG_ADDA6_MTKAIF_PMIC_TXIF_8TO5_SFT		3
#define RG_ADDA6_MTKAIF_PMIC_TXIF_8TO5_MASK		0x1
#define RG_ADDA6_MTKAIF_PMIC_TXIF_8TO5_MASK_SFT		(0x1 << 3)
#define RG_MTKAIF_PMIC_TXIF_8TO5_SFT			2
#define RG_MTKAIF_PMIC_TXIF_8TO5_MASK			0x1
#define RG_MTKAIF_PMIC_TXIF_8TO5_MASK_SFT		(0x1 << 2)
#define RG_MTKAIF_LOOPBACK_TEST2_SFT			1
#define RG_MTKAIF_LOOPBACK_TEST2_MASK			0x1
#define RG_MTKAIF_LOOPBACK_TEST2_MASK_SFT		(0x1 << 1)
#define RG_MTKAIF_LOOPBACK_TEST1_SFT			0
#define RG_MTKAIF_LOOPBACK_TEST1_MASK			0x1
#define RG_MTKAIF_LOOPBACK_TEST1_MASK_SFT		(0x1 << 0)

/* AFE_ADDA_MTKAIF_RX_CFG0 */
#define RG_MTKAIF_RXIF_VOICE_MODE_SFT			12
#define RG_MTKAIF_RXIF_VOICE_MODE_MASK			0xf
#define RG_MTKAIF_RXIF_VOICE_MODE_MASK_SFT		(0xf << 12)
#define RG_MTKAIF_RXIF_DATA_BIT_SFT			8
#define RG_MTKAIF_RXIF_DATA_BIT_MASK			0x7
#define RG_MTKAIF_RXIF_DATA_BIT_MASK_SFT		(0x7 << 8)
#define RG_MTKAIF_RXIF_FIFO_RSP_SFT			4
#define RG_MTKAIF_RXIF_FIFO_RSP_MASK			0x7
#define RG_MTKAIF_RXIF_FIFO_RSP_MASK_SFT		(0x7 << 4)
#define RG_MTKAIF_RXIF_DETECT_ON_SFT			3
#define RG_MTKAIF_RXIF_DETECT_ON_MASK			0x1
#define RG_MTKAIF_RXIF_DETECT_ON_MASK_SFT		(0x1 << 3)
#define RG_MTKAIF_RXIF_DATA_MODE_SFT			0
#define RG_MTKAIF_RXIF_DATA_MODE_MASK			0x1
#define RG_MTKAIF_RXIF_DATA_MODE_MASK_SFT		(0x1 << 0)

/* AFE_ADDA_MTKAIF_RX_CFG1 */
#define RG_MTKAIF_RXIF_SYNC_SEARCH_TABLE_SFT		12
#define RG_MTKAIF_RXIF_SYNC_SEARCH_TABLE_MASK		0xf
#define RG_MTKAIF_RXIF_SYNC_SEARCH_TABLE_MASK_SFT	(0xf << 12)
#define RG_MTKAIF_RXIF_INVALID_SYNC_CHECK_ROUND_SFT	8
#define RG_MTKAIF_RXIF_INVALID_SYNC_CHECK_ROUND_MASK	0xf
#define RG_MTKAIF_RXIF_INVALID_SYNC_CHECK_ROUND_MASK_SFT	(0xf << 8)
#define RG_MTKAIF_RXIF_SYNC_CHECK_ROUND_SFT		4
#define RG_MTKAIF_RXIF_SYNC_CHECK_ROUND_MASK		0xf
#define RG_MTKAIF_RXIF_SYNC_CHECK_ROUND_MASK_SFT	(0xf << 4)
#define RG_MTKAIF_RXIF_VOICE_MODE_PROTOCOL2_SFT		0
#define RG_MTKAIF_RXIF_VOICE_MODE_PROTOCOL2_MASK	0xf
#define RG_MTKAIF_RXIF_VOICE_MODE_PROTOCOL2_MASK_SFT	(0xf << 0)

/* AFE_ADDA_MTKAIF_RX_CFG2 */
#define RG_MTKAIF_RXIF_P2_INPUT_SEL_SFT			15
#define RG_MTKAIF_RXIF_P2_INPUT_SEL_MASK		0x1
#define RG_MTKAIF_RXIF_P2_INPUT_SEL_MASK_SFT		(0x1 << 15)
#define RG_MTKAIF_RXIF_SYNC_WORD2_DISABLE_SFT		14
#define RG_MTKAIF_RXIF_SYNC_WORD2_DISABLE_MASK		0x1
#define RG_MTKAIF_RXIF_SYNC_WORD2_DISABLE_MASK_SFT	(0x1 << 14)
#define RG_MTKAIF_RXIF_SYNC_WORD1_DISABLE_SFT		13
#define RG_MTKAIF_RXIF_SYNC_WORD1_DISABLE_MASK		0x1
#define RG_MTKAIF_RXIF_SYNC_WORD1_DISABLE_MASK_SFT	(0x1 << 13)
#define RG_MTKAIF_RXIF_CLEAR_SYNC_FAIL_SFT		12
#define RG_MTKAIF_RXIF_CLEAR_SYNC_FAIL_MASK		0x1
#define RG_MTKAIF_RXIF_CLEAR_SYNC_FAIL_MASK_SFT		(0x1 << 12)
#define RG_MTKAIF_RXIF_SYNC_CNT_TABLE_SFT		0
#define RG_MTKAIF_RXIF_SYNC_CNT_TABLE_MASK		0xfff
#define RG_MTKAIF_RXIF_SYNC_CNT_TABLE_MASK_SFT		(0xfff << 0)

/* AFE_ADDA_MTKAIF_RX_CFG3 */
#define RG_MTKAIF_RXIF_LOOPBACK_USE_NLE_SFT		7
#define RG_MTKAIF_RXIF_LOOPBACK_USE_NLE_MASK		0x1
#define RG_MTKAIF_RXIF_LOOPBACK_USE_NLE_MASK_SFT	(0x1 << 7)
#define RG_MTKAIF_RXIF_FIFO_RSP_PROTOCOL2_SFT		4
#define RG_MTKAIF_RXIF_FIFO_RSP_PROTOCOL2_MASK		0x7
#define RG_MTKAIF_RXIF_FIFO_RSP_PROTOCOL2_MASK_SFT	(0x7 << 4)
#define RG_MTKAIF_RXIF_DETECT_ON_PROTOCOL2_SFT		3
#define RG_MTKAIF_RXIF_DETECT_ON_PROTOCOL2_MASK		0x1
#define RG_MTKAIF_RXIF_DETECT_ON_PROTOCOL2_MASK_SFT	(0x1 << 3)

/* AFE_ADDA_MTKAIF_SYNCWORD_CFG0 */
#define RG_MTKAIF_RX_SYNC_WORD2_SFT			4
#define RG_MTKAIF_RX_SYNC_WORD2_MASK			0x7
#define RG_MTKAIF_RX_SYNC_WORD2_MASK_SFT		(0x7 << 4)
#define RG_MTKAIF_RX_SYNC_WORD1_SFT			0
#define RG_MTKAIF_RX_SYNC_WORD1_MASK			0x7
#define RG_MTKAIF_RX_SYNC_WORD1_MASK_SFT		(0x7 << 0)

/* AFE_ADDA_MTKAIF_SYNCWORD_CFG1 */
#define RG_ADDA6_MTKAIF_TX_SYNC_WORD2_SFT		12
#define RG_ADDA6_MTKAIF_TX_SYNC_WORD2_MASK		0x7
#define RG_ADDA6_MTKAIF_TX_SYNC_WORD2_MASK_SFT		(0x7 << 12)
#define RG_ADDA6_MTKAIF_TX_SYNC_WORD1_SFT		8
#define RG_ADDA6_MTKAIF_TX_SYNC_WORD1_MASK		0x7
#define RG_ADDA6_MTKAIF_TX_SYNC_WORD1_MASK_SFT		(0x7 << 8)
#define RG_ADDA_MTKAIF_TX_SYNC_WORD2_SFT		4
#define RG_ADDA_MTKAIF_TX_SYNC_WORD2_MASK		0x7
#define RG_ADDA_MTKAIF_TX_SYNC_WORD2_MASK_SFT		(0x7 << 4)
#define RG_ADDA_MTKAIF_TX_SYNC_WORD1_SFT		0
#define RG_ADDA_MTKAIF_TX_SYNC_WORD1_MASK		0x7
#define RG_ADDA_MTKAIF_TX_SYNC_WORD1_MASK_SFT		(0x7 << 0)

/* AFE_SGEN_CFG0 */
#define SGEN_AMP_DIV_CH1_CTL_SFT			12
#define SGEN_AMP_DIV_CH1_CTL_MASK			0xf
#define SGEN_AMP_DIV_CH1_CTL_MASK_SFT			(0xf << 12)
#define SGEN_DAC_EN_CTL_SFT				7
#define SGEN_DAC_EN_CTL_MASK				0x1
#define SGEN_DAC_EN_CTL_MASK_SFT			(0x1 << 7)
#define SGEN_MUTE_SW_CTL_SFT				6
#define SGEN_MUTE_SW_CTL_MASK				0x1
#define SGEN_MUTE_SW_CTL_MASK_SFT			(0x1 << 6)
#define R_AUD_SDM_MUTE_L_SFT				5
#define R_AUD_SDM_MUTE_L_MASK				0x1
#define R_AUD_SDM_MUTE_L_MASK_SFT			(0x1 << 5)
#define R_AUD_SDM_MUTE_R_SFT				4
#define R_AUD_SDM_MUTE_R_MASK				0x1
#define R_AUD_SDM_MUTE_R_MASK_SFT			(0x1 << 4)
#define R_AUD_SDM_MUTE_L_2ND_SFT			3
#define R_AUD_SDM_MUTE_L_2ND_MASK			0x1
#define R_AUD_SDM_MUTE_L_2ND_MASK_SFT			(0x1 << 3)
#define R_AUD_SDM_MUTE_R_2ND_SFT			2
#define R_AUD_SDM_MUTE_R_2ND_MASK			0x1
#define R_AUD_SDM_MUTE_R_2ND_MASK_SFT			(0x1 << 2)

/* AFE_SGEN_CFG1 */
#define C_SGEN_RCH_INV_5BIT_SFT				15
#define C_SGEN_RCH_INV_5BIT_MASK			0x1
#define C_SGEN_RCH_INV_5BIT_MASK_SFT			(0x1 << 15)
#define C_SGEN_RCH_INV_8BIT_SFT				14
#define C_SGEN_RCH_INV_8BIT_MASK			0x1
#define C_SGEN_RCH_INV_8BIT_MASK_SFT			(0x1 << 14)
#define SGEN_FREQ_DIV_CH1_CTL_SFT			0
#define SGEN_FREQ_DIV_CH1_CTL_MASK			0x1f
#define SGEN_FREQ_DIV_CH1_CTL_MASK_SFT			(0x1f << 0)

/* AFE_ADC_ASYNC_FIFO_CFG */
#define RG_UL_ASYNC_FIFO_SOFT_RST_EN_SFT		5
#define RG_UL_ASYNC_FIFO_SOFT_RST_EN_MASK		0x1
#define RG_UL_ASYNC_FIFO_SOFT_RST_EN_MASK_SFT		(0x1 << 5)
#define RG_UL_ASYNC_FIFO_SOFT_RST_SFT			4
#define RG_UL_ASYNC_FIFO_SOFT_RST_MASK			0x1
#define RG_UL_ASYNC_FIFO_SOFT_RST_MASK_SFT		(0x1 << 4)
#define RG_AMIC_UL_ADC_CLK_SEL_SFT			1
#define RG_AMIC_UL_ADC_CLK_SEL_MASK			0x1
#define RG_AMIC_UL_ADC_CLK_SEL_MASK_SFT			(0x1 << 1)

/* AFE_ADC_ASYNC_FIFO_CFG1 */
#define RG_UL2_ASYNC_FIFO_SOFT_RST_EN_SFT		5
#define RG_UL2_ASYNC_FIFO_SOFT_RST_EN_MASK		0x1
#define RG_UL2_ASYNC_FIFO_SOFT_RST_EN_MASK_SFT		(0x1 << 5)
#define RG_UL2_ASYNC_FIFO_SOFT_RST_SFT			4
#define RG_UL2_ASYNC_FIFO_SOFT_RST_MASK			0x1
#define RG_UL2_ASYNC_FIFO_SOFT_RST_MASK_SFT		(0x1 << 4)

/* AFE_DCCLK_CFG0 */
#define DCCLK_DIV_SFT					5
#define DCCLK_DIV_MASK					0x7ff
#define DCCLK_DIV_MASK_SFT				(0x7ff << 5)
#define DCCLK_INV_SFT					4
#define DCCLK_INV_MASK					0x1
#define DCCLK_INV_MASK_SFT				(0x1 << 4)
#define DCCLK_REF_CK_SEL_SFT				2
#define DCCLK_REF_CK_SEL_MASK				0x3
#define DCCLK_REF_CK_SEL_MASK_SFT			(0x3 << 2)
#define DCCLK_PDN_SFT					1
#define DCCLK_PDN_MASK					0x1
#define DCCLK_PDN_MASK_SFT				(0x1 << 1)
#define DCCLK_GEN_ON_SFT				0
#define DCCLK_GEN_ON_MASK				0x1
#define DCCLK_GEN_ON_MASK_SFT				(0x1 << 0)

/* AFE_DCCLK_CFG1 */
#define RESYNC_SRC_SEL_SFT				10
#define RESYNC_SRC_SEL_MASK				0x3
#define RESYNC_SRC_SEL_MASK_SFT				(0x3 << 10)
#define RESYNC_SRC_CK_INV_SFT				9
#define RESYNC_SRC_CK_INV_MASK				0x1
#define RESYNC_SRC_CK_INV_MASK_SFT			(0x1 << 9)
#define DCCLK_RESYNC_BYPASS_SFT				8
#define DCCLK_RESYNC_BYPASS_MASK			0x1
#define DCCLK_RESYNC_BYPASS_MASK_SFT			(0x1 << 8)
#define DCCLK_PHASE_SEL_SFT				4
#define DCCLK_PHASE_SEL_MASK				0xf
#define DCCLK_PHASE_SEL_MASK_SFT			(0xf << 4)

/* AUDIO_DIG_CFG */
#define RG_AUD_PAD_TOP_DAT_MISO2_LOOPBACK_SFT		15
#define RG_AUD_PAD_TOP_DAT_MISO2_LOOPBACK_MASK		0x1
#define RG_AUD_PAD_TOP_DAT_MISO2_LOOPBACK_MASK_SFT	(0x1 << 15)
#define RG_AUD_PAD_TOP_PHASE_MODE2_SFT			8
#define RG_AUD_PAD_TOP_PHASE_MODE2_MASK			0x7f
#define RG_AUD_PAD_TOP_PHASE_MODE2_MASK_SFT		(0x7f << 8)
#define RG_AUD_PAD_TOP_DAT_MISO_LOOPBACK_SFT		7
#define RG_AUD_PAD_TOP_DAT_MISO_LOOPBACK_MASK		0x1
#define RG_AUD_PAD_TOP_DAT_MISO_LOOPBACK_MASK_SFT	(0x1 << 7)
#define RG_AUD_PAD_TOP_PHASE_MODE_SFT			0
#define RG_AUD_PAD_TOP_PHASE_MODE_MASK			0x7f
#define RG_AUD_PAD_TOP_PHASE_MODE_MASK_SFT		(0x7f << 0)

/* AUDIO_DIG_CFG1 */
#define RG_AUD_PAD_TOP_DAT_MISO3_LOOPBACK_SFT		7
#define RG_AUD_PAD_TOP_DAT_MISO3_LOOPBACK_MASK		0x1
#define RG_AUD_PAD_TOP_DAT_MISO3_LOOPBACK_MASK_SFT	(0x1 << 7)
#define RG_AUD_PAD_TOP_PHASE_MODE3_SFT			0
#define RG_AUD_PAD_TOP_PHASE_MODE3_MASK			0x7f
#define RG_AUD_PAD_TOP_PHASE_MODE3_MASK_SFT		(0x7f << 0)

/* AFE_AUD_PAD_TOP */
#define RG_AUD_PAD_TOP_TX_FIFO_RSP_SFT			12
#define RG_AUD_PAD_TOP_TX_FIFO_RSP_MASK			0x7
#define RG_AUD_PAD_TOP_TX_FIFO_RSP_MASK_SFT		(0x7 << 12)
#define RG_AUD_PAD_TOP_MTKAIF_CLK_PROTOCOL2_SFT		11
#define RG_AUD_PAD_TOP_MTKAIF_CLK_PROTOCOL2_MASK	0x1
#define RG_AUD_PAD_TOP_MTKAIF_CLK_PROTOCOL2_MASK_SFT	(0x1 << 11)
#define RG_AUD_PAD_TOP_TX_FIFO_ON_SFT			8
#define RG_AUD_PAD_TOP_TX_FIFO_ON_MASK			0x1
#define RG_AUD_PAD_TOP_TX_FIFO_ON_MASK_SFT		(0x1 << 8)

/* AFE_AUD_PAD_TOP_MON */
#define ADDA_AUD_PAD_TOP_MON_SFT			0
#define ADDA_AUD_PAD_TOP_MON_MASK			0xffff
#define ADDA_AUD_PAD_TOP_MON_MASK_SFT			(0xffff << 0)

/* AFE_AUD_PAD_TOP_MON1 */
#define ADDA_AUD_PAD_TOP_MON1_SFT			0
#define ADDA_AUD_PAD_TOP_MON1_MASK			0xffff
#define ADDA_AUD_PAD_TOP_MON1_MASK_SFT			(0xffff << 0)

/* AFE_AUD_PAD_TOP_MON2 */
#define ADDA_AUD_PAD_TOP_MON2_SFT			0
#define ADDA_AUD_PAD_TOP_MON2_MASK			0xffff
#define ADDA_AUD_PAD_TOP_MON2_MASK_SFT			(0xffff << 0)

/* AFE_DL_NLE_CFG */
#define NLE_RCH_HPGAIN_SEL_SFT				10
#define NLE_RCH_HPGAIN_SEL_MASK				0x1
#define NLE_RCH_HPGAIN_SEL_MASK_SFT			(0x1 << 10)
#define NLE_RCH_CH_SEL_SFT				9
#define NLE_RCH_CH_SEL_MASK				0x1
#define NLE_RCH_CH_SEL_MASK_SFT				(0x1 << 9)
#define NLE_RCH_ON_SFT					8
#define NLE_RCH_ON_MASK					0x1
#define NLE_RCH_ON_MASK_SFT				(0x1 << 8)
#define NLE_LCH_HPGAIN_SEL_SFT				2
#define NLE_LCH_HPGAIN_SEL_MASK				0x1
#define NLE_LCH_HPGAIN_SEL_MASK_SFT			(0x1 << 2)
#define NLE_LCH_CH_SEL_SFT				1
#define NLE_LCH_CH_SEL_MASK				0x1
#define NLE_LCH_CH_SEL_MASK_SFT				(0x1 << 1)
#define NLE_LCH_ON_SFT					0
#define NLE_LCH_ON_MASK					0x1
#define NLE_LCH_ON_MASK_SFT				(0x1 << 0)

/* AFE_DL_NLE_MON */
#define NLE_MONITOR_SFT					0
#define NLE_MONITOR_MASK				0x3fff
#define NLE_MONITOR_MASK_SFT				(0x3fff << 0)

/* AFE_CG_EN_MON */
#define CK_CG_EN_MON_SFT				0
#define CK_CG_EN_MON_MASK				0x3f
#define CK_CG_EN_MON_MASK_SFT				(0x3f << 0)

/* AFE_MIC_ARRAY_CFG */
#define RG_AMIC_ADC1_SOURCE_SEL_SFT			10
#define RG_AMIC_ADC1_SOURCE_SEL_MASK			0x3
#define RG_AMIC_ADC1_SOURCE_SEL_MASK_SFT		(0x3 << 10)
#define RG_AMIC_ADC2_SOURCE_SEL_SFT			8
#define RG_AMIC_ADC2_SOURCE_SEL_MASK			0x3
#define RG_AMIC_ADC2_SOURCE_SEL_MASK_SFT		(0x3 << 8)
#define RG_AMIC_ADC3_SOURCE_SEL_SFT			6
#define RG_AMIC_ADC3_SOURCE_SEL_MASK			0x3
#define RG_AMIC_ADC3_SOURCE_SEL_MASK_SFT		(0x3 << 6)
#define RG_DMIC_ADC1_SOURCE_SEL_SFT			4
#define RG_DMIC_ADC1_SOURCE_SEL_MASK			0x3
#define RG_DMIC_ADC1_SOURCE_SEL_MASK_SFT		(0x3 << 4)
#define RG_DMIC_ADC2_SOURCE_SEL_SFT			2
#define RG_DMIC_ADC2_SOURCE_SEL_MASK			0x3
#define RG_DMIC_ADC2_SOURCE_SEL_MASK_SFT		(0x3 << 2)
#define RG_DMIC_ADC3_SOURCE_SEL_SFT			0
#define RG_DMIC_ADC3_SOURCE_SEL_MASK			0x3
#define RG_DMIC_ADC3_SOURCE_SEL_MASK_SFT		(0x3 << 0)

/* AFE_CHOP_CFG0 */
#define RG_CHOP_DIV_SEL_SFT				4
#define RG_CHOP_DIV_SEL_MASK				0x1f
#define RG_CHOP_DIV_SEL_MASK_SFT			(0x1f << 4)
#define RG_CHOP_DIV_EN_SFT				0
#define RG_CHOP_DIV_EN_MASK				0x1
#define RG_CHOP_DIV_EN_MASK_SFT				(0x1 << 0)

/* AFE_MTKAIF_MUX_CFG */
#define RG_ADDA6_EN_SEL_SFT				12
#define RG_ADDA6_EN_SEL_MASK				0x1
#define RG_ADDA6_EN_SEL_MASK_SFT			(0x1 << 12)
#define RG_ADDA6_CH2_SEL_SFT				10
#define RG_ADDA6_CH2_SEL_MASK				0x3
#define RG_ADDA6_CH2_SEL_MASK_SFT			(0x3 << 10)
#define RG_ADDA6_CH1_SEL_SFT				8
#define RG_ADDA6_CH1_SEL_MASK				0x3
#define RG_ADDA6_CH1_SEL_MASK_SFT			(0x3 << 8)
#define RG_ADDA_EN_SEL_SFT				4
#define RG_ADDA_EN_SEL_MASK				0x1
#define RG_ADDA_EN_SEL_MASK_SFT				(0x1 << 4)
#define RG_ADDA_CH2_SEL_SFT				2
#define RG_ADDA_CH2_SEL_MASK				0x3
#define RG_ADDA_CH2_SEL_MASK_SFT			(0x3 << 2)
#define RG_ADDA_CH1_SEL_SFT				0
#define RG_ADDA_CH1_SEL_MASK				0x3
#define RG_ADDA_CH1_SEL_MASK_SFT			(0x3 << 0)

/* AFE_PMIC_NEWIF_CFG3 */
#define RG_UP8X_SYNC_WORD_SFT				0
#define RG_UP8X_SYNC_WORD_MASK				0xffff
#define RG_UP8X_SYNC_WORD_MASK_SFT			(0xffff << 0)

/* AFE_NCP_CFG0 */
#define  RG_NCP_CK1_VALID_CNT_SFT			9
#define  RG_NCP_CK1_VALID_CNT_MASK			0x7f
#define  RG_NCP_CK1_VALID_CNT_MASK_SFT			(0x7f << 9)
#define RG_NCP_ADITH_SFT				8
#define RG_NCP_ADITH_MASK				0x1
#define RG_NCP_ADITH_MASK_SFT				(0x1 << 8)
#define RG_NCP_DITHER_EN_SFT				7
#define RG_NCP_DITHER_EN_MASK				0x1
#define RG_NCP_DITHER_EN_MASK_SFT			(0x1 << 7)
#define RG_NCP_DITHER_FIXED_CK0_ACK1_2P_SFT		4
#define RG_NCP_DITHER_FIXED_CK0_ACK1_2P_MASK		0x7
#define RG_NCP_DITHER_FIXED_CK0_ACK1_2P_MASK_SFT	(0x7 << 4)
#define RG_NCP_DITHER_FIXED_CK0_ACK2_2P_SFT		1
#define RG_NCP_DITHER_FIXED_CK0_ACK2_2P_MASK		0x7
#define RG_NCP_DITHER_FIXED_CK0_ACK2_2P_MASK_SFT	(0x7 << 1)
#define RG_NCP_ON_SFT					0
#define RG_NCP_ON_MASK					0x1
#define RG_NCP_ON_MASK_SFT				(0x1 << 0)

/* AFE_NCP_CFG1 */
#define RG_XY_VAL_CFG_EN_SFT				15
#define RG_XY_VAL_CFG_EN_MASK				0x1
#define RG_XY_VAL_CFG_EN_MASK_SFT			(0x1 << 15)
#define RG_X_VAL_CFG_SFT				8
#define RG_X_VAL_CFG_MASK				0x7f
#define RG_X_VAL_CFG_MASK_SFT				(0x7f << 8)
#define RG_Y_VAL_CFG_SFT				0
#define RG_Y_VAL_CFG_MASK				0x7f
#define RG_Y_VAL_CFG_MASK_SFT				(0x7f << 0)

/* AFE_NCP_CFG2 */
#define RG_NCP_NONCLK_SET_SFT				1
#define RG_NCP_NONCLK_SET_MASK				0x1
#define RG_NCP_NONCLK_SET_MASK_SFT			(0x1 << 1)
#define RG_NCP_PDDIS_EN_SFT				0
#define RG_NCP_PDDIS_EN_MASK				0x1
#define RG_NCP_PDDIS_EN_MASK_SFT			(0x1 << 0)

/* AUDENC_ANA_CON0 */
#define RG_AUDPREAMPLON_SFT				0
#define RG_AUDPREAMPLON_MASK				0x1
#define RG_AUDPREAMPLON_MASK_SFT			(0x1 << 0)
#define RG_AUDPREAMPLDCCEN_SFT				1
#define RG_AUDPREAMPLDCCEN_MASK				0x1
#define RG_AUDPREAMPLDCCEN_MASK_SFT			(0x1 << 1)
#define RG_AUDPREAMPLDCPRECHARGE_SFT			2
#define RG_AUDPREAMPLDCPRECHARGE_MASK			0x1
#define RG_AUDPREAMPLDCPRECHARGE_MASK_SFT		(0x1 << 2)
#define RG_AUDPREAMPLPGATEST_SFT			3
#define RG_AUDPREAMPLPGATEST_MASK			0x1
#define RG_AUDPREAMPLPGATEST_MASK_SFT			(0x1 << 3)
#define RG_AUDPREAMPLVSCALE_SFT				4
#define RG_AUDPREAMPLVSCALE_MASK			0x3
#define RG_AUDPREAMPLVSCALE_MASK_SFT			(0x3 << 4)
#define RG_AUDPREAMPLINPUTSEL_SFT			6
#define RG_AUDPREAMPLINPUTSEL_MASK			0x3
#define RG_AUDPREAMPLINPUTSEL_MASK_SFT			(0x3 << 6)
#define RG_AUDPREAMPLGAIN_SFT				8
#define RG_AUDPREAMPLGAIN_MASK				0x7
#define RG_AUDPREAMPLGAIN_MASK_SFT			(0x7 << 8)
#define RG_BULKL_VCM_EN_SFT				11
#define RG_BULKL_VCM_EN_MASK				0x1
#define RG_BULKL_VCM_EN_MASK_SFT			(0x1 << 11)
#define RG_AUDADCLPWRUP_SFT				12
#define RG_AUDADCLPWRUP_MASK				0x1
#define RG_AUDADCLPWRUP_MASK_SFT			(0x1 << 12)
#define RG_AUDADCLINPUTSEL_SFT				13
#define RG_AUDADCLINPUTSEL_MASK				0x3
#define RG_AUDADCLINPUTSEL_MASK_SFT			(0x3 << 13)

/* AUDENC_ANA_CON1 */
#define RG_AUDPREAMPRON_SFT				0
#define RG_AUDPREAMPRON_MASK				0x1
#define RG_AUDPREAMPRON_MASK_SFT			(0x1 << 0)
#define RG_AUDPREAMPRDCCEN_SFT				1
#define RG_AUDPREAMPRDCCEN_MASK				0x1
#define RG_AUDPREAMPRDCCEN_MASK_SFT			(0x1 << 1)
#define RG_AUDPREAMPRDCPRECHARGE_SFT			2
#define RG_AUDPREAMPRDCPRECHARGE_MASK			0x1
#define RG_AUDPREAMPRDCPRECHARGE_MASK_SFT		(0x1 << 2)
#define RG_AUDPREAMPRPGATEST_SFT			3
#define RG_AUDPREAMPRPGATEST_MASK			0x1
#define RG_AUDPREAMPRPGATEST_MASK_SFT			(0x1 << 3)
#define RG_AUDPREAMPRVSCALE_SFT				4
#define RG_AUDPREAMPRVSCALE_MASK			0x3
#define RG_AUDPREAMPRVSCALE_MASK_SFT			(0x3 << 4)
#define RG_AUDPREAMPRINPUTSEL_SFT			6
#define RG_AUDPREAMPRINPUTSEL_MASK			0x3
#define RG_AUDPREAMPRINPUTSEL_MASK_SFT			(0x3 << 6)
#define RG_AUDPREAMPRGAIN_SFT				8
#define RG_AUDPREAMPRGAIN_MASK				0x7
#define RG_AUDPREAMPRGAIN_MASK_SFT			(0x7 << 8)
#define RG_BULKR_VCM_EN_SFT				11
#define RG_BULKR_VCM_EN_MASK				0x1
#define RG_BULKR_VCM_EN_MASK_SFT			(0x1 << 11)
#define RG_AUDADCRPWRUP_SFT				12
#define RG_AUDADCRPWRUP_MASK				0x1
#define RG_AUDADCRPWRUP_MASK_SFT			(0x1 << 12)
#define RG_AUDADCRINPUTSEL_SFT				13
#define RG_AUDADCRINPUTSEL_MASK				0x3
#define RG_AUDADCRINPUTSEL_MASK_SFT			(0x3 << 13)

/* AUDENC_ANA_CON2 */
#define RG_AUDPREAMP3ON_SFT				0
#define RG_AUDPREAMP3ON_MASK				0x1
#define RG_AUDPREAMP3ON_MASK_SFT			(0x1 << 0)
#define RG_AUDPREAMP3DCCEN_SFT				1
#define RG_AUDPREAMP3DCCEN_MASK				0x1
#define RG_AUDPREAMP3DCCEN_MASK_SFT			(0x1 << 1)
#define RG_AUDPREAMP3DCPRECHARGE_SFT			2
#define RG_AUDPREAMP3DCPRECHARGE_MASK			0x1
#define RG_AUDPREAMP3DCPRECHARGE_MASK_SFT		(0x1 << 2)
#define RG_AUDPREAMP3PGATEST_SFT			3
#define RG_AUDPREAMP3PGATEST_MASK			0x1
#define RG_AUDPREAMP3PGATEST_MASK_SFT			(0x1 << 3)
#define RG_AUDPREAMP3VSCALE_SFT				4
#define RG_AUDPREAMP3VSCALE_MASK			0x3
#define RG_AUDPREAMP3VSCALE_MASK_SFT			(0x3 << 4)
#define RG_AUDPREAMP3INPUTSEL_SFT			6
#define RG_AUDPREAMP3INPUTSEL_MASK			0x3
#define RG_AUDPREAMP3INPUTSEL_MASK_SFT			(0x3 << 6)
#define RG_AUDPREAMP3GAIN_SFT				8
#define RG_AUDPREAMP3GAIN_MASK				0x7
#define RG_AUDPREAMP3GAIN_MASK_SFT			(0x7 << 8)
#define RG_BULK3_VCM_EN_SFT				11
#define RG_BULK3_VCM_EN_MASK				0x1
#define RG_BULK3_VCM_EN_MASK_SFT			(0x1 << 11)
#define RG_AUDADC3PWRUP_SFT				12
#define RG_AUDADC3PWRUP_MASK				0x1
#define RG_AUDADC3PWRUP_MASK_SFT			(0x1 << 12)
#define RG_AUDADC3INPUTSEL_SFT				13
#define RG_AUDADC3INPUTSEL_MASK				0x3
#define RG_AUDADC3INPUTSEL_MASK_SFT			(0x3 << 13)

/* AUDENC_ANA_CON3 */
#define RG_AUDULHALFBIAS_SFT				0
#define RG_AUDULHALFBIAS_MASK				0x1
#define RG_AUDULHALFBIAS_MASK_SFT			(0x1 << 0)
#define RG_AUDGLBVOWLPWEN_SFT				1
#define RG_AUDGLBVOWLPWEN_MASK				0x1
#define RG_AUDGLBVOWLPWEN_MASK_SFT			(0x1 << 1)
#define RG_AUDPREAMPLPEN_SFT				2
#define RG_AUDPREAMPLPEN_MASK				0x1
#define RG_AUDPREAMPLPEN_MASK_SFT			(0x1 << 2)
#define RG_AUDADC1STSTAGELPEN_SFT			3
#define RG_AUDADC1STSTAGELPEN_MASK			0x1
#define RG_AUDADC1STSTAGELPEN_MASK_SFT			(0x1 << 3)
#define RG_AUDADC2NDSTAGELPEN_SFT			4
#define RG_AUDADC2NDSTAGELPEN_MASK			0x1
#define RG_AUDADC2NDSTAGELPEN_MASK_SFT			(0x1 << 4)
#define RG_AUDADCFLASHLPEN_SFT				5
#define RG_AUDADCFLASHLPEN_MASK				0x1
#define RG_AUDADCFLASHLPEN_MASK_SFT			(0x1 << 5)
#define RG_AUDPREAMPIDDTEST_SFT				6
#define RG_AUDPREAMPIDDTEST_MASK			0x3
#define RG_AUDPREAMPIDDTEST_MASK_SFT			(0x3 << 6)
#define RG_AUDADC1STSTAGEIDDTEST_SFT			8
#define RG_AUDADC1STSTAGEIDDTEST_MASK			0x3
#define RG_AUDADC1STSTAGEIDDTEST_MASK_SFT		(0x3 << 8)
#define RG_AUDADC2NDSTAGEIDDTEST_SFT			10
#define RG_AUDADC2NDSTAGEIDDTEST_MASK			0x3
#define RG_AUDADC2NDSTAGEIDDTEST_MASK_SFT		(0x3 << 10)
#define RG_AUDADCREFBUFIDDTEST_SFT			12
#define RG_AUDADCREFBUFIDDTEST_MASK			0x3
#define RG_AUDADCREFBUFIDDTEST_MASK_SFT			(0x3 << 12)
#define RG_AUDADCFLASHIDDTEST_SFT			14
#define RG_AUDADCFLASHIDDTEST_MASK			0x3
#define RG_AUDADCFLASHIDDTEST_MASK_SFT			(0x3 << 14)

/* AUDENC_ANA_CON4 */
#define RG_AUDRULHALFBIAS_SFT				0
#define RG_AUDRULHALFBIAS_MASK				0x1
#define RG_AUDRULHALFBIAS_MASK_SFT			(0x1 << 0)
#define RG_AUDGLBRVOWLPWEN_SFT				1
#define RG_AUDGLBRVOWLPWEN_MASK				0x1
#define RG_AUDGLBRVOWLPWEN_MASK_SFT			(0x1 << 1)
#define RG_AUDRPREAMPLPEN_SFT				2
#define RG_AUDRPREAMPLPEN_MASK				0x1
#define RG_AUDRPREAMPLPEN_MASK_SFT			(0x1 << 2)
#define RG_AUDRADC1STSTAGELPEN_SFT			3
#define RG_AUDRADC1STSTAGELPEN_MASK			0x1
#define RG_AUDRADC1STSTAGELPEN_MASK_SFT			(0x1 << 3)
#define RG_AUDRADC2NDSTAGELPEN_SFT			4
#define RG_AUDRADC2NDSTAGELPEN_MASK			0x1
#define RG_AUDRADC2NDSTAGELPEN_MASK_SFT			(0x1 << 4)
#define RG_AUDRADCFLASHLPEN_SFT				5
#define RG_AUDRADCFLASHLPEN_MASK			0x1
#define RG_AUDRADCFLASHLPEN_MASK_SFT			(0x1 << 5)
#define RG_AUDRPREAMPIDDTEST_SFT			6
#define RG_AUDRPREAMPIDDTEST_MASK			0x3
#define RG_AUDRPREAMPIDDTEST_MASK_SFT			(0x3 << 6)
#define RG_AUDRADC1STSTAGEIDDTEST_SFT			8
#define RG_AUDRADC1STSTAGEIDDTEST_MASK			0x3
#define RG_AUDRADC1STSTAGEIDDTEST_MASK_SFT		(0x3 << 8)
#define RG_AUDRADC2NDSTAGEIDDTEST_SFT			10
#define RG_AUDRADC2NDSTAGEIDDTEST_MASK			0x3
#define RG_AUDRADC2NDSTAGEIDDTEST_MASK_SFT		(0x3 << 10)
#define RG_AUDRADCREFBUFIDDTEST_SFT			12
#define RG_AUDRADCREFBUFIDDTEST_MASK			0x3
#define RG_AUDRADCREFBUFIDDTEST_MASK_SFT		(0x3 << 12)
#define RG_AUDRADCFLASHIDDTEST_SFT			14
#define RG_AUDRADCFLASHIDDTEST_MASK			0x3
#define RG_AUDRADCFLASHIDDTEST_MASK_SFT			(0x3 << 14)

/* AUDENC_ANA_CON5 */
#define RG_AUDADCCLKRSTB_SFT				0
#define RG_AUDADCCLKRSTB_MASK				0x1
#define RG_AUDADCCLKRSTB_MASK_SFT			(0x1 << 0)
#define RG_AUDADCCLKSEL_SFT				1
#define RG_AUDADCCLKSEL_MASK				0x3
#define RG_AUDADCCLKSEL_MASK_SFT			(0x3 << 1)
#define RG_AUDADCCLKSOURCE_SFT				3
#define RG_AUDADCCLKSOURCE_MASK				0x3
#define RG_AUDADCCLKSOURCE_MASK_SFT			(0x3 << 3)
#define RG_AUDADCCLKGENMODE_SFT				5
#define RG_AUDADCCLKGENMODE_MASK			0x3
#define RG_AUDADCCLKGENMODE_MASK_SFT			(0x3 << 5)
#define RG_AUDPREAMP_ACCFS_SFT				7
#define RG_AUDPREAMP_ACCFS_MASK				0x1
#define RG_AUDPREAMP_ACCFS_MASK_SFT			(0x1 << 7)
#define RG_AUDPREAMPAAFEN_SFT				8
#define RG_AUDPREAMPAAFEN_MASK				0x1
#define RG_AUDPREAMPAAFEN_MASK_SFT			(0x1 << 8)
#define RG_DCCVCMBUFLPMODSEL_SFT			9
#define RG_DCCVCMBUFLPMODSEL_MASK			0x1
#define RG_DCCVCMBUFLPMODSEL_MASK_SFT			(0x1 << 9)
#define RG_DCCVCMBUFLPSWEN_SFT				10
#define RG_DCCVCMBUFLPSWEN_MASK				0x1
#define RG_DCCVCMBUFLPSWEN_MASK_SFT			(0x1 << 10)
#define RG_AUDSPAREPGA_SFT				11
#define RG_AUDSPAREPGA_MASK				0x1f
#define RG_AUDSPAREPGA_MASK_SFT				(0x1f << 11)

/* AUDENC_ANA_CON6 */
#define RG_AUDADC1STSTAGESDENB_SFT			0
#define RG_AUDADC1STSTAGESDENB_MASK			0x1
#define RG_AUDADC1STSTAGESDENB_MASK_SFT			(0x1 << 0)
#define RG_AUDADC2NDSTAGERESET_SFT			1
#define RG_AUDADC2NDSTAGERESET_MASK			0x1
#define RG_AUDADC2NDSTAGERESET_MASK_SFT			(0x1 << 1)
#define RG_AUDADC3RDSTAGERESET_SFT			2
#define RG_AUDADC3RDSTAGERESET_MASK			0x1
#define RG_AUDADC3RDSTAGERESET_MASK_SFT			(0x1 << 2)
#define RG_AUDADCFSRESET_SFT				3
#define RG_AUDADCFSRESET_MASK				0x1
#define RG_AUDADCFSRESET_MASK_SFT			(0x1 << 3)
#define RG_AUDADCWIDECM_SFT				4
#define RG_AUDADCWIDECM_MASK				0x1
#define RG_AUDADCWIDECM_MASK_SFT			(0x1 << 4)
#define RG_AUDADCNOPATEST_SFT				5
#define RG_AUDADCNOPATEST_MASK				0x1
#define RG_AUDADCNOPATEST_MASK_SFT			(0x1 << 5)
#define RG_AUDADCBYPASS_SFT				6
#define RG_AUDADCBYPASS_MASK				0x1
#define RG_AUDADCBYPASS_MASK_SFT			(0x1 << 6)
#define RG_AUDADCFFBYPASS_SFT				7
#define RG_AUDADCFFBYPASS_MASK				0x1
#define RG_AUDADCFFBYPASS_MASK_SFT			(0x1 << 7)
#define RG_AUDADCDACFBCURRENT_SFT			8
#define RG_AUDADCDACFBCURRENT_MASK			0x1
#define RG_AUDADCDACFBCURRENT_MASK_SFT			(0x1 << 8)
#define RG_AUDADCDACIDDTEST_SFT				9
#define RG_AUDADCDACIDDTEST_MASK			0x3
#define RG_AUDADCDACIDDTEST_MASK_SFT			(0x3 << 9)
#define RG_AUDADCDACNRZ_SFT				11
#define RG_AUDADCDACNRZ_MASK				0x1
#define RG_AUDADCDACNRZ_MASK_SFT			(0x1 << 11)
#define RG_AUDADCNODEM_SFT				12
#define RG_AUDADCNODEM_MASK				0x1
#define RG_AUDADCNODEM_MASK_SFT				(0x1 << 12)
#define RG_AUDADCDACTEST_SFT				13
#define RG_AUDADCDACTEST_MASK				0x1
#define RG_AUDADCDACTEST_MASK_SFT			(0x1 << 13)
#define RG_AUDADCDAC0P25FS_SFT				14
#define RG_AUDADCDAC0P25FS_MASK				0x1
#define RG_AUDADCDAC0P25FS_MASK_SFT			(0x1 << 14)
#define RG_AUDADCRDAC0P25FS_SFT				15
#define RG_AUDADCRDAC0P25FS_MASK			0x1
#define RG_AUDADCRDAC0P25FS_MASK_SFT			(0x1 << 15)

/* AUDENC_ANA_CON7 */
#define RG_AUDADCTESTDATA_SFT				0
#define RG_AUDADCTESTDATA_MASK				0xffff
#define RG_AUDADCTESTDATA_MASK_SFT			(0xffff << 0)

/* AUDENC_ANA_CON8 */
#define RG_AUDRCTUNEL_SFT				0
#define RG_AUDRCTUNEL_MASK				0x1f
#define RG_AUDRCTUNEL_MASK_SFT				(0x1f << 0)
#define RG_AUDRCTUNELSEL_SFT				5
#define RG_AUDRCTUNELSEL_MASK				0x1
#define RG_AUDRCTUNELSEL_MASK_SFT			(0x1 << 5)
#define RG_AUDRCTUNER_SFT				8
#define RG_AUDRCTUNER_MASK				0x1f
#define RG_AUDRCTUNER_MASK_SFT				(0x1f << 8)
#define RG_AUDRCTUNERSEL_SFT				13
#define RG_AUDRCTUNERSEL_MASK				0x1
#define RG_AUDRCTUNERSEL_MASK_SFT			(0x1 << 13)

/* AUDENC_ANA_CON9 */
#define RG_AUD3CTUNEL_SFT				0
#define RG_AUD3CTUNEL_MASK				0x1f
#define RG_AUD3CTUNEL_MASK_SFT				(0x1f << 0)
#define RG_AUD3CTUNELSEL_SFT				5
#define RG_AUD3CTUNELSEL_MASK				0x1
#define RG_AUD3CTUNELSEL_MASK_SFT			(0x1 << 5)
#define RGS_AUDRCTUNE3READ_SFT				6
#define RGS_AUDRCTUNE3READ_MASK				0x1f
#define RGS_AUDRCTUNE3READ_MASK_SFT			(0x1f << 6)
#define RG_AUD3SPARE_SFT				11
#define RG_AUD3SPARE_MASK				0x1f
#define RG_AUD3SPARE_MASK_SFT				(0x1f << 11)

/* AUDENC_ANA_CON10 */
#define RGS_AUDRCTUNELREAD_SFT				0
#define RGS_AUDRCTUNELREAD_MASK				0x1f
#define RGS_AUDRCTUNELREAD_MASK_SFT			(0x1f << 0)
#define RGS_AUDRCTUNERREAD_SFT				8
#define RGS_AUDRCTUNERREAD_MASK				0x1f
#define RGS_AUDRCTUNERREAD_MASK_SFT			(0x1f << 8)

/* AUDENC_ANA_CON11 */
#define RG_AUDSPAREVA30_SFT				0
#define RG_AUDSPAREVA30_MASK				0xff
#define RG_AUDSPAREVA30_MASK_SFT			(0xff << 0)
#define RG_AUDSPAREVA18_SFT				8
#define RG_AUDSPAREVA18_MASK				0xff
#define RG_AUDSPAREVA18_MASK_SFT			(0xff << 8)

/* AUDENC_ANA_CON12 */
#define RG_AUDPGA_DECAP_SFT				0
#define RG_AUDPGA_DECAP_MASK				0x1
#define RG_AUDPGA_DECAP_MASK_SFT			(0x1 << 0)
#define RG_AUDPGA_CAPRA_SFT				1
#define RG_AUDPGA_CAPRA_MASK				0x1
#define RG_AUDPGA_CAPRA_MASK_SFT			(0x1 << 1)
#define RG_AUDPGA_ACCCMP_SFT				2
#define RG_AUDPGA_ACCCMP_MASK				0x1
#define RG_AUDPGA_ACCCMP_MASK_SFT			(0x1 << 2)
#define RG_AUDENC_SPARE2_SFT				3
#define RG_AUDENC_SPARE2_MASK				0x1fff
#define RG_AUDENC_SPARE2_MASK_SFT			(0x1fff << 3)

/* AUDENC_ANA_CON13 */
#define RG_AUDDIGMICEN_SFT				0
#define RG_AUDDIGMICEN_MASK				0x1
#define RG_AUDDIGMICEN_MASK_SFT				(0x1 << 0)
#define RG_AUDDIGMICBIAS_SFT				1
#define RG_AUDDIGMICBIAS_MASK				0x3
#define RG_AUDDIGMICBIAS_MASK_SFT			(0x3 << 1)
#define RG_DMICHPCLKEN_SFT				3
#define RG_DMICHPCLKEN_MASK				0x1
#define RG_DMICHPCLKEN_MASK_SFT				(0x1 << 3)
#define RG_AUDDIGMICPDUTY_SFT				4
#define RG_AUDDIGMICPDUTY_MASK				0x3
#define RG_AUDDIGMICPDUTY_MASK_SFT			(0x3 << 4)
#define RG_AUDDIGMICNDUTY_SFT				6
#define RG_AUDDIGMICNDUTY_MASK				0x3
#define RG_AUDDIGMICNDUTY_MASK_SFT			(0x3 << 6)
#define RG_DMICMONEN_SFT				8
#define RG_DMICMONEN_MASK				0x1
#define RG_DMICMONEN_MASK_SFT				(0x1 << 8)
#define RG_DMICMONSEL_SFT				9
#define RG_DMICMONSEL_MASK				0x7
#define RG_DMICMONSEL_MASK_SFT				(0x7 << 9)

/* AUDENC_ANA_CON14 */
#define RG_AUDDIGMIC1EN_SFT				0
#define RG_AUDDIGMIC1EN_MASK				0x1
#define RG_AUDDIGMIC1EN_MASK_SFT			(0x1 << 0)
#define RG_AUDDIGMICBIAS1_SFT				1
#define RG_AUDDIGMICBIAS1_MASK				0x3
#define RG_AUDDIGMICBIAS1_MASK_SFT			(0x3 << 1)
#define RG_DMIC1HPCLKEN_SFT				3
#define RG_DMIC1HPCLKEN_MASK				0x1
#define RG_DMIC1HPCLKEN_MASK_SFT			(0x1 << 3)
#define RG_AUDDIGMIC1PDUTY_SFT				4
#define RG_AUDDIGMIC1PDUTY_MASK				0x3
#define RG_AUDDIGMIC1PDUTY_MASK_SFT			(0x3 << 4)
#define RG_AUDDIGMIC1NDUTY_SFT				6
#define RG_AUDDIGMIC1NDUTY_MASK				0x3
#define RG_AUDDIGMIC1NDUTY_MASK_SFT			(0x3 << 6)
#define RG_DMIC1MONEN_SFT				8
#define RG_DMIC1MONEN_MASK				0x1
#define RG_DMIC1MONEN_MASK_SFT				(0x1 << 8)
#define RG_DMIC1MONSEL_SFT				9
#define RG_DMIC1MONSEL_MASK				0x7
#define RG_DMIC1MONSEL_MASK_SFT				(0x7 << 9)
#define RG_AUDSPAREVMIC_SFT				12
#define RG_AUDSPAREVMIC_MASK				0xf
#define RG_AUDSPAREVMIC_MASK_SFT			(0xf << 12)

/* AUDENC_ANA_CON15 */
#define RG_AUDPWDBMICBIAS0_SFT				0
#define RG_AUDPWDBMICBIAS0_MASK				0x1
#define RG_AUDPWDBMICBIAS0_MASK_SFT			(0x1 << 0)
#define RG_AUDMICBIAS0BYPASSEN_SFT			1
#define RG_AUDMICBIAS0BYPASSEN_MASK			0x1
#define RG_AUDMICBIAS0BYPASSEN_MASK_SFT			(0x1 << 1)
#define RG_AUDMICBIAS0LOWPEN_SFT			2
#define RG_AUDMICBIAS0LOWPEN_MASK			0x1
#define RG_AUDMICBIAS0LOWPEN_MASK_SFT			(0x1 << 2)
#define RG_AUDPWDBMICBIAS3_SFT				3
#define RG_AUDPWDBMICBIAS3_MASK				0x1
#define RG_AUDPWDBMICBIAS3_MASK_SFT			(0x1 << 3)
#define RG_AUDMICBIAS0VREF_SFT				4
#define RG_AUDMICBIAS0VREF_MASK				0x7
#define RG_AUDMICBIAS0VREF_MASK_SFT			(0x7 << 4)
#define RG_AUDMICBIAS0DCSW0P1EN_SFT			8
#define RG_AUDMICBIAS0DCSW0P1EN_MASK			0x1
#define RG_AUDMICBIAS0DCSW0P1EN_MASK_SFT		(0x1 << 8)
#define RG_AUDMICBIAS0DCSW0P2EN_SFT			9
#define RG_AUDMICBIAS0DCSW0P2EN_MASK			0x1
#define RG_AUDMICBIAS0DCSW0P2EN_MASK_SFT		(0x1 << 9)
#define RG_AUDMICBIAS0DCSW0NEN_SFT			10
#define RG_AUDMICBIAS0DCSW0NEN_MASK			0x1
#define RG_AUDMICBIAS0DCSW0NEN_MASK_SFT			(0x1 << 10)
#define RG_AUDMICBIAS0DCSW2P1EN_SFT			12
#define RG_AUDMICBIAS0DCSW2P1EN_MASK			0x1
#define RG_AUDMICBIAS0DCSW2P1EN_MASK_SFT		(0x1 << 12)
#define RG_AUDMICBIAS0DCSW2P2EN_SFT			13
#define RG_AUDMICBIAS0DCSW2P2EN_MASK			0x1
#define RG_AUDMICBIAS0DCSW2P2EN_MASK_SFT		(0x1 << 13)
#define RG_AUDMICBIAS0DCSW2NEN_SFT			14
#define RG_AUDMICBIAS0DCSW2NEN_MASK			0x1
#define RG_AUDMICBIAS0DCSW2NEN_MASK_SFT			(0x1 << 14)

/* AUDENC_ANA_CON16 */
#define RG_AUDPWDBMICBIAS1_SFT				0
#define RG_AUDPWDBMICBIAS1_MASK				0x1
#define RG_AUDPWDBMICBIAS1_MASK_SFT			(0x1 << 0)
#define RG_AUDMICBIAS1BYPASSEN_SFT			1
#define RG_AUDMICBIAS1BYPASSEN_MASK			0x1
#define RG_AUDMICBIAS1BYPASSEN_MASK_SFT			(0x1 << 1)
#define RG_AUDMICBIAS1LOWPEN_SFT			2
#define RG_AUDMICBIAS1LOWPEN_MASK			0x1
#define RG_AUDMICBIAS1LOWPEN_MASK_SFT			(0x1 << 2)
#define RG_AUDMICBIAS1VREF_SFT				4
#define RG_AUDMICBIAS1VREF_MASK				0x7
#define RG_AUDMICBIAS1VREF_MASK_SFT			(0x7 << 4)
#define RG_AUDMICBIAS1DCSW1PEN_SFT			8
#define RG_AUDMICBIAS1DCSW1PEN_MASK			0x1
#define RG_AUDMICBIAS1DCSW1PEN_MASK_SFT			(0x1 << 8)
#define RG_AUDMICBIAS1DCSW1NEN_SFT			9
#define RG_AUDMICBIAS1DCSW1NEN_MASK			0x1
#define RG_AUDMICBIAS1DCSW1NEN_MASK_SFT			(0x1 << 9)
#define RG_BANDGAPGEN_SFT				10
#define RG_BANDGAPGEN_MASK				0x1
#define RG_BANDGAPGEN_MASK_SFT				(0x1 << 10)
#define RG_AUDMICBIAS1HVEN_SFT				12
#define RG_AUDMICBIAS1HVEN_MASK				0x1
#define RG_AUDMICBIAS1HVEN_MASK_SFT			(0x1 << 12)
#define RG_AUDMICBIAS1HVVREF_SFT			13
#define RG_AUDMICBIAS1HVVREF_MASK			0x1
#define RG_AUDMICBIAS1HVVREF_MASK_SFT			(0x1 << 13)

/* AUDENC_ANA_CON17 */
#define RG_AUDPWDBMICBIAS2_SFT				0
#define RG_AUDPWDBMICBIAS2_MASK				0x1
#define RG_AUDPWDBMICBIAS2_MASK_SFT			(0x1 << 0)
#define RG_AUDMICBIAS2BYPASSEN_SFT			1
#define RG_AUDMICBIAS2BYPASSEN_MASK			0x1
#define RG_AUDMICBIAS2BYPASSEN_MASK_SFT			(0x1 << 1)
#define RG_AUDMICBIAS2LOWPEN_SFT			2
#define RG_AUDMICBIAS2LOWPEN_MASK			0x1
#define RG_AUDMICBIAS2LOWPEN_MASK_SFT			(0x1 << 2)
#define RG_AUDMICBIAS2VREF_SFT				4
#define RG_AUDMICBIAS2VREF_MASK				0x7
#define RG_AUDMICBIAS2VREF_MASK_SFT			(0x7 << 4)
#define RG_AUDMICBIAS2DCSW3P1EN_SFT			8
#define RG_AUDMICBIAS2DCSW3P1EN_MASK			0x1
#define RG_AUDMICBIAS2DCSW3P1EN_MASK_SFT		(0x1 << 8)
#define RG_AUDMICBIAS2DCSW3P2EN_SFT			9
#define RG_AUDMICBIAS2DCSW3P2EN_MASK			0x1
#define RG_AUDMICBIAS2DCSW3P2EN_MASK_SFT		(0x1 << 9)
#define RG_AUDMICBIAS2DCSW3NEN_SFT			10
#define RG_AUDMICBIAS2DCSW3NEN_MASK			0x1
#define RG_AUDMICBIAS2DCSW3NEN_MASK_SFT			(0x1 << 10)
#define RG_AUDMICBIASSPARE_SFT				12
#define RG_AUDMICBIASSPARE_MASK				0xf
#define RG_AUDMICBIASSPARE_MASK_SFT			(0xf << 12)

/* AUDENC_ANA_CON18 */
#define RG_AUDACCDETMICBIAS0PULLLOW_SFT			0
#define RG_AUDACCDETMICBIAS0PULLLOW_MASK		0x1
#define RG_AUDACCDETMICBIAS0PULLLOW_MASK_SFT		(0x1 << 0)
#define RG_AUDACCDETMICBIAS1PULLLOW_SFT			1
#define RG_AUDACCDETMICBIAS1PULLLOW_MASK		0x1
#define RG_AUDACCDETMICBIAS1PULLLOW_MASK_SFT		(0x1 << 1)
#define RG_AUDACCDETMICBIAS2PULLLOW_SFT			2
#define RG_AUDACCDETMICBIAS2PULLLOW_MASK		0x1
#define RG_AUDACCDETMICBIAS2PULLLOW_MASK_SFT		(0x1 << 2)
#define RG_AUDACCDETVIN1PULLLOW_SFT			3
#define RG_AUDACCDETVIN1PULLLOW_MASK			0x1
#define RG_AUDACCDETVIN1PULLLOW_MASK_SFT		(0x1 << 3)
#define RG_AUDACCDETVTHACAL_SFT				4
#define RG_AUDACCDETVTHACAL_MASK			0x1
#define RG_AUDACCDETVTHACAL_MASK_SFT			(0x1 << 4)
#define RG_AUDACCDETVTHBCAL_SFT				5
#define RG_AUDACCDETVTHBCAL_MASK			0x1
#define RG_AUDACCDETVTHBCAL_MASK_SFT			(0x1 << 5)
#define RG_AUDACCDETTVDET_SFT				6
#define RG_AUDACCDETTVDET_MASK				0x1
#define RG_AUDACCDETTVDET_MASK_SFT			(0x1 << 6)
#define RG_ACCDETSEL_SFT				7
#define RG_ACCDETSEL_MASK				0x1
#define RG_ACCDETSEL_MASK_SFT				(0x1 << 7)
#define RG_SWBUFMODSEL_SFT				8
#define RG_SWBUFMODSEL_MASK				0x1
#define RG_SWBUFMODSEL_MASK_SFT				(0x1 << 8)
#define RG_SWBUFSWEN_SFT				9
#define RG_SWBUFSWEN_MASK				0x1
#define RG_SWBUFSWEN_MASK_SFT				(0x1 << 9)
#define RG_EINT0NOHYS_SFT				10
#define RG_EINT0NOHYS_MASK				0x1
#define RG_EINT0NOHYS_MASK_SFT				(0x1 << 10)
#define RG_EINT0CONFIGACCDET_SFT			11
#define RG_EINT0CONFIGACCDET_MASK			0x1
#define RG_EINT0CONFIGACCDET_MASK_SFT			(0x1 << 11)
#define RG_EINT0HIRENB_SFT				12
#define RG_EINT0HIRENB_MASK				0x1
#define RG_EINT0HIRENB_MASK_SFT				(0x1 << 12)
#define RG_ACCDET2AUXRESBYPASS_SFT			13
#define RG_ACCDET2AUXRESBYPASS_MASK			0x1
#define RG_ACCDET2AUXRESBYPASS_MASK_SFT			(0x1 << 13)
#define RG_ACCDET2AUXSWEN_SFT				14
#define RG_ACCDET2AUXSWEN_MASK				0x1
#define RG_ACCDET2AUXSWEN_MASK_SFT			(0x1 << 14)
#define RG_AUDACCDETMICBIAS3PULLLOW_SFT			15
#define RG_AUDACCDETMICBIAS3PULLLOW_MASK		0x1
#define RG_AUDACCDETMICBIAS3PULLLOW_MASK_SFT		(0x1 << 15)

/* AUDENC_ANA_CON19 */
#define RG_EINT1CONFIGACCDET_SFT			0
#define RG_EINT1CONFIGACCDET_MASK			0x1
#define RG_EINT1CONFIGACCDET_MASK_SFT			(0x1 << 0)
#define RG_EINT1HIRENB_SFT				1
#define RG_EINT1HIRENB_MASK				0x1
#define RG_EINT1HIRENB_MASK_SFT				(0x1 << 1)
#define RG_EINT1NOHYS_SFT				2
#define RG_EINT1NOHYS_MASK				0x1
#define RG_EINT1NOHYS_MASK_SFT				(0x1 << 2)
#define RG_EINTCOMPVTH_SFT				4
#define RG_EINTCOMPVTH_MASK				0xf
#define RG_EINTCOMPVTH_MASK_SFT				(0xf << 4)
#define RG_MTEST_EN_SFT					8
#define RG_MTEST_EN_MASK				0x1
#define RG_MTEST_EN_MASK_SFT				(0x1 << 8)
#define RG_MTEST_SEL_SFT				9
#define RG_MTEST_SEL_MASK				0x1
#define RG_MTEST_SEL_MASK_SFT				(0x1 << 9)
#define RG_MTEST_CURRENT_SFT				10
#define RG_MTEST_CURRENT_MASK				0x1
#define RG_MTEST_CURRENT_MASK_SFT			(0x1 << 10)
#define RG_ANALOGFDEN_SFT				12
#define RG_ANALOGFDEN_MASK				0x1
#define RG_ANALOGFDEN_MASK_SFT				(0x1 << 12)
#define RG_FDVIN1PPULLLOW_SFT				13
#define RG_FDVIN1PPULLLOW_MASK				0x1
#define RG_FDVIN1PPULLLOW_MASK_SFT			(0x1 << 13)
#define RG_FDEINT0TYPE_SFT				14
#define RG_FDEINT0TYPE_MASK				0x1
#define RG_FDEINT0TYPE_MASK_SFT				(0x1 << 14)
#define RG_FDEINT1TYPE_SFT				15
#define RG_FDEINT1TYPE_MASK				0x1
#define RG_FDEINT1TYPE_MASK_SFT				(0x1 << 15)

/* AUDENC_ANA_CON20 */
#define RG_EINT0CMPEN_SFT				0
#define RG_EINT0CMPEN_MASK				0x1
#define RG_EINT0CMPEN_MASK_SFT				(0x1 << 0)
#define RG_EINT0CMPMEN_SFT				1
#define RG_EINT0CMPMEN_MASK				0x1
#define RG_EINT0CMPMEN_MASK_SFT				(0x1 << 1)
#define RG_EINT0EN_SFT					2
#define RG_EINT0EN_MASK					0x1
#define RG_EINT0EN_MASK_SFT				(0x1 << 2)
#define RG_EINT0CEN_SFT					3
#define RG_EINT0CEN_MASK				0x1
#define RG_EINT0CEN_MASK_SFT				(0x1 << 3)
#define RG_EINT0INVEN_SFT				4
#define RG_EINT0INVEN_MASK				0x1
#define RG_EINT0INVEN_MASK_SFT				(0x1 << 4)
#define RG_EINT0CTURBO_SFT				5
#define RG_EINT0CTURBO_MASK				0x7
#define RG_EINT0CTURBO_MASK_SFT				(0x7 << 5)
#define RG_EINT1CMPEN_SFT				8
#define RG_EINT1CMPEN_MASK				0x1
#define RG_EINT1CMPEN_MASK_SFT				(0x1 << 8)
#define RG_EINT1CMPMEN_SFT				9
#define RG_EINT1CMPMEN_MASK				0x1
#define RG_EINT1CMPMEN_MASK_SFT				(0x1 << 9)
#define RG_EINT1EN_SFT					10
#define RG_EINT1EN_MASK					0x1
#define RG_EINT1EN_MASK_SFT				(0x1 << 10)
#define RG_EINT1CEN_SFT					11
#define RG_EINT1CEN_MASK				0x1
#define RG_EINT1CEN_MASK_SFT				(0x1 << 11)
#define RG_EINT1INVEN_SFT				12
#define RG_EINT1INVEN_MASK				0x1
#define RG_EINT1INVEN_MASK_SFT				(0x1 << 12)
#define RG_EINT1CTURBO_SFT				13
#define RG_EINT1CTURBO_MASK				0x7
#define RG_EINT1CTURBO_MASK_SFT				(0x7 << 13)

/* AUDENC_ANA_CON21 */
#define RG_ACCDETSPARE_SFT				0
#define RG_ACCDETSPARE_MASK				0xffff
#define RG_ACCDETSPARE_MASK_SFT				(0xffff << 0)

/* AUDENC_ANA_CON22 */
#define RG_AUDENCSPAREVA30_SFT				0
#define RG_AUDENCSPAREVA30_MASK				0xff
#define RG_AUDENCSPAREVA30_MASK_SFT			(0xff << 0)
#define RG_AUDENCSPAREVA18_SFT				8
#define RG_AUDENCSPAREVA18_MASK				0xff
#define RG_AUDENCSPAREVA18_MASK_SFT			(0xff << 8)

/* AUDENC_ANA_CON23 */
#define RG_CLKSQ_EN_SFT					0
#define RG_CLKSQ_EN_MASK				0x1
#define RG_CLKSQ_EN_MASK_SFT				(0x1 << 0)
#define RG_CLKSQ_IN_SEL_TEST_SFT			1
#define RG_CLKSQ_IN_SEL_TEST_MASK			0x1
#define RG_CLKSQ_IN_SEL_TEST_MASK_SFT			(0x1 << 1)
#define RG_CM_REFGENSEL_SFT				2
#define RG_CM_REFGENSEL_MASK				0x1
#define RG_CM_REFGENSEL_MASK_SFT			(0x1 << 2)
#define RG_AUDIO_VOW_EN_SFT				3
#define RG_AUDIO_VOW_EN_MASK				0x1
#define RG_AUDIO_VOW_EN_MASK_SFT			(0x1 << 3)
#define RG_CLKSQ_EN_VOW_SFT				4
#define RG_CLKSQ_EN_VOW_MASK				0x1
#define RG_CLKSQ_EN_VOW_MASK_SFT			(0x1 << 4)
#define RG_CLKAND_EN_VOW_SFT				5
#define RG_CLKAND_EN_VOW_MASK				0x1
#define RG_CLKAND_EN_VOW_MASK_SFT			(0x1 << 5)
#define RG_VOWCLK_SEL_EN_VOW_SFT			6
#define RG_VOWCLK_SEL_EN_VOW_MASK			0x1
#define RG_VOWCLK_SEL_EN_VOW_MASK_SFT			(0x1 << 6)
#define RG_SPARE_VOW_SFT				7
#define RG_SPARE_VOW_MASK				0x7
#define RG_SPARE_VOW_MASK_SFT				(0x7 << 7)

/* AUDDEC_ANA_CON0 */
#define RG_AUDDACLPWRUP_VAUDP32_SFT			0
#define RG_AUDDACLPWRUP_VAUDP32_MASK			0x1
#define RG_AUDDACLPWRUP_VAUDP32_MASK_SFT		(0x1 << 0)
#define RG_AUDDACRPWRUP_VAUDP32_SFT			1
#define RG_AUDDACRPWRUP_VAUDP32_MASK			0x1
#define RG_AUDDACRPWRUP_VAUDP32_MASK_SFT		(0x1 << 1)
#define RG_AUD_DAC_PWR_UP_VA32_SFT			2
#define RG_AUD_DAC_PWR_UP_VA32_MASK			0x1
#define RG_AUD_DAC_PWR_UP_VA32_MASK_SFT			(0x1 << 2)
#define RG_AUD_DAC_PWL_UP_VA32_SFT			3
#define RG_AUD_DAC_PWL_UP_VA32_MASK			0x1
#define RG_AUD_DAC_PWL_UP_VA32_MASK_SFT			(0x1 << 3)
#define RG_AUDHPLPWRUP_VAUDP32_SFT			4
#define RG_AUDHPLPWRUP_VAUDP32_MASK			0x1
#define RG_AUDHPLPWRUP_VAUDP32_MASK_SFT			(0x1 << 4)
#define RG_AUDHPRPWRUP_VAUDP32_SFT			5
#define RG_AUDHPRPWRUP_VAUDP32_MASK			0x1
#define RG_AUDHPRPWRUP_VAUDP32_MASK_SFT			(0x1 << 5)
#define RG_AUDHPLPWRUP_IBIAS_VAUDP32_SFT		6
#define RG_AUDHPLPWRUP_IBIAS_VAUDP32_MASK		0x1
#define RG_AUDHPLPWRUP_IBIAS_VAUDP32_MASK_SFT		(0x1 << 6)
#define RG_AUDHPRPWRUP_IBIAS_VAUDP32_SFT		7
#define RG_AUDHPRPWRUP_IBIAS_VAUDP32_MASK		0x1
#define RG_AUDHPRPWRUP_IBIAS_VAUDP32_MASK_SFT		(0x1 << 7)
#define RG_AUDHPLMUXINPUTSEL_VAUDP32_SFT		8
#define RG_AUDHPLMUXINPUTSEL_VAUDP32_MASK		0x3
#define RG_AUDHPLMUXINPUTSEL_VAUDP32_MASK_SFT		(0x3 << 8)
#define RG_AUDHPRMUXINPUTSEL_VAUDP32_SFT		10
#define RG_AUDHPRMUXINPUTSEL_VAUDP32_MASK		0x3
#define RG_AUDHPRMUXINPUTSEL_VAUDP32_MASK_SFT		(0x3 << 10)
#define RG_AUDHPLSCDISABLE_VAUDP32_SFT			12
#define RG_AUDHPLSCDISABLE_VAUDP32_MASK			0x1
#define RG_AUDHPLSCDISABLE_VAUDP32_MASK_SFT		(0x1 << 12)
#define RG_AUDHPRSCDISABLE_VAUDP32_SFT			13
#define RG_AUDHPRSCDISABLE_VAUDP32_MASK			0x1
#define RG_AUDHPRSCDISABLE_VAUDP32_MASK_SFT		(0x1 << 13)
#define RG_AUDHPLBSCCURRENT_VAUDP32_SFT			14
#define RG_AUDHPLBSCCURRENT_VAUDP32_MASK		0x1
#define RG_AUDHPLBSCCURRENT_VAUDP32_MASK_SFT		(0x1 << 14)
#define RG_AUDHPRBSCCURRENT_VAUDP32_SFT			15
#define RG_AUDHPRBSCCURRENT_VAUDP32_MASK		0x1
#define RG_AUDHPRBSCCURRENT_VAUDP32_MASK_SFT		(0x1 << 15)

/* AUDDEC_ANA_CON1 */
#define RG_AUDHPLOUTPWRUP_VAUDP32_SFT			0
#define RG_AUDHPLOUTPWRUP_VAUDP32_MASK			0x1
#define RG_AUDHPLOUTPWRUP_VAUDP32_MASK_SFT		(0x1 << 0)
#define RG_AUDHPROUTPWRUP_VAUDP32_SFT			1
#define RG_AUDHPROUTPWRUP_VAUDP32_MASK			0x1
#define RG_AUDHPROUTPWRUP_VAUDP32_MASK_SFT		(0x1 << 1)
#define RG_AUDHPLOUTAUXPWRUP_VAUDP32_SFT		2
#define RG_AUDHPLOUTAUXPWRUP_VAUDP32_MASK		0x1
#define RG_AUDHPLOUTAUXPWRUP_VAUDP32_MASK_SFT		(0x1 << 2)
#define RG_AUDHPROUTAUXPWRUP_VAUDP32_SFT		3
#define RG_AUDHPROUTAUXPWRUP_VAUDP32_MASK		0x1
#define RG_AUDHPROUTAUXPWRUP_VAUDP32_MASK_SFT		(0x1 << 3)
#define RG_HPLAUXFBRSW_EN_VAUDP32_SFT			4
#define RG_HPLAUXFBRSW_EN_VAUDP32_MASK			0x1
#define RG_HPLAUXFBRSW_EN_VAUDP32_MASK_SFT		(0x1 << 4)
#define RG_HPRAUXFBRSW_EN_VAUDP32_SFT			5
#define RG_HPRAUXFBRSW_EN_VAUDP32_MASK			0x1
#define RG_HPRAUXFBRSW_EN_VAUDP32_MASK_SFT		(0x1 << 5)
#define RG_HPLSHORT2HPLAUX_EN_VAUDP32_SFT		6
#define RG_HPLSHORT2HPLAUX_EN_VAUDP32_MASK		0x1
#define RG_HPLSHORT2HPLAUX_EN_VAUDP32_MASK_SFT		(0x1 << 6)
#define RG_HPRSHORT2HPRAUX_EN_VAUDP32_SFT		7
#define RG_HPRSHORT2HPRAUX_EN_VAUDP32_MASK		0x1
#define RG_HPRSHORT2HPRAUX_EN_VAUDP32_MASK_SFT		(0x1 << 7)
#define RG_HPLOUTSTGCTRL_VAUDP32_SFT			8
#define RG_HPLOUTSTGCTRL_VAUDP32_MASK			0x7
#define RG_HPLOUTSTGCTRL_VAUDP32_MASK_SFT		(0x7 << 8)
#define RG_HPROUTSTGCTRL_VAUDP32_SFT			12
#define RG_HPROUTSTGCTRL_VAUDP32_MASK			0x7
#define RG_HPROUTSTGCTRL_VAUDP32_MASK_SFT		(0x7 << 12)

/* AUDDEC_ANA_CON2 */
#define RG_HPLOUTPUTSTBENH_VAUDP32_SFT			0
#define RG_HPLOUTPUTSTBENH_VAUDP32_MASK			0x7
#define RG_HPLOUTPUTSTBENH_VAUDP32_MASK_SFT		(0x7 << 0)
#define RG_HPROUTPUTSTBENH_VAUDP32_SFT			4
#define RG_HPROUTPUTSTBENH_VAUDP32_MASK			0x7
#define RG_HPROUTPUTSTBENH_VAUDP32_MASK_SFT		(0x7 << 4)
#define RG_AUDHPSTARTUP_VAUDP32_SFT			7
#define RG_AUDHPSTARTUP_VAUDP32_MASK			0x1
#define RG_AUDHPSTARTUP_VAUDP32_MASK_SFT		(0x1 << 7)
#define RG_AUDREFN_DERES_EN_VAUDP32_SFT			8
#define RG_AUDREFN_DERES_EN_VAUDP32_MASK		0x1
#define RG_AUDREFN_DERES_EN_VAUDP32_MASK_SFT		(0x1 << 8)
#define RG_HPINPUTSTBENH_VAUDP32_SFT			9
#define RG_HPINPUTSTBENH_VAUDP32_MASK			0x1
#define RG_HPINPUTSTBENH_VAUDP32_MASK_SFT		(0x1 << 9)
#define RG_HPINPUTRESET0_VAUDP32_SFT			10
#define RG_HPINPUTRESET0_VAUDP32_MASK			0x1
#define RG_HPINPUTRESET0_VAUDP32_MASK_SFT		(0x1 << 10)
#define RG_HPOUTPUTRESET0_VAUDP32_SFT			11
#define RG_HPOUTPUTRESET0_VAUDP32_MASK			0x1
#define RG_HPOUTPUTRESET0_VAUDP32_MASK_SFT		(0x1 << 11)
#define RG_HPPSHORT2VCM_VAUDP32_SFT			12
#define RG_HPPSHORT2VCM_VAUDP32_MASK			0x7
#define RG_HPPSHORT2VCM_VAUDP32_MASK_SFT		(0x7 << 12)
#define RG_AUDHPTRIM_EN_VAUDP32_SFT			15
#define RG_AUDHPTRIM_EN_VAUDP32_MASK			0x1
#define RG_AUDHPTRIM_EN_VAUDP32_MASK_SFT		(0x1 << 15)

/* AUDDEC_ANA_CON3 */
#define RG_AUDHPLTRIM_VAUDP32_SFT			0
#define RG_AUDHPLTRIM_VAUDP32_MASK			0x1f
#define RG_AUDHPLTRIM_VAUDP32_MASK_SFT			(0x1f << 0)
#define RG_AUDHPLFINETRIM_VAUDP32_SFT			5
#define RG_AUDHPLFINETRIM_VAUDP32_MASK			0x7
#define RG_AUDHPLFINETRIM_VAUDP32_MASK_SFT		(0x7 << 5)
#define RG_AUDHPRTRIM_VAUDP32_SFT			8
#define RG_AUDHPRTRIM_VAUDP32_MASK			0x1f
#define RG_AUDHPRTRIM_VAUDP32_MASK_SFT			(0x1f << 8)
#define RG_AUDHPRFINETRIM_VAUDP32_SFT			13
#define RG_AUDHPRFINETRIM_VAUDP32_MASK			0x7
#define RG_AUDHPRFINETRIM_VAUDP32_MASK_SFT		(0x7 << 13)

/* AUDDEC_ANA_CON4 */
#define RG_AUDHPDIFFINPBIASADJ_VAUDP32_SFT		0
#define RG_AUDHPDIFFINPBIASADJ_VAUDP32_MASK		0x7
#define RG_AUDHPDIFFINPBIASADJ_VAUDP32_MASK_SFT		(0x7 << 0)
#define RG_AUDHPLFCOMPRESSEL_VAUDP32_SFT		4
#define RG_AUDHPLFCOMPRESSEL_VAUDP32_MASK		0x7
#define RG_AUDHPLFCOMPRESSEL_VAUDP32_MASK_SFT		(0x7 << 4)
#define RG_AUDHPHFCOMPRESSEL_VAUDP32_SFT		8
#define RG_AUDHPHFCOMPRESSEL_VAUDP32_MASK		0x7
#define RG_AUDHPHFCOMPRESSEL_VAUDP32_MASK_SFT		(0x7 << 8)
#define RG_AUDHPHFCOMPBUFGAINSEL_VAUDP32_SFT		12
#define RG_AUDHPHFCOMPBUFGAINSEL_VAUDP32_MASK		0x3
#define RG_AUDHPHFCOMPBUFGAINSEL_VAUDP32_MASK_SFT	(0x3 << 12)
#define RG_AUDHPCOMP_EN_VAUDP32_SFT			15
#define RG_AUDHPCOMP_EN_VAUDP32_MASK			0x1
#define RG_AUDHPCOMP_EN_VAUDP32_MASK_SFT		(0x1 << 15)

/* AUDDEC_ANA_CON5 */
#define RG_AUDHPDECMGAINADJ_VAUDP32_SFT			0
#define RG_AUDHPDECMGAINADJ_VAUDP32_MASK		0x7
#define RG_AUDHPDECMGAINADJ_VAUDP32_MASK_SFT		(0x7 << 0)
#define RG_AUDHPDEDMGAINADJ_VAUDP32_SFT			4
#define RG_AUDHPDEDMGAINADJ_VAUDP32_MASK		0x7
#define RG_AUDHPDEDMGAINADJ_VAUDP32_MASK_SFT		(0x7 << 4)

/* AUDDEC_ANA_CON6 */
#define RG_AUDHSPWRUP_VAUDP32_SFT			0
#define RG_AUDHSPWRUP_VAUDP32_MASK			0x1
#define RG_AUDHSPWRUP_VAUDP32_MASK_SFT			(0x1 << 0)
#define RG_AUDHSPWRUP_IBIAS_VAUDP32_SFT			1
#define RG_AUDHSPWRUP_IBIAS_VAUDP32_MASK		0x1
#define RG_AUDHSPWRUP_IBIAS_VAUDP32_MASK_SFT		(0x1 << 1)
#define RG_AUDHSMUXINPUTSEL_VAUDP32_SFT			2
#define RG_AUDHSMUXINPUTSEL_VAUDP32_MASK		0x3
#define RG_AUDHSMUXINPUTSEL_VAUDP32_MASK_SFT		(0x3 << 2)
#define RG_AUDHSSCDISABLE_VAUDP32_SFT			4
#define RG_AUDHSSCDISABLE_VAUDP32_MASK			0x1
#define RG_AUDHSSCDISABLE_VAUDP32_MASK_SFT		(0x1 << 4)
#define RG_AUDHSBSCCURRENT_VAUDP32_SFT			5
#define RG_AUDHSBSCCURRENT_VAUDP32_MASK			0x1
#define RG_AUDHSBSCCURRENT_VAUDP32_MASK_SFT		(0x1 << 5)
#define RG_AUDHSSTARTUP_VAUDP32_SFT			6
#define RG_AUDHSSTARTUP_VAUDP32_MASK			0x1
#define RG_AUDHSSTARTUP_VAUDP32_MASK_SFT		(0x1 << 6)
#define RG_HSOUTPUTSTBENH_VAUDP32_SFT			7
#define RG_HSOUTPUTSTBENH_VAUDP32_MASK			0x1
#define RG_HSOUTPUTSTBENH_VAUDP32_MASK_SFT		(0x1 << 7)
#define RG_HSINPUTSTBENH_VAUDP32_SFT			8
#define RG_HSINPUTSTBENH_VAUDP32_MASK			0x1
#define RG_HSINPUTSTBENH_VAUDP32_MASK_SFT		(0x1 << 8)
#define RG_HSINPUTRESET0_VAUDP32_SFT			9
#define RG_HSINPUTRESET0_VAUDP32_MASK			0x1
#define RG_HSINPUTRESET0_VAUDP32_MASK_SFT		(0x1 << 9)
#define RG_HSOUTPUTRESET0_VAUDP32_SFT			10
#define RG_HSOUTPUTRESET0_VAUDP32_MASK			0x1
#define RG_HSOUTPUTRESET0_VAUDP32_MASK_SFT		(0x1 << 10)
#define RG_HSOUT_SHORTVCM_VAUDP32_SFT			11
#define RG_HSOUT_SHORTVCM_VAUDP32_MASK			0x1
#define RG_HSOUT_SHORTVCM_VAUDP32_MASK_SFT		(0x1 << 11)

/* AUDDEC_ANA_CON7 */
#define RG_AUDLOLPWRUP_VAUDP32_SFT			0
#define RG_AUDLOLPWRUP_VAUDP32_MASK			0x1
#define RG_AUDLOLPWRUP_VAUDP32_MASK_SFT			(0x1 << 0)
#define RG_AUDLOLPWRUP_IBIAS_VAUDP32_SFT		1
#define RG_AUDLOLPWRUP_IBIAS_VAUDP32_MASK		0x1
#define RG_AUDLOLPWRUP_IBIAS_VAUDP32_MASK_SFT		(0x1 << 1)
#define RG_AUDLOLMUXINPUTSEL_VAUDP32_SFT		2
#define RG_AUDLOLMUXINPUTSEL_VAUDP32_MASK		0x3
#define RG_AUDLOLMUXINPUTSEL_VAUDP32_MASK_SFT		(0x3 << 2)
#define RG_AUDLOLSCDISABLE_VAUDP32_SFT			4
#define RG_AUDLOLSCDISABLE_VAUDP32_MASK			0x1
#define RG_AUDLOLSCDISABLE_VAUDP32_MASK_SFT		(0x1 << 4)
#define RG_AUDLOLBSCCURRENT_VAUDP32_SFT			5
#define RG_AUDLOLBSCCURRENT_VAUDP32_MASK		0x1
#define RG_AUDLOLBSCCURRENT_VAUDP32_MASK_SFT		(0x1 << 5)
#define RG_AUDLOSTARTUP_VAUDP32_SFT			6
#define RG_AUDLOSTARTUP_VAUDP32_MASK			0x1
#define RG_AUDLOSTARTUP_VAUDP32_MASK_SFT		(0x1 << 6)
#define RG_LOINPUTSTBENH_VAUDP32_SFT			7
#define RG_LOINPUTSTBENH_VAUDP32_MASK			0x1
#define RG_LOINPUTSTBENH_VAUDP32_MASK_SFT		(0x1 << 7)
#define RG_LOOUTPUTSTBENH_VAUDP32_SFT			8
#define RG_LOOUTPUTSTBENH_VAUDP32_MASK			0x1
#define RG_LOOUTPUTSTBENH_VAUDP32_MASK_SFT		(0x1 << 8)
#define RG_LOINPUTRESET0_VAUDP32_SFT			9
#define RG_LOINPUTRESET0_VAUDP32_MASK			0x1
#define RG_LOINPUTRESET0_VAUDP32_MASK_SFT		(0x1 << 9)
#define RG_LOOUTPUTRESET0_VAUDP32_SFT			10
#define RG_LOOUTPUTRESET0_VAUDP32_MASK			0x1
#define RG_LOOUTPUTRESET0_VAUDP32_MASK_SFT		(0x1 << 10)
#define RG_LOOUT_SHORTVCM_VAUDP32_SFT			11
#define RG_LOOUT_SHORTVCM_VAUDP32_MASK			0x1
#define RG_LOOUT_SHORTVCM_VAUDP32_MASK_SFT		(0x1 << 11)
#define RG_AUDDACTPWRUP_VAUDP32_SFT			12
#define RG_AUDDACTPWRUP_VAUDP32_MASK			0x1
#define RG_AUDDACTPWRUP_VAUDP32_MASK_SFT		(0x1 << 12)
#define RG_AUD_DAC_PWT_UP_VA32_SFT			13
#define RG_AUD_DAC_PWT_UP_VA32_MASK			0x1
#define RG_AUD_DAC_PWT_UP_VA32_MASK_SFT			(0x1 << 13)

/* AUDDEC_ANA_CON8 */
#define RG_AUDTRIMBUF_INPUTMUXSEL_VAUDP32_SFT		0
#define RG_AUDTRIMBUF_INPUTMUXSEL_VAUDP32_MASK		0xf
#define RG_AUDTRIMBUF_INPUTMUXSEL_VAUDP32_MASK_SFT	(0xf << 0)
#define RG_AUDTRIMBUF_GAINSEL_VAUDP32_SFT		4
#define RG_AUDTRIMBUF_GAINSEL_VAUDP32_MASK		0x3
#define RG_AUDTRIMBUF_GAINSEL_VAUDP32_MASK_SFT		(0x3 << 4)
#define RG_AUDTRIMBUF_EN_VAUDP32_SFT			6
#define RG_AUDTRIMBUF_EN_VAUDP32_MASK			0x1
#define RG_AUDTRIMBUF_EN_VAUDP32_MASK_SFT		(0x1 << 6)
#define RG_AUDHPSPKDET_INPUTMUXSEL_VAUDP32_SFT		8
#define RG_AUDHPSPKDET_INPUTMUXSEL_VAUDP32_MASK		0x3
#define RG_AUDHPSPKDET_INPUTMUXSEL_VAUDP32_MASK_SFT	(0x3 << 8)
#define RG_AUDHPSPKDET_OUTPUTMUXSEL_VAUDP32_SFT		10
#define RG_AUDHPSPKDET_OUTPUTMUXSEL_VAUDP32_MASK	0x3
#define RG_AUDHPSPKDET_OUTPUTMUXSEL_VAUDP32_MASK_SFT	(0x3 << 10)
#define RG_AUDHPSPKDET_EN_VAUDP32_SFT			12
#define RG_AUDHPSPKDET_EN_VAUDP32_MASK			0x1
#define RG_AUDHPSPKDET_EN_VAUDP32_MASK_SFT		(0x1 << 12)

/* AUDDEC_ANA_CON9 */
#define RG_ABIDEC_RSVD0_VA32_SFT			0
#define RG_ABIDEC_RSVD0_VA32_MASK			0xff
#define RG_ABIDEC_RSVD0_VA32_MASK_SFT			(0xff << 0)
#define RG_ABIDEC_RSVD0_VAUDP32_SFT			8
#define RG_ABIDEC_RSVD0_VAUDP32_MASK			0xff
#define RG_ABIDEC_RSVD0_VAUDP32_MASK_SFT		(0xff << 8)

/* AUDDEC_ANA_CON10 */
#define RG_ABIDEC_RSVD1_VAUDP32_SFT			0
#define RG_ABIDEC_RSVD1_VAUDP32_MASK			0xff
#define RG_ABIDEC_RSVD1_VAUDP32_MASK_SFT		(0xff << 0)
#define RG_ABIDEC_RSVD2_VAUDP32_SFT			8
#define RG_ABIDEC_RSVD2_VAUDP32_MASK			0xff
#define RG_ABIDEC_RSVD2_VAUDP32_MASK_SFT		(0xff << 8)

/* AUDDEC_ANA_CON11 */
#define RG_AUDZCDMUXSEL_VAUDP32_SFT			0
#define RG_AUDZCDMUXSEL_VAUDP32_MASK			0x7
#define RG_AUDZCDMUXSEL_VAUDP32_MASK_SFT		(0x7 << 0)
#define RG_AUDZCDCLKSEL_VAUDP32_SFT			3
#define RG_AUDZCDCLKSEL_VAUDP32_MASK			0x1
#define RG_AUDZCDCLKSEL_VAUDP32_MASK_SFT		(0x1 << 3)
#define RG_AUDBIASADJ_0_VAUDP32_SFT			7
#define RG_AUDBIASADJ_0_VAUDP32_MASK			0x1ff
#define RG_AUDBIASADJ_0_VAUDP32_MASK_SFT		(0x1ff << 7)

/* AUDDEC_ANA_CON12 */
#define RG_AUDBIASADJ_1_VAUDP32_SFT			0
#define RG_AUDBIASADJ_1_VAUDP32_MASK			0xff
#define RG_AUDBIASADJ_1_VAUDP32_MASK_SFT		(0xff << 0)
#define RG_AUDIBIASPWRDN_VAUDP32_SFT			8
#define RG_AUDIBIASPWRDN_VAUDP32_MASK			0x1
#define RG_AUDIBIASPWRDN_VAUDP32_MASK_SFT		(0x1 << 8)

/* AUDDEC_ANA_CON13 */
#define RG_RSTB_DECODER_VA32_SFT			0
#define RG_RSTB_DECODER_VA32_MASK			0x1
#define RG_RSTB_DECODER_VA32_MASK_SFT			(0x1 << 0)
#define RG_SEL_DECODER_96K_VA32_SFT			1
#define RG_SEL_DECODER_96K_VA32_MASK			0x1
#define RG_SEL_DECODER_96K_VA32_MASK_SFT		(0x1 << 1)
#define RG_SEL_DELAY_VCORE_SFT				2
#define RG_SEL_DELAY_VCORE_MASK				0x1
#define RG_SEL_DELAY_VCORE_MASK_SFT			(0x1 << 2)
#define RG_AUDGLB_PWRDN_VA32_SFT			4
#define RG_AUDGLB_PWRDN_VA32_MASK			0x1
#define RG_AUDGLB_PWRDN_VA32_MASK_SFT			(0x1 << 4)
#define RG_AUDGLB_LP_VOW_EN_VA32_SFT			5
#define RG_AUDGLB_LP_VOW_EN_VA32_MASK			0x1
#define RG_AUDGLB_LP_VOW_EN_VA32_MASK_SFT		(0x1 << 5)
#define RG_AUDGLB_LP2_VOW_EN_VA32_SFT			6
#define RG_AUDGLB_LP2_VOW_EN_VA32_MASK			0x1
#define RG_AUDGLB_LP2_VOW_EN_VA32_MASK_SFT		(0x1 << 6)

/* AUDDEC_ANA_CON14 */
#define RG_LCLDO_DEC_EN_VA32_SFT			0
#define RG_LCLDO_DEC_EN_VA32_MASK			0x1
#define RG_LCLDO_DEC_EN_VA32_MASK_SFT			(0x1 << 0)
#define RG_LCLDO_DEC_PDDIS_EN_VA18_SFT			1
#define RG_LCLDO_DEC_PDDIS_EN_VA18_MASK			0x1
#define RG_LCLDO_DEC_PDDIS_EN_VA18_MASK_SFT		(0x1 << 1)
#define RG_LCLDO_DEC_REMOTE_SENSE_VA18_SFT		2
#define RG_LCLDO_DEC_REMOTE_SENSE_VA18_MASK		0x1
#define RG_LCLDO_DEC_REMOTE_SENSE_VA18_MASK_SFT		(0x1 << 2)
#define RG_NVREG_EN_VAUDP32_SFT				4
#define RG_NVREG_EN_VAUDP32_MASK			0x1
#define RG_NVREG_EN_VAUDP32_MASK_SFT			(0x1 << 4)
#define RG_NVREG_PULL0V_VAUDP32_SFT			5
#define RG_NVREG_PULL0V_VAUDP32_MASK			0x1
#define RG_NVREG_PULL0V_VAUDP32_MASK_SFT		(0x1 << 5)
#define RG_AUDPMU_RSVD_VA18_SFT				8
#define RG_AUDPMU_RSVD_VA18_MASK			0xff
#define RG_AUDPMU_RSVD_VA18_MASK_SFT			(0xff << 8)

/* MT6359_ZCD_CON0 */
#define RG_AUDZCDENABLE_SFT				0
#define RG_AUDZCDENABLE_MASK				0x1
#define RG_AUDZCDENABLE_MASK_SFT			(0x1 << 0)
#define RG_AUDZCDGAINSTEPTIME_SFT			1
#define RG_AUDZCDGAINSTEPTIME_MASK			0x7
#define RG_AUDZCDGAINSTEPTIME_MASK_SFT			(0x7 << 1)
#define RG_AUDZCDGAINSTEPSIZE_SFT			4
#define RG_AUDZCDGAINSTEPSIZE_MASK			0x3
#define RG_AUDZCDGAINSTEPSIZE_MASK_SFT			(0x3 << 4)
#define RG_AUDZCDTIMEOUTMODESEL_SFT			6
#define RG_AUDZCDTIMEOUTMODESEL_MASK			0x1
#define RG_AUDZCDTIMEOUTMODESEL_MASK_SFT		(0x1 << 6)

/* MT6359_ZCD_CON1 */
#define RG_AUDLOLGAIN_SFT				0
#define RG_AUDLOLGAIN_MASK				0x1f
#define RG_AUDLOLGAIN_MASK_SFT				(0x1f << 0)
#define RG_AUDLORGAIN_SFT				7
#define RG_AUDLORGAIN_MASK				0x1f
#define RG_AUDLORGAIN_MASK_SFT				(0x1f << 7)

/* MT6359_ZCD_CON2 */
#define RG_AUDHPLGAIN_SFT				0
#define RG_AUDHPLGAIN_MASK				0x1f
#define RG_AUDHPLGAIN_MASK_SFT				(0x1f << 0)
#define RG_AUDHPRGAIN_SFT				7
#define RG_AUDHPRGAIN_MASK				0x1f
#define RG_AUDHPRGAIN_MASK_SFT				(0x1f << 7)

/* MT6359_ZCD_CON3 */
#define RG_AUDHSGAIN_SFT				0
#define RG_AUDHSGAIN_MASK				0x1f
#define RG_AUDHSGAIN_MASK_SFT				(0x1f << 0)

/* MT6359_ZCD_CON4 */
#define RG_AUDIVLGAIN_SFT				0
#define RG_AUDIVLGAIN_MASK				0x7
#define RG_AUDIVLGAIN_MASK_SFT				(0x7 << 0)
#define RG_AUDIVRGAIN_SFT				8
#define RG_AUDIVRGAIN_MASK				0x7
#define RG_AUDIVRGAIN_MASK_SFT				(0x7 << 8)

/* MT6359_ZCD_CON5 */
#define RG_AUDINTGAIN1_SFT				0
#define RG_AUDINTGAIN1_MASK				0x3f
#define RG_AUDINTGAIN1_MASK_SFT				(0x3f << 0)
#define RG_AUDINTGAIN2_SFT				8
#define RG_AUDINTGAIN2_MASK				0x3f
#define RG_AUDINTGAIN2_MASK_SFT				(0x3f << 8)

/* audio register */
#define MT6359_GPIO_DIR0				0x88
#define MT6359_GPIO_DIR0_SET				0x8a
#define MT6359_GPIO_DIR0_CLR				0x8c
#define MT6359_GPIO_DIR1				0x8e
#define MT6359_GPIO_DIR1_SET				0x90
#define MT6359_GPIO_DIR1_CLR				0x92

#define MT6359_DCXO_CW11				0x7a6
#define MT6359_DCXO_CW12				0x7a8

#define MT6359_GPIO_MODE0				0xcc
#define MT6359_GPIO_MODE0_SET				0xce
#define MT6359_GPIO_MODE0_CLR				0xd0
#define MT6359_GPIO_MODE1				0xd2
#define MT6359_GPIO_MODE1_SET				0xd4
#define MT6359_GPIO_MODE1_CLR				0xd6
#define MT6359_GPIO_MODE2				0xd8
#define MT6359_GPIO_MODE2_SET				0xda
#define MT6359_GPIO_MODE2_CLR				0xdc
#define MT6359_GPIO_MODE3				0xde
#define MT6359_GPIO_MODE3_SET				0xe0
#define MT6359_GPIO_MODE3_CLR				0xe2
#define MT6359_GPIO_MODE4				0xe4
#define MT6359_GPIO_MODE4_SET				0xe6
#define MT6359_GPIO_MODE4_CLR				0xe8

#define MT6359_AUD_TOP_ID				0x2300
#define MT6359_AUD_TOP_REV0				0x2302
#define MT6359_AUD_TOP_DBI				0x2304
#define MT6359_AUD_TOP_DXI				0x2306
#define MT6359_AUD_TOP_CKPDN_TPM0			0x2308
#define MT6359_AUD_TOP_CKPDN_TPM1			0x230a
#define MT6359_AUD_TOP_CKPDN_CON0			0x230c
#define MT6359_AUD_TOP_CKPDN_CON0_SET			0x230e
#define MT6359_AUD_TOP_CKPDN_CON0_CLR			0x2310
#define MT6359_AUD_TOP_CKSEL_CON0			0x2312
#define MT6359_AUD_TOP_CKSEL_CON0_SET			0x2314
#define MT6359_AUD_TOP_CKSEL_CON0_CLR			0x2316
#define MT6359_AUD_TOP_CKTST_CON0			0x2318
#define MT6359_AUD_TOP_CLK_HWEN_CON0			0x231a
#define MT6359_AUD_TOP_CLK_HWEN_CON0_SET		0x231c
#define MT6359_AUD_TOP_CLK_HWEN_CON0_CLR		0x231e
#define MT6359_AUD_TOP_RST_CON0				0x2320
#define MT6359_AUD_TOP_RST_CON0_SET			0x2322
#define MT6359_AUD_TOP_RST_CON0_CLR			0x2324
#define MT6359_AUD_TOP_RST_BANK_CON0			0x2326
#define MT6359_AUD_TOP_INT_CON0				0x2328
#define MT6359_AUD_TOP_INT_CON0_SET			0x232a
#define MT6359_AUD_TOP_INT_CON0_CLR			0x232c
#define MT6359_AUD_TOP_INT_MASK_CON0			0x232e
#define MT6359_AUD_TOP_INT_MASK_CON0_SET		0x2330
#define MT6359_AUD_TOP_INT_MASK_CON0_CLR		0x2332
#define MT6359_AUD_TOP_INT_STATUS0			0x2334
#define MT6359_AUD_TOP_INT_RAW_STATUS0			0x2336
#define MT6359_AUD_TOP_INT_MISC_CON0			0x2338
#define MT6359_AUD_TOP_MON_CON0				0x233a
#define MT6359_AUDIO_DIG_DSN_ID				0x2380
#define MT6359_AUDIO_DIG_DSN_REV0			0x2382
#define MT6359_AUDIO_DIG_DSN_DBI			0x2384
#define MT6359_AUDIO_DIG_DSN_DXI			0x2386
#define MT6359_AFE_UL_DL_CON0				0x2388
#define MT6359_AFE_DL_SRC2_CON0_L			0x238a
#define MT6359_AFE_UL_SRC_CON0_H			0x238c
#define MT6359_AFE_UL_SRC_CON0_L			0x238e
#define MT6359_AFE_ADDA6_L_SRC_CON0_H			0x2390
#define MT6359_AFE_ADDA6_UL_SRC_CON0_L			0x2392
#define MT6359_AFE_TOP_CON0				0x2394
#define MT6359_AUDIO_TOP_CON0				0x2396
#define MT6359_AFE_MON_DEBUG0				0x2398
#define MT6359_AFUNC_AUD_CON0				0x239a
#define MT6359_AFUNC_AUD_CON1				0x239c
#define MT6359_AFUNC_AUD_CON2				0x239e
#define MT6359_AFUNC_AUD_CON3				0x23a0
#define MT6359_AFUNC_AUD_CON4				0x23a2
#define MT6359_AFUNC_AUD_CON5				0x23a4
#define MT6359_AFUNC_AUD_CON6				0x23a6
#define MT6359_AFUNC_AUD_CON7				0x23a8
#define MT6359_AFUNC_AUD_CON8				0x23aa
#define MT6359_AFUNC_AUD_CON9				0x23ac
#define MT6359_AFUNC_AUD_CON10				0x23ae
#define MT6359_AFUNC_AUD_CON11				0x23b0
#define MT6359_AFUNC_AUD_CON12				0x23b2
#define MT6359_AFUNC_AUD_MON0				0x23b4
#define MT6359_AFUNC_AUD_MON1				0x23b6
#define MT6359_AUDRC_TUNE_MON0				0x23b8
#define MT6359_AFE_ADDA_MTKAIF_FIFO_CFG0		0x23ba
#define MT6359_AFE_ADDA_MTKAIF_FIFO_LOG_MON1		0x23bc
#define MT6359_AFE_ADDA_MTKAIF_MON0			0x23be
#define MT6359_AFE_ADDA_MTKAIF_MON1			0x23c0
#define MT6359_AFE_ADDA_MTKAIF_MON2			0x23c2
#define MT6359_AFE_ADDA6_MTKAIF_MON3			0x23c4
#define MT6359_AFE_ADDA_MTKAIF_MON4			0x23c6
#define MT6359_AFE_ADDA_MTKAIF_MON5			0x23c8
#define MT6359_AFE_ADDA_MTKAIF_CFG0			0x23ca
#define MT6359_AFE_ADDA_MTKAIF_RX_CFG0			0x23cc
#define MT6359_AFE_ADDA_MTKAIF_RX_CFG1			0x23ce
#define MT6359_AFE_ADDA_MTKAIF_RX_CFG2			0x23d0
#define MT6359_AFE_ADDA_MTKAIF_RX_CFG3			0x23d2
#define MT6359_AFE_ADDA_MTKAIF_SYNCWORD_CFG0		0x23d4
#define MT6359_AFE_ADDA_MTKAIF_SYNCWORD_CFG1		0x23d6
#define MT6359_AFE_SGEN_CFG0				0x23d8
#define MT6359_AFE_SGEN_CFG1				0x23da
#define MT6359_AFE_ADC_ASYNC_FIFO_CFG			0x23dc
#define MT6359_AFE_ADC_ASYNC_FIFO_CFG1			0x23de
#define MT6359_AFE_DCCLK_CFG0				0x23e0
#define MT6359_AFE_DCCLK_CFG1				0x23e2
#define MT6359_AUDIO_DIG_CFG				0x23e4
#define MT6359_AUDIO_DIG_CFG1				0x23e6
#define MT6359_AFE_AUD_PAD_TOP				0x23e8
#define MT6359_AFE_AUD_PAD_TOP_MON			0x23ea
#define MT6359_AFE_AUD_PAD_TOP_MON1			0x23ec
#define MT6359_AFE_AUD_PAD_TOP_MON2			0x23ee
#define MT6359_AFE_DL_NLE_CFG				0x23f0
#define MT6359_AFE_DL_NLE_MON				0x23f2
#define MT6359_AFE_CG_EN_MON				0x23f4
#define MT6359_AFE_MIC_ARRAY_CFG			0x23f6
#define MT6359_AFE_CHOP_CFG0				0x23f8
#define MT6359_AFE_MTKAIF_MUX_CFG			0x23fa
#define MT6359_AUDIO_DIG_2ND_DSN_ID			0x2400
#define MT6359_AUDIO_DIG_2ND_DSN_REV0			0x2402
#define MT6359_AUDIO_DIG_2ND_DSN_DBI			0x2404
#define MT6359_AUDIO_DIG_2ND_DSN_DXI			0x2406
#define MT6359_AFE_PMIC_NEWIF_CFG3			0x2408
#define MT6359_AUDIO_DIG_3RD_DSN_ID			0x2480
#define MT6359_AUDIO_DIG_3RD_DSN_REV0			0x2482
#define MT6359_AUDIO_DIG_3RD_DSN_DBI			0x2484
#define MT6359_AUDIO_DIG_3RD_DSN_DXI			0x2486
#define MT6359_AFE_NCP_CFG0				0x24de
#define MT6359_AFE_NCP_CFG1				0x24e0
#define MT6359_AFE_NCP_CFG2				0x24e2
#define MT6359_AUDENC_DSN_ID				0x2500
#define MT6359_AUDENC_DSN_REV0				0x2502
#define MT6359_AUDENC_DSN_DBI				0x2504
#define MT6359_AUDENC_DSN_FPI				0x2506
#define MT6359_AUDENC_ANA_CON0				0x2508
#define MT6359_AUDENC_ANA_CON1				0x250a
#define MT6359_AUDENC_ANA_CON2				0x250c
#define MT6359_AUDENC_ANA_CON3				0x250e
#define MT6359_AUDENC_ANA_CON4				0x2510
#define MT6359_AUDENC_ANA_CON5				0x2512
#define MT6359_AUDENC_ANA_CON6				0x2514
#define MT6359_AUDENC_ANA_CON7				0x2516
#define MT6359_AUDENC_ANA_CON8				0x2518
#define MT6359_AUDENC_ANA_CON9				0x251a
#define MT6359_AUDENC_ANA_CON10				0x251c
#define MT6359_AUDENC_ANA_CON11				0x251e
#define MT6359_AUDENC_ANA_CON12				0x2520
#define MT6359_AUDENC_ANA_CON13				0x2522
#define MT6359_AUDENC_ANA_CON14				0x2524
#define MT6359_AUDENC_ANA_CON15				0x2526
#define MT6359_AUDENC_ANA_CON16				0x2528
#define MT6359_AUDENC_ANA_CON17				0x252a
#define MT6359_AUDENC_ANA_CON18				0x252c
#define MT6359_AUDENC_ANA_CON19				0x252e
#define MT6359_AUDENC_ANA_CON20				0x2530
#define MT6359_AUDENC_ANA_CON21				0x2532
#define MT6359_AUDENC_ANA_CON22				0x2534
#define MT6359_AUDENC_ANA_CON23				0x2536
#define MT6359_AUDDEC_DSN_ID				0x2580
#define MT6359_AUDDEC_DSN_REV0				0x2582
#define MT6359_AUDDEC_DSN_DBI				0x2584
#define MT6359_AUDDEC_DSN_FPI				0x2586
#define MT6359_AUDDEC_ANA_CON0				0x2588
#define MT6359_AUDDEC_ANA_CON1				0x258a
#define MT6359_AUDDEC_ANA_CON2				0x258c
#define MT6359_AUDDEC_ANA_CON3				0x258e
#define MT6359_AUDDEC_ANA_CON4				0x2590
#define MT6359_AUDDEC_ANA_CON5				0x2592
#define MT6359_AUDDEC_ANA_CON6				0x2594
#define MT6359_AUDDEC_ANA_CON7				0x2596
#define MT6359_AUDDEC_ANA_CON8				0x2598
#define MT6359_AUDDEC_ANA_CON9				0x259a
#define MT6359_AUDDEC_ANA_CON10				0x259c
#define MT6359_AUDDEC_ANA_CON11				0x259e
#define MT6359_AUDDEC_ANA_CON12				0x25a0
#define MT6359_AUDDEC_ANA_CON13				0x25a2
#define MT6359_AUDDEC_ANA_CON14				0x25a4
#define MT6359_AUDZCD_DSN_ID				0x2600
#define MT6359_AUDZCD_DSN_REV0				0x2602
#define MT6359_AUDZCD_DSN_DBI				0x2604
#define MT6359_AUDZCD_DSN_FPI				0x2606
#define MT6359_ZCD_CON0					0x2608
#define MT6359_ZCD_CON1					0x260a
#define MT6359_ZCD_CON2					0x260c
#define MT6359_ZCD_CON3					0x260e
#define MT6359_ZCD_CON4					0x2610
#define MT6359_ZCD_CON5					0x2612
#define MT6359_ACCDET_DSN_DIG_ID			0x2680
#define MT6359_ACCDET_DSN_DIG_REV0			0x2682
#define MT6359_ACCDET_DSN_DBI				0x2684
#define MT6359_ACCDET_DSN_FPI				0x2686
#define MT6359_ACCDET_CON0				0x2688
#define MT6359_ACCDET_CON1				0x268a
#define MT6359_ACCDET_CON2				0x268c
#define MT6359_ACCDET_CON3				0x268e
#define MT6359_ACCDET_CON4				0x2690
#define MT6359_ACCDET_CON5				0x2692
#define MT6359_ACCDET_CON6				0x2694
#define MT6359_ACCDET_CON7				0x2696
#define MT6359_ACCDET_CON8				0x2698
#define MT6359_ACCDET_CON9				0x269a
#define MT6359_ACCDET_CON10				0x269c
#define MT6359_ACCDET_CON11				0x269e
#define MT6359_ACCDET_CON12				0x26a0
#define MT6359_ACCDET_CON13				0x26a2
#define MT6359_ACCDET_CON14				0x26a4
#define MT6359_ACCDET_CON15				0x26a6
#define MT6359_ACCDET_CON16				0x26a8
#define MT6359_ACCDET_CON17				0x26aa
#define MT6359_ACCDET_CON18				0x26ac
#define MT6359_ACCDET_CON19				0x26ae
#define MT6359_ACCDET_CON20				0x26b0
#define MT6359_ACCDET_CON21				0x26b2
#define MT6359_ACCDET_CON22				0x26b4
#define MT6359_ACCDET_CON23				0x26b6
#define MT6359_ACCDET_CON24				0x26b8
#define MT6359_ACCDET_CON25				0x26ba
#define MT6359_ACCDET_CON26				0x26bc
#define MT6359_ACCDET_CON27				0x26be
#define MT6359_ACCDET_CON28				0x26c0
#define MT6359_ACCDET_CON29				0x26c2
#define MT6359_ACCDET_CON30				0x26c4
#define MT6359_ACCDET_CON31				0x26c6
#define MT6359_ACCDET_CON32				0x26c8
#define MT6359_ACCDET_CON33				0x26ca
#define MT6359_ACCDET_CON34				0x26cc
#define MT6359_ACCDET_CON35				0x26ce
#define MT6359_ACCDET_CON36				0x26d0
#define MT6359_ACCDET_CON37				0x26d2
#define MT6359_ACCDET_CON38				0x26d4
#define MT6359_ACCDET_CON39				0x26d6
#define MT6359_ACCDET_CON40				0x26d8
#define MT6359_MAX_REGISTER				MT6359_ZCD_CON5

/* dl bias */
#define DRBIAS_MASK 0x7
#define DRBIAS_HP_SFT (RG_AUDBIASADJ_0_VAUDP32_SFT + 0)
#define DRBIAS_HP_MASK_SFT (DRBIAS_MASK << DRBIAS_HP_SFT)
#define DRBIAS_HS_SFT (RG_AUDBIASADJ_0_VAUDP32_SFT + 3)
#define DRBIAS_HS_MASK_SFT (DRBIAS_MASK << DRBIAS_HS_SFT)
#define DRBIAS_LO_SFT (RG_AUDBIASADJ_0_VAUDP32_SFT + 6)
#define DRBIAS_LO_MASK_SFT (DRBIAS_MASK << DRBIAS_LO_SFT)
#define IBIAS_MASK 0x3
#define IBIAS_HP_SFT (RG_AUDBIASADJ_1_VAUDP32_SFT + 0)
#define IBIAS_HP_MASK_SFT (IBIAS_MASK << IBIAS_HP_SFT)
#define IBIAS_HS_SFT (RG_AUDBIASADJ_1_VAUDP32_SFT + 2)
#define IBIAS_HS_MASK_SFT (IBIAS_MASK << IBIAS_HS_SFT)
#define IBIAS_LO_SFT (RG_AUDBIASADJ_1_VAUDP32_SFT + 4)
#define IBIAS_LO_MASK_SFT (IBIAS_MASK << IBIAS_LO_SFT)
#define IBIAS_ZCD_SFT (RG_AUDBIASADJ_1_VAUDP32_SFT + 6)
#define IBIAS_ZCD_MASK_SFT (IBIAS_MASK << IBIAS_ZCD_SFT)

/* dl gain */
#define DL_GAIN_N_10DB_REG (DL_GAIN_N_10DB << 7 | DL_GAIN_N_10DB)
#define DL_GAIN_N_22DB_REG (DL_GAIN_N_22DB << 7 | DL_GAIN_N_22DB)
#define DL_GAIN_N_40DB_REG (DL_GAIN_N_40DB << 7 | DL_GAIN_N_40DB)
#define DL_GAIN_REG_MASK 0x0f9f

/* mic type mux */
#define MT_SOC_ENUM_EXT_ID(xname, xenum, xhandler_get, xhandler_put, id) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, .device = id,\
	.info = snd_soc_info_enum_double, \
	.get = xhandler_get, .put = xhandler_put, \
	.private_value = (unsigned long)&(xenum) }

enum {
	MT6359_MTKAIF_PROTOCOL_1 = 0,
	MT6359_MTKAIF_PROTOCOL_2,
	MT6359_MTKAIF_PROTOCOL_2_CLK_P2,
};

enum {
	MT6359_AIF_1 = 0,	/* dl: hp, rcv, hp+lo */
	MT6359_AIF_2,		/* dl: lo only */
	MT6359_AIF_NUM,
};

enum {
	AUDIO_ANALOG_VOLUME_HSOUTL,
	AUDIO_ANALOG_VOLUME_HSOUTR,
	AUDIO_ANALOG_VOLUME_HPOUTL,
	AUDIO_ANALOG_VOLUME_HPOUTR,
	AUDIO_ANALOG_VOLUME_LINEOUTL,
	AUDIO_ANALOG_VOLUME_LINEOUTR,
	AUDIO_ANALOG_VOLUME_MICAMP1,
	AUDIO_ANALOG_VOLUME_MICAMP2,
	AUDIO_ANALOG_VOLUME_MICAMP3,
	AUDIO_ANALOG_VOLUME_TYPE_MAX
};

enum {
	MUX_MIC_TYPE_0,	/* ain0, micbias 0 */
	MUX_MIC_TYPE_1,	/* ain1, micbias 1 */
	MUX_MIC_TYPE_2,	/* ain2/3, micbias 2 */
	MUX_PGA_L,
	MUX_PGA_R,
	MUX_PGA_3,
	MUX_HP,
	MUX_NUM,
};

enum {
	DEVICE_HP,
	DEVICE_LO,
	DEVICE_RCV,
	DEVICE_MIC1,
	DEVICE_MIC2,
	DEVICE_NUM
};

enum {
	HP_GAIN_CTL_ZCD = 0,
	HP_GAIN_CTL_NLE,
	HP_GAIN_CTL_NUM,
};

enum {
	HP_MUX_OPEN = 0,
	HP_MUX_HPSPK,
	HP_MUX_HP,
	HP_MUX_TEST_MODE,
	HP_MUX_HP_IMPEDANCE,
	HP_MUX_MASK = 0x7,
};

enum {
	RCV_MUX_OPEN = 0,
	RCV_MUX_MUTE,
	RCV_MUX_VOICE_PLAYBACK,
	RCV_MUX_TEST_MODE,
	RCV_MUX_MASK = 0x3,
};

enum {
	LO_MUX_OPEN = 0,
	LO_MUX_L_DAC,
	LO_MUX_3RD_DAC,
	LO_MUX_TEST_MODE,
	LO_MUX_MASK = 0x3,
};

/* Supply widget subseq */
enum {
	/* common */
	SUPPLY_SEQ_CLK_BUF,
	SUPPLY_SEQ_AUD_GLB,
	SUPPLY_SEQ_HP_PULL_DOWN,
	SUPPLY_SEQ_CLKSQ,
	SUPPLY_SEQ_ADC_CLKGEN,
	SUPPLY_SEQ_TOP_CK,
	SUPPLY_SEQ_TOP_CK_LAST,
	SUPPLY_SEQ_DCC_CLK,
	SUPPLY_SEQ_MIC_BIAS,
	SUPPLY_SEQ_DMIC,
	SUPPLY_SEQ_AUD_TOP,
	SUPPLY_SEQ_AUD_TOP_LAST,
	SUPPLY_SEQ_DL_SDM_FIFO_CLK,
	SUPPLY_SEQ_DL_SDM,
	SUPPLY_SEQ_DL_NCP,
	SUPPLY_SEQ_AFE,
	/* playback */
	SUPPLY_SEQ_DL_SRC,
	SUPPLY_SEQ_DL_ESD_RESIST,
	SUPPLY_SEQ_HP_DAMPING_OFF_RESET_CMFB,
	SUPPLY_SEQ_HP_MUTE,
	SUPPLY_SEQ_DL_LDO_REMOTE_SENSE,
	SUPPLY_SEQ_DL_LDO,
	SUPPLY_SEQ_DL_NV,
	SUPPLY_SEQ_HP_ANA_TRIM,
	SUPPLY_SEQ_DL_IBIST,
	/* capture */
	SUPPLY_SEQ_UL_PGA,
	SUPPLY_SEQ_UL_ADC,
	SUPPLY_SEQ_UL_MTKAIF,
	SUPPLY_SEQ_UL_SRC_DMIC,
	SUPPLY_SEQ_UL_SRC,
};

enum {
	CH_L = 0,
	CH_R,
	NUM_CH,
};

enum {
	DRBIAS_4UA = 0,
	DRBIAS_5UA,
	DRBIAS_6UA,
	DRBIAS_7UA,
	DRBIAS_8UA,
	DRBIAS_9UA,
	DRBIAS_10UA,
	DRBIAS_11UA,
};

enum {
	IBIAS_4UA = 0,
	IBIAS_5UA,
	IBIAS_6UA,
	IBIAS_7UA,
};

enum {
	IBIAS_ZCD_3UA = 0,
	IBIAS_ZCD_4UA,
	IBIAS_ZCD_5UA,
	IBIAS_ZCD_6UA,
};

enum {
	MIC_BIAS_1P7 = 0,
	MIC_BIAS_1P8,
	MIC_BIAS_1P9,
	MIC_BIAS_2P0,
	MIC_BIAS_2P1,
	MIC_BIAS_2P5,
	MIC_BIAS_2P6,
	MIC_BIAS_2P7,
};

/* dl pga gain */
enum {
	DL_GAIN_8DB = 0,
	DL_GAIN_0DB = 8,
	DL_GAIN_N_1DB = 9,
	DL_GAIN_N_10DB = 18,
	DL_GAIN_N_22DB = 30,
	DL_GAIN_N_40DB = 0x1f,
};

/* Mic Type MUX */
enum {
	MIC_TYPE_MUX_IDLE = 0,
	MIC_TYPE_MUX_ACC,
	MIC_TYPE_MUX_DMIC,
	MIC_TYPE_MUX_DCC,
	MIC_TYPE_MUX_DCC_ECM_DIFF,
	MIC_TYPE_MUX_DCC_ECM_SINGLE,
};

/* UL SRC MUX */
enum {
	UL_SRC_MUX_AMIC = 0,
	UL_SRC_MUX_DMIC,
};

/* MISO MUX */
enum {
	MISO_MUX_UL1_CH1 = 0,
	MISO_MUX_UL1_CH2,
	MISO_MUX_UL2_CH1,
	MISO_MUX_UL2_CH2,
};

/* DMIC MUX */
enum {
	DMIC_MUX_DMIC_DATA0 = 0,
	DMIC_MUX_DMIC_DATA1_L,
	DMIC_MUX_DMIC_DATA1_L_1,
	DMIC_MUX_DMIC_DATA1_R,
};

/* ADC L MUX */
enum {
	ADC_MUX_IDLE = 0,
	ADC_MUX_AIN0,
	ADC_MUX_PREAMPLIFIER,
	ADC_MUX_IDLE1,
};

/* PGA L MUX */
enum {
	PGA_L_MUX_NONE = 0,
	PGA_L_MUX_AIN0,
	PGA_L_MUX_AIN1,
};

/* PGA R MUX */
enum {
	PGA_R_MUX_NONE = 0,
	PGA_R_MUX_AIN2,
	PGA_R_MUX_AIN3,
	PGA_R_MUX_AIN0,
};

/* PGA 3 MUX */
enum {
	PGA_3_MUX_NONE = 0,
	PGA_3_MUX_AIN3,
	PGA_3_MUX_AIN2,
};

struct mt6359_priv {
	struct device *dev;
	struct regmap *regmap;
	unsigned int dl_rate[MT6359_AIF_NUM];
	unsigned int ul_rate[MT6359_AIF_NUM];
	int ana_gain[AUDIO_ANALOG_VOLUME_TYPE_MAX];
	unsigned int mux_select[MUX_NUM];
	unsigned int dmic_one_wire_mode;
	int dev_counter[DEVICE_NUM];
	int hp_gain_ctl;
	int hp_hifi_mode;
	int mtkaif_protocol;
};

#define CODEC_MT6359_NAME "mtk-codec-mt6359"
#define IS_DCC_BASE(type) ((type) == MIC_TYPE_MUX_DCC || \
			   (type) == MIC_TYPE_MUX_DCC_ECM_DIFF || \
			   (type) == MIC_TYPE_MUX_DCC_ECM_SINGLE)

void mt6359_set_mtkaif_protocol(struct snd_soc_component *cmpnt,
				int mtkaif_protocol);
void mt6359_mtkaif_calibration_enable(struct snd_soc_component *cmpnt);
void mt6359_mtkaif_calibration_disable(struct snd_soc_component *cmpnt);
void mt6359_set_mtkaif_calibration_phase(struct snd_soc_component *cmpnt,
					 int phase_1, int phase_2, int phase_3);

#endif/* end _MT6359_H_ */
