// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2006 Jake Moilanen <moilanen@austin.ibm.com>, IBM Corp.
 * Copyright 2006-2007 Michael Ellerman, IBM Corp.
 */

#include <linux/crash_dump.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/msi.h>

#include <asm/rtas.h>
#include <asm/hw_irq.h>
#include <asm/ppc-pci.h>
#include <asm/machdep.h>

#include "pseries.h"

static int query_token, change_token;

#define RTAS_QUERY_FN		0
#define RTAS_CHANGE_FN		1
#define RTAS_RESET_FN		2
#define RTAS_CHANGE_MSI_FN	3
#define RTAS_CHANGE_MSIX_FN	4
#define RTAS_CHANGE_32MSI_FN	5

/* RTAS Helpers */

static int rtas_change_msi(struct pci_dn *pdn, u32 func, u32 num_irqs)
{
	u32 addr, seq_num, rtas_ret[3];
	unsigned long buid;
	int rc;

	addr = rtas_config_addr(pdn->busno, pdn->devfn, 0);
	buid = pdn->phb->buid;

	seq_num = 1;
	do {
		if (func == RTAS_CHANGE_MSI_FN || func == RTAS_CHANGE_MSIX_FN ||
		    func == RTAS_CHANGE_32MSI_FN)
			rc = rtas_call(change_token, 6, 4, rtas_ret, addr,
					BUID_HI(buid), BUID_LO(buid),
					func, num_irqs, seq_num);
		else
			rc = rtas_call(change_token, 6, 3, rtas_ret, addr,
					BUID_HI(buid), BUID_LO(buid),
					func, num_irqs, seq_num);

		seq_num = rtas_ret[1];
	} while (rtas_busy_delay(rc));

	/*
	 * If the RTAS call succeeded, return the number of irqs allocated.
	 * If not, make sure we return a negative error code.
	 */
	if (rc == 0)
		rc = rtas_ret[0];
	else if (rc > 0)
		rc = -rc;

	pr_debug("rtas_msi: ibm,change_msi(func=%d,num=%d), got %d rc = %d\n",
		 func, num_irqs, rtas_ret[0], rc);

	return rc;
}

static void rtas_disable_msi(struct pci_dev *pdev)
{
	struct pci_dn *pdn;

	pdn = pci_get_pdn(pdev);
	if (!pdn)
		return;

	/*
	 * disabling MSI with the explicit interface also disables MSI-X
	 */
	if (rtas_change_msi(pdn, RTAS_CHANGE_MSI_FN, 0) != 0) {
		/* 
		 * may have failed because explicit interface is not
		 * present
		 */
		if (rtas_change_msi(pdn, RTAS_CHANGE_FN, 0) != 0) {
			pr_debug("rtas_msi: Setting MSIs to 0 failed!\n");
		}
	}
}

static int rtas_query_irq_number(struct pci_dn *pdn, int offset)
{
	u32 addr, rtas_ret[2];
	unsigned long buid;
	int rc;

	addr = rtas_config_addr(pdn->busno, pdn->devfn, 0);
	buid = pdn->phb->buid;

	do {
		rc = rtas_call(query_token, 4, 3, rtas_ret, addr,
			       BUID_HI(buid), BUID_LO(buid), offset);
	} while (rtas_busy_delay(rc));

	if (rc) {
		pr_debug("rtas_msi: error (%d) querying source number\n", rc);
		return rc;
	}

	return rtas_ret[0];
}

static void rtas_teardown_msi_irqs(struct pci_dev *pdev)
{
	struct msi_desc *entry;

	for_each_pci_msi_entry(entry, pdev) {
		if (!entry->irq)
			continue;

		irq_set_msi_desc(entry->irq, NULL);
		irq_dispose_mapping(entry->irq);
	}

	rtas_disable_msi(pdev);
}

static int check_req(struct pci_dev *pdev, int nvec, char *prop_name)
{
	struct device_node *dn;
	const __be32 *p;
	u32 req_msi;

	dn = pci_device_to_OF_node(pdev);

	p = of_get_property(dn, prop_name, NULL);
	if (!p) {
		pr_debug("rtas_msi: No %s on %pOF\n", prop_name, dn);
		return -ENOENT;
	}

	req_msi = be32_to_cpup(p);
	if (req_msi < nvec) {
		pr_debug("rtas_msi: %s requests < %d MSIs\n", prop_name, nvec);

		if (req_msi == 0) /* Be paranoid */
			return -ENOSPC;

		return req_msi;
	}

	return 0;
}

static int check_req_msi(struct pci_dev *pdev, int nvec)
{
	return check_req(pdev, nvec, "ibm,req#msi");
}

static int check_req_msix(struct pci_dev *pdev, int nvec)
{
	return check_req(pdev, nvec, "ibm,req#msi-x");
}

/* Quota calculation */

static struct device_node *find_pe_total_msi(struct pci_dev *dev, int *total)
{
	struct device_node *dn;
	const __be32 *p;

	dn = of_node_get(pci_device_to_OF_node(dev));
	while (dn) {
		p = of_get_property(dn, "ibm,pe-total-#msi", NULL);
		if (p) {
			pr_debug("rtas_msi: found prop on dn %pOF\n",
				dn);
			*total = be32_to_cpup(p);
			return dn;
		}

		dn = of_get_next_parent(dn);
	}

	return NULL;
}

static struct device_node *find_pe_dn(struct pci_dev *dev, int *total)
{
	struct device_node *dn;
	struct eeh_dev *edev;

	/* Found our PE and assume 8 at that point. */

	dn = pci_device_to_OF_node(dev);
	if (!dn)
		return NULL;

	/* Get the top level device in the PE */
	edev = pdn_to_eeh_dev(PCI_DN(dn));
	if (edev->pe)
		edev = list_first_entry(&edev->pe->edevs, struct eeh_dev,
					entry);
	dn = pci_device_to_OF_node(edev->pdev);
	if (!dn)
		return NULL;

	/* We actually want the parent */
	dn = of_get_parent(dn);
	if (!dn)
		return NULL;

	/* Hardcode of 8 for old firmwares */
	*total = 8;
	pr_debug("rtas_msi: using PE dn %pOF\n", dn);

	return dn;
}

struct msi_counts {
	struct device_node *requestor;
	int num_devices;
	int request;
	int quota;
	int spare;
	int over_quota;
};

static void *count_non_bridge_devices(struct device_node *dn, void *data)
{
	struct msi_counts *counts = data;
	const __be32 *p;
	u32 class;

	pr_debug("rtas_msi: counting %pOF\n", dn);

	p = of_get_property(dn, "class-code", NULL);
	class = p ? be32_to_cpup(p) : 0;

	if ((class >> 8) != PCI_CLASS_BRIDGE_PCI)
		counts->num_devices++;

	return NULL;
}

static void *count_spare_msis(struct device_node *dn, void *data)
{
	struct msi_counts *counts = data;
	const __be32 *p;
	int req;

	if (dn == counts->requestor)
		req = counts->request;
	else {
		/* We don't know if a driver will try to use MSI or MSI-X,
		 * so we just have to punt and use the larger of the two. */
		req = 0;
		p = of_get_property(dn, "ibm,req#msi", NULL);
		if (p)
			req = be32_to_cpup(p);

		p = of_get_property(dn, "ibm,req#msi-x", NULL);
		if (p)
			req = max(req, (int)be32_to_cpup(p));
	}

	if (req < counts->quota)
		counts->spare += counts->quota - req;
	else if (req > counts->quota)
		counts->over_quota++;

	return NULL;
}

static int msi_quota_for_device(struct pci_dev *dev, int request)
{
	struct device_node *pe_dn;
	struct msi_counts counts;
	int total;

	pr_debug("rtas_msi: calc quota for %s, request %d\n", pci_name(dev),
		  request);

	pe_dn = find_pe_total_msi(dev, &total);
	if (!pe_dn)
		pe_dn = find_pe_dn(dev, &total);

	if (!pe_dn) {
		pr_err("rtas_msi: couldn't find PE for %s\n", pci_name(dev));
		goto out;
	}

	pr_debug("rtas_msi: found PE %pOF\n", pe_dn);

	memset(&counts, 0, sizeof(struct msi_counts));

	/* Work out how many devices we have below this PE */
	pci_traverse_device_nodes(pe_dn, count_non_bridge_devices, &counts);

	if (counts.num_devices == 0) {
		pr_err("rtas_msi: found 0 devices under PE for %s\n",
			pci_name(dev));
		goto out;
	}

	counts.quota = total / counts.num_devices;
	if (request <= counts.quota)
		goto out;

	/* else, we have some more calculating to do */
	counts.requestor = pci_device_to_OF_node(dev);
	counts.request = request;
	pci_traverse_device_nodes(pe_dn, count_spare_msis, &counts);

	/* If the quota isn't an integer multiple of the total, we can
	 * use the remainder as spare MSIs for anyone that wants them. */
	counts.spare += total % counts.num_devices;

	/* Divide any spare by the number of over-quota requestors */
	if (counts.over_quota)
		counts.quota += counts.spare / counts.over_quota;

	/* And finally clamp the request to the possibly adjusted quota */
	request = min(counts.quota, request);

	pr_debug("rtas_msi: request clamped to quota %d\n", request);
out:
	of_node_put(pe_dn);

	return request;
}

static int check_msix_entries(struct pci_dev *pdev)
{
	struct msi_desc *entry;
	int expected;

	/* There's no way for us to express to firmware that we want
	 * a discontiguous, or non-zero based, range of MSI-X entries.
	 * So we must reject such requests. */

	expected = 0;
	for_each_pci_msi_entry(entry, pdev) {
		if (entry->msi_attrib.entry_nr != expected) {
			pr_debug("rtas_msi: bad MSI-X entries.\n");
			return -EINVAL;
		}
		expected++;
	}

	return 0;
}

static void rtas_hack_32bit_msi_gen2(struct pci_dev *pdev)
{
	u32 addr_hi, addr_lo;

	/*
	 * We should only get in here for IODA1 configs. This is based on the
	 * fact that we using RTAS for MSIs, we don't have the 32 bit MSI RTAS
	 * support, and we are in a PCIe Gen2 slot.
	 */
	dev_info(&pdev->dev,
		 "rtas_msi: No 32 bit MSI firmware support, forcing 32 bit MSI\n");
	pci_read_config_dword(pdev, pdev->msi_cap + PCI_MSI_ADDRESS_HI, &addr_hi);
	addr_lo = 0xffff0000 | ((addr_hi >> (48 - 32)) << 4);
	pci_write_config_dword(pdev, pdev->msi_cap + PCI_MSI_ADDRESS_LO, addr_lo);
	pci_write_config_dword(pdev, pdev->msi_cap + PCI_MSI_ADDRESS_HI, 0);
}

static int rtas_setup_msi_irqs(struct pci_dev *pdev, int nvec_in, int type)
{
	struct pci_dn *pdn;
	int hwirq, virq, i, quota, rc;
	struct msi_desc *entry;
	struct msi_msg msg;
	int nvec = nvec_in;
	int use_32bit_msi_hack = 0;

	if (type == PCI_CAP_ID_MSIX)
		rc = check_req_msix(pdev, nvec);
	else
		rc = check_req_msi(pdev, nvec);

	if (rc)
		return rc;

	quota = msi_quota_for_device(pdev, nvec);

	if (quota && quota < nvec)
		return quota;

	if (type == PCI_CAP_ID_MSIX && check_msix_entries(pdev))
		return -EINVAL;

	/*
	 * Firmware currently refuse any non power of two allocation
	 * so we round up if the quota will allow it.
	 */
	if (type == PCI_CAP_ID_MSIX) {
		int m = roundup_pow_of_two(nvec);
		quota = msi_quota_for_device(pdev, m);

		if (quota >= m)
			nvec = m;
	}

	pdn = pci_get_pdn(pdev);

	/*
	 * Try the new more explicit firmware interface, if that fails fall
	 * back to the old interface. The old interface is known to never
	 * return MSI-Xs.
	 */
again:
	if (type == PCI_CAP_ID_MSI) {
		if (pdev->no_64bit_msi) {
			rc = rtas_change_msi(pdn, RTAS_CHANGE_32MSI_FN, nvec);
			if (rc < 0) {
				/*
				 * We only want to run the 32 bit MSI hack below if
				 * the max bus speed is Gen2 speed
				 */
				if (pdev->bus->max_bus_speed != PCIE_SPEED_5_0GT)
					return rc;

				use_32bit_msi_hack = 1;
			}
		} else
			rc = -1;

		if (rc < 0)
			rc = rtas_change_msi(pdn, RTAS_CHANGE_MSI_FN, nvec);

		if (rc < 0) {
			pr_debug("rtas_msi: trying the old firmware call.\n");
			rc = rtas_change_msi(pdn, RTAS_CHANGE_FN, nvec);
		}

		if (use_32bit_msi_hack && rc > 0)
			rtas_hack_32bit_msi_gen2(pdev);
	} else
		rc = rtas_change_msi(pdn, RTAS_CHANGE_MSIX_FN, nvec);

	if (rc != nvec) {
		if (nvec != nvec_in) {
			nvec = nvec_in;
			goto again;
		}
		pr_debug("rtas_msi: rtas_change_msi() failed\n");
		return rc;
	}

	i = 0;
	for_each_pci_msi_entry(entry, pdev) {
		hwirq = rtas_query_irq_number(pdn, i++);
		if (hwirq < 0) {
			pr_debug("rtas_msi: error (%d) getting hwirq\n", rc);
			return hwirq;
		}

		/*
		 * Depending on the number of online CPUs in the original
		 * kernel, it is likely for CPU #0 to be offline in a kdump
		 * kernel. The associated IRQs in the affinity mappings
		 * provided by irq_create_affinity_masks() are thus not
		 * started by irq_startup(), as per-design for managed IRQs.
		 * This can be a problem with multi-queue block devices driven
		 * by blk-mq : such a non-started IRQ is very likely paired
		 * with the single queue enforced by blk-mq during kdump (see
		 * blk_mq_alloc_tag_set()). This causes the device to remain
		 * silent and likely hangs the guest at some point.
		 *
		 * We don't really care for fine-grained affinity when doing
		 * kdump actually : simply ignore the pre-computed affinity
		 * masks in this case and let the default mask with all CPUs
		 * be used when creating the IRQ mappings.
		 */
		if (is_kdump_kernel())
			virq = irq_create_mapping(NULL, hwirq);
		else
			virq = irq_create_mapping_affinity(NULL, hwirq,
							   entry->affinity);

		if (!virq) {
			pr_debug("rtas_msi: Failed mapping hwirq %d\n", hwirq);
			return -ENOSPC;
		}

		dev_dbg(&pdev->dev, "rtas_msi: allocated virq %d\n", virq);
		irq_set_msi_desc(virq, entry);

		/* Read config space back so we can restore after reset */
		__pci_read_msi_msg(entry, &msg);
		entry->msg = msg;
	}

	return 0;
}

static void rtas_msi_pci_irq_fixup(struct pci_dev *pdev)
{
	/* No LSI -> leave MSIs (if any) configured */
	if (!pdev->irq) {
		dev_dbg(&pdev->dev, "rtas_msi: no LSI, nothing to do.\n");
		return;
	}

	/* No MSI -> MSIs can't have been assigned by fw, leave LSI */
	if (check_req_msi(pdev, 1) && check_req_msix(pdev, 1)) {
		dev_dbg(&pdev->dev, "rtas_msi: no req#msi/x, nothing to do.\n");
		return;
	}

	dev_dbg(&pdev->dev, "rtas_msi: disabling existing MSI.\n");
	rtas_disable_msi(pdev);
}

static int rtas_msi_init(void)
{
	struct pci_controller *phb;

	query_token  = rtas_token("ibm,query-interrupt-source-number");
	change_token = rtas_token("ibm,change-msi");

	if ((query_token == RTAS_UNKNOWN_SERVICE) ||
			(change_token == RTAS_UNKNOWN_SERVICE)) {
		pr_debug("rtas_msi: no RTAS tokens, no MSI support.\n");
		return -1;
	}

	pr_debug("rtas_msi: Registering RTAS MSI callbacks.\n");

	WARN_ON(pseries_pci_controller_ops.setup_msi_irqs);
	pseries_pci_controller_ops.setup_msi_irqs = rtas_setup_msi_irqs;
	pseries_pci_controller_ops.teardown_msi_irqs = rtas_teardown_msi_irqs;

	list_for_each_entry(phb, &hose_list, list_node) {
		WARN_ON(phb->controller_ops.setup_msi_irqs);
		phb->controller_ops.setup_msi_irqs = rtas_setup_msi_irqs;
		phb->controller_ops.teardown_msi_irqs = rtas_teardown_msi_irqs;
	}

	WARN_ON(ppc_md.pci_irq_fixup);
	ppc_md.pci_irq_fixup = rtas_msi_pci_irq_fixup;

	return 0;
}
machine_arch_initcall(pseries, rtas_msi_init);
