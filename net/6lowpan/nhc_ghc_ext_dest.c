// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	6LoWPAN Extension Header compression according to RFC7400
 */

#include "nhc.h"

#define LOWPAN_GHC_EXT_DEST_IDLEN	1
#define LOWPAN_GHC_EXT_DEST_ID_0	0xb6
#define LOWPAN_GHC_EXT_DEST_MASK_0	0xfe

static void dest_ghid_setup(struct lowpan_nhc *nhc)
{
	nhc->id[0] = LOWPAN_GHC_EXT_DEST_ID_0;
	nhc->idmask[0] = LOWPAN_GHC_EXT_DEST_MASK_0;
}

LOWPAN_NHC(ghc_ext_dest, "RFC7400 Destination Extension Header", NEXTHDR_DEST,
	   0, dest_ghid_setup, LOWPAN_GHC_EXT_DEST_IDLEN, NULL, NULL);

module_lowpan_nhc(ghc_ext_dest);
MODULE_DESCRIPTION("6LoWPAN generic header destination extension compression");
MODULE_LICENSE("GPL");
