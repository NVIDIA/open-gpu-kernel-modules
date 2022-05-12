/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * vivid-rds-gen.h - rds (radio data system) generator support functions.
 *
 * Copyright 2014 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#ifndef _VIVID_RDS_GEN_H_
#define _VIVID_RDS_GEN_H_

/*
 * It takes almost exactly 5 seconds to transmit 57 RDS groups.
 * Each group has 4 blocks and each block has a payload of 16 bits + a
 * block identification. The driver will generate the contents of these
 * 57 groups only when necessary and it will just be played continuously.
 */
#define VIVID_RDS_GEN_GROUPS 57
#define VIVID_RDS_GEN_BLKS_PER_GRP 4
#define VIVID_RDS_GEN_BLOCKS (VIVID_RDS_GEN_BLKS_PER_GRP * VIVID_RDS_GEN_GROUPS)
#define VIVID_RDS_NSEC_PER_BLK (u32)(5ull * NSEC_PER_SEC / VIVID_RDS_GEN_BLOCKS)

struct vivid_rds_gen {
	struct v4l2_rds_data	data[VIVID_RDS_GEN_BLOCKS];
	bool			use_rbds;
	u16			picode;
	u8			pty;
	bool			mono_stereo;
	bool			art_head;
	bool			compressed;
	bool			dyn_pty;
	bool			ta;
	bool			tp;
	bool			ms;
	char			psname[8 + 1];
	char			radiotext[64 + 1];
};

void vivid_rds_gen_fill(struct vivid_rds_gen *rds, unsigned freq,
		    bool use_alternate);
void vivid_rds_generate(struct vivid_rds_gen *rds);

#endif
