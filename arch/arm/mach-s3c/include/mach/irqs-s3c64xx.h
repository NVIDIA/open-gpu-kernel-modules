/* SPDX-License-Identifier: GPL-2.0 */
/* linux/arch/arm/mach-s3c64xx/include/mach/irqs.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *      Ben Dooks <ben@simtec.co.uk>
 *      http://armlinux.simtec.co.uk/
 *
 * S3C64XX - IRQ support
 */

#ifndef __ASM_MACH_S3C64XX_IRQS_H
#define __ASM_MACH_S3C64XX_IRQS_H __FILE__

/* we keep the first set of CPU IRQs out of the range of
 * the ISA space, so that the PC104 has them to itself
 * and we don't end up having to do horrible things to the
 * standard ISA drivers....
 *
 * note, since we're using the VICs, our start must be a
 * mulitple of 32 to allow the common code to work
 */

#define S3C_IRQ_OFFSET	(32)

#define S3C_IRQ(x)	((x) + S3C_IRQ_OFFSET)

#define IRQ_VIC0_BASE	S3C_IRQ(0)
#define IRQ_VIC1_BASE	S3C_IRQ(32)

/* VIC based IRQs */

#define S3C64XX_IRQ_VIC0(x)	(IRQ_VIC0_BASE + (x))
#define S3C64XX_IRQ_VIC1(x)	(IRQ_VIC1_BASE + (x))

/* VIC0 */

#define IRQ_EINT0_3		S3C64XX_IRQ_VIC0(0)
#define IRQ_EINT4_11		S3C64XX_IRQ_VIC0(1)
#define IRQ_RTC_TIC		S3C64XX_IRQ_VIC0(2)
#define IRQ_CAMIF_C		S3C64XX_IRQ_VIC0(3)
#define IRQ_CAMIF_P		S3C64XX_IRQ_VIC0(4)
#define IRQ_CAMIF_MC		S3C64XX_IRQ_VIC0(5)
#define IRQ_S3C6410_IIC1	S3C64XX_IRQ_VIC0(5)
#define IRQ_S3C6410_IIS		S3C64XX_IRQ_VIC0(6)
#define IRQ_S3C6400_CAMIF_MP	S3C64XX_IRQ_VIC0(6)
#define IRQ_CAMIF_WE_C		S3C64XX_IRQ_VIC0(7)
#define IRQ_S3C6410_G3D		S3C64XX_IRQ_VIC0(8)
#define IRQ_S3C6400_CAMIF_WE_P	S3C64XX_IRQ_VIC0(8)
#define IRQ_POST0		S3C64XX_IRQ_VIC0(9)
#define IRQ_ROTATOR		S3C64XX_IRQ_VIC0(10)
#define IRQ_2D			S3C64XX_IRQ_VIC0(11)
#define IRQ_TVENC		S3C64XX_IRQ_VIC0(12)
#define IRQ_SCALER		S3C64XX_IRQ_VIC0(13)
#define IRQ_BATF		S3C64XX_IRQ_VIC0(14)
#define IRQ_JPEG		S3C64XX_IRQ_VIC0(15)
#define IRQ_MFC			S3C64XX_IRQ_VIC0(16)
#define IRQ_SDMA0		S3C64XX_IRQ_VIC0(17)
#define IRQ_SDMA1		S3C64XX_IRQ_VIC0(18)
#define IRQ_ARM_DMAERR		S3C64XX_IRQ_VIC0(19)
#define IRQ_ARM_DMA		S3C64XX_IRQ_VIC0(20)
#define IRQ_ARM_DMAS		S3C64XX_IRQ_VIC0(21)
#define IRQ_KEYPAD		S3C64XX_IRQ_VIC0(22)
#define IRQ_TIMER0_VIC		S3C64XX_IRQ_VIC0(23)
#define IRQ_TIMER1_VIC		S3C64XX_IRQ_VIC0(24)
#define IRQ_TIMER2_VIC		S3C64XX_IRQ_VIC0(25)
#define IRQ_WDT			S3C64XX_IRQ_VIC0(26)
#define IRQ_TIMER3_VIC		S3C64XX_IRQ_VIC0(27)
#define IRQ_TIMER4_VIC		S3C64XX_IRQ_VIC0(28)
#define IRQ_LCD_FIFO		S3C64XX_IRQ_VIC0(29)
#define IRQ_LCD_VSYNC		S3C64XX_IRQ_VIC0(30)
#define IRQ_LCD_SYSTEM		S3C64XX_IRQ_VIC0(31)

/* VIC1 */

#define IRQ_EINT12_19		S3C64XX_IRQ_VIC1(0)
#define IRQ_EINT20_27		S3C64XX_IRQ_VIC1(1)
#define IRQ_PCM0		S3C64XX_IRQ_VIC1(2)
#define IRQ_PCM1		S3C64XX_IRQ_VIC1(3)
#define IRQ_AC97		S3C64XX_IRQ_VIC1(4)
#define IRQ_UART0		S3C64XX_IRQ_VIC1(5)
#define IRQ_UART1		S3C64XX_IRQ_VIC1(6)
#define IRQ_UART2		S3C64XX_IRQ_VIC1(7)
#define IRQ_UART3		S3C64XX_IRQ_VIC1(8)
#define IRQ_DMA0		S3C64XX_IRQ_VIC1(9)
#define IRQ_DMA1		S3C64XX_IRQ_VIC1(10)
#define IRQ_ONENAND0		S3C64XX_IRQ_VIC1(11)
#define IRQ_ONENAND1		S3C64XX_IRQ_VIC1(12)
#define IRQ_NFC			S3C64XX_IRQ_VIC1(13)
#define IRQ_CFCON		S3C64XX_IRQ_VIC1(14)
#define IRQ_USBH		S3C64XX_IRQ_VIC1(15)
#define IRQ_SPI0		S3C64XX_IRQ_VIC1(16)
#define IRQ_SPI1		S3C64XX_IRQ_VIC1(17)
#define IRQ_IIC			S3C64XX_IRQ_VIC1(18)
#define IRQ_HSItx		S3C64XX_IRQ_VIC1(19)
#define IRQ_HSIrx		S3C64XX_IRQ_VIC1(20)
#define IRQ_RESERVED		S3C64XX_IRQ_VIC1(21)
#define IRQ_MSM			S3C64XX_IRQ_VIC1(22)
#define IRQ_HOSTIF		S3C64XX_IRQ_VIC1(23)
#define IRQ_HSMMC0		S3C64XX_IRQ_VIC1(24)
#define IRQ_HSMMC1		S3C64XX_IRQ_VIC1(25)
#define IRQ_HSMMC2		IRQ_SPI1	/* shared with SPI1 */
#define IRQ_OTG			S3C64XX_IRQ_VIC1(26)
#define IRQ_IRDA		S3C64XX_IRQ_VIC1(27)
#define IRQ_RTC_ALARM		S3C64XX_IRQ_VIC1(28)
#define IRQ_SEC			S3C64XX_IRQ_VIC1(29)
#define IRQ_PENDN		S3C64XX_IRQ_VIC1(30)
#define IRQ_TC			IRQ_PENDN
#define IRQ_ADC			S3C64XX_IRQ_VIC1(31)

/* compatibility for device defines */

#define IRQ_IIC1		IRQ_S3C6410_IIC1

/* Since the IRQ_EINT(x) are a linear mapping on current s3c64xx series
 * we just defined them as an IRQ_EINT(x) macro from S3C_IRQ_EINT_BASE
 * which we place after the pair of VICs. */

#define S3C_IRQ_EINT_BASE	S3C_IRQ(64+5)

#define S3C_EINT(x)		((x) + S3C_IRQ_EINT_BASE)
#define IRQ_EINT(x)		S3C_EINT(x)
#define IRQ_EINT_BIT(x)		((x) - S3C_EINT(0))

/* Next the external interrupt groups. These are similar to the IRQ_EINT(x)
 * that they are sourced from the GPIO pins but with a different scheme for
 * priority and source indication.
 *
 * The IRQ_EINT(x) can be thought of as 'group 0' of the available GPIO
 * interrupts, but for historical reasons they are kept apart from these
 * next interrupts.
 *
 * Use IRQ_EINT_GROUP(group, offset) to get the number for use in the
 * machine specific support files.
 */

#define IRQ_EINT_GROUP1_NR	(15)
#define IRQ_EINT_GROUP2_NR	(8)
#define IRQ_EINT_GROUP3_NR	(5)
#define IRQ_EINT_GROUP4_NR	(14)
#define IRQ_EINT_GROUP5_NR	(7)
#define IRQ_EINT_GROUP6_NR	(10)
#define IRQ_EINT_GROUP7_NR	(16)
#define IRQ_EINT_GROUP8_NR	(15)
#define IRQ_EINT_GROUP9_NR	(9)

#define IRQ_EINT_GROUP_BASE	S3C_EINT(28)
#define IRQ_EINT_GROUP1_BASE	(IRQ_EINT_GROUP_BASE + 0x00)
#define IRQ_EINT_GROUP2_BASE	(IRQ_EINT_GROUP1_BASE + IRQ_EINT_GROUP1_NR)
#define IRQ_EINT_GROUP3_BASE	(IRQ_EINT_GROUP2_BASE + IRQ_EINT_GROUP2_NR)
#define IRQ_EINT_GROUP4_BASE	(IRQ_EINT_GROUP3_BASE + IRQ_EINT_GROUP3_NR)
#define IRQ_EINT_GROUP5_BASE	(IRQ_EINT_GROUP4_BASE + IRQ_EINT_GROUP4_NR)
#define IRQ_EINT_GROUP6_BASE	(IRQ_EINT_GROUP5_BASE + IRQ_EINT_GROUP5_NR)
#define IRQ_EINT_GROUP7_BASE	(IRQ_EINT_GROUP6_BASE + IRQ_EINT_GROUP6_NR)
#define IRQ_EINT_GROUP8_BASE	(IRQ_EINT_GROUP7_BASE + IRQ_EINT_GROUP7_NR)
#define IRQ_EINT_GROUP9_BASE	(IRQ_EINT_GROUP8_BASE + IRQ_EINT_GROUP8_NR)

#define IRQ_EINT_GROUP(group, no)	(IRQ_EINT_GROUP##group##_BASE + (no))

/* Some boards have their own IRQs behind this */
#define IRQ_BOARD_START (IRQ_EINT_GROUP9_BASE + IRQ_EINT_GROUP9_NR + 1)

/* Set the default nr_irqs, boards can override if necessary */
#define S3C64XX_NR_IRQS	IRQ_BOARD_START

/* Compatibility */

#define IRQ_ONENAND	IRQ_ONENAND0
#define IRQ_I2S0	IRQ_S3C6410_IIS

#endif /* __ASM_MACH_S3C64XX_IRQS_H */

