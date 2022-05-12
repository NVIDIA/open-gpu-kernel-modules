/*
 * CCI cache coherent interconnect driver
 *
 * Copyright (C) 2013 ARM Ltd.
 * Author: Lorenzo Pieralisi <lorenzo.pieralisi@arm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/arm-cci.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <asm/cacheflush.h>
#include <asm/smp_plat.h>

static void __iomem *cci_ctrl_base __ro_after_init;
static unsigned long cci_ctrl_phys __ro_after_init;

#ifdef CONFIG_ARM_CCI400_PORT_CTRL
struct cci_nb_ports {
	unsigned int nb_ace;
	unsigned int nb_ace_lite;
};

static const struct cci_nb_ports cci400_ports = {
	.nb_ace = 2,
	.nb_ace_lite = 3
};

#define CCI400_PORTS_DATA	(&cci400_ports)
#else
#define CCI400_PORTS_DATA	(NULL)
#endif

static const struct of_device_id arm_cci_matches[] = {
#ifdef CONFIG_ARM_CCI400_COMMON
	{.compatible = "arm,cci-400", .data = CCI400_PORTS_DATA },
#endif
#ifdef CONFIG_ARM_CCI5xx_PMU
	{ .compatible = "arm,cci-500", },
	{ .compatible = "arm,cci-550", },
#endif
	{},
};

static const struct of_dev_auxdata arm_cci_auxdata[] = {
	OF_DEV_AUXDATA("arm,cci-400-pmu", 0, NULL, &cci_ctrl_base),
	OF_DEV_AUXDATA("arm,cci-400-pmu,r0", 0, NULL, &cci_ctrl_base),
	OF_DEV_AUXDATA("arm,cci-400-pmu,r1", 0, NULL, &cci_ctrl_base),
	OF_DEV_AUXDATA("arm,cci-500-pmu,r0", 0, NULL, &cci_ctrl_base),
	OF_DEV_AUXDATA("arm,cci-550-pmu,r0", 0, NULL, &cci_ctrl_base),
	{}
};

#define DRIVER_NAME		"ARM-CCI"

static int cci_platform_probe(struct platform_device *pdev)
{
	if (!cci_probed())
		return -ENODEV;

	return of_platform_populate(pdev->dev.of_node, NULL,
				    arm_cci_auxdata, &pdev->dev);
}

static struct platform_driver cci_platform_driver = {
	.driver = {
		   .name = DRIVER_NAME,
		   .of_match_table = arm_cci_matches,
		  },
	.probe = cci_platform_probe,
};

static int __init cci_platform_init(void)
{
	return platform_driver_register(&cci_platform_driver);
}

#ifdef CONFIG_ARM_CCI400_PORT_CTRL

#define CCI_PORT_CTRL		0x0
#define CCI_CTRL_STATUS		0xc

#define CCI_ENABLE_SNOOP_REQ	0x1
#define CCI_ENABLE_DVM_REQ	0x2
#define CCI_ENABLE_REQ		(CCI_ENABLE_SNOOP_REQ | CCI_ENABLE_DVM_REQ)

enum cci_ace_port_type {
	ACE_INVALID_PORT = 0x0,
	ACE_PORT,
	ACE_LITE_PORT,
};

struct cci_ace_port {
	void __iomem *base;
	unsigned long phys;
	enum cci_ace_port_type type;
	struct device_node *dn;
};

static struct cci_ace_port *ports;
static unsigned int nb_cci_ports;

struct cpu_port {
	u64 mpidr;
	u32 port;
};

/*
 * Use the port MSB as valid flag, shift can be made dynamic
 * by computing number of bits required for port indexes.
 * Code disabling CCI cpu ports runs with D-cache invalidated
 * and SCTLR bit clear so data accesses must be kept to a minimum
 * to improve performance; for now shift is left static to
 * avoid one more data access while disabling the CCI port.
 */
#define PORT_VALID_SHIFT	31
#define PORT_VALID		(0x1 << PORT_VALID_SHIFT)

static inline void init_cpu_port(struct cpu_port *port, u32 index, u64 mpidr)
{
	port->port = PORT_VALID | index;
	port->mpidr = mpidr;
}

static inline bool cpu_port_is_valid(struct cpu_port *port)
{
	return !!(port->port & PORT_VALID);
}

static inline bool cpu_port_match(struct cpu_port *port, u64 mpidr)
{
	return port->mpidr == (mpidr & MPIDR_HWID_BITMASK);
}

static struct cpu_port cpu_port[NR_CPUS];

/**
 * __cci_ace_get_port - Function to retrieve the port index connected to
 *			a cpu or device.
 *
 * @dn: device node of the device to look-up
 * @type: port type
 *
 * Return value:
 *	- CCI port index if success
 *	- -ENODEV if failure
 */
static int __cci_ace_get_port(struct device_node *dn, int type)
{
	int i;
	bool ace_match;
	struct device_node *cci_portn;

	cci_portn = of_parse_phandle(dn, "cci-control-port", 0);
	for (i = 0; i < nb_cci_ports; i++) {
		ace_match = ports[i].type == type;
		if (ace_match && cci_portn == ports[i].dn)
			return i;
	}
	return -ENODEV;
}

int cci_ace_get_port(struct device_node *dn)
{
	return __cci_ace_get_port(dn, ACE_LITE_PORT);
}
EXPORT_SYMBOL_GPL(cci_ace_get_port);

static void cci_ace_init_ports(void)
{
	int port, cpu;
	struct device_node *cpun;

	/*
	 * Port index look-up speeds up the function disabling ports by CPU,
	 * since the logical to port index mapping is done once and does
	 * not change after system boot.
	 * The stashed index array is initialized for all possible CPUs
	 * at probe time.
	 */
	for_each_possible_cpu(cpu) {
		/* too early to use cpu->of_node */
		cpun = of_get_cpu_node(cpu, NULL);

		if (WARN(!cpun, "Missing cpu device node\n"))
			continue;

		port = __cci_ace_get_port(cpun, ACE_PORT);
		if (port < 0)
			continue;

		init_cpu_port(&cpu_port[cpu], port, cpu_logical_map(cpu));
	}

	for_each_possible_cpu(cpu) {
		WARN(!cpu_port_is_valid(&cpu_port[cpu]),
			"CPU %u does not have an associated CCI port\n",
			cpu);
	}
}
/*
 * Functions to enable/disable a CCI interconnect slave port
 *
 * They are called by low-level power management code to disable slave
 * interfaces snoops and DVM broadcast.
 * Since they may execute with cache data allocation disabled and
 * after the caches have been cleaned and invalidated the functions provide
 * no explicit locking since they may run with D-cache disabled, so normal
 * cacheable kernel locks based on ldrex/strex may not work.
 * Locking has to be provided by BSP implementations to ensure proper
 * operations.
 */

/**
 * cci_port_control() - function to control a CCI port
 *
 * @port: index of the port to setup
 * @enable: if true enables the port, if false disables it
 */
static void notrace cci_port_control(unsigned int port, bool enable)
{
	void __iomem *base = ports[port].base;

	writel_relaxed(enable ? CCI_ENABLE_REQ : 0, base + CCI_PORT_CTRL);
	/*
	 * This function is called from power down procedures
	 * and must not execute any instruction that might
	 * cause the processor to be put in a quiescent state
	 * (eg wfi). Hence, cpu_relax() can not be added to this
	 * read loop to optimize power, since it might hide possibly
	 * disruptive operations.
	 */
	while (readl_relaxed(cci_ctrl_base + CCI_CTRL_STATUS) & 0x1)
			;
}

/**
 * cci_disable_port_by_cpu() - function to disable a CCI port by CPU
 *			       reference
 *
 * @mpidr: mpidr of the CPU whose CCI port should be disabled
 *
 * Disabling a CCI port for a CPU implies disabling the CCI port
 * controlling that CPU cluster. Code disabling CPU CCI ports
 * must make sure that the CPU running the code is the last active CPU
 * in the cluster ie all other CPUs are quiescent in a low power state.
 *
 * Return:
 *	0 on success
 *	-ENODEV on port look-up failure
 */
int notrace cci_disable_port_by_cpu(u64 mpidr)
{
	int cpu;
	bool is_valid;
	for (cpu = 0; cpu < nr_cpu_ids; cpu++) {
		is_valid = cpu_port_is_valid(&cpu_port[cpu]);
		if (is_valid && cpu_port_match(&cpu_port[cpu], mpidr)) {
			cci_port_control(cpu_port[cpu].port, false);
			return 0;
		}
	}
	return -ENODEV;
}
EXPORT_SYMBOL_GPL(cci_disable_port_by_cpu);

/**
 * cci_enable_port_for_self() - enable a CCI port for calling CPU
 *
 * Enabling a CCI port for the calling CPU implies enabling the CCI
 * port controlling that CPU's cluster. Caller must make sure that the
 * CPU running the code is the first active CPU in the cluster and all
 * other CPUs are quiescent in a low power state  or waiting for this CPU
 * to complete the CCI initialization.
 *
 * Because this is called when the MMU is still off and with no stack,
 * the code must be position independent and ideally rely on callee
 * clobbered registers only.  To achieve this we must code this function
 * entirely in assembler.
 *
 * On success this returns with the proper CCI port enabled.  In case of
 * any failure this never returns as the inability to enable the CCI is
 * fatal and there is no possible recovery at this stage.
 */
asmlinkage void __naked cci_enable_port_for_self(void)
{
	asm volatile ("\n"
"	.arch armv7-a\n"
"	mrc	p15, 0, r0, c0, c0, 5	@ get MPIDR value \n"
"	and	r0, r0, #"__stringify(MPIDR_HWID_BITMASK)" \n"
"	adr	r1, 5f \n"
"	ldr	r2, [r1] \n"
"	add	r1, r1, r2		@ &cpu_port \n"
"	add	ip, r1, %[sizeof_cpu_port] \n"

	/* Loop over the cpu_port array looking for a matching MPIDR */
"1:	ldr	r2, [r1, %[offsetof_cpu_port_mpidr_lsb]] \n"
"	cmp	r2, r0 			@ compare MPIDR \n"
"	bne	2f \n"

	/* Found a match, now test port validity */
"	ldr	r3, [r1, %[offsetof_cpu_port_port]] \n"
"	tst	r3, #"__stringify(PORT_VALID)" \n"
"	bne	3f \n"

	/* no match, loop with the next cpu_port entry */
"2:	add	r1, r1, %[sizeof_struct_cpu_port] \n"
"	cmp	r1, ip			@ done? \n"
"	blo	1b \n"

	/* CCI port not found -- cheaply try to stall this CPU */
"cci_port_not_found: \n"
"	wfi \n"
"	wfe \n"
"	b	cci_port_not_found \n"

	/* Use matched port index to look up the corresponding ports entry */
"3:	bic	r3, r3, #"__stringify(PORT_VALID)" \n"
"	adr	r0, 6f \n"
"	ldmia	r0, {r1, r2} \n"
"	sub	r1, r1, r0 		@ virt - phys \n"
"	ldr	r0, [r0, r2] 		@ *(&ports) \n"
"	mov	r2, %[sizeof_struct_ace_port] \n"
"	mla	r0, r2, r3, r0		@ &ports[index] \n"
"	sub	r0, r0, r1		@ virt_to_phys() \n"

	/* Enable the CCI port */
"	ldr	r0, [r0, %[offsetof_port_phys]] \n"
"	mov	r3, %[cci_enable_req]\n"		   
"	str	r3, [r0, #"__stringify(CCI_PORT_CTRL)"] \n"

	/* poll the status reg for completion */
"	adr	r1, 7f \n"
"	ldr	r0, [r1] \n"
"	ldr	r0, [r0, r1]		@ cci_ctrl_base \n"
"4:	ldr	r1, [r0, #"__stringify(CCI_CTRL_STATUS)"] \n"
"	tst	r1, %[cci_control_status_bits] \n"			
"	bne	4b \n"

"	mov	r0, #0 \n"
"	bx	lr \n"

"	.align	2 \n"
"5:	.word	cpu_port - . \n"
"6:	.word	. \n"
"	.word	ports - 6b \n"
"7:	.word	cci_ctrl_phys - . \n"
	: :
	[sizeof_cpu_port] "i" (sizeof(cpu_port)),
	[cci_enable_req] "i" cpu_to_le32(CCI_ENABLE_REQ),
	[cci_control_status_bits] "i" cpu_to_le32(1),
#ifndef __ARMEB__
	[offsetof_cpu_port_mpidr_lsb] "i" (offsetof(struct cpu_port, mpidr)),
#else
	[offsetof_cpu_port_mpidr_lsb] "i" (offsetof(struct cpu_port, mpidr)+4),
#endif
	[offsetof_cpu_port_port] "i" (offsetof(struct cpu_port, port)),
	[sizeof_struct_cpu_port] "i" (sizeof(struct cpu_port)),
	[sizeof_struct_ace_port] "i" (sizeof(struct cci_ace_port)),
	[offsetof_port_phys] "i" (offsetof(struct cci_ace_port, phys)) );
}

/**
 * __cci_control_port_by_device() - function to control a CCI port by device
 *				    reference
 *
 * @dn: device node pointer of the device whose CCI port should be
 *      controlled
 * @enable: if true enables the port, if false disables it
 *
 * Return:
 *	0 on success
 *	-ENODEV on port look-up failure
 */
int notrace __cci_control_port_by_device(struct device_node *dn, bool enable)
{
	int port;

	if (!dn)
		return -ENODEV;

	port = __cci_ace_get_port(dn, ACE_LITE_PORT);
	if (WARN_ONCE(port < 0, "node %pOF ACE lite port look-up failure\n",
				dn))
		return -ENODEV;
	cci_port_control(port, enable);
	return 0;
}
EXPORT_SYMBOL_GPL(__cci_control_port_by_device);

/**
 * __cci_control_port_by_index() - function to control a CCI port by port index
 *
 * @port: port index previously retrieved with cci_ace_get_port()
 * @enable: if true enables the port, if false disables it
 *
 * Return:
 *	0 on success
 *	-ENODEV on port index out of range
 *	-EPERM if operation carried out on an ACE PORT
 */
int notrace __cci_control_port_by_index(u32 port, bool enable)
{
	if (port >= nb_cci_ports || ports[port].type == ACE_INVALID_PORT)
		return -ENODEV;
	/*
	 * CCI control for ports connected to CPUS is extremely fragile
	 * and must be made to go through a specific and controlled
	 * interface (ie cci_disable_port_by_cpu(); control by general purpose
	 * indexing is therefore disabled for ACE ports.
	 */
	if (ports[port].type == ACE_PORT)
		return -EPERM;

	cci_port_control(port, enable);
	return 0;
}
EXPORT_SYMBOL_GPL(__cci_control_port_by_index);

static const struct of_device_id arm_cci_ctrl_if_matches[] = {
	{.compatible = "arm,cci-400-ctrl-if", },
	{},
};

static int cci_probe_ports(struct device_node *np)
{
	struct cci_nb_ports const *cci_config;
	int ret, i, nb_ace = 0, nb_ace_lite = 0;
	struct device_node *cp;
	struct resource res;
	const char *match_str;
	bool is_ace;


	cci_config = of_match_node(arm_cci_matches, np)->data;
	if (!cci_config)
		return -ENODEV;

	nb_cci_ports = cci_config->nb_ace + cci_config->nb_ace_lite;

	ports = kcalloc(nb_cci_ports, sizeof(*ports), GFP_KERNEL);
	if (!ports)
		return -ENOMEM;

	for_each_available_child_of_node(np, cp) {
		if (!of_match_node(arm_cci_ctrl_if_matches, cp))
			continue;

		i = nb_ace + nb_ace_lite;

		if (i >= nb_cci_ports)
			break;

		if (of_property_read_string(cp, "interface-type",
					&match_str)) {
			WARN(1, "node %pOF missing interface-type property\n",
				  cp);
			continue;
		}
		is_ace = strcmp(match_str, "ace") == 0;
		if (!is_ace && strcmp(match_str, "ace-lite")) {
			WARN(1, "node %pOF containing invalid interface-type property, skipping it\n",
					cp);
			continue;
		}

		ret = of_address_to_resource(cp, 0, &res);
		if (!ret) {
			ports[i].base = ioremap(res.start, resource_size(&res));
			ports[i].phys = res.start;
		}
		if (ret || !ports[i].base) {
			WARN(1, "unable to ioremap CCI port %d\n", i);
			continue;
		}

		if (is_ace) {
			if (WARN_ON(nb_ace >= cci_config->nb_ace))
				continue;
			ports[i].type = ACE_PORT;
			++nb_ace;
		} else {
			if (WARN_ON(nb_ace_lite >= cci_config->nb_ace_lite))
				continue;
			ports[i].type = ACE_LITE_PORT;
			++nb_ace_lite;
		}
		ports[i].dn = cp;
	}

	/*
	 * If there is no CCI port that is under kernel control
	 * return early and report probe status.
	 */
	if (!nb_ace && !nb_ace_lite)
		return -ENODEV;

	 /* initialize a stashed array of ACE ports to speed-up look-up */
	cci_ace_init_ports();

	/*
	 * Multi-cluster systems may need this data when non-coherent, during
	 * cluster power-up/power-down. Make sure it reaches main memory.
	 */
	sync_cache_w(&cci_ctrl_base);
	sync_cache_w(&cci_ctrl_phys);
	sync_cache_w(&ports);
	sync_cache_w(&cpu_port);
	__sync_cache_range_w(ports, sizeof(*ports) * nb_cci_ports);
	pr_info("ARM CCI driver probed\n");

	return 0;
}
#else /* !CONFIG_ARM_CCI400_PORT_CTRL */
static inline int cci_probe_ports(struct device_node *np)
{
	return 0;
}
#endif /* CONFIG_ARM_CCI400_PORT_CTRL */

static int cci_probe(void)
{
	int ret;
	struct device_node *np;
	struct resource res;

	np = of_find_matching_node(NULL, arm_cci_matches);
	if (!of_device_is_available(np))
		return -ENODEV;

	ret = of_address_to_resource(np, 0, &res);
	if (!ret) {
		cci_ctrl_base = ioremap(res.start, resource_size(&res));
		cci_ctrl_phys =	res.start;
	}
	if (ret || !cci_ctrl_base) {
		WARN(1, "unable to ioremap CCI ctrl\n");
		return -ENXIO;
	}

	return cci_probe_ports(np);
}

static int cci_init_status = -EAGAIN;
static DEFINE_MUTEX(cci_probing);

static int cci_init(void)
{
	if (cci_init_status != -EAGAIN)
		return cci_init_status;

	mutex_lock(&cci_probing);
	if (cci_init_status == -EAGAIN)
		cci_init_status = cci_probe();
	mutex_unlock(&cci_probing);
	return cci_init_status;
}

/*
 * To sort out early init calls ordering a helper function is provided to
 * check if the CCI driver has beed initialized. Function check if the driver
 * has been initialized, if not it calls the init function that probes
 * the driver and updates the return value.
 */
bool cci_probed(void)
{
	return cci_init() == 0;
}
EXPORT_SYMBOL_GPL(cci_probed);

early_initcall(cci_init);
core_initcall(cci_platform_init);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ARM CCI support");
