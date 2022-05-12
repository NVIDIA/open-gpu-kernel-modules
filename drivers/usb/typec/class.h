/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __USB_TYPEC_CLASS__
#define __USB_TYPEC_CLASS__

#include <linux/device.h>
#include <linux/usb/typec.h>

struct typec_mux;
struct typec_switch;

struct typec_plug {
	struct device			dev;
	enum typec_plug_index		index;
	struct ida			mode_ids;
	int				num_altmodes;
};

struct typec_cable {
	struct device			dev;
	enum typec_plug_type		type;
	struct usb_pd_identity		*identity;
	unsigned int			active:1;
	u16				pd_revision; /* 0300H = "3.0" */
};

struct typec_partner {
	struct device			dev;
	unsigned int			usb_pd:1;
	struct usb_pd_identity		*identity;
	enum typec_accessory		accessory;
	struct ida			mode_ids;
	int				num_altmodes;
	u16				pd_revision; /* 0300H = "3.0" */
	enum usb_pd_svdm_ver		svdm_version;
};

struct typec_port {
	unsigned int			id;
	struct device			dev;
	struct ida			mode_ids;

	int				prefer_role;
	enum typec_data_role		data_role;
	enum typec_role			pwr_role;
	enum typec_role			vconn_role;
	enum typec_pwr_opmode		pwr_opmode;
	enum typec_port_type		port_type;
	struct mutex			port_type_lock;

	enum typec_orientation		orientation;
	struct typec_switch		*sw;
	struct typec_mux		*mux;

	const struct typec_capability	*cap;
	const struct typec_operations   *ops;

	struct list_head		port_list;
	struct mutex			port_list_lock; /* Port list lock */

	void				*pld;
};

#define to_typec_port(_dev_) container_of(_dev_, struct typec_port, dev)
#define to_typec_plug(_dev_) container_of(_dev_, struct typec_plug, dev)
#define to_typec_cable(_dev_) container_of(_dev_, struct typec_cable, dev)
#define to_typec_partner(_dev_) container_of(_dev_, struct typec_partner, dev)

extern const struct device_type typec_partner_dev_type;
extern const struct device_type typec_cable_dev_type;
extern const struct device_type typec_plug_dev_type;
extern const struct device_type typec_port_dev_type;

#define is_typec_partner(dev) ((dev)->type == &typec_partner_dev_type)
#define is_typec_cable(dev) ((dev)->type == &typec_cable_dev_type)
#define is_typec_plug(dev) ((dev)->type == &typec_plug_dev_type)
#define is_typec_port(dev) ((dev)->type == &typec_port_dev_type)

extern struct class typec_mux_class;
extern struct class typec_class;

int typec_link_ports(struct typec_port *connector);
void typec_unlink_ports(struct typec_port *connector);

#endif /* __USB_TYPEC_CLASS__ */
