// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com
 *  Author: Peter Ujfalusi <peter.ujfalusi@ti.com>
 */
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>

#define TI_XBAR_DRA7		0
#define TI_XBAR_AM335X		1
static const u32 ti_xbar_type[] = {
	[TI_XBAR_DRA7] = TI_XBAR_DRA7,
	[TI_XBAR_AM335X] = TI_XBAR_AM335X,
};

static const struct of_device_id ti_dma_xbar_match[] = {
	{
		.compatible = "ti,dra7-dma-crossbar",
		.data = &ti_xbar_type[TI_XBAR_DRA7],
	},
	{
		.compatible = "ti,am335x-edma-crossbar",
		.data = &ti_xbar_type[TI_XBAR_AM335X],
	},
	{},
};

/* Crossbar on AM335x/AM437x family */
#define TI_AM335X_XBAR_LINES	64

struct ti_am335x_xbar_data {
	void __iomem *iomem;

	struct dma_router dmarouter;

	u32 xbar_events; /* maximum number of events to select in xbar */
	u32 dma_requests; /* number of DMA requests on eDMA */
};

struct ti_am335x_xbar_map {
	u16 dma_line;
	u8 mux_val;
};

static inline void ti_am335x_xbar_write(void __iomem *iomem, int event, u8 val)
{
	/*
	 * TPCC_EVT_MUX_60_63 register layout is different than the
	 * rest, in the sense, that event 63 is mapped to lowest byte
	 * and event 60 is mapped to highest, handle it separately.
	 */
	if (event >= 60 && event <= 63)
		writeb_relaxed(val, iomem + (63 - event % 4));
	else
		writeb_relaxed(val, iomem + event);
}

static void ti_am335x_xbar_free(struct device *dev, void *route_data)
{
	struct ti_am335x_xbar_data *xbar = dev_get_drvdata(dev);
	struct ti_am335x_xbar_map *map = route_data;

	dev_dbg(dev, "Unmapping XBAR event %u on channel %u\n",
		map->mux_val, map->dma_line);

	ti_am335x_xbar_write(xbar->iomem, map->dma_line, 0);
	kfree(map);
}

static void *ti_am335x_xbar_route_allocate(struct of_phandle_args *dma_spec,
					   struct of_dma *ofdma)
{
	struct platform_device *pdev = of_find_device_by_node(ofdma->of_node);
	struct ti_am335x_xbar_data *xbar = platform_get_drvdata(pdev);
	struct ti_am335x_xbar_map *map;

	if (dma_spec->args_count != 3)
		return ERR_PTR(-EINVAL);

	if (dma_spec->args[2] >= xbar->xbar_events) {
		dev_err(&pdev->dev, "Invalid XBAR event number: %d\n",
			dma_spec->args[2]);
		return ERR_PTR(-EINVAL);
	}

	if (dma_spec->args[0] >= xbar->dma_requests) {
		dev_err(&pdev->dev, "Invalid DMA request line number: %d\n",
			dma_spec->args[0]);
		return ERR_PTR(-EINVAL);
	}

	/* The of_node_put() will be done in the core for the node */
	dma_spec->np = of_parse_phandle(ofdma->of_node, "dma-masters", 0);
	if (!dma_spec->np) {
		dev_err(&pdev->dev, "Can't get DMA master\n");
		return ERR_PTR(-EINVAL);
	}

	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (!map) {
		of_node_put(dma_spec->np);
		return ERR_PTR(-ENOMEM);
	}

	map->dma_line = (u16)dma_spec->args[0];
	map->mux_val = (u8)dma_spec->args[2];

	dma_spec->args[2] = 0;
	dma_spec->args_count = 2;

	dev_dbg(&pdev->dev, "Mapping XBAR event%u to DMA%u\n",
		map->mux_val, map->dma_line);

	ti_am335x_xbar_write(xbar->iomem, map->dma_line, map->mux_val);

	return map;
}

static const struct of_device_id ti_am335x_master_match[] __maybe_unused = {
	{ .compatible = "ti,edma3-tpcc", },
	{},
};

static int ti_am335x_xbar_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	const struct of_device_id *match;
	struct device_node *dma_node;
	struct ti_am335x_xbar_data *xbar;
	void __iomem *iomem;
	int i, ret;

	if (!node)
		return -ENODEV;

	xbar = devm_kzalloc(&pdev->dev, sizeof(*xbar), GFP_KERNEL);
	if (!xbar)
		return -ENOMEM;

	dma_node = of_parse_phandle(node, "dma-masters", 0);
	if (!dma_node) {
		dev_err(&pdev->dev, "Can't get DMA master node\n");
		return -ENODEV;
	}

	match = of_match_node(ti_am335x_master_match, dma_node);
	if (!match) {
		dev_err(&pdev->dev, "DMA master is not supported\n");
		of_node_put(dma_node);
		return -EINVAL;
	}

	if (of_property_read_u32(dma_node, "dma-requests",
				 &xbar->dma_requests)) {
		dev_info(&pdev->dev,
			 "Missing XBAR output information, using %u.\n",
			 TI_AM335X_XBAR_LINES);
		xbar->dma_requests = TI_AM335X_XBAR_LINES;
	}
	of_node_put(dma_node);

	if (of_property_read_u32(node, "dma-requests", &xbar->xbar_events)) {
		dev_info(&pdev->dev,
			 "Missing XBAR input information, using %u.\n",
			 TI_AM335X_XBAR_LINES);
		xbar->xbar_events = TI_AM335X_XBAR_LINES;
	}

	iomem = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(iomem))
		return PTR_ERR(iomem);

	xbar->iomem = iomem;

	xbar->dmarouter.dev = &pdev->dev;
	xbar->dmarouter.route_free = ti_am335x_xbar_free;

	platform_set_drvdata(pdev, xbar);

	/* Reset the crossbar */
	for (i = 0; i < xbar->dma_requests; i++)
		ti_am335x_xbar_write(xbar->iomem, i, 0);

	ret = of_dma_router_register(node, ti_am335x_xbar_route_allocate,
				     &xbar->dmarouter);

	return ret;
}

/* Crossbar on DRA7xx family */
#define TI_DRA7_XBAR_OUTPUTS	127
#define TI_DRA7_XBAR_INPUTS	256

struct ti_dra7_xbar_data {
	void __iomem *iomem;

	struct dma_router dmarouter;
	struct mutex mutex;
	unsigned long *dma_inuse;

	u16 safe_val; /* Value to rest the crossbar lines */
	u32 xbar_requests; /* number of DMA requests connected to XBAR */
	u32 dma_requests; /* number of DMA requests forwarded to DMA */
	u32 dma_offset;
};

struct ti_dra7_xbar_map {
	u16 xbar_in;
	int xbar_out;
};

static inline void ti_dra7_xbar_write(void __iomem *iomem, int xbar, u16 val)
{
	writew_relaxed(val, iomem + (xbar * 2));
}

static void ti_dra7_xbar_free(struct device *dev, void *route_data)
{
	struct ti_dra7_xbar_data *xbar = dev_get_drvdata(dev);
	struct ti_dra7_xbar_map *map = route_data;

	dev_dbg(dev, "Unmapping XBAR%u (was routed to %d)\n",
		map->xbar_in, map->xbar_out);

	ti_dra7_xbar_write(xbar->iomem, map->xbar_out, xbar->safe_val);
	mutex_lock(&xbar->mutex);
	clear_bit(map->xbar_out, xbar->dma_inuse);
	mutex_unlock(&xbar->mutex);
	kfree(map);
}

static void *ti_dra7_xbar_route_allocate(struct of_phandle_args *dma_spec,
					 struct of_dma *ofdma)
{
	struct platform_device *pdev = of_find_device_by_node(ofdma->of_node);
	struct ti_dra7_xbar_data *xbar = platform_get_drvdata(pdev);
	struct ti_dra7_xbar_map *map;

	if (dma_spec->args[0] >= xbar->xbar_requests) {
		dev_err(&pdev->dev, "Invalid XBAR request number: %d\n",
			dma_spec->args[0]);
		return ERR_PTR(-EINVAL);
	}

	/* The of_node_put() will be done in the core for the node */
	dma_spec->np = of_parse_phandle(ofdma->of_node, "dma-masters", 0);
	if (!dma_spec->np) {
		dev_err(&pdev->dev, "Can't get DMA master\n");
		return ERR_PTR(-EINVAL);
	}

	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (!map) {
		of_node_put(dma_spec->np);
		return ERR_PTR(-ENOMEM);
	}

	mutex_lock(&xbar->mutex);
	map->xbar_out = find_first_zero_bit(xbar->dma_inuse,
					    xbar->dma_requests);
	if (map->xbar_out == xbar->dma_requests) {
		mutex_unlock(&xbar->mutex);
		dev_err(&pdev->dev, "Run out of free DMA requests\n");
		kfree(map);
		return ERR_PTR(-ENOMEM);
	}
	set_bit(map->xbar_out, xbar->dma_inuse);
	mutex_unlock(&xbar->mutex);

	map->xbar_in = (u16)dma_spec->args[0];

	dma_spec->args[0] = map->xbar_out + xbar->dma_offset;

	dev_dbg(&pdev->dev, "Mapping XBAR%u to DMA%d\n",
		map->xbar_in, map->xbar_out);

	ti_dra7_xbar_write(xbar->iomem, map->xbar_out, map->xbar_in);

	return map;
}

#define TI_XBAR_EDMA_OFFSET	0
#define TI_XBAR_SDMA_OFFSET	1
static const u32 ti_dma_offset[] = {
	[TI_XBAR_EDMA_OFFSET] = 0,
	[TI_XBAR_SDMA_OFFSET] = 1,
};

static const struct of_device_id ti_dra7_master_match[] __maybe_unused = {
	{
		.compatible = "ti,omap4430-sdma",
		.data = &ti_dma_offset[TI_XBAR_SDMA_OFFSET],
	},
	{
		.compatible = "ti,edma3",
		.data = &ti_dma_offset[TI_XBAR_EDMA_OFFSET],
	},
	{
		.compatible = "ti,edma3-tpcc",
		.data = &ti_dma_offset[TI_XBAR_EDMA_OFFSET],
	},
	{},
};

static inline void ti_dra7_xbar_reserve(int offset, int len, unsigned long *p)
{
	for (; len > 0; len--)
		set_bit(offset + (len - 1), p);
}

static int ti_dra7_xbar_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	const struct of_device_id *match;
	struct device_node *dma_node;
	struct ti_dra7_xbar_data *xbar;
	struct property *prop;
	u32 safe_val;
	int sz;
	void __iomem *iomem;
	int i, ret;

	if (!node)
		return -ENODEV;

	xbar = devm_kzalloc(&pdev->dev, sizeof(*xbar), GFP_KERNEL);
	if (!xbar)
		return -ENOMEM;

	dma_node = of_parse_phandle(node, "dma-masters", 0);
	if (!dma_node) {
		dev_err(&pdev->dev, "Can't get DMA master node\n");
		return -ENODEV;
	}

	match = of_match_node(ti_dra7_master_match, dma_node);
	if (!match) {
		dev_err(&pdev->dev, "DMA master is not supported\n");
		of_node_put(dma_node);
		return -EINVAL;
	}

	if (of_property_read_u32(dma_node, "dma-requests",
				 &xbar->dma_requests)) {
		dev_info(&pdev->dev,
			 "Missing XBAR output information, using %u.\n",
			 TI_DRA7_XBAR_OUTPUTS);
		xbar->dma_requests = TI_DRA7_XBAR_OUTPUTS;
	}
	of_node_put(dma_node);

	xbar->dma_inuse = devm_kcalloc(&pdev->dev,
				       BITS_TO_LONGS(xbar->dma_requests),
				       sizeof(unsigned long), GFP_KERNEL);
	if (!xbar->dma_inuse)
		return -ENOMEM;

	if (of_property_read_u32(node, "dma-requests", &xbar->xbar_requests)) {
		dev_info(&pdev->dev,
			 "Missing XBAR input information, using %u.\n",
			 TI_DRA7_XBAR_INPUTS);
		xbar->xbar_requests = TI_DRA7_XBAR_INPUTS;
	}

	if (!of_property_read_u32(node, "ti,dma-safe-map", &safe_val))
		xbar->safe_val = (u16)safe_val;


	prop = of_find_property(node, "ti,reserved-dma-request-ranges", &sz);
	if (prop) {
		const char pname[] = "ti,reserved-dma-request-ranges";
		u32 (*rsv_events)[2];
		size_t nelm = sz / sizeof(*rsv_events);
		int i;

		if (!nelm)
			return -EINVAL;

		rsv_events = kcalloc(nelm, sizeof(*rsv_events), GFP_KERNEL);
		if (!rsv_events)
			return -ENOMEM;

		ret = of_property_read_u32_array(node, pname, (u32 *)rsv_events,
						 nelm * 2);
		if (ret) {
			kfree(rsv_events);
			return ret;
		}

		for (i = 0; i < nelm; i++) {
			ti_dra7_xbar_reserve(rsv_events[i][0], rsv_events[i][1],
					     xbar->dma_inuse);
		}
		kfree(rsv_events);
	}

	iomem = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(iomem))
		return PTR_ERR(iomem);

	xbar->iomem = iomem;

	xbar->dmarouter.dev = &pdev->dev;
	xbar->dmarouter.route_free = ti_dra7_xbar_free;
	xbar->dma_offset = *(u32 *)match->data;

	mutex_init(&xbar->mutex);
	platform_set_drvdata(pdev, xbar);

	/* Reset the crossbar */
	for (i = 0; i < xbar->dma_requests; i++) {
		if (!test_bit(i, xbar->dma_inuse))
			ti_dra7_xbar_write(xbar->iomem, i, xbar->safe_val);
	}

	ret = of_dma_router_register(node, ti_dra7_xbar_route_allocate,
				     &xbar->dmarouter);
	if (ret) {
		/* Restore the defaults for the crossbar */
		for (i = 0; i < xbar->dma_requests; i++) {
			if (!test_bit(i, xbar->dma_inuse))
				ti_dra7_xbar_write(xbar->iomem, i, i);
		}
	}

	return ret;
}

static int ti_dma_xbar_probe(struct platform_device *pdev)
{
	const struct of_device_id *match;
	int ret;

	match = of_match_node(ti_dma_xbar_match, pdev->dev.of_node);
	if (unlikely(!match))
		return -EINVAL;

	switch (*(u32 *)match->data) {
	case TI_XBAR_DRA7:
		ret = ti_dra7_xbar_probe(pdev);
		break;
	case TI_XBAR_AM335X:
		ret = ti_am335x_xbar_probe(pdev);
		break;
	default:
		dev_err(&pdev->dev, "Unsupported crossbar\n");
		ret = -ENODEV;
		break;
	}

	return ret;
}

static struct platform_driver ti_dma_xbar_driver = {
	.driver = {
		.name = "ti-dma-crossbar",
		.of_match_table = ti_dma_xbar_match,
	},
	.probe	= ti_dma_xbar_probe,
};

static int omap_dmaxbar_init(void)
{
	return platform_driver_register(&ti_dma_xbar_driver);
}
arch_initcall(omap_dmaxbar_init);
