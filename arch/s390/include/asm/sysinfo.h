/* SPDX-License-Identifier: GPL-2.0 */
/*
 * definition for store system information stsi
 *
 * Copyright IBM Corp. 2001, 2008
 *
 *    Author(s): Ulrich Weigand <weigand@de.ibm.com>
 *		 Christian Borntraeger <borntraeger@de.ibm.com>
 */

#ifndef __ASM_S390_SYSINFO_H
#define __ASM_S390_SYSINFO_H

#include <asm/bitsperlong.h>
#include <linux/uuid.h>

struct sysinfo_1_1_1 {
	unsigned char p:1;
	unsigned char :6;
	unsigned char t:1;
	unsigned char :8;
	unsigned char ccr;
	unsigned char cai;
	char reserved_0[20];
	unsigned long lic;
	char manufacturer[16];
	char type[4];
	char reserved_1[12];
	char model_capacity[16];
	char sequence[16];
	char plant[4];
	char model[16];
	char model_perm_cap[16];
	char model_temp_cap[16];
	unsigned int model_cap_rating;
	unsigned int model_perm_cap_rating;
	unsigned int model_temp_cap_rating;
	unsigned char typepct[5];
	unsigned char reserved_2[3];
	unsigned int ncr;
	unsigned int npr;
	unsigned int ntr;
};

struct sysinfo_1_2_1 {
	char reserved_0[80];
	char sequence[16];
	char plant[4];
	char reserved_1[2];
	unsigned short cpu_address;
};

struct sysinfo_1_2_2 {
	char format;
	char reserved_0[1];
	unsigned short acc_offset;
	unsigned char mt_installed :1;
	unsigned char :2;
	unsigned char mt_stid :5;
	unsigned char :3;
	unsigned char mt_gtid :5;
	char reserved_1[18];
	unsigned int nominal_cap;
	unsigned int secondary_cap;
	unsigned int capability;
	unsigned short cpus_total;
	unsigned short cpus_configured;
	unsigned short cpus_standby;
	unsigned short cpus_reserved;
	unsigned short adjustment[0];
};

struct sysinfo_1_2_2_extension {
	unsigned int alt_capability;
	unsigned short alt_adjustment[0];
};

struct sysinfo_2_2_1 {
	char reserved_0[80];
	char sequence[16];
	char plant[4];
	unsigned short cpu_id;
	unsigned short cpu_address;
};

struct sysinfo_2_2_2 {
	char reserved_0[32];
	unsigned short lpar_number;
	char reserved_1;
	unsigned char characteristics;
	unsigned short cpus_total;
	unsigned short cpus_configured;
	unsigned short cpus_standby;
	unsigned short cpus_reserved;
	char name[8];
	unsigned int caf;
	char reserved_2[8];
	unsigned char mt_installed :1;
	unsigned char :2;
	unsigned char mt_stid :5;
	unsigned char :3;
	unsigned char mt_gtid :5;
	unsigned char :3;
	unsigned char mt_psmtid :5;
	char reserved_3[5];
	unsigned short cpus_dedicated;
	unsigned short cpus_shared;
	char reserved_4[3];
	unsigned char vsne;
	uuid_t uuid;
	char reserved_5[160];
	char ext_name[256];
};

#define LPAR_CHAR_DEDICATED	(1 << 7)
#define LPAR_CHAR_SHARED	(1 << 6)
#define LPAR_CHAR_LIMITED	(1 << 5)

struct sysinfo_3_2_2 {
	char reserved_0[31];
	unsigned char :4;
	unsigned char count:4;
	struct {
		char reserved_0[4];
		unsigned short cpus_total;
		unsigned short cpus_configured;
		unsigned short cpus_standby;
		unsigned short cpus_reserved;
		char name[8];
		unsigned int caf;
		char cpi[16];
		char reserved_1[3];
		unsigned char evmne;
		unsigned int reserved_2;
		uuid_t uuid;
	} vm[8];
	char reserved_3[1504];
	char ext_names[8][256];
};

extern int topology_max_mnest;

/*
 * Returns the maximum nesting level supported by the cpu topology code.
 * The current maximum level is 4 which is the drawer level.
 */
static inline unsigned char topology_mnest_limit(void)
{
	return min(topology_max_mnest, 4);
}

#define TOPOLOGY_NR_MAG		6

struct topology_core {
	unsigned char nl;
	unsigned char reserved0[3];
	unsigned char :5;
	unsigned char d:1;
	unsigned char pp:2;
	unsigned char reserved1;
	unsigned short origin;
	unsigned long mask;
};

struct topology_container {
	unsigned char nl;
	unsigned char reserved[6];
	unsigned char id;
};

union topology_entry {
	unsigned char nl;
	struct topology_core cpu;
	struct topology_container container;
};

struct sysinfo_15_1_x {
	unsigned char reserved0[2];
	unsigned short length;
	unsigned char mag[TOPOLOGY_NR_MAG];
	unsigned char reserved1;
	unsigned char mnest;
	unsigned char reserved2[4];
	union topology_entry tle[0];
};

int stsi(void *sysinfo, int fc, int sel1, int sel2);

/*
 * Service level reporting interface.
 */
struct service_level {
	struct list_head list;
	void (*seq_print)(struct seq_file *, struct service_level *);
};

int register_service_level(struct service_level *);
int unregister_service_level(struct service_level *);

int sthyi_fill(void *dst, u64 *rc);
#endif /* __ASM_S390_SYSINFO_H */
