/*
 * BRIEF MODULE DESCRIPTION
 *	Au1xx0 Power Management routines.
 *
 * Copyright 2001, 2008 MontaVista Software Inc.
 * Author: MontaVista Software, Inc. <source@mvista.com>
 *
 *  Some of the routines are right out of init/main.c, whose
 *  copyrights apply here.
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED	  ``AS	IS'' AND   ANY	EXPRESS OR IMPLIED
 *  WARRANTIES,	  INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO	EVENT  SHALL   THE AUTHOR  BE	 LIABLE FOR ANY	  DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED	  TO, PROCUREMENT OF  SUBSTITUTE GOODS	OR SERVICES; LOSS OF
 *  USE, DATA,	OR PROFITS; OR	BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN	 CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/jiffies.h>

#include <linux/uaccess.h>
#include <asm/mach-au1x00/au1000.h>

/*
 * We need to save/restore a bunch of core registers that are
 * either volatile or reset to some state across a processor sleep.
 * If reading a register doesn't provide a proper result for a
 * later restore, we have to provide a function for loading that
 * register and save a copy.
 *
 * We only have to save/restore registers that aren't otherwise
 * done as part of a driver pm_* function.
 */
static unsigned int sleep_sys_clocks[5];
static unsigned int sleep_sys_pinfunc;
static unsigned int sleep_static_memctlr[4][3];


static void save_core_regs(void)
{
	/* Clocks and PLLs. */
	sleep_sys_clocks[0] = alchemy_rdsys(AU1000_SYS_FREQCTRL0);
	sleep_sys_clocks[1] = alchemy_rdsys(AU1000_SYS_FREQCTRL1);
	sleep_sys_clocks[2] = alchemy_rdsys(AU1000_SYS_CLKSRC);
	sleep_sys_clocks[3] = alchemy_rdsys(AU1000_SYS_CPUPLL);
	sleep_sys_clocks[4] = alchemy_rdsys(AU1000_SYS_AUXPLL);

	/* pin mux config */
	sleep_sys_pinfunc = alchemy_rdsys(AU1000_SYS_PINFUNC);

	/* Save the static memory controller configuration. */
	sleep_static_memctlr[0][0] = alchemy_rdsmem(AU1000_MEM_STCFG0);
	sleep_static_memctlr[0][1] = alchemy_rdsmem(AU1000_MEM_STTIME0);
	sleep_static_memctlr[0][2] = alchemy_rdsmem(AU1000_MEM_STADDR0);
	sleep_static_memctlr[1][0] = alchemy_rdsmem(AU1000_MEM_STCFG1);
	sleep_static_memctlr[1][1] = alchemy_rdsmem(AU1000_MEM_STTIME1);
	sleep_static_memctlr[1][2] = alchemy_rdsmem(AU1000_MEM_STADDR1);
	sleep_static_memctlr[2][0] = alchemy_rdsmem(AU1000_MEM_STCFG2);
	sleep_static_memctlr[2][1] = alchemy_rdsmem(AU1000_MEM_STTIME2);
	sleep_static_memctlr[2][2] = alchemy_rdsmem(AU1000_MEM_STADDR2);
	sleep_static_memctlr[3][0] = alchemy_rdsmem(AU1000_MEM_STCFG3);
	sleep_static_memctlr[3][1] = alchemy_rdsmem(AU1000_MEM_STTIME3);
	sleep_static_memctlr[3][2] = alchemy_rdsmem(AU1000_MEM_STADDR3);
}

static void restore_core_regs(void)
{
	/* restore clock configuration.  Writing CPUPLL last will
	 * stall a bit and stabilize other clocks (unless this is
	 * one of those Au1000 with a write-only PLL, where we dont
	 * have a valid value)
	 */
	alchemy_wrsys(sleep_sys_clocks[0], AU1000_SYS_FREQCTRL0);
	alchemy_wrsys(sleep_sys_clocks[1], AU1000_SYS_FREQCTRL1);
	alchemy_wrsys(sleep_sys_clocks[2], AU1000_SYS_CLKSRC);
	alchemy_wrsys(sleep_sys_clocks[4], AU1000_SYS_AUXPLL);
	if (!au1xxx_cpu_has_pll_wo())
		alchemy_wrsys(sleep_sys_clocks[3], AU1000_SYS_CPUPLL);

	alchemy_wrsys(sleep_sys_pinfunc, AU1000_SYS_PINFUNC);

	/* Restore the static memory controller configuration. */
	alchemy_wrsmem(sleep_static_memctlr[0][0], AU1000_MEM_STCFG0);
	alchemy_wrsmem(sleep_static_memctlr[0][1], AU1000_MEM_STTIME0);
	alchemy_wrsmem(sleep_static_memctlr[0][2], AU1000_MEM_STADDR0);
	alchemy_wrsmem(sleep_static_memctlr[1][0], AU1000_MEM_STCFG1);
	alchemy_wrsmem(sleep_static_memctlr[1][1], AU1000_MEM_STTIME1);
	alchemy_wrsmem(sleep_static_memctlr[1][2], AU1000_MEM_STADDR1);
	alchemy_wrsmem(sleep_static_memctlr[2][0], AU1000_MEM_STCFG2);
	alchemy_wrsmem(sleep_static_memctlr[2][1], AU1000_MEM_STTIME2);
	alchemy_wrsmem(sleep_static_memctlr[2][2], AU1000_MEM_STADDR2);
	alchemy_wrsmem(sleep_static_memctlr[3][0], AU1000_MEM_STCFG3);
	alchemy_wrsmem(sleep_static_memctlr[3][1], AU1000_MEM_STTIME3);
	alchemy_wrsmem(sleep_static_memctlr[3][2], AU1000_MEM_STADDR3);
}

void au_sleep(void)
{
	save_core_regs();

	switch (alchemy_get_cputype()) {
	case ALCHEMY_CPU_AU1000:
	case ALCHEMY_CPU_AU1500:
	case ALCHEMY_CPU_AU1100:
		alchemy_sleep_au1000();
		break;
	case ALCHEMY_CPU_AU1550:
	case ALCHEMY_CPU_AU1200:
		alchemy_sleep_au1550();
		break;
	case ALCHEMY_CPU_AU1300:
		alchemy_sleep_au1300();
		break;
	}

	restore_core_regs();
}
