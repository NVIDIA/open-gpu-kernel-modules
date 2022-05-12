/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _INCLUDE_GUARD_i82092aa_H_
#define _INCLUDE_GUARD_i82092aa_H_

#include <linux/interrupt.h>

/* prototypes */

static int  i82092aa_pci_probe(struct pci_dev *dev, const struct pci_device_id *id);
static void i82092aa_pci_remove(struct pci_dev *dev);
static int card_present(int socketno);
static irqreturn_t i82092aa_interrupt(int irq, void *dev);




static int i82092aa_get_status(struct pcmcia_socket *socket, u_int *value);
static int i82092aa_set_socket(struct pcmcia_socket *socket, socket_state_t *state);
static int i82092aa_set_io_map(struct pcmcia_socket *socket, struct pccard_io_map *io);
static int i82092aa_set_mem_map(struct pcmcia_socket *socket, struct pccard_mem_map *mem);
static int i82092aa_init(struct pcmcia_socket *socket);

#endif

