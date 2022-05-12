/*
 * Copyright 2005-2009 Freescale Semiconductor, Inc.
 *
 * The code contained herein is licensed under the GNU Lesser General
 * Public License.  You may obtain a copy of the GNU Lesser General
 * Public License Version 2.1 or later at the following locations:
 *
 * http://www.opensource.org/licenses/lgpl-license.html
 * http://www.gnu.org/copyleft/lgpl.html
 */

#ifndef __DRM_IPU_H__
#define __DRM_IPU_H__

#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/bitmap.h>
#include <linux/fb.h>
#include <linux/of.h>
#include <drm/drm_color_mgmt.h>
#include <media/v4l2-mediabus.h>
#include <video/videomode.h>

struct ipu_soc;

enum ipuv3_type {
	IPUV3EX,
	IPUV3M,
	IPUV3H,
};

#define IPU_PIX_FMT_GBR24	v4l2_fourcc('G', 'B', 'R', '3')

/*
 * Bitfield of Display Interface signal polarities.
 */
struct ipu_di_signal_cfg {
	unsigned data_pol:1;	/* true = inverted */
	unsigned clk_pol:1;	/* true = rising edge */
	unsigned enable_pol:1;

	struct videomode mode;

	u32 bus_format;
	u32 v_to_h_sync;

#define IPU_DI_CLKMODE_SYNC	(1 << 0)
#define IPU_DI_CLKMODE_EXT	(1 << 1)
	unsigned long clkflags;

	u8 hsync_pin;
	u8 vsync_pin;
};

/*
 * Enumeration of CSI destinations
 */
enum ipu_csi_dest {
	IPU_CSI_DEST_IDMAC, /* to memory via SMFC */
	IPU_CSI_DEST_IC,	/* to Image Converter */
	IPU_CSI_DEST_VDIC,  /* to VDIC */
};

/*
 * Enumeration of IPU rotation modes
 */
#define IPU_ROT_BIT_VFLIP (1 << 0)
#define IPU_ROT_BIT_HFLIP (1 << 1)
#define IPU_ROT_BIT_90    (1 << 2)

enum ipu_rotate_mode {
	IPU_ROTATE_NONE = 0,
	IPU_ROTATE_VERT_FLIP = IPU_ROT_BIT_VFLIP,
	IPU_ROTATE_HORIZ_FLIP = IPU_ROT_BIT_HFLIP,
	IPU_ROTATE_180 = (IPU_ROT_BIT_VFLIP | IPU_ROT_BIT_HFLIP),
	IPU_ROTATE_90_RIGHT = IPU_ROT_BIT_90,
	IPU_ROTATE_90_RIGHT_VFLIP = (IPU_ROT_BIT_90 | IPU_ROT_BIT_VFLIP),
	IPU_ROTATE_90_RIGHT_HFLIP = (IPU_ROT_BIT_90 | IPU_ROT_BIT_HFLIP),
	IPU_ROTATE_90_LEFT = (IPU_ROT_BIT_90 |
			      IPU_ROT_BIT_VFLIP | IPU_ROT_BIT_HFLIP),
};

/* 90-degree rotations require the IRT unit */
#define ipu_rot_mode_is_irt(m) (((m) & IPU_ROT_BIT_90) != 0)

enum ipu_color_space {
	IPUV3_COLORSPACE_RGB,
	IPUV3_COLORSPACE_YUV,
	IPUV3_COLORSPACE_UNKNOWN,
};

/*
 * Enumeration of VDI MOTION select
 */
enum ipu_motion_sel {
	MOTION_NONE = 0,
	LOW_MOTION,
	MED_MOTION,
	HIGH_MOTION,
};

struct ipuv3_channel;

enum ipu_channel_irq {
	IPU_IRQ_EOF = 0,
	IPU_IRQ_NFACK = 64,
	IPU_IRQ_NFB4EOF = 128,
	IPU_IRQ_EOS = 192,
};

/*
 * Enumeration of IDMAC channels
 */
#define IPUV3_CHANNEL_CSI0			 0
#define IPUV3_CHANNEL_CSI1			 1
#define IPUV3_CHANNEL_CSI2			 2
#define IPUV3_CHANNEL_CSI3			 3
#define IPUV3_CHANNEL_VDI_MEM_IC_VF		 5
/*
 * NOTE: channels 6,7 are unused in the IPU and are not IDMAC channels,
 * but the direct CSI->VDI linking is handled the same way as IDMAC
 * channel linking in the FSU via the IPU_FS_PROC_FLOW registers, so
 * these channel names are used to support the direct CSI->VDI link.
 */
#define IPUV3_CHANNEL_CSI_DIRECT		 6
#define IPUV3_CHANNEL_CSI_VDI_PREV		 7
#define IPUV3_CHANNEL_MEM_VDI_PREV		 8
#define IPUV3_CHANNEL_MEM_VDI_CUR		 9
#define IPUV3_CHANNEL_MEM_VDI_NEXT		10
#define IPUV3_CHANNEL_MEM_IC_PP			11
#define IPUV3_CHANNEL_MEM_IC_PRP_VF		12
#define IPUV3_CHANNEL_VDI_MEM_RECENT		13
#define IPUV3_CHANNEL_G_MEM_IC_PRP_VF		14
#define IPUV3_CHANNEL_G_MEM_IC_PP		15
#define IPUV3_CHANNEL_G_MEM_IC_PRP_VF_ALPHA	17
#define IPUV3_CHANNEL_G_MEM_IC_PP_ALPHA		18
#define IPUV3_CHANNEL_MEM_VDI_PLANE1_COMB_ALPHA	19
#define IPUV3_CHANNEL_IC_PRP_ENC_MEM		20
#define IPUV3_CHANNEL_IC_PRP_VF_MEM		21
#define IPUV3_CHANNEL_IC_PP_MEM			22
#define IPUV3_CHANNEL_MEM_BG_SYNC		23
#define IPUV3_CHANNEL_MEM_BG_ASYNC		24
#define IPUV3_CHANNEL_MEM_VDI_PLANE1_COMB	25
#define IPUV3_CHANNEL_MEM_VDI_PLANE3_COMB	26
#define IPUV3_CHANNEL_MEM_FG_SYNC		27
#define IPUV3_CHANNEL_MEM_DC_SYNC		28
#define IPUV3_CHANNEL_MEM_FG_ASYNC		29
#define IPUV3_CHANNEL_MEM_FG_SYNC_ALPHA		31
#define IPUV3_CHANNEL_MEM_FG_ASYNC_ALPHA	33
#define IPUV3_CHANNEL_DC_MEM_READ		40
#define IPUV3_CHANNEL_MEM_DC_ASYNC		41
#define IPUV3_CHANNEL_MEM_DC_COMMAND		42
#define IPUV3_CHANNEL_MEM_DC_COMMAND2		43
#define IPUV3_CHANNEL_MEM_DC_OUTPUT_MASK	44
#define IPUV3_CHANNEL_MEM_ROT_ENC		45
#define IPUV3_CHANNEL_MEM_ROT_VF		46
#define IPUV3_CHANNEL_MEM_ROT_PP		47
#define IPUV3_CHANNEL_ROT_ENC_MEM		48
#define IPUV3_CHANNEL_ROT_VF_MEM		49
#define IPUV3_CHANNEL_ROT_PP_MEM		50
#define IPUV3_CHANNEL_MEM_BG_SYNC_ALPHA		51
#define IPUV3_CHANNEL_MEM_BG_ASYNC_ALPHA	52
#define IPUV3_NUM_CHANNELS			64

static inline int ipu_channel_alpha_channel(int ch_num)
{
	switch (ch_num) {
	case IPUV3_CHANNEL_G_MEM_IC_PRP_VF:
		return IPUV3_CHANNEL_G_MEM_IC_PRP_VF_ALPHA;
	case IPUV3_CHANNEL_G_MEM_IC_PP:
		return IPUV3_CHANNEL_G_MEM_IC_PP_ALPHA;
	case IPUV3_CHANNEL_MEM_FG_SYNC:
		return IPUV3_CHANNEL_MEM_FG_SYNC_ALPHA;
	case IPUV3_CHANNEL_MEM_FG_ASYNC:
		return IPUV3_CHANNEL_MEM_FG_ASYNC_ALPHA;
	case IPUV3_CHANNEL_MEM_BG_SYNC:
		return IPUV3_CHANNEL_MEM_BG_SYNC_ALPHA;
	case IPUV3_CHANNEL_MEM_BG_ASYNC:
		return IPUV3_CHANNEL_MEM_BG_ASYNC_ALPHA;
	case IPUV3_CHANNEL_MEM_VDI_PLANE1_COMB:
		return IPUV3_CHANNEL_MEM_VDI_PLANE1_COMB_ALPHA;
	default:
		return -EINVAL;
	}
}

int ipu_map_irq(struct ipu_soc *ipu, int irq);
int ipu_idmac_channel_irq(struct ipu_soc *ipu, struct ipuv3_channel *channel,
		enum ipu_channel_irq irq);

#define IPU_IRQ_DP_SF_START		(448 + 2)
#define IPU_IRQ_DP_SF_END		(448 + 3)
#define IPU_IRQ_BG_SF_END		IPU_IRQ_DP_SF_END,
#define IPU_IRQ_DC_FC_0			(448 + 8)
#define IPU_IRQ_DC_FC_1			(448 + 9)
#define IPU_IRQ_DC_FC_2			(448 + 10)
#define IPU_IRQ_DC_FC_3			(448 + 11)
#define IPU_IRQ_DC_FC_4			(448 + 12)
#define IPU_IRQ_DC_FC_6			(448 + 13)
#define IPU_IRQ_VSYNC_PRE_0		(448 + 14)
#define IPU_IRQ_VSYNC_PRE_1		(448 + 15)

/*
 * IPU Common functions
 */
int ipu_get_num(struct ipu_soc *ipu);
void ipu_set_csi_src_mux(struct ipu_soc *ipu, int csi_id, bool mipi_csi2);
void ipu_set_ic_src_mux(struct ipu_soc *ipu, int csi_id, bool vdi);
void ipu_dump(struct ipu_soc *ipu);

/*
 * IPU Image DMA Controller (idmac) functions
 */
struct ipuv3_channel *ipu_idmac_get(struct ipu_soc *ipu, unsigned channel);
void ipu_idmac_put(struct ipuv3_channel *);

int ipu_idmac_enable_channel(struct ipuv3_channel *channel);
int ipu_idmac_disable_channel(struct ipuv3_channel *channel);
void ipu_idmac_enable_watermark(struct ipuv3_channel *channel, bool enable);
int ipu_idmac_lock_enable(struct ipuv3_channel *channel, int num_bursts);
int ipu_idmac_wait_busy(struct ipuv3_channel *channel, int ms);

void ipu_idmac_set_double_buffer(struct ipuv3_channel *channel,
		bool doublebuffer);
int ipu_idmac_get_current_buffer(struct ipuv3_channel *channel);
bool ipu_idmac_buffer_is_ready(struct ipuv3_channel *channel, u32 buf_num);
void ipu_idmac_select_buffer(struct ipuv3_channel *channel, u32 buf_num);
void ipu_idmac_clear_buffer(struct ipuv3_channel *channel, u32 buf_num);
int ipu_fsu_link(struct ipu_soc *ipu, int src_ch, int sink_ch);
int ipu_fsu_unlink(struct ipu_soc *ipu, int src_ch, int sink_ch);
int ipu_idmac_link(struct ipuv3_channel *src, struct ipuv3_channel *sink);
int ipu_idmac_unlink(struct ipuv3_channel *src, struct ipuv3_channel *sink);

/*
 * IPU Channel Parameter Memory (cpmem) functions
 */
struct ipu_rgb {
	struct fb_bitfield      red;
	struct fb_bitfield      green;
	struct fb_bitfield      blue;
	struct fb_bitfield      transp;
	int                     bits_per_pixel;
};

struct ipu_image {
	struct v4l2_pix_format pix;
	struct v4l2_rect rect;
	dma_addr_t phys0;
	dma_addr_t phys1;
	/* chroma plane offset overrides */
	u32 u_offset;
	u32 v_offset;
};

void ipu_cpmem_zero(struct ipuv3_channel *ch);
void ipu_cpmem_set_resolution(struct ipuv3_channel *ch, int xres, int yres);
void ipu_cpmem_skip_odd_chroma_rows(struct ipuv3_channel *ch);
void ipu_cpmem_set_stride(struct ipuv3_channel *ch, int stride);
void ipu_cpmem_set_high_priority(struct ipuv3_channel *ch);
void ipu_cpmem_set_buffer(struct ipuv3_channel *ch, int bufnum, dma_addr_t buf);
void ipu_cpmem_set_uv_offset(struct ipuv3_channel *ch, u32 u_off, u32 v_off);
void ipu_cpmem_interlaced_scan(struct ipuv3_channel *ch, int stride,
			       u32 pixelformat);
void ipu_cpmem_set_axi_id(struct ipuv3_channel *ch, u32 id);
int ipu_cpmem_get_burstsize(struct ipuv3_channel *ch);
void ipu_cpmem_set_burstsize(struct ipuv3_channel *ch, int burstsize);
void ipu_cpmem_set_block_mode(struct ipuv3_channel *ch);
void ipu_cpmem_set_rotation(struct ipuv3_channel *ch,
			    enum ipu_rotate_mode rot);
int ipu_cpmem_set_format_rgb(struct ipuv3_channel *ch,
			     const struct ipu_rgb *rgb);
int ipu_cpmem_set_format_passthrough(struct ipuv3_channel *ch, int width);
void ipu_cpmem_set_yuv_interleaved(struct ipuv3_channel *ch, u32 pixel_format);
void ipu_cpmem_set_yuv_planar_full(struct ipuv3_channel *ch,
				   unsigned int uv_stride,
				   unsigned int u_offset,
				   unsigned int v_offset);
int ipu_cpmem_set_fmt(struct ipuv3_channel *ch, u32 drm_fourcc);
int ipu_cpmem_set_image(struct ipuv3_channel *ch, struct ipu_image *image);
void ipu_cpmem_dump(struct ipuv3_channel *ch);

/*
 * IPU Display Controller (dc) functions
 */
struct ipu_dc;
struct ipu_di;
struct ipu_dc *ipu_dc_get(struct ipu_soc *ipu, int channel);
void ipu_dc_put(struct ipu_dc *dc);
int ipu_dc_init_sync(struct ipu_dc *dc, struct ipu_di *di, bool interlaced,
		u32 pixel_fmt, u32 width);
void ipu_dc_enable(struct ipu_soc *ipu);
void ipu_dc_enable_channel(struct ipu_dc *dc);
void ipu_dc_disable_channel(struct ipu_dc *dc);
void ipu_dc_disable(struct ipu_soc *ipu);

/*
 * IPU Display Interface (di) functions
 */
struct ipu_di *ipu_di_get(struct ipu_soc *ipu, int disp);
void ipu_di_put(struct ipu_di *);
int ipu_di_disable(struct ipu_di *);
int ipu_di_enable(struct ipu_di *);
int ipu_di_get_num(struct ipu_di *);
int ipu_di_adjust_videomode(struct ipu_di *di, struct videomode *mode);
int ipu_di_init_sync_panel(struct ipu_di *, struct ipu_di_signal_cfg *sig);

/*
 * IPU Display Multi FIFO Controller (dmfc) functions
 */
struct dmfc_channel;
int ipu_dmfc_enable_channel(struct dmfc_channel *dmfc);
void ipu_dmfc_disable_channel(struct dmfc_channel *dmfc);
void ipu_dmfc_config_wait4eot(struct dmfc_channel *dmfc, int width);
struct dmfc_channel *ipu_dmfc_get(struct ipu_soc *ipu, int ipuv3_channel);
void ipu_dmfc_put(struct dmfc_channel *dmfc);

/*
 * IPU Display Processor (dp) functions
 */
#define IPU_DP_FLOW_SYNC_BG	0
#define IPU_DP_FLOW_SYNC_FG	1
#define IPU_DP_FLOW_ASYNC0_BG	2
#define IPU_DP_FLOW_ASYNC0_FG	3
#define IPU_DP_FLOW_ASYNC1_BG	4
#define IPU_DP_FLOW_ASYNC1_FG	5

struct ipu_dp *ipu_dp_get(struct ipu_soc *ipu, unsigned int flow);
void ipu_dp_put(struct ipu_dp *);
int ipu_dp_enable(struct ipu_soc *ipu);
int ipu_dp_enable_channel(struct ipu_dp *dp);
void ipu_dp_disable_channel(struct ipu_dp *dp, bool sync);
void ipu_dp_disable(struct ipu_soc *ipu);
int ipu_dp_setup_channel(struct ipu_dp *dp,
		enum drm_color_encoding ycbcr_enc, enum drm_color_range range,
		enum ipu_color_space in, enum ipu_color_space out);
int ipu_dp_set_window_pos(struct ipu_dp *, u16 x_pos, u16 y_pos);
int ipu_dp_set_global_alpha(struct ipu_dp *dp, bool enable, u8 alpha,
		bool bg_chan);

/*
 * IPU Prefetch Resolve Gasket (prg) functions
 */
int ipu_prg_max_active_channels(void);
bool ipu_prg_present(struct ipu_soc *ipu);
bool ipu_prg_format_supported(struct ipu_soc *ipu, uint32_t format,
			      uint64_t modifier);
int ipu_prg_enable(struct ipu_soc *ipu);
void ipu_prg_disable(struct ipu_soc *ipu);
void ipu_prg_channel_disable(struct ipuv3_channel *ipu_chan);
int ipu_prg_channel_configure(struct ipuv3_channel *ipu_chan,
			      unsigned int axi_id,  unsigned int width,
			      unsigned int height, unsigned int stride,
			      u32 format, uint64_t modifier, unsigned long *eba);
bool ipu_prg_channel_configure_pending(struct ipuv3_channel *ipu_chan);

/*
 * IPU CMOS Sensor Interface (csi) functions
 */
struct ipu_csi;
int ipu_csi_init_interface(struct ipu_csi *csi,
			   const struct v4l2_mbus_config *mbus_cfg,
			   const struct v4l2_mbus_framefmt *infmt,
			   const struct v4l2_mbus_framefmt *outfmt);
bool ipu_csi_is_interlaced(struct ipu_csi *csi);
void ipu_csi_get_window(struct ipu_csi *csi, struct v4l2_rect *w);
void ipu_csi_set_window(struct ipu_csi *csi, struct v4l2_rect *w);
void ipu_csi_set_downsize(struct ipu_csi *csi, bool horiz, bool vert);
void ipu_csi_set_test_generator(struct ipu_csi *csi, bool active,
				u32 r_value, u32 g_value, u32 b_value,
				u32 pix_clk);
int ipu_csi_set_mipi_datatype(struct ipu_csi *csi, u32 vc,
			      struct v4l2_mbus_framefmt *mbus_fmt);
int ipu_csi_set_skip_smfc(struct ipu_csi *csi, u32 skip,
			  u32 max_ratio, u32 id);
int ipu_csi_set_dest(struct ipu_csi *csi, enum ipu_csi_dest csi_dest);
int ipu_csi_enable(struct ipu_csi *csi);
int ipu_csi_disable(struct ipu_csi *csi);
struct ipu_csi *ipu_csi_get(struct ipu_soc *ipu, int id);
void ipu_csi_put(struct ipu_csi *csi);
void ipu_csi_dump(struct ipu_csi *csi);

/*
 * IPU Image Converter (ic) functions
 */
enum ipu_ic_task {
	IC_TASK_ENCODER,
	IC_TASK_VIEWFINDER,
	IC_TASK_POST_PROCESSOR,
	IC_NUM_TASKS,
};

/*
 * The parameters that describe a colorspace according to the
 * Image Converter:
 *    - Y'CbCr encoding
 *    - quantization
 *    - "colorspace" (RGB or YUV).
 */
struct ipu_ic_colorspace {
	enum v4l2_ycbcr_encoding enc;
	enum v4l2_quantization quant;
	enum ipu_color_space cs;
};

static inline void
ipu_ic_fill_colorspace(struct ipu_ic_colorspace *ic_cs,
		       enum v4l2_ycbcr_encoding enc,
		       enum v4l2_quantization quant,
		       enum ipu_color_space cs)
{
	ic_cs->enc = enc;
	ic_cs->quant = quant;
	ic_cs->cs = cs;
}

struct ipu_ic_csc_params {
	s16 coeff[3][3];	/* signed 9-bit integer coefficients */
	s16 offset[3];		/* signed 11+2-bit fixed point offset */
	u8 scale:2;		/* scale coefficients * 2^(scale-1) */
	bool sat:1;		/* saturate to (16, 235(Y) / 240(U, V)) */
};

struct ipu_ic_csc {
	struct ipu_ic_colorspace in_cs;
	struct ipu_ic_colorspace out_cs;
	struct ipu_ic_csc_params params;
};

struct ipu_ic;

int __ipu_ic_calc_csc(struct ipu_ic_csc *csc);
int ipu_ic_calc_csc(struct ipu_ic_csc *csc,
		    enum v4l2_ycbcr_encoding in_enc,
		    enum v4l2_quantization in_quant,
		    enum ipu_color_space in_cs,
		    enum v4l2_ycbcr_encoding out_enc,
		    enum v4l2_quantization out_quant,
		    enum ipu_color_space out_cs);
int ipu_ic_task_init(struct ipu_ic *ic,
		     const struct ipu_ic_csc *csc,
		     int in_width, int in_height,
		     int out_width, int out_height);
int ipu_ic_task_init_rsc(struct ipu_ic *ic,
			 const struct ipu_ic_csc *csc,
			 int in_width, int in_height,
			 int out_width, int out_height,
			 u32 rsc);
int ipu_ic_task_graphics_init(struct ipu_ic *ic,
			      const struct ipu_ic_colorspace *g_in_cs,
			      bool galpha_en, u32 galpha,
			      bool colorkey_en, u32 colorkey);
void ipu_ic_task_enable(struct ipu_ic *ic);
void ipu_ic_task_disable(struct ipu_ic *ic);
int ipu_ic_task_idma_init(struct ipu_ic *ic, struct ipuv3_channel *channel,
			  u32 width, u32 height, int burst_size,
			  enum ipu_rotate_mode rot);
int ipu_ic_enable(struct ipu_ic *ic);
int ipu_ic_disable(struct ipu_ic *ic);
struct ipu_ic *ipu_ic_get(struct ipu_soc *ipu, enum ipu_ic_task task);
void ipu_ic_put(struct ipu_ic *ic);
void ipu_ic_dump(struct ipu_ic *ic);

/*
 * IPU Video De-Interlacer (vdi) functions
 */
struct ipu_vdi;
void ipu_vdi_set_field_order(struct ipu_vdi *vdi, v4l2_std_id std, u32 field);
void ipu_vdi_set_motion(struct ipu_vdi *vdi, enum ipu_motion_sel motion_sel);
void ipu_vdi_setup(struct ipu_vdi *vdi, u32 code, int xres, int yres);
void ipu_vdi_unsetup(struct ipu_vdi *vdi);
int ipu_vdi_enable(struct ipu_vdi *vdi);
int ipu_vdi_disable(struct ipu_vdi *vdi);
struct ipu_vdi *ipu_vdi_get(struct ipu_soc *ipu);
void ipu_vdi_put(struct ipu_vdi *vdi);

/*
 * IPU Sensor Multiple FIFO Controller (SMFC) functions
 */
struct ipu_smfc *ipu_smfc_get(struct ipu_soc *ipu, unsigned int chno);
void ipu_smfc_put(struct ipu_smfc *smfc);
int ipu_smfc_enable(struct ipu_smfc *smfc);
int ipu_smfc_disable(struct ipu_smfc *smfc);
int ipu_smfc_map_channel(struct ipu_smfc *smfc, int csi_id, int mipi_id);
int ipu_smfc_set_burstsize(struct ipu_smfc *smfc, int burstsize);
int ipu_smfc_set_watermark(struct ipu_smfc *smfc, u32 set_level, u32 clr_level);

enum ipu_color_space ipu_drm_fourcc_to_colorspace(u32 drm_fourcc);
enum ipu_color_space ipu_pixelformat_to_colorspace(u32 pixelformat);
int ipu_degrees_to_rot_mode(enum ipu_rotate_mode *mode, int degrees,
			    bool hflip, bool vflip);
int ipu_rot_mode_to_degrees(int *degrees, enum ipu_rotate_mode mode,
			    bool hflip, bool vflip);

struct ipu_client_platformdata {
	int csi;
	int di;
	int dc;
	int dp;
	int dma[2];
	struct device_node *of_node;
};

#endif /* __DRM_IPU_H__ */
