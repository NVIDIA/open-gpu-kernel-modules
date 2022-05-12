/* SPDX-License-Identifier: GPL-2.0 */

    /*
     *  Zorro bus
     */

extern struct bus_type zorro_bus_type;


#ifdef CONFIG_ZORRO_NAMES
extern void zorro_name_device(struct zorro_dev *z);
#else
static inline void zorro_name_device(struct zorro_dev *dev) { }
#endif

extern const struct attribute_group *zorro_device_attribute_groups[];
