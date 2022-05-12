// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-omap2/id.c
 *
 * OMAP2 CPU identification code
 *
 * Copyright (C) 2005 Nokia Corporation
 * Written by Tony Lindgren <tony@atomide.com>
 *
 * Copyright (C) 2009-11 Texas Instruments
 * Added OMAP4 support - Santosh Shilimkar <santosh.shilimkar@ti.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/random.h>
#include <linux/slab.h>

#ifdef CONFIG_SOC_BUS
#include <linux/sys_soc.h>
#endif

#include <asm/cputype.h>

#include "common.h"

#include "id.h"

#include "soc.h"
#include "control.h"

#define OMAP4_SILICON_TYPE_STANDARD		0x01
#define OMAP4_SILICON_TYPE_PERFORMANCE		0x02

#define OMAP_SOC_MAX_NAME_LENGTH		16

static unsigned int omap_revision;
static char soc_name[OMAP_SOC_MAX_NAME_LENGTH];
static char soc_rev[OMAP_SOC_MAX_NAME_LENGTH];
u32 omap_features;

unsigned int omap_rev(void)
{
	return omap_revision;
}
EXPORT_SYMBOL(omap_rev);

int omap_type(void)
{
	static u32 val = OMAP2_DEVICETYPE_MASK;

	if (val < OMAP2_DEVICETYPE_MASK)
		return val;

	if (soc_is_omap24xx()) {
		val = omap_ctrl_readl(OMAP24XX_CONTROL_STATUS);
	} else if (soc_is_ti81xx()) {
		val = omap_ctrl_readl(TI81XX_CONTROL_STATUS);
	} else if (soc_is_am33xx() || soc_is_am43xx()) {
		val = omap_ctrl_readl(AM33XX_CONTROL_STATUS);
	} else if (soc_is_omap34xx()) {
		val = omap_ctrl_readl(OMAP343X_CONTROL_STATUS);
	} else if (soc_is_omap44xx()) {
		val = omap_ctrl_readl(OMAP4_CTRL_MODULE_CORE_STATUS);
	} else if (soc_is_omap54xx() || soc_is_dra7xx()) {
		val = omap_ctrl_readl(OMAP5XXX_CONTROL_STATUS);
		val &= OMAP5_DEVICETYPE_MASK;
		val >>= 6;
		goto out;
	} else {
		pr_err("Cannot detect omap type!\n");
		goto out;
	}

	val &= OMAP2_DEVICETYPE_MASK;
	val >>= 8;

out:
	return val;
}
EXPORT_SYMBOL(omap_type);


/*----------------------------------------------------------------------------*/

#define OMAP_TAP_IDCODE		0x0204
#define OMAP_TAP_DIE_ID_0	0x0218
#define OMAP_TAP_DIE_ID_1	0x021C
#define OMAP_TAP_DIE_ID_2	0x0220
#define OMAP_TAP_DIE_ID_3	0x0224

#define OMAP_TAP_DIE_ID_44XX_0	0x0200
#define OMAP_TAP_DIE_ID_44XX_1	0x0208
#define OMAP_TAP_DIE_ID_44XX_2	0x020c
#define OMAP_TAP_DIE_ID_44XX_3	0x0210

#define read_tap_reg(reg)	readl_relaxed(tap_base  + (reg))

struct omap_id {
	u16	hawkeye;	/* Silicon type (Hawkeye id) */
	u8	dev;		/* Device type from production_id reg */
	u32	type;		/* Combined type id copied to omap_revision */
};

/* Register values to detect the OMAP version */
static struct omap_id omap_ids[] __initdata = {
	{ .hawkeye = 0xb5d9, .dev = 0x0, .type = 0x24200024 },
	{ .hawkeye = 0xb5d9, .dev = 0x1, .type = 0x24201024 },
	{ .hawkeye = 0xb5d9, .dev = 0x2, .type = 0x24202024 },
	{ .hawkeye = 0xb5d9, .dev = 0x4, .type = 0x24220024 },
	{ .hawkeye = 0xb5d9, .dev = 0x8, .type = 0x24230024 },
	{ .hawkeye = 0xb68a, .dev = 0x0, .type = 0x24300024 },
};

static void __iomem *tap_base;
static u16 tap_prod_id;

void omap_get_die_id(struct omap_die_id *odi)
{
	if (soc_is_omap44xx() || soc_is_omap54xx() || soc_is_dra7xx()) {
		odi->id_0 = read_tap_reg(OMAP_TAP_DIE_ID_44XX_0);
		odi->id_1 = read_tap_reg(OMAP_TAP_DIE_ID_44XX_1);
		odi->id_2 = read_tap_reg(OMAP_TAP_DIE_ID_44XX_2);
		odi->id_3 = read_tap_reg(OMAP_TAP_DIE_ID_44XX_3);

		return;
	}
	odi->id_0 = read_tap_reg(OMAP_TAP_DIE_ID_0);
	odi->id_1 = read_tap_reg(OMAP_TAP_DIE_ID_1);
	odi->id_2 = read_tap_reg(OMAP_TAP_DIE_ID_2);
	odi->id_3 = read_tap_reg(OMAP_TAP_DIE_ID_3);
}

static int __init omap_feed_randpool(void)
{
	struct omap_die_id odi;

	/* Throw the die ID into the entropy pool at boot */
	omap_get_die_id(&odi);
	add_device_randomness(&odi, sizeof(odi));
	return 0;
}
omap_device_initcall(omap_feed_randpool);

void __init omap2xxx_check_revision(void)
{
	int i, j;
	u32 idcode, prod_id;
	u16 hawkeye;
	u8  dev_type, rev;
	struct omap_die_id odi;

	idcode = read_tap_reg(OMAP_TAP_IDCODE);
	prod_id = read_tap_reg(tap_prod_id);
	hawkeye = (idcode >> 12) & 0xffff;
	rev = (idcode >> 28) & 0x0f;
	dev_type = (prod_id >> 16) & 0x0f;
	omap_get_die_id(&odi);

	pr_debug("OMAP_TAP_IDCODE 0x%08x REV %i HAWKEYE 0x%04x MANF %03x\n",
		 idcode, rev, hawkeye, (idcode >> 1) & 0x7ff);
	pr_debug("OMAP_TAP_DIE_ID_0: 0x%08x\n", odi.id_0);
	pr_debug("OMAP_TAP_DIE_ID_1: 0x%08x DEV_REV: %i\n",
		 odi.id_1, (odi.id_1 >> 28) & 0xf);
	pr_debug("OMAP_TAP_DIE_ID_2: 0x%08x\n", odi.id_2);
	pr_debug("OMAP_TAP_DIE_ID_3: 0x%08x\n", odi.id_3);
	pr_debug("OMAP_TAP_PROD_ID_0: 0x%08x DEV_TYPE: %i\n",
		 prod_id, dev_type);

	/* Check hawkeye ids */
	for (i = 0; i < ARRAY_SIZE(omap_ids); i++) {
		if (hawkeye == omap_ids[i].hawkeye)
			break;
	}

	if (i == ARRAY_SIZE(omap_ids)) {
		printk(KERN_ERR "Unknown OMAP CPU id\n");
		return;
	}

	for (j = i; j < ARRAY_SIZE(omap_ids); j++) {
		if (dev_type == omap_ids[j].dev)
			break;
	}

	if (j == ARRAY_SIZE(omap_ids)) {
		pr_err("Unknown OMAP device type. Handling it as OMAP%04x\n",
		       omap_ids[i].type >> 16);
		j = i;
	}

	sprintf(soc_name, "OMAP%04x", omap_rev() >> 16);
	sprintf(soc_rev, "ES%x", (omap_rev() >> 12) & 0xf);

	pr_info("%s", soc_name);
	if ((omap_rev() >> 8) & 0x0f)
		pr_cont("%s", soc_rev);
	pr_cont("\n");
}

#define OMAP3_SHOW_FEATURE(feat)		\
	if (omap3_has_ ##feat())		\
		n += scnprintf(buf + n, sizeof(buf) - n, #feat " ");

static void __init omap3_cpuinfo(void)
{
	const char *cpu_name;
	char buf[64];
	int n = 0;

	memset(buf, 0, sizeof(buf));

	/*
	 * OMAP3430 and OMAP3530 are assumed to be same.
	 *
	 * OMAP3525, OMAP3515 and OMAP3503 can be detected only based
	 * on available features. Upon detection, update the CPU id
	 * and CPU class bits.
	 */
	if (soc_is_omap3630()) {
		if (omap3_has_iva() && omap3_has_sgx()) {
			cpu_name = (omap3_has_isp()) ? "OMAP3630/DM3730" : "OMAP3621";
		} else if (omap3_has_iva()) {
			cpu_name = "DM3725";
		} else if (omap3_has_sgx()) {
			cpu_name = "OMAP3615/AM3715";
		} else {
			cpu_name = (omap3_has_isp()) ? "AM3703" : "OMAP3611";
		}
	} else if (soc_is_am35xx()) {
		cpu_name = (omap3_has_sgx()) ? "AM3517" : "AM3505";
	} else if (soc_is_ti816x()) {
		cpu_name = "TI816X";
	} else if (soc_is_am335x()) {
		cpu_name =  "AM335X";
	} else if (soc_is_am437x()) {
		cpu_name =  "AM437x";
	} else if (soc_is_ti814x()) {
		cpu_name = "TI814X";
	} else if (omap3_has_iva() && omap3_has_sgx()) {
		/* OMAP3430, OMAP3525, OMAP3515, OMAP3503 devices */
		cpu_name = "OMAP3430/3530";
	} else if (omap3_has_iva()) {
		cpu_name = "OMAP3525";
	} else if (omap3_has_sgx()) {
		cpu_name = "OMAP3515";
	} else {
		cpu_name = "OMAP3503";
	}

	scnprintf(soc_name, sizeof(soc_name), "%s", cpu_name);

	/* Print verbose information */
	n += scnprintf(buf, sizeof(buf) - n, "%s %s (", soc_name, soc_rev);

	OMAP3_SHOW_FEATURE(l2cache);
	OMAP3_SHOW_FEATURE(iva);
	OMAP3_SHOW_FEATURE(sgx);
	OMAP3_SHOW_FEATURE(neon);
	OMAP3_SHOW_FEATURE(isp);
	OMAP3_SHOW_FEATURE(192mhz_clk);
	if (*(buf + n - 1) == ' ')
		n--;
	n += scnprintf(buf + n, sizeof(buf) - n, ")\n");
	pr_info("%s", buf);
}

#define OMAP3_CHECK_FEATURE(status,feat)				\
	if (((status & OMAP3_ ##feat## _MASK) 				\
		>> OMAP3_ ##feat## _SHIFT) != FEAT_ ##feat## _NONE) { 	\
		omap_features |= OMAP3_HAS_ ##feat;			\
	}

void __init omap3xxx_check_features(void)
{
	u32 status;

	omap_features = 0;

	status = omap_ctrl_readl(OMAP3_CONTROL_OMAP_STATUS);

	OMAP3_CHECK_FEATURE(status, L2CACHE);
	OMAP3_CHECK_FEATURE(status, IVA);
	OMAP3_CHECK_FEATURE(status, SGX);
	OMAP3_CHECK_FEATURE(status, NEON);
	OMAP3_CHECK_FEATURE(status, ISP);
	if (soc_is_omap3630())
		omap_features |= OMAP3_HAS_192MHZ_CLK;
	if (soc_is_omap3430() || soc_is_omap3630())
		omap_features |= OMAP3_HAS_IO_WAKEUP;
	if (soc_is_omap3630() || omap_rev() == OMAP3430_REV_ES3_1 ||
	    omap_rev() == OMAP3430_REV_ES3_1_2)
		omap_features |= OMAP3_HAS_IO_CHAIN_CTRL;

	omap_features |= OMAP3_HAS_SDRC;

	/*
	 * am35x fixups:
	 * - The am35x Chip ID register has bits 12, 7:5, and 3:2 marked as
	 *   reserved and therefore return 0 when read.  Unfortunately,
	 *   OMAP3_CHECK_FEATURE() will interpret some of those zeroes to
	 *   mean that a feature is present even though it isn't so clear
	 *   the incorrectly set feature bits.
	 */
	if (soc_is_am35xx())
		omap_features &= ~(OMAP3_HAS_IVA | OMAP3_HAS_ISP);

	/*
	 * TODO: Get additional info (where applicable)
	 *       e.g. Size of L2 cache.
	 */

	omap3_cpuinfo();
}

void __init omap4xxx_check_features(void)
{
	u32 si_type;

	si_type =
	(read_tap_reg(OMAP4_CTRL_MODULE_CORE_STD_FUSE_PROD_ID_1) >> 16) & 0x03;

	if (si_type == OMAP4_SILICON_TYPE_PERFORMANCE)
		omap_features = OMAP4_HAS_PERF_SILICON;
}

void __init ti81xx_check_features(void)
{
	omap_features = OMAP3_HAS_NEON;
	omap3_cpuinfo();
}

void __init am33xx_check_features(void)
{
	u32 status;

	omap_features = OMAP3_HAS_NEON;

	status = omap_ctrl_readl(AM33XX_DEV_FEATURE);
	if (status & AM33XX_SGX_MASK)
		omap_features |= OMAP3_HAS_SGX;

	omap3_cpuinfo();
}

void __init omap3xxx_check_revision(void)
{
	const char *cpu_rev;
	u32 cpuid, idcode;
	u16 hawkeye;
	u8 rev;

	/*
	 * We cannot access revision registers on ES1.0.
	 * If the processor type is Cortex-A8 and the revision is 0x0
	 * it means its Cortex r0p0 which is 3430 ES1.0.
	 */
	cpuid = read_cpuid_id();
	if ((((cpuid >> 4) & 0xfff) == 0xc08) && ((cpuid & 0xf) == 0x0)) {
		omap_revision = OMAP3430_REV_ES1_0;
		cpu_rev = "1.0";
		return;
	}

	/*
	 * Detection for 34xx ES2.0 and above can be done with just
	 * hawkeye and rev. See TRM 1.5.2 Device Identification.
	 * Note that rev does not map directly to our defined processor
	 * revision numbers as ES1.0 uses value 0.
	 */
	idcode = read_tap_reg(OMAP_TAP_IDCODE);
	hawkeye = (idcode >> 12) & 0xffff;
	rev = (idcode >> 28) & 0xff;

	switch (hawkeye) {
	case 0xb7ae:
		/* Handle 34xx/35xx devices */
		switch (rev) {
		case 0: /* Take care of early samples */
		case 1:
			omap_revision = OMAP3430_REV_ES2_0;
			cpu_rev = "2.0";
			break;
		case 2:
			omap_revision = OMAP3430_REV_ES2_1;
			cpu_rev = "2.1";
			break;
		case 3:
			omap_revision = OMAP3430_REV_ES3_0;
			cpu_rev = "3.0";
			break;
		case 4:
			omap_revision = OMAP3430_REV_ES3_1;
			cpu_rev = "3.1";
			break;
		case 7:
		default:
			/* Use the latest known revision as default */
			omap_revision = OMAP3430_REV_ES3_1_2;
			cpu_rev = "3.1.2";
		}
		break;
	case 0xb868:
		/*
		 * Handle OMAP/AM 3505/3517 devices
		 *
		 * Set the device to be OMAP3517 here. Actual device
		 * is identified later based on the features.
		 */
		switch (rev) {
		case 0:
			omap_revision = AM35XX_REV_ES1_0;
			cpu_rev = "1.0";
			break;
		case 1:
		default:
			omap_revision = AM35XX_REV_ES1_1;
			cpu_rev = "1.1";
		}
		break;
	case 0xb891:
		/* Handle 36xx devices */

		switch(rev) {
		case 0: /* Take care of early samples */
			omap_revision = OMAP3630_REV_ES1_0;
			cpu_rev = "1.0";
			break;
		case 1:
			omap_revision = OMAP3630_REV_ES1_1;
			cpu_rev = "1.1";
			break;
		case 2:
		default:
			omap_revision = OMAP3630_REV_ES1_2;
			cpu_rev = "1.2";
		}
		break;
	case 0xb81e:
		switch (rev) {
		case 0:
			omap_revision = TI8168_REV_ES1_0;
			cpu_rev = "1.0";
			break;
		case 1:
			omap_revision = TI8168_REV_ES1_1;
			cpu_rev = "1.1";
			break;
		case 2:
			omap_revision = TI8168_REV_ES2_0;
			cpu_rev = "2.0";
			break;
		case 3:
		default:
			omap_revision = TI8168_REV_ES2_1;
			cpu_rev = "2.1";
		}
		break;
	case 0xb944:
		switch (rev) {
		case 0:
			omap_revision = AM335X_REV_ES1_0;
			cpu_rev = "1.0";
			break;
		case 1:
			omap_revision = AM335X_REV_ES2_0;
			cpu_rev = "2.0";
			break;
		case 2:
		default:
			omap_revision = AM335X_REV_ES2_1;
			cpu_rev = "2.1";
			break;
		}
		break;
	case 0xb98c:
		switch (rev) {
		case 0:
			omap_revision = AM437X_REV_ES1_0;
			cpu_rev = "1.0";
			break;
		case 1:
			omap_revision = AM437X_REV_ES1_1;
			cpu_rev = "1.1";
			break;
		case 2:
		default:
			omap_revision = AM437X_REV_ES1_2;
			cpu_rev = "1.2";
			break;
		}
		break;
	case 0xb8f2:
	case 0xb968:
		switch (rev) {
		case 0:
		case 1:
			omap_revision = TI8148_REV_ES1_0;
			cpu_rev = "1.0";
			break;
		case 2:
			omap_revision = TI8148_REV_ES2_0;
			cpu_rev = "2.0";
			break;
		case 3:
		default:
			omap_revision = TI8148_REV_ES2_1;
			cpu_rev = "2.1";
			break;
		}
		break;
	default:
		/* Unknown default to latest silicon rev as default */
		omap_revision = OMAP3630_REV_ES1_2;
		cpu_rev = "1.2";
		pr_warn("Warning: unknown chip type: hawkeye %04x, assuming OMAP3630ES1.2\n",
			hawkeye);
	}
	sprintf(soc_rev, "ES%s", cpu_rev);
}

void __init omap4xxx_check_revision(void)
{
	u32 idcode;
	u16 hawkeye;
	u8 rev;

	/*
	 * The IC rev detection is done with hawkeye and rev.
	 * Note that rev does not map directly to defined processor
	 * revision numbers as ES1.0 uses value 0.
	 */
	idcode = read_tap_reg(OMAP_TAP_IDCODE);
	hawkeye = (idcode >> 12) & 0xffff;
	rev = (idcode >> 28) & 0xf;

	/*
	 * Few initial 4430 ES2.0 samples IDCODE is same as ES1.0
	 * Use ARM register to detect the correct ES version
	 */
	if (!rev && (hawkeye != 0xb94e) && (hawkeye != 0xb975)) {
		idcode = read_cpuid_id();
		rev = (idcode & 0xf) - 1;
	}

	switch (hawkeye) {
	case 0xb852:
		switch (rev) {
		case 0:
			omap_revision = OMAP4430_REV_ES1_0;
			break;
		case 1:
		default:
			omap_revision = OMAP4430_REV_ES2_0;
		}
		break;
	case 0xb95c:
		switch (rev) {
		case 3:
			omap_revision = OMAP4430_REV_ES2_1;
			break;
		case 4:
			omap_revision = OMAP4430_REV_ES2_2;
			break;
		case 6:
		default:
			omap_revision = OMAP4430_REV_ES2_3;
		}
		break;
	case 0xb94e:
		switch (rev) {
		case 0:
			omap_revision = OMAP4460_REV_ES1_0;
			break;
		case 2:
		default:
			omap_revision = OMAP4460_REV_ES1_1;
			break;
		}
		break;
	case 0xb975:
		switch (rev) {
		case 0:
		default:
			omap_revision = OMAP4470_REV_ES1_0;
			break;
		}
		break;
	default:
		/* Unknown default to latest silicon rev as default */
		omap_revision = OMAP4430_REV_ES2_3;
	}

	sprintf(soc_name, "OMAP%04x", omap_rev() >> 16);
	sprintf(soc_rev, "ES%d.%d", (omap_rev() >> 12) & 0xf,
						(omap_rev() >> 8) & 0xf);
	pr_info("%s %s\n", soc_name, soc_rev);
}

void __init omap5xxx_check_revision(void)
{
	u32 idcode;
	u16 hawkeye;
	u8 rev;

	idcode = read_tap_reg(OMAP_TAP_IDCODE);
	hawkeye = (idcode >> 12) & 0xffff;
	rev = (idcode >> 28) & 0xff;
	switch (hawkeye) {
	case 0xb942:
		switch (rev) {
		case 0:
			/* No support for ES1.0 Test chip */
			BUG();
		case 1:
		default:
			omap_revision = OMAP5430_REV_ES2_0;
		}
		break;

	case 0xb998:
		switch (rev) {
		case 0:
			/* No support for ES1.0 Test chip */
			BUG();
		case 1:
		default:
			omap_revision = OMAP5432_REV_ES2_0;
		}
		break;

	default:
		/* Unknown default to latest silicon rev as default*/
		omap_revision = OMAP5430_REV_ES2_0;
	}

	sprintf(soc_name, "OMAP%04x", omap_rev() >> 16);
	sprintf(soc_rev, "ES%d.0", (omap_rev() >> 12) & 0xf);

	pr_info("%s %s\n", soc_name, soc_rev);
}

void __init dra7xxx_check_revision(void)
{
	u32 idcode;
	u16 hawkeye;
	u8 rev, package;
	struct omap_die_id odi;

	omap_get_die_id(&odi);
	package = (odi.id_2 >> 16) & 0x3;
	idcode = read_tap_reg(OMAP_TAP_IDCODE);
	hawkeye = (idcode >> 12) & 0xffff;
	rev = (idcode >> 28) & 0xff;
	switch (hawkeye) {
	case 0xbb50:
		switch (rev) {
		case 0:
		default:
			switch (package) {
			case 0x2:
				omap_revision = DRA762_ABZ_REV_ES1_0;
				break;
			case 0x3:
				omap_revision = DRA762_ACD_REV_ES1_0;
				break;
			default:
				omap_revision = DRA762_REV_ES1_0;
				break;
			}
			break;
		}
		break;

	case 0xb990:
		switch (rev) {
		case 0:
			omap_revision = DRA752_REV_ES1_0;
			break;
		case 1:
			omap_revision = DRA752_REV_ES1_1;
			break;
		case 2:
		default:
			omap_revision = DRA752_REV_ES2_0;
			break;
		}
		break;

	case 0xb9bc:
		switch (rev) {
		case 0:
			omap_revision = DRA722_REV_ES1_0;
			break;
		case 1:
			omap_revision = DRA722_REV_ES2_0;
			break;
		case 2:
		default:
			omap_revision = DRA722_REV_ES2_1;
			break;
		}
		break;

	default:
		/* Unknown default to latest silicon rev as default*/
		pr_warn("%s: unknown idcode=0x%08x (hawkeye=0x%08x,rev=0x%x)\n",
			__func__, idcode, hawkeye, rev);
		omap_revision = DRA752_REV_ES2_0;
	}

	sprintf(soc_name, "DRA%03x", omap_rev() >> 16);
	sprintf(soc_rev, "ES%d.%d", (omap_rev() >> 12) & 0xf,
		(omap_rev() >> 8) & 0xf);

	pr_info("%s %s\n", soc_name, soc_rev);
}

/*
 * Set up things for map_io and processor detection later on. Gets called
 * pretty much first thing from board init. For multi-omap, this gets
 * cpu_is_omapxxxx() working accurately enough for map_io. Then we'll try to
 * detect the exact revision later on in omap2_detect_revision() once map_io
 * is done.
 */
void __init omap2_set_globals_tap(u32 class, void __iomem *tap)
{
	omap_revision = class;
	tap_base = tap;

	/* XXX What is this intended to do? */
	if (soc_is_omap34xx())
		tap_prod_id = 0x0210;
	else
		tap_prod_id = 0x0208;
}

#ifdef CONFIG_SOC_BUS

static const char * const omap_types[] = {
	[OMAP2_DEVICE_TYPE_TEST]	= "TST",
	[OMAP2_DEVICE_TYPE_EMU]		= "EMU",
	[OMAP2_DEVICE_TYPE_SEC]		= "HS",
	[OMAP2_DEVICE_TYPE_GP]		= "GP",
	[OMAP2_DEVICE_TYPE_BAD]		= "BAD",
};

static const char * __init omap_get_family(void)
{
	if (soc_is_omap24xx())
		return kasprintf(GFP_KERNEL, "OMAP2");
	else if (soc_is_omap34xx())
		return kasprintf(GFP_KERNEL, "OMAP3");
	else if (soc_is_omap44xx())
		return kasprintf(GFP_KERNEL, "OMAP4");
	else if (soc_is_omap54xx())
		return kasprintf(GFP_KERNEL, "OMAP5");
	else if (soc_is_am33xx() || soc_is_am335x())
		return kasprintf(GFP_KERNEL, "AM33xx");
	else if (soc_is_am43xx())
		return kasprintf(GFP_KERNEL, "AM43xx");
	else if (soc_is_dra7xx())
		return kasprintf(GFP_KERNEL, "DRA7");
	else
		return kasprintf(GFP_KERNEL, "Unknown");
}

static ssize_t
type_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", omap_types[omap_type()]);
}

static DEVICE_ATTR_RO(type);

static struct attribute *omap_soc_attrs[] = {
	&dev_attr_type.attr,
	NULL
};

ATTRIBUTE_GROUPS(omap_soc);

void __init omap_soc_device_init(void)
{
	struct soc_device *soc_dev;
	struct soc_device_attribute *soc_dev_attr;

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return;

	soc_dev_attr->machine  = soc_name;
	soc_dev_attr->family   = omap_get_family();
	soc_dev_attr->revision = soc_rev;
	soc_dev_attr->custom_attr_group = omap_soc_groups[0];

	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev)) {
		kfree(soc_dev_attr);
		return;
	}
}
#endif /* CONFIG_SOC_BUS */
