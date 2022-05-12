// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 */

#include <linux/acpi.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_graph.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/amba/bus.h>
#include <linux/coresight.h>
#include <linux/cpumask.h>
#include <asm/smp_plat.h>

#include "coresight-priv.h"
/*
 * coresight_alloc_conns: Allocate connections record for each output
 * port from the device.
 */
static int coresight_alloc_conns(struct device *dev,
				 struct coresight_platform_data *pdata)
{
	if (pdata->nr_outport) {
		pdata->conns = devm_kcalloc(dev, pdata->nr_outport,
					    sizeof(*pdata->conns), GFP_KERNEL);
		if (!pdata->conns)
			return -ENOMEM;
	}

	return 0;
}

static struct device *
coresight_find_device_by_fwnode(struct fwnode_handle *fwnode)
{
	struct device *dev = NULL;

	/*
	 * If we have a non-configurable replicator, it will be found on the
	 * platform bus.
	 */
	dev = bus_find_device_by_fwnode(&platform_bus_type, fwnode);
	if (dev)
		return dev;

	/*
	 * We have a configurable component - circle through the AMBA bus
	 * looking for the device that matches the endpoint node.
	 */
	return bus_find_device_by_fwnode(&amba_bustype, fwnode);
}

/*
 * Find a registered coresight device from a device fwnode.
 * The node info is associated with the AMBA parent, but the
 * csdev keeps a copy so iterate round the coresight bus to
 * find the device.
 */
struct coresight_device *
coresight_find_csdev_by_fwnode(struct fwnode_handle *r_fwnode)
{
	struct device *dev;
	struct coresight_device *csdev = NULL;

	dev = bus_find_device_by_fwnode(&coresight_bustype, r_fwnode);
	if (dev) {
		csdev = to_coresight_device(dev);
		put_device(dev);
	}
	return csdev;
}
EXPORT_SYMBOL_GPL(coresight_find_csdev_by_fwnode);

#ifdef CONFIG_OF
static inline bool of_coresight_legacy_ep_is_input(struct device_node *ep)
{
	return of_property_read_bool(ep, "slave-mode");
}

static void of_coresight_get_ports_legacy(const struct device_node *node,
					  int *nr_inport, int *nr_outport)
{
	struct device_node *ep = NULL;
	struct of_endpoint endpoint;
	int in = 0, out = 0;

	/*
	 * Avoid warnings in of_graph_get_next_endpoint()
	 * if the device doesn't have any graph connections
	 */
	if (!of_graph_is_present(node))
		return;
	do {
		ep = of_graph_get_next_endpoint(node, ep);
		if (!ep)
			break;

		if (of_graph_parse_endpoint(ep, &endpoint))
			continue;

		if (of_coresight_legacy_ep_is_input(ep)) {
			in = (endpoint.port + 1 > in) ?
				endpoint.port + 1 : in;
		} else {
			out = (endpoint.port + 1) > out ?
				endpoint.port + 1 : out;
		}

	} while (ep);

	*nr_inport = in;
	*nr_outport = out;
}

static struct device_node *of_coresight_get_port_parent(struct device_node *ep)
{
	struct device_node *parent = of_graph_get_port_parent(ep);

	/*
	 * Skip one-level up to the real device node, if we
	 * are using the new bindings.
	 */
	if (of_node_name_eq(parent, "in-ports") ||
	    of_node_name_eq(parent, "out-ports"))
		parent = of_get_next_parent(parent);

	return parent;
}

static inline struct device_node *
of_coresight_get_input_ports_node(const struct device_node *node)
{
	return of_get_child_by_name(node, "in-ports");
}

static inline struct device_node *
of_coresight_get_output_ports_node(const struct device_node *node)
{
	return of_get_child_by_name(node, "out-ports");
}

static inline int
of_coresight_count_ports(struct device_node *port_parent)
{
	int i = 0;
	struct device_node *ep = NULL;
	struct of_endpoint endpoint;

	while ((ep = of_graph_get_next_endpoint(port_parent, ep))) {
		/* Defer error handling to parsing */
		if (of_graph_parse_endpoint(ep, &endpoint))
			continue;
		if (endpoint.port + 1 > i)
			i = endpoint.port + 1;
	}

	return i;
}

static void of_coresight_get_ports(const struct device_node *node,
				   int *nr_inport, int *nr_outport)
{
	struct device_node *input_ports = NULL, *output_ports = NULL;

	input_ports = of_coresight_get_input_ports_node(node);
	output_ports = of_coresight_get_output_ports_node(node);

	if (input_ports || output_ports) {
		if (input_ports) {
			*nr_inport = of_coresight_count_ports(input_ports);
			of_node_put(input_ports);
		}
		if (output_ports) {
			*nr_outport = of_coresight_count_ports(output_ports);
			of_node_put(output_ports);
		}
	} else {
		/* Fall back to legacy DT bindings parsing */
		of_coresight_get_ports_legacy(node, nr_inport, nr_outport);
	}
}

static int of_coresight_get_cpu(struct device *dev)
{
	int cpu;
	struct device_node *dn;

	if (!dev->of_node)
		return -ENODEV;

	dn = of_parse_phandle(dev->of_node, "cpu", 0);
	if (!dn)
		return -ENODEV;

	cpu = of_cpu_node_to_id(dn);
	of_node_put(dn);

	return cpu;
}

/*
 * of_coresight_parse_endpoint : Parse the given output endpoint @ep
 * and fill the connection information in @conn
 *
 * Parses the local port, remote device name and the remote port.
 *
 * Returns :
 *	 0	- If the parsing completed without any fatal errors.
 *	-Errno	- Fatal error, abort the scanning.
 */
static int of_coresight_parse_endpoint(struct device *dev,
				       struct device_node *ep,
				       struct coresight_platform_data *pdata)
{
	int ret = 0;
	struct of_endpoint endpoint, rendpoint;
	struct device_node *rparent = NULL;
	struct device_node *rep = NULL;
	struct device *rdev = NULL;
	struct fwnode_handle *rdev_fwnode;
	struct coresight_connection *conn;

	do {
		/* Parse the local port details */
		if (of_graph_parse_endpoint(ep, &endpoint))
			break;
		/*
		 * Get a handle on the remote endpoint and the device it is
		 * attached to.
		 */
		rep = of_graph_get_remote_endpoint(ep);
		if (!rep)
			break;
		rparent = of_coresight_get_port_parent(rep);
		if (!rparent)
			break;
		if (of_graph_parse_endpoint(rep, &rendpoint))
			break;

		rdev_fwnode = of_fwnode_handle(rparent);
		/* If the remote device is not available, defer probing */
		rdev = coresight_find_device_by_fwnode(rdev_fwnode);
		if (!rdev) {
			ret = -EPROBE_DEFER;
			break;
		}

		conn = &pdata->conns[endpoint.port];
		if (conn->child_fwnode) {
			dev_warn(dev, "Duplicate output port %d\n",
				 endpoint.port);
			ret = -EINVAL;
			break;
		}
		conn->outport = endpoint.port;
		/*
		 * Hold the refcount to the target device. This could be
		 * released via:
		 * 1) coresight_release_platform_data() if the probe fails or
		 *    this device is unregistered.
		 * 2) While removing the target device via
		 *    coresight_remove_match()
		 */
		conn->child_fwnode = fwnode_handle_get(rdev_fwnode);
		conn->child_port = rendpoint.port;
		/* Connection record updated */
	} while (0);

	of_node_put(rparent);
	of_node_put(rep);
	put_device(rdev);

	return ret;
}

static int of_get_coresight_platform_data(struct device *dev,
					  struct coresight_platform_data *pdata)
{
	int ret = 0;
	struct device_node *ep = NULL;
	const struct device_node *parent = NULL;
	bool legacy_binding = false;
	struct device_node *node = dev->of_node;

	/* Get the number of input and output port for this component */
	of_coresight_get_ports(node, &pdata->nr_inport, &pdata->nr_outport);

	/* If there are no output connections, we are done */
	if (!pdata->nr_outport)
		return 0;

	ret = coresight_alloc_conns(dev, pdata);
	if (ret)
		return ret;

	parent = of_coresight_get_output_ports_node(node);
	/*
	 * If the DT uses obsoleted bindings, the ports are listed
	 * under the device and we need to filter out the input
	 * ports.
	 */
	if (!parent) {
		legacy_binding = true;
		parent = node;
		dev_warn_once(dev, "Uses obsolete Coresight DT bindings\n");
	}

	/* Iterate through each output port to discover topology */
	while ((ep = of_graph_get_next_endpoint(parent, ep))) {
		/*
		 * Legacy binding mixes input/output ports under the
		 * same parent. So, skip the input ports if we are dealing
		 * with legacy binding, as they processed with their
		 * connected output ports.
		 */
		if (legacy_binding && of_coresight_legacy_ep_is_input(ep))
			continue;

		ret = of_coresight_parse_endpoint(dev, ep, pdata);
		if (ret)
			return ret;
	}

	return 0;
}
#else
static inline int
of_get_coresight_platform_data(struct device *dev,
			       struct coresight_platform_data *pdata)
{
	return -ENOENT;
}

static inline int of_coresight_get_cpu(struct device *dev)
{
	return -ENODEV;
}
#endif

#ifdef CONFIG_ACPI

#include <acpi/actypes.h>
#include <acpi/processor.h>

/* ACPI Graph _DSD UUID : "ab02a46b-74c7-45a2-bd68-f7d344ef2153" */
static const guid_t acpi_graph_uuid = GUID_INIT(0xab02a46b, 0x74c7, 0x45a2,
						0xbd, 0x68, 0xf7, 0xd3,
						0x44, 0xef, 0x21, 0x53);
/* Coresight ACPI Graph UUID : "3ecbc8b6-1d0e-4fb3-8107-e627f805c6cd" */
static const guid_t coresight_graph_uuid = GUID_INIT(0x3ecbc8b6, 0x1d0e, 0x4fb3,
						     0x81, 0x07, 0xe6, 0x27,
						     0xf8, 0x05, 0xc6, 0xcd);
#define ACPI_CORESIGHT_LINK_SLAVE	0
#define ACPI_CORESIGHT_LINK_MASTER	1

static inline bool is_acpi_guid(const union acpi_object *obj)
{
	return (obj->type == ACPI_TYPE_BUFFER) && (obj->buffer.length == 16);
}

/*
 * acpi_guid_matches	- Checks if the given object is a GUID object and
 * that it matches the supplied the GUID.
 */
static inline bool acpi_guid_matches(const union acpi_object *obj,
				   const guid_t *guid)
{
	return is_acpi_guid(obj) &&
	       guid_equal((guid_t *)obj->buffer.pointer, guid);
}

static inline bool is_acpi_dsd_graph_guid(const union acpi_object *obj)
{
	return acpi_guid_matches(obj, &acpi_graph_uuid);
}

static inline bool is_acpi_coresight_graph_guid(const union acpi_object *obj)
{
	return acpi_guid_matches(obj, &coresight_graph_uuid);
}

static inline bool is_acpi_coresight_graph(const union acpi_object *obj)
{
	const union acpi_object *graphid, *guid, *links;

	if (obj->type != ACPI_TYPE_PACKAGE ||
	    obj->package.count < 3)
		return false;

	graphid = &obj->package.elements[0];
	guid = &obj->package.elements[1];
	links = &obj->package.elements[2];

	if (graphid->type != ACPI_TYPE_INTEGER ||
	    links->type != ACPI_TYPE_INTEGER)
		return false;

	return is_acpi_coresight_graph_guid(guid);
}

/*
 * acpi_validate_dsd_graph	- Make sure the given _DSD graph conforms
 * to the ACPI _DSD Graph specification.
 *
 * ACPI Devices Graph property has the following format:
 *  {
 *	Revision	- Integer, must be 0
 *	NumberOfGraphs	- Integer, N indicating the following list.
 *	Graph[1],
 *	 ...
 *	Graph[N]
 *  }
 *
 * And each Graph entry has the following format:
 *  {
 *	GraphID		- Integer, identifying a graph the device belongs to.
 *	UUID		- UUID identifying the specification that governs
 *			  this graph. (e.g, see is_acpi_coresight_graph())
 *	NumberOfLinks	- Number "N" of connections on this node of the graph.
 *	Links[1]
 *	...
 *	Links[N]
 *  }
 *
 * Where each "Links" entry has the following format:
 *
 * {
 *	SourcePortAddress	- Integer
 *	DestinationPortAddress	- Integer
 *	DestinationDeviceName	- Reference to another device
 *	( --- CoreSight specific extensions below ---)
 *	DirectionOfFlow		- Integer 1 for output(master)
 *				  0 for input(slave)
 * }
 *
 * e.g:
 * For a Funnel device
 *
 * Device(MFUN) {
 *   ...
 *
 *   Name (_DSD, Package() {
 *	// DSD Package contains tuples of {  Proeprty_Type_UUID, Package() }
 *	ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"), //Std. Property UUID
 *	Package() {
 *		Package(2) { "property-name", <property-value> }
 *	},
 *
 *	ToUUID("ab02a46b-74c7-45a2-bd68-f7d344ef2153"), // ACPI Graph UUID
 *	Package() {
 *	  0,		// Revision
 *	  1,		// NumberOfGraphs.
 *	  Package() {	// Graph[0] Package
 *	     1,		// GraphID
 *	     // Coresight Graph UUID
 *	     ToUUID("3ecbc8b6-1d0e-4fb3-8107-e627f805c6cd"),
 *	     3,		// NumberOfLinks aka ports
 *	     // Link[0]: Output_0 -> Replicator:Input_0
 *	     Package () { 0, 0, \_SB_.RPL0, 1 },
 *	     // Link[1]: Input_0 <- Cluster0_Funnel0:Output_0
 *	     Package () { 0, 0, \_SB_.CLU0.FUN0, 0 },
 *	     // Link[2]: Input_1 <- Cluster1_Funnel0:Output_0
 *	      Package () { 1, 0, \_SB_.CLU1.FUN0, 0 },
 *	  }	// End of Graph[0] Package
 *
 *	}, // End of ACPI Graph Property
 *  })
 */
static inline bool acpi_validate_dsd_graph(const union acpi_object *graph)
{
	int i, n;
	const union acpi_object *rev, *nr_graphs;

	/* The graph must contain at least the Revision and Number of Graphs */
	if (graph->package.count < 2)
		return false;

	rev = &graph->package.elements[0];
	nr_graphs = &graph->package.elements[1];

	if (rev->type != ACPI_TYPE_INTEGER ||
	    nr_graphs->type != ACPI_TYPE_INTEGER)
		return false;

	/* We only support revision 0 */
	if (rev->integer.value != 0)
		return false;

	n = nr_graphs->integer.value;
	/* CoreSight devices are only part of a single Graph */
	if (n != 1)
		return false;

	/* Make sure the ACPI graph package has right number of elements */
	if (graph->package.count != (n + 2))
		return false;

	/*
	 * Each entry must be a graph package with at least 3 members :
	 * { GraphID, UUID, NumberOfLinks(n), Links[.],... }
	 */
	for (i = 2; i < n + 2; i++) {
		const union acpi_object *obj = &graph->package.elements[i];

		if (obj->type != ACPI_TYPE_PACKAGE ||
		    obj->package.count < 3)
			return false;
	}

	return true;
}

/* acpi_get_dsd_graph	- Find the _DSD Graph property for the given device. */
static const union acpi_object *
acpi_get_dsd_graph(struct acpi_device *adev)
{
	int i;
	struct acpi_buffer buf = { ACPI_ALLOCATE_BUFFER };
	acpi_status status;
	const union acpi_object *dsd;

	status = acpi_evaluate_object_typed(adev->handle, "_DSD", NULL,
					    &buf, ACPI_TYPE_PACKAGE);
	if (ACPI_FAILURE(status))
		return NULL;

	dsd = buf.pointer;

	/*
	 * _DSD property consists tuples { Prop_UUID, Package() }
	 * Iterate through all the packages and find the Graph.
	 */
	for (i = 0; i + 1 < dsd->package.count; i += 2) {
		const union acpi_object *guid, *package;

		guid = &dsd->package.elements[i];
		package = &dsd->package.elements[i + 1];

		/* All _DSD elements must have a UUID and a Package */
		if (!is_acpi_guid(guid) || package->type != ACPI_TYPE_PACKAGE)
			break;
		/* Skip the non-Graph _DSD packages */
		if (!is_acpi_dsd_graph_guid(guid))
			continue;
		if (acpi_validate_dsd_graph(package))
			return package;
		/* Invalid graph format, continue */
		dev_warn(&adev->dev, "Invalid Graph _DSD property\n");
	}

	return NULL;
}

static inline bool
acpi_validate_coresight_graph(const union acpi_object *cs_graph)
{
	int nlinks;

	nlinks = cs_graph->package.elements[2].integer.value;
	/*
	 * Graph must have the following fields :
	 * { GraphID, GraphUUID, NumberOfLinks, Links... }
	 */
	if (cs_graph->package.count != (nlinks + 3))
		return false;
	/* The links are validated in acpi_coresight_parse_link() */
	return true;
}

/*
 * acpi_get_coresight_graph	- Parse the device _DSD tables and find
 * the Graph property matching the CoreSight Graphs.
 *
 * Returns the pointer to the CoreSight Graph Package when found. Otherwise
 * returns NULL.
 */
static const union acpi_object *
acpi_get_coresight_graph(struct acpi_device *adev)
{
	const union acpi_object *graph_list, *graph;
	int i, nr_graphs;

	graph_list = acpi_get_dsd_graph(adev);
	if (!graph_list)
		return graph_list;

	nr_graphs = graph_list->package.elements[1].integer.value;

	for (i = 2; i < nr_graphs + 2; i++) {
		graph = &graph_list->package.elements[i];
		if (!is_acpi_coresight_graph(graph))
			continue;
		if (acpi_validate_coresight_graph(graph))
			return graph;
		/* Invalid graph format */
		break;
	}

	return NULL;
}

/*
 * acpi_coresight_parse_link	- Parse the given Graph connection
 * of the device and populate the coresight_connection for an output
 * connection.
 *
 * CoreSight Graph specification mandates that the direction of the data
 * flow must be specified in the link. i.e,
 *
 *	SourcePortAddress,	// Integer
 *	DestinationPortAddress,	// Integer
 *	DestinationDeviceName,	// Reference to another device
 *	DirectionOfFlow,	// 1 for output(master), 0 for input(slave)
 *
 * Returns the direction of the data flow [ Input(slave) or Output(master) ]
 * upon success.
 * Returns an negative error number otherwise.
 */
static int acpi_coresight_parse_link(struct acpi_device *adev,
				     const union acpi_object *link,
				     struct coresight_connection *conn)
{
	int rc, dir;
	const union acpi_object *fields;
	struct acpi_device *r_adev;
	struct device *rdev;

	if (link->type != ACPI_TYPE_PACKAGE ||
	    link->package.count != 4)
		return -EINVAL;

	fields = link->package.elements;

	if (fields[0].type != ACPI_TYPE_INTEGER ||
	    fields[1].type != ACPI_TYPE_INTEGER ||
	    fields[2].type != ACPI_TYPE_LOCAL_REFERENCE ||
	    fields[3].type != ACPI_TYPE_INTEGER)
		return -EINVAL;

	rc = acpi_bus_get_device(fields[2].reference.handle, &r_adev);
	if (rc)
		return rc;

	dir = fields[3].integer.value;
	if (dir == ACPI_CORESIGHT_LINK_MASTER) {
		conn->outport = fields[0].integer.value;
		conn->child_port = fields[1].integer.value;
		rdev = coresight_find_device_by_fwnode(&r_adev->fwnode);
		if (!rdev)
			return -EPROBE_DEFER;
		/*
		 * Hold the refcount to the target device. This could be
		 * released via:
		 * 1) coresight_release_platform_data() if the probe fails or
		 *    this device is unregistered.
		 * 2) While removing the target device via
		 *    coresight_remove_match().
		 */
		conn->child_fwnode = fwnode_handle_get(&r_adev->fwnode);
	} else if (dir == ACPI_CORESIGHT_LINK_SLAVE) {
		/*
		 * We are only interested in the port number
		 * for the input ports at this component.
		 * Store the port number in child_port.
		 */
		conn->child_port = fields[0].integer.value;
	} else {
		/* Invalid direction */
		return -EINVAL;
	}

	return dir;
}

/*
 * acpi_coresight_parse_graph	- Parse the _DSD CoreSight graph
 * connection information and populate the supplied coresight_platform_data
 * instance.
 */
static int acpi_coresight_parse_graph(struct acpi_device *adev,
				      struct coresight_platform_data *pdata)
{
	int rc, i, nlinks;
	const union acpi_object *graph;
	struct coresight_connection *conns, *ptr;

	pdata->nr_inport = pdata->nr_outport = 0;
	graph = acpi_get_coresight_graph(adev);
	if (!graph)
		return -ENOENT;

	nlinks = graph->package.elements[2].integer.value;
	if (!nlinks)
		return 0;

	/*
	 * To avoid scanning the table twice (once for finding the number of
	 * output links and then later for parsing the output links),
	 * cache the links information in one go and then later copy
	 * it to the pdata.
	 */
	conns = devm_kcalloc(&adev->dev, nlinks, sizeof(*conns), GFP_KERNEL);
	if (!conns)
		return -ENOMEM;
	ptr = conns;
	for (i = 0; i < nlinks; i++) {
		const union acpi_object *link = &graph->package.elements[3 + i];
		int dir;

		dir = acpi_coresight_parse_link(adev, link, ptr);
		if (dir < 0)
			return dir;

		if (dir == ACPI_CORESIGHT_LINK_MASTER) {
			if (ptr->outport >= pdata->nr_outport)
				pdata->nr_outport = ptr->outport + 1;
			ptr++;
		} else {
			WARN_ON(pdata->nr_inport == ptr->child_port + 1);
			/*
			 * We do not track input port connections for a device.
			 * However we need the highest port number described,
			 * which can be recorded now and reuse this connection
			 * record for an output connection. Hence, do not move
			 * the ptr for input connections
			 */
			if (ptr->child_port >= pdata->nr_inport)
				pdata->nr_inport = ptr->child_port + 1;
		}
	}

	rc = coresight_alloc_conns(&adev->dev, pdata);
	if (rc)
		return rc;

	/* Copy the connection information to the final location */
	for (i = 0; conns + i < ptr; i++) {
		int port = conns[i].outport;

		/* Duplicate output port */
		WARN_ON(pdata->conns[port].child_fwnode);
		pdata->conns[port] = conns[i];
	}

	devm_kfree(&adev->dev, conns);
	return 0;
}

/*
 * acpi_handle_to_logical_cpuid - Map a given acpi_handle to the
 * logical CPU id of the corresponding CPU device.
 *
 * Returns the logical CPU id when found. Otherwise returns >= nr_cpus_id.
 */
static int
acpi_handle_to_logical_cpuid(acpi_handle handle)
{
	int i;
	struct acpi_processor *pr;

	for_each_possible_cpu(i) {
		pr = per_cpu(processors, i);
		if (pr && pr->handle == handle)
			break;
	}

	return i;
}

/*
 * acpi_coresigh_get_cpu - Find the logical CPU id of the CPU associated
 * with this coresight device. With ACPI bindings, the CoreSight components
 * are listed as child device of the associated CPU.
 *
 * Returns the logical CPU id when found. Otherwise returns 0.
 */
static int acpi_coresight_get_cpu(struct device *dev)
{
	int cpu;
	acpi_handle cpu_handle;
	acpi_status status;
	struct acpi_device *adev = ACPI_COMPANION(dev);

	if (!adev)
		return -ENODEV;
	status = acpi_get_parent(adev->handle, &cpu_handle);
	if (ACPI_FAILURE(status))
		return -ENODEV;

	cpu = acpi_handle_to_logical_cpuid(cpu_handle);
	if (cpu >= nr_cpu_ids)
		return -ENODEV;
	return cpu;
}

static int
acpi_get_coresight_platform_data(struct device *dev,
				 struct coresight_platform_data *pdata)
{
	struct acpi_device *adev;

	adev = ACPI_COMPANION(dev);
	if (!adev)
		return -EINVAL;

	return acpi_coresight_parse_graph(adev, pdata);
}

#else

static inline int
acpi_get_coresight_platform_data(struct device *dev,
				 struct coresight_platform_data *pdata)
{
	return -ENOENT;
}

static inline int acpi_coresight_get_cpu(struct device *dev)
{
	return -ENODEV;
}
#endif

int coresight_get_cpu(struct device *dev)
{
	if (is_of_node(dev->fwnode))
		return of_coresight_get_cpu(dev);
	else if (is_acpi_device_node(dev->fwnode))
		return acpi_coresight_get_cpu(dev);
	return 0;
}
EXPORT_SYMBOL_GPL(coresight_get_cpu);

struct coresight_platform_data *
coresight_get_platform_data(struct device *dev)
{
	int ret = -ENOENT;
	struct coresight_platform_data *pdata = NULL;
	struct fwnode_handle *fwnode = dev_fwnode(dev);

	if (IS_ERR_OR_NULL(fwnode))
		goto error;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		ret = -ENOMEM;
		goto error;
	}

	if (is_of_node(fwnode))
		ret = of_get_coresight_platform_data(dev, pdata);
	else if (is_acpi_device_node(fwnode))
		ret = acpi_get_coresight_platform_data(dev, pdata);

	if (!ret)
		return pdata;
error:
	if (!IS_ERR_OR_NULL(pdata))
		/* Cleanup the connection information */
		coresight_release_platform_data(NULL, pdata);
	return ERR_PTR(ret);
}
EXPORT_SYMBOL_GPL(coresight_get_platform_data);
