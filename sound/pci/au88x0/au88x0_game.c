// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Manuel Jander.
 *
 *  Based on the work of:
 *  Vojtech Pavlik
 *  Raymond Ingles
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@suse.cz>, or by paper mail:
 * Vojtech Pavlik, Ucitelska 1576, Prague 8, 182 00 Czech Republic
 *
 * Based 90% on Vojtech Pavlik pcigame driver.
 * Merged and modified by Manuel Jander, for the OpenVortex
 * driver. (email: mjander@embedded.cl).
 */

#include <linux/time.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <sound/core.h>
#include "au88x0.h"
#include <linux/gameport.h>
#include <linux/export.h>

#if IS_REACHABLE(CONFIG_GAMEPORT)

#define VORTEX_GAME_DWAIT	20	/* 20 ms */

static unsigned char vortex_game_read(struct gameport *gameport)
{
	vortex_t *vortex = gameport_get_port_data(gameport);
	return hwread(vortex->mmio, VORTEX_GAME_LEGACY);
}

static void vortex_game_trigger(struct gameport *gameport)
{
	vortex_t *vortex = gameport_get_port_data(gameport);
	hwwrite(vortex->mmio, VORTEX_GAME_LEGACY, 0xff);
}

static int
vortex_game_cooked_read(struct gameport *gameport, int *axes, int *buttons)
{
	vortex_t *vortex = gameport_get_port_data(gameport);
	int i;

	*buttons = (~hwread(vortex->mmio, VORTEX_GAME_LEGACY) >> 4) & 0xf;

	for (i = 0; i < 4; i++) {
		axes[i] =
		    hwread(vortex->mmio, VORTEX_GAME_AXIS + (i * AXIS_SIZE));
		if (axes[i] == AXIS_RANGE)
			axes[i] = -1;
	}
	return 0;
}

static int vortex_game_open(struct gameport *gameport, int mode)
{
	vortex_t *vortex = gameport_get_port_data(gameport);

	switch (mode) {
	case GAMEPORT_MODE_COOKED:
		hwwrite(vortex->mmio, VORTEX_CTRL2,
			hwread(vortex->mmio,
			       VORTEX_CTRL2) | CTRL2_GAME_ADCMODE);
		msleep(VORTEX_GAME_DWAIT);
		return 0;
	case GAMEPORT_MODE_RAW:
		hwwrite(vortex->mmio, VORTEX_CTRL2,
			hwread(vortex->mmio,
			       VORTEX_CTRL2) & ~CTRL2_GAME_ADCMODE);
		return 0;
	default:
		return -1;
	}

	return 0;
}

static int vortex_gameport_register(vortex_t *vortex)
{
	struct gameport *gp;

	vortex->gameport = gp = gameport_allocate_port();
	if (!gp) {
		dev_err(vortex->card->dev,
			"cannot allocate memory for gameport\n");
		return -ENOMEM;
	}

	gameport_set_name(gp, "AU88x0 Gameport");
	gameport_set_phys(gp, "pci%s/gameport0", pci_name(vortex->pci_dev));
	gameport_set_dev_parent(gp, &vortex->pci_dev->dev);

	gp->read = vortex_game_read;
	gp->trigger = vortex_game_trigger;
	gp->cooked_read = vortex_game_cooked_read;
	gp->open = vortex_game_open;

	gameport_set_port_data(gp, vortex);
	gp->fuzz = 64;

	gameport_register_port(gp);

	return 0;
}

static void vortex_gameport_unregister(vortex_t * vortex)
{
	if (vortex->gameport) {
		gameport_unregister_port(vortex->gameport);
		vortex->gameport = NULL;
	}
}

#else
static inline int vortex_gameport_register(vortex_t * vortex) { return -ENOSYS; }
static inline void vortex_gameport_unregister(vortex_t * vortex) { }
#endif
