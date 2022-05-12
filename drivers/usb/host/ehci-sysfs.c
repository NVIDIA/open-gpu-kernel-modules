// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2007 by Alan Stern
 */

/* this file is part of ehci-hcd.c */


/* Display the ports dedicated to the companion controller */
static ssize_t companion_show(struct device *dev,
			      struct device_attribute *attr,
			      char *buf)
{
	struct ehci_hcd		*ehci;
	int			nports, index, n;
	int			count = PAGE_SIZE;
	char			*ptr = buf;

	ehci = hcd_to_ehci(dev_get_drvdata(dev));
	nports = HCS_N_PORTS(ehci->hcs_params);

	for (index = 0; index < nports; ++index) {
		if (test_bit(index, &ehci->companion_ports)) {
			n = scnprintf(ptr, count, "%d\n", index + 1);
			ptr += n;
			count -= n;
		}
	}
	return ptr - buf;
}

/*
 * Dedicate or undedicate a port to the companion controller.
 * Syntax is "[-]portnum", where a leading '-' sign means
 * return control of the port to the EHCI controller.
 */
static ssize_t companion_store(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct ehci_hcd		*ehci;
	int			portnum, new_owner;

	ehci = hcd_to_ehci(dev_get_drvdata(dev));
	new_owner = PORT_OWNER;		/* Owned by companion */
	if (sscanf(buf, "%d", &portnum) != 1)
		return -EINVAL;
	if (portnum < 0) {
		portnum = - portnum;
		new_owner = 0;		/* Owned by EHCI */
	}
	if (portnum <= 0 || portnum > HCS_N_PORTS(ehci->hcs_params))
		return -ENOENT;
	portnum--;
	if (new_owner)
		set_bit(portnum, &ehci->companion_ports);
	else
		clear_bit(portnum, &ehci->companion_ports);
	set_owner(ehci, portnum, new_owner);
	return count;
}
static DEVICE_ATTR_RW(companion);


/*
 * Display / Set uframe_periodic_max
 */
static ssize_t uframe_periodic_max_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct ehci_hcd		*ehci;
	int			n;

	ehci = hcd_to_ehci(dev_get_drvdata(dev));
	n = scnprintf(buf, PAGE_SIZE, "%d\n", ehci->uframe_periodic_max);
	return n;
}


static ssize_t uframe_periodic_max_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct ehci_hcd		*ehci;
	unsigned		uframe_periodic_max;
	unsigned		uframe;
	unsigned long		flags;
	ssize_t			ret;

	ehci = hcd_to_ehci(dev_get_drvdata(dev));
	if (kstrtouint(buf, 0, &uframe_periodic_max) < 0)
		return -EINVAL;

	if (uframe_periodic_max < 100 || uframe_periodic_max >= 125) {
		ehci_info(ehci, "rejecting invalid request for "
				"uframe_periodic_max=%u\n", uframe_periodic_max);
		return -EINVAL;
	}

	ret = -EINVAL;

	/*
	 * lock, so that our checking does not race with possible periodic
	 * bandwidth allocation through submitting new urbs.
	 */
	spin_lock_irqsave (&ehci->lock, flags);

	/*
	 * for request to decrease max periodic bandwidth, we have to check
	 * to see whether the decrease is possible.
	 */
	if (uframe_periodic_max < ehci->uframe_periodic_max) {
		u8		allocated_max = 0;

		for (uframe = 0; uframe < EHCI_BANDWIDTH_SIZE; ++uframe)
			allocated_max = max(allocated_max,
					ehci->bandwidth[uframe]);

		if (allocated_max > uframe_periodic_max) {
			ehci_info(ehci,
				"cannot decrease uframe_periodic_max because "
				"periodic bandwidth is already allocated "
				"(%u > %u)\n",
				allocated_max, uframe_periodic_max);
			goto out_unlock;
		}
	}

	/* increasing is always ok */

	ehci_info(ehci, "setting max periodic bandwidth to %u%% "
			"(== %u usec/uframe)\n",
			100*uframe_periodic_max/125, uframe_periodic_max);

	if (uframe_periodic_max != 100)
		ehci_warn(ehci, "max periodic bandwidth set is non-standard\n");

	ehci->uframe_periodic_max = uframe_periodic_max;
	ret = count;

out_unlock:
	spin_unlock_irqrestore (&ehci->lock, flags);
	return ret;
}
static DEVICE_ATTR_RW(uframe_periodic_max);


static inline int create_sysfs_files(struct ehci_hcd *ehci)
{
	struct device	*controller = ehci_to_hcd(ehci)->self.controller;
	int	i = 0;

	/* with integrated TT there is no companion! */
	if (!ehci_is_TDI(ehci))
		i = device_create_file(controller, &dev_attr_companion);
	if (i)
		goto out;

	i = device_create_file(controller, &dev_attr_uframe_periodic_max);
out:
	return i;
}

static inline void remove_sysfs_files(struct ehci_hcd *ehci)
{
	struct device	*controller = ehci_to_hcd(ehci)->self.controller;

	/* with integrated TT there is no companion! */
	if (!ehci_is_TDI(ehci))
		device_remove_file(controller, &dev_attr_companion);

	device_remove_file(controller, &dev_attr_uframe_periodic_max);
}
