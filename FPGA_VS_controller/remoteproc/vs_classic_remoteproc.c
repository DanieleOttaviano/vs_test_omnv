#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mailbox_client.h>
#include <linux/mailbox/zynqmp-ipi-message.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/remoteproc.h>
#include <linux/skbuff.h>
#include <linux/sysfs.h>
#include <linux/types.h>

#include "remoteproc_internal.h"

struct classic_vs_pl_conf_port {
    /*0x00*/ 
    u16 enabled; // 1
    /* Shared page ref */
    /*0x02*/ 
    u16 counter_offset;
    /*0x04*/ 
    u16 ivs3_offset;
    /*0x06*/ 
    u16 zdot_offset;
    /*0x08*/ 
    u16 marte_strategy_offset;
    /*0x0A*/ 
    u16 safe_control_output_offset;
    /*0x0C*/ 
    u32 res_0_0;
    /*0x10*/
    u64 shared_page_base;
    /* Constant values */
    /*0x18*/
    u64 Af_val;
    /*0x20*/
    u64 Bf_val;
    /*0x28*/
    u64 Cf_val;
    /*0x30*/
    u64 Df_val;
    /*0x38*/
    u64 gain_val;
};

struct vs_classic_rproc_data {
    const char *device_name;
};

struct vs_classic_rproc {
    struct device *dev;
    struct rproc *rproc;
};

static void *vs_classic_da_to_va(struct rproc *rproc, u64 da, size_t len, bool *is_iomem) {
    void __iomem *va = NULL;
	struct vs_classic_rproc *vscc_rproc = rproc->priv;

	dev_info(vscc_rproc->dev, "da 0x%llx len %zu, is_iomem %d\n", da, len, is_iomem ? *is_iomem : 0);
	va = ioremap_wc(da, len);
	return (__force void *)va;
}

static int vs_classic_rproc_start(struct rproc *rproc) {
	struct vs_classic_rproc *p_rproc = rproc->priv;
	unsigned long conf_port_base_addr = 0x80000000;	// take from device tree
	volatile unsigned long __iomem *conf_port_base;
	struct classic_vs_pl_conf_port *conf_port;

	dev_info(p_rproc->dev, "Starting VS Classic Controller remoteproc\n");
	// Map the physical address to virtual address space
	conf_port_base = ioremap(conf_port_base_addr, sizeof(struct classic_vs_pl_conf_port));
	if (!conf_port_base) {
		dev_err(p_rproc->dev, "Failed to map FPGA base address\n");
		return -ENOMEM;
	}

	conf_port = (struct classic_vs_pl_conf_port *)conf_port_base;

    /* Write computation constants */
    iowrite64(0x3feffb161616edad , &conf_port->Af_val);
    iowrite64(0x4000000000000000 , &conf_port->Bf_val);
    iowrite64(0xbffc8afa2f05a709 , &conf_port->Cf_val);
    iowrite64(0xc0906f1eb851eb85 , &conf_port->Df_val);
    iowrite64(0xbee4f8b588e368f1 , &conf_port->gain_val);

    /* Shared page data */
    iowrite64(0x46d00000, &conf_port->shared_page_base);

    iowrite16(0x1D0, &conf_port->counter_offset);
    iowrite16(0x0B8, &conf_port->ivs3_offset);
    iowrite16(0x0D0, &conf_port->zdot_offset);
    iowrite16(0x1D1, &conf_port->marte_strategy_offset);
    iowrite16(0x1C8, &conf_port->safe_control_output_offset);
    
    /* Finally, enable the module */
    iowrite8(1, &conf_port->enabled);

	// Unmap the address after use
	iounmap(conf_port_base);

	return 0;
}

static int vs_classic_rproc_stop(struct rproc *rproc) {
	struct vs_classic_rproc *p_rproc = rproc->priv;
	unsigned long conf_port_base_addr = 0x80000000;	// take from device tree
	volatile unsigned long __iomem *conf_port_base;
	struct classic_vs_pl_conf_port *conf_port; 

	// Map the physical address to virtual address space
	conf_port_base = ioremap(conf_port_base_addr, sizeof(struct classic_vs_pl_conf_port));
	if (!conf_port_base) {
		dev_err(p_rproc->dev, "Failed to map FPGA base address\n");
		return -ENOMEM;
	}

	conf_port = (struct classic_vs_pl_conf_port *)conf_port_base;

	// Disable the module
	iowrite8(0, &conf_port->enabled);

	// Unmap the address after use
	iounmap(conf_port_base);

	return 0;
}


static void vs_classic_rproc_kick(struct rproc *rproc, int vqid) {}

static int vs_classic_rproc_load(struct rproc *rproc, const struct firmware *fw){
	return 0;
}

static int vs_classic_rproc_parse_fw(struct rproc *rproc, const struct firmware *fw){
	return 0;
}

static struct rproc_ops vs_classic_rproc_ops = {
	.start		= vs_classic_rproc_start,
	.stop		= vs_classic_rproc_stop,
	.kick		= vs_classic_rproc_kick,
	.load		= vs_classic_rproc_load, //rproc_elf_load_segments,
	.parse_fw	= vs_classic_rproc_parse_fw,
	.get_boot_addr	= rproc_elf_get_boot_addr, 
	.da_to_va	= vs_classic_da_to_va,
};

static int vs_classic_rproc_probe(struct platform_device *pdev) {

	struct device *dev = &pdev->dev;
	struct rproc *rproc;
	struct vs_classic_rproc **p_rproc;
	int ret;

	dev_info(dev, "VS Classic Controller remoteproc probing ...\n");
	/* Allocate remoteproc */
	rproc = devm_rproc_alloc(dev, dev_name(dev), &vs_classic_rproc_ops,
				 NULL, sizeof(struct vs_classic_rproc));
	if (!rproc) {
		ret = -ENOMEM;
		goto error;
	}
	dev_info(dev, "VS Classic Controller remoteproc allocated\n");

	rproc->auto_boot = false;	
	// *p_rproc = rproc->priv;
	// (*p_rproc)->rproc = rproc;
	// (*p_rproc)->dev = dev;

	dev_info(dev, "Adding the remoteproc\n");
	/* Add vs_classic remoteproc */
	ret = devm_rproc_add(dev, rproc);
	if (ret)
		goto error;
	dev_info(dev, "VS Classic Controller remoteproc added\n");

	return 0;
error:
	*p_rproc = NULL;
	return ret;
}

static int vs_classic_rproc_remove(struct platform_device *pdev) {
	return 0;
}


static const struct vs_classic_rproc_data vs_classic_data = {
	.device_name = "vs_classic",
};

static const struct of_device_id vs_classic_rproc_of_match[] = {
	{ .compatible	= "fciraolo,vs_classic-remoteproc", .data = &vs_classic_data, },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, vs_classic_rproc_of_match);

/* Match table for OF platform binding */
static struct platform_driver vs_classic_rproc_driver = {
	.probe = vs_classic_rproc_probe,
	.remove = vs_classic_rproc_remove,
	.driver = {
		.name = "vs_classic-rproc",
		.of_match_table = vs_classic_rproc_of_match,
	},
};
module_platform_driver(vs_classic_rproc_driver);

MODULE_AUTHOR("Francesco Ciraolo <fciraolo@bu.edu>");
MODULE_LICENSE("GPL v2");