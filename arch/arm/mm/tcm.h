/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2008-2009 ST-Ericsson AB
 * TCM memory handling for ARM systems
 *
 * Author: Linus Walleij <linus.walleij@stericsson.com>
 * Author: Rickard Andersson <rickard.andersson@stericsson.com>
 */

#ifdef CONFIG_HAVE_TCM
void __init tcm_init(void);
#else
/* No TCM support, just blank inlines to be optimized out */
static inline void tcm_init(void)
{
}
#endif
