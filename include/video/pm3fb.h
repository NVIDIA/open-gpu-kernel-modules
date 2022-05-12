/*
 *  linux/drivers/video/pm3fb.h -- 3DLabs Permedia3 frame buffer device
 *
 *  Copyright (C) 2001 Romain Dolbeau <dolbeau@irisa.fr>
 *  Copyright (C) 2001 Sven Luther, <luther@dpt-info.u-strasbg.fr>
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#ifndef PM3FB_H
#define PM3FB_H

/**********************************************
*  GLINT Permedia3 Control Status registers   *
***********************************************/
/* Control Status Registers */
#define PM3ResetStatus						0x0000
#define PM3IntEnable						0x0008
#define PM3IntFlags						0x0010
#define PM3InFIFOSpace						0x0018
#define PM3OutFIFOWords						0x0020
#define PM3DMAAddress						0x0028
#define PM3DMACount						0x0030
#define PM3ErrorFlags						0x0038
#define PM3VClkCtl						0x0040
#define PM3TestRegister						0x0048
#define PM3Aperture0						0x0050
#define PM3Aperture1						0x0058
#define PM3DMAControl						0x0060
#define PM3FIFODis						0x0068
#define PM3ChipConfig						0x0070
#define PM3AGPControl						0x0078

#define PM3GPOutDMAAddress					0x0080
#define PM3PCIFeedbackCount					0x0088
#define PM3PCIAbortStatus					0x0090
#define PM3PCIAbortAddress					0x0098

#define PM3PCIPLLStatus						0x00f0

#define PM3HostTextureAddress					0x0100
#define PM3TextureDownloadControl				0x0108
#define PM3TextureOperation					0x0110
#define PM3LogicalTexturePage					0x0118
#define PM3TexDMAAddress					0x0120
#define PM3TexFIFOSpace						0x0128

/**********************************************
*  GLINT Permedia3 Region 0 Bypass Controls   *
***********************************************/
#define PM3ByAperture1Mode					0x0300
	#define PM3ByApertureMode_BYTESWAP_ABCD			(0 << 0)
	#define PM3ByApertureMode_BYTESWAP_BADC			(1 << 0)
	#define PM3ByApertureMode_BYTESWAP_CDAB			(2 << 0)
	#define PM3ByApertureMode_BYTESWAP_DCBA			(3 << 0)
	#define PM3ByApertureMode_PATCH_ENABLE			(1 << 2)
	#define PM3ByApertureMode_FORMAT_RAW			(0 << 3)
	#define PM3ByApertureMode_FORMAT_YUYV			(1 << 3)
	#define PM3ByApertureMode_FORMAT_UYVY			(2 << 3)
	#define PM3ByApertureMode_PIXELSIZE_8BIT		(0 << 5)
	#define PM3ByApertureMode_PIXELSIZE_16BIT		(1 << 5)
	#define PM3ByApertureMode_PIXELSIZE_32BIT		(2 << 5)
		#define PM3ByApertureMode_PIXELSIZE_MASK	(3 << 5)
	#define PM3ByApertureMode_EFFECTIVE_STRIDE_1024		(0 << 7)
	#define PM3ByApertureMode_EFFECTIVE_STRIDE_2048		(1 << 7)
	#define PM3ByApertureMode_EFFECTIVE_STRIDE_4096		(2 << 7)
	#define PM3ByApertureMode_EFFECTIVE_STRIDE_8192		(3 << 7)
	#define PM3ByApertureMode_PATCH_OFFSET_X(off)	(((off) & 0x7f) << 9)
	#define PM3ByApertureMode_PATCH_OFFSET_Y(off)	(((off) & 0x7f) << 16)
	#define PM3ByApertureMode_FRAMEBUFFER			(0 << 21)
	#define PM3ByApertureMode_LOCALBUFFER			(1 << 21)
	#define PM3ByApertureMode_DOUBLE_WRITE_OFF		(0 << 22)
	#define PM3ByApertureMode_DOUBLE_WRITE_1MB		(1 << 22)
	#define PM3ByApertureMode_DOUBLE_WRITE_2MB		(2 << 22)
	#define PM3ByApertureMode_DOUBLE_WRITE_4MB		(3 << 22)
	#define PM3ByApertureMode_DOUBLE_WRITE_8MB		(4 << 22)
	#define PM3ByApertureMode_DOUBLE_WRITE_16MB		(5 << 22)
	#define PM3ByApertureMode_DOUBLE_WRITE_32MB		(6 << 22)

#define PM3ByAperture2Mode					0x0328

/**********************************************
*  GLINT Permedia3 Memory Control (0x1000)    *
***********************************************/
#define PM3MemCounter						0x1000
#define PM3MemBypassWriteMask					0x1008
#define PM3MemScratch						0x1010
#define PM3LocalMemCaps						0x1018
	#define PM3LocalMemCaps_NoWriteMask			(1 << 28)
#define PM3LocalMemTimings					0x1020
#define PM3LocalMemControl					0x1028
#define PM3LocalMemRefresh					0x1030
#define PM3LocalMemPowerDown					0x1038
#define PM3RemoteMemControl					0x1100

/**********************************************
*  GLINT Permedia3 Video Control (0x3000)     *
***********************************************/

#define PM3ScreenBase						0x3000
#define PM3ScreenStride						0x3008
#define PM3HTotal						0x3010
#define PM3HgEnd						0x3018
#define PM3HbEnd						0x3020
#define PM3HsStart						0x3028
#define PM3HsEnd						0x3030
#define PM3VTotal						0x3038
#define PM3VbEnd						0x3040
#define PM3VsStart						0x3048
#define PM3VsEnd						0x3050
#define PM3VideoControl						0x3058
	#define PM3VideoControl_ENABLE				(1 << 0)
	#define PM3VideoControl_BLANK_ACTIVE_HIGH		(0 << 1)
	#define PM3VideoControl_BLANK_ACTIVE_LOW		(1 << 1)
	#define PM3VideoControl_LINE_DOUBLE_OFF			(0 << 2)
	#define PM3VideoControl_LINE_DOUBLE_ON			(1 << 2)
	#define PM3VideoControl_HSYNC_FORCE_HIGH		(0 << 3)
	#define PM3VideoControl_HSYNC_ACTIVE_HIGH		(1 << 3)
	#define PM3VideoControl_HSYNC_FORCE_LOW			(2 << 3)
	#define PM3VideoControl_HSYNC_ACTIVE_LOW		(3 << 3)
	#define PM3VideoControl_HSYNC_MASK			(3 << 3)
	#define PM3VideoControl_VSYNC_FORCE_HIGH		(0 << 5)
	#define PM3VideoControl_VSYNC_ACTIVE_HIGH		(1 << 5)
	#define PM3VideoControl_VSYNC_FORCE_LOW			(2 << 5)
	#define PM3VideoControl_VSYNC_ACTIVE_LOW		(3 << 5)
	#define PM3VideoControl_VSYNC_MASK			(3 << 5)
	#define PM3VideoControl_BYTE_DOUBLE_OFF			(0 << 7)
	#define PM3VideoControl_BYTE_DOUBLE_ON			(1 << 7)
	#define PM3VideoControl_BUFFER_SWAP_SYNCON_FRAMEBLANK	(0 << 9)
	#define PM3VideoControl_BUFFER_SWAP_FREE_RUNNING	(1 << 9)
	#define PM3VideoControl_BUFFER_SWAP_LIMITETO_FRAMERATE	(2 << 9)
	#define PM3VideoControl_STEREO_ENABLE			(1 << 11)
	#define PM3VideoControl_RIGHT_EYE_ACTIVE_HIGH		(0 << 12)
	#define PM3VideoControl_RIGHT_EYE_ACTIVE_LOW		(1 << 12)
	#define PM3VideoControl_VIDEO_EXT_LOW			(0 << 14)
	#define PM3VideoControl_VIDEO_EXT_HIGH			(1 << 14)
	#define PM3VideoControl_SYNC_MODE_INDEPENDENT		(0 << 16)
	#define PM3VideoControl_SYNC_MODE_SYNCTO_VSA		(1 << 16)
	#define PM3VideoControl_SYNC_MODE_SYNCTO_VSB		(2 << 16)
	#define PM3VideoControl_PATCH_ENABLE			(1 << 18)
	#define PM3VideoControl_PIXELSIZE_8BIT			(0 << 19)
	#define PM3VideoControl_PIXELSIZE_16BIT			(1 << 19)
	#define PM3VideoControl_PIXELSIZE_32BIT			(2 << 19)
	#define PM3VideoControl_DISPLAY_ENABLE			(1 << 21)
	#define PM3VideoControl_PATCH_OFFSET_X(off)	(((off) & 0x3f) << 22)
	#define PM3VideoControl_PATCH_OFFSET_Y(off)	(((off) & 0x3f) << 28)
#define PM3InterruptLine					0x3060
#define PM3DisplayData						0x3068
#define PM3VerticalLineCount					0x3070
#define PM3FifoControl						0x3078
#define PM3ScreenBaseRight					0x3080
#define PM3MiscControl						0x3088

#define PM3VideoOverlayUpdate					0x3100
	#define PM3VideoOverlayUpdate_ENABLE			(1 << 0)
#define PM3VideoOverlayMode					0x3108
	#define PM3VideoOverlayMode_ENABLE			(1 << 0)
	#define PM3VideoOverlayMode_BUFFERSYNC_MANUAL		(0 << 1)
	#define PM3VideoOverlayMode_BUFFERSYNC_VIDEOSTREAMA	(1 << 1)
	#define PM3VideoOverlayMode_BUFFERSYNC_VIDEOSTREAMB	(2 << 1)
	#define PM3VideoOverlayMode_FIELDPOLARITY_NORMAL	(0 << 4)
	#define PM3VideoOverlayMode_FIELDPOLARITY_INVERT	(1 << 4)
	#define PM3VideoOverlayMode_PIXELSIZE_8BIT		(0 << 5)
	#define PM3VideoOverlayMode_PIXELSIZE_16BIT		(1 << 5)
	#define PM3VideoOverlayMode_PIXELSIZE_32BIT		(2 << 5)
	#define PM3VideoOverlayMode_COLORFORMAT_RGB8888		\
					((0 << 7)|(1 << 12)|(2 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_RGB4444		\
					((1 << 7)|(1 << 12)|(1 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_RGB5551		\
					((2 << 7)|(1 << 12)|(1 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_RGB565		\
					((3 << 7)|(1 << 12)|(1 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_RGB332		\
					((4 << 7)|(1 << 12)|(0 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_BGR8888		\
					((0 << 7)|(2 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_BGR4444		\
					((1 << 7)|(1 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_BGR5551		\
					((2 << 7)|(1 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_BGR565		\
					((3 << 7)|(1 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_BGR332		\
					((4 << 7)|(0 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_CI8		\
					((5 << 7)|(1 << 12)|(0 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_VUY444		\
					((2 << 10)|(1 << 12)|(2 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_YUV444		\
					((2 << 10)|(2 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_VUY422		\
					((1 << 10)|(1 << 12)|(1 << 5))
	#define PM3VideoOverlayMode_COLORFORMAT_YUV422		\
					((1 << 10)|(1 << 5))
	#define PM3VideoOverlayMode_COLORORDER_BGR		(0 << 12)
	#define PM3VideoOverlayMode_COLORORDER_RGB		(1 << 12)
	#define PM3VideoOverlayMode_LINEARCOLOREXT_OFF		(0 << 13)
	#define PM3VideoOverlayMode_LINEARCOLOREXT_ON		(1 << 13)
	#define PM3VideoOverlayMode_FILTER_MASK			(3 << 14)
	#define PM3VideoOverlayMode_FILTER_OFF			(0 << 14)
	#define PM3VideoOverlayMode_FILTER_FULL			(1 << 14)
	#define PM3VideoOverlayMode_FILTER_PARTIAL		(2 << 14)
	#define PM3VideoOverlayMode_DEINTERLACE_OFF		(0 << 16)
	#define PM3VideoOverlayMode_DEINTERLACE_BOB		(1 << 16)
	#define PM3VideoOverlayMode_PATCHMODE_OFF		(0 << 18)
	#define PM3VideoOverlayMode_PATCHMODE_ON		(1 << 18)
	#define PM3VideoOverlayMode_FLIP_VIDEO			(0 << 20)
	#define PM3VideoOverlayMode_FLIP_VIDEOSTREAMA		(1 << 20)
	#define PM3VideoOverlayMode_FLIP_VIDEOSTREAMB		(2 << 20)
	#define PM3VideoOverlayMode_MIRROR_MASK			(3 << 23)
	#define PM3VideoOverlayMode_MIRRORX_OFF			(0 << 23)
	#define PM3VideoOverlayMode_MIRRORX_ON			(1 << 23)
	#define PM3VideoOverlayMode_MIRRORY_OFF			(0 << 24)
	#define PM3VideoOverlayMode_MIRRORY_ON			(1 << 24)
#define PM3VideoOverlayFifoControl				0x3110
#define PM3VideoOverlayIndex					0x3118
#define PM3VideoOverlayBase0					0x3120
#define PM3VideoOverlayBase1					0x3128
#define PM3VideoOverlayBase2					0x3130
#define PM3VideoOverlayStride					0x3138
	#define PM3VideoOverlayStride_STRIDE(s)		(((s) & 0xfff) << 0)
#define PM3VideoOverlayWidth					0x3140
	#define PM3VideoOverlayWidth_WIDTH(w)		(((w) & 0xfff) << 0)
#define PM3VideoOverlayHeight					0x3148
	#define PM3VideoOverlayHeight_HEIGHT(h)		(((h) & 0xfff) << 0)
#define PM3VideoOverlayOrigin					0x3150
	#define PM3VideoOverlayOrigin_XORIGIN(x)	(((x) & 0xfff) << 0)
	#define PM3VideoOverlayOrigin_YORIGIN(y)	(((y) & 0xfff) << 16)
#define PM3VideoOverlayShrinkXDelta				0x3158
	#define PM3VideoOverlayShrinkXDelta_NONE		(1 << 16)
	#define PM3VideoOverlayShrinkXDelta_DELTA(s,d)	\
		((((s) << 16)/(d)) & 0x0ffffff0)
#define PM3VideoOverlayZoomXDelta				0x3160
	#define PM3VideoOverlayZoomXDelta_NONE			(1 << 16)
	#define PM3VideoOverlayZoomXDelta_DELTA(s,d)	\
		((((s) << 16)/(d)) & 0x0001fff0)
#define PM3VideoOverlayYDelta					0x3168
	#define PM3VideoOverlayYDelta_NONE			(1 << 16)
	#define PM3VideoOverlayYDelta_DELTA(s,d)	\
		((((s) << 16)/(d)) & 0x0ffffff0)
#define PM3VideoOverlayFieldOffset				0x3170
#define PM3VideoOverlayStatus					0x3178

/**********************************************
*  GLINT Permedia3 RAMDAC Registers (0x4000)  *
***********************************************/
/* Direct Registers */
#define PM3RD_PaletteWriteAddress				0x4000
#define PM3RD_PaletteData					0x4008
#define PM3RD_PixelMask						0x4010
#define PM3RD_PaletteReadAddress				0x4018

#define PM3RD_IndexLow						0x4020
#define PM3RD_IndexHigh						0x4028
#define PM3RD_IndexedData					0x4030
#define PM3RD_IndexControl					0x4038
	#define PM3RD_IndexControl_AUTOINCREMENT_ENABLE		(1 << 0)

/* Indirect Registers */
#define PM3RD_MiscControl					0x000
	#define PM3RD_MiscControl_HIGHCOLOR_RES_ENABLE		(1 << 0)
	#define PM3RD_MiscControl_PIXELDOUBLE_ENABLE		(1 << 1)
	#define PM3RD_MiscControl_LASTREAD_ADDR_ENABLE		(1 << 2)
	#define PM3RD_MiscControl_DIRECTCOLOR_ENABLE		(1 << 3)
	#define PM3RD_MiscControl_OVERLAY_ENABLE		(1 << 4)
	#define PM3RD_MiscControl_PIXELDOUBLE_BUFFER_ENABLE	(1 << 5)
	#define PM3RD_MiscControl_VSB_OUTPUT_ENABLE		(1 << 6)
	#define PM3RD_MiscControl_STEREODOUBLE_BUFFER_ENABLE	(1 << 7)
#define PM3RD_SyncControl					0x001
	#define PM3RD_SyncControl_HSYNC_ACTIVE_LOW		(0 << 0)
	#define PM3RD_SyncControl_HSYNC_ACTIVE_HIGH		(1 << 0)
	#define PM3RD_SyncControl_HSYNC_FORCE_ACTIVE		(3 << 0)
	#define PM3RD_SyncControl_HSYNC_FORCE_INACTIVE		(4 << 0)
	#define PM3RD_SyncControl_HSYNC_TRI_STATE		(2 << 0)
	#define PM3RD_SyncControl_VSYNC_ACTIVE_LOW		(0 << 3)
	#define PM3RD_SyncControl_VSYNC_ACTIVE_HIGH		(1 << 3)
	#define PM3RD_SyncControl_VSYNC_TRI_STATE		(2 << 3)
	#define PM3RD_SyncControl_VSYNC_FORCE_ACTIVE		(3 << 3)
	#define PM3RD_SyncControl_VSYNC_FORCE_INACTIVE		(4 << 3)
	#define PM3RD_SyncControl_HSYNC_OVERRIDE_SETBY_HSYNC	(0 << 6)
	#define PM3RD_SyncControl_HSYNC_OVERRIDE_FORCE_HIGH	(1 << 6)
	#define PM3RD_SyncControl_VSYNC_OVERRIDE_SETBY_VSYNC	(0 << 7)
	#define PM3RD_SyncControl_VSYNC_OVERRIDE_FORCE_HIGH	(1 << 7)
#define PM3RD_DACControl					0x002
	#define PM3RD_DACControl_DAC_POWER_ON			(0 << 0)
	#define PM3RD_DACControl_DAC_POWER_OFF			(1 << 0)
	#define PM3RD_DACControl_SYNC_ON_GREEN_ENABLE		(1 << 3)
	#define PM3RD_DACControl_BLANK_RED_DAC_ENABLE		(1 << 4)
	#define PM3RD_DACControl_BLANK_GREEN_DAC_ENABLE		(1 << 5)
	#define PM3RD_DACControl_BLANK_BLUE_DAC_ENABLE		(1 << 6)
	#define PM3RD_DACControl_BLANK_PEDESTAL_ENABLE		(1 << 7)
#define PM3RD_PixelSize						0x003
	#define PM3RD_PixelSize_24_BIT_PIXELS			(4 << 0)
	#define PM3RD_PixelSize_32_BIT_PIXELS			(2 << 0)
	#define PM3RD_PixelSize_16_BIT_PIXELS			(1 << 0)
	#define PM3RD_PixelSize_8_BIT_PIXELS			(0 << 0)
#define PM3RD_ColorFormat					0x004
	#define PM3RD_ColorFormat_LINEAR_COLOR_EXT_ENABLE	(1 << 6)
	#define PM3RD_ColorFormat_COLOR_ORDER_BLUE_LOW		(1 << 5)
	#define PM3RD_ColorFormat_COLOR_ORDER_RED_LOW		(0 << 5)
	#define PM3RD_ColorFormat_COLOR_FORMAT_MASK		(0x1f << 0)
	#define PM3RD_ColorFormat_8888_COLOR			(0 << 0)
	#define PM3RD_ColorFormat_5551_FRONT_COLOR		(1 << 0)
	#define PM3RD_ColorFormat_4444_COLOR			(2 << 0)
	#define PM3RD_ColorFormat_332_FRONT_COLOR		(5 << 0)
	#define PM3RD_ColorFormat_332_BACK_COLOR		(6 << 0)
	#define PM3RD_ColorFormat_2321_FRONT_COLOR		(9 << 0)
	#define PM3RD_ColorFormat_2321_BACK_COLOR		(10 << 0)
	#define PM3RD_ColorFormat_232_FRONTOFF_COLOR		(11 << 0)
	#define PM3RD_ColorFormat_232_BACKOFF_COLOR		(12 << 0)
	#define PM3RD_ColorFormat_5551_BACK_COLOR		(13 << 0)
	#define PM3RD_ColorFormat_CI8_COLOR			(14 << 0)
	#define PM3RD_ColorFormat_565_FRONT_COLOR		(16 << 0)
	#define PM3RD_ColorFormat_565_BACK_COLOR		(17 << 0)
#define PM3RD_CursorMode					0x005
	#define PM3RD_CursorMode_CURSOR_ENABLE			(1 << 0)
	#define PM3RD_CursorMode_FORMAT_64x64_2BPE_P0123	(0 << 2)
	#define PM3RD_CursorMode_FORMAT_32x32_2BPE_P0		(1 << 2)
	#define PM3RD_CursorMode_FORMAT_32x32_2BPE_P1		(2 << 2)
	#define PM3RD_CursorMode_FORMAT_32x32_2BPE_P2		(3 << 2)
	#define PM3RD_CursorMode_FORMAT_32x32_2BPE_P3		(4 << 2)
	#define PM3RD_CursorMode_FORMAT_32x32_4BPE_P01		(5 << 2)
	#define PM3RD_CursorMode_FORMAT_32x32_4BPE_P23		(6 << 2)
	#define PM3RD_CursorMode_TYPE_MS			(0 << 4)
	#define PM3RD_CursorMode_TYPE_X				(1 << 4)
	#define PM3RD_CursorMode_REVERSE_PIXEL_ORDER_ENABLE	(1 << 6)
	#define PM3RD_CursorMode_REVERSE_PIXEL_ORDER_3_COLOR	(2 << 6)
	#define PM3RD_CursorMode_REVERSE_PIXEL_ORDER_15_COLOR	(3 << 6)
#define PM3RD_CursorControl					0x006
	#define PM3RD_CursorControl_DOUBLE_X_ENABLED		(1 << 0)
	#define PM3RD_CursorControl_DOUBLE_Y_ENABLED		(1 << 1)
	#define PM3RD_CursorControl_READBACK_POS_ENABLED	(1 << 2)

#define PM3RD_CursorXLow					0x007
#define PM3RD_CursorXHigh					0x008
#define PM3RD_CursorYLow					0x009
#define PM3RD_CursorYHigh					0x00a
#define PM3RD_CursorHotSpotX					0x00b
#define PM3RD_CursorHotSpotY					0x00c
#define PM3RD_OverlayKey					0x00d
#define PM3RD_Pan						0x00e
	#define PM3RD_Pan_ENABLE				(1 << 0)
	#define PM3RD_Pan_GATE_ENABLE				(1 << 1)
#define PM3RD_Sense						0x00f

#define PM3RD_CheckControl					0x018
	#define PM3RD_CheckControl_PIXEL_ENABLED		(1 << 0)
	#define PM3RD_CheckControl_LUT_ENABLED			(1 << 1)
#define PM3RD_CheckPixelRed					0x019
#define PM3RD_CheckPixelGreen					0x01a
#define PM3RD_CheckPixelBlue					0x01b
#define PM3RD_CheckLUTRed					0x01c
#define PM3RD_CheckLUTGreen					0x01d
#define PM3RD_CheckLUTBlue					0x01e
#define PM3RD_Scratch						0x01f

#define PM3RD_VideoOverlayControl				0x020
	#define PM3RD_VideoOverlayControl_ENABLE		(1 << 0)
	#define PM3RD_VideoOverlayControl_MODE_MASK		(3 << 1)
	#define PM3RD_VideoOverlayControl_MODE_MAINKEY		(0 << 1)
	#define PM3RD_VideoOverlayControl_MODE_OVERLAYKEY	(1 << 1)
	#define PM3RD_VideoOverlayControl_MODE_ALWAYS		(2 << 1)
	#define PM3RD_VideoOverlayControl_MODE_BLEND		(3 << 1)
	#define PM3RD_VideoOverlayControl_DIRECTCOLOR_ENABLED	(1 << 3)
	#define PM3RD_VideoOverlayControl_BLENDSRC_MAIN		(0 << 4)
	#define PM3RD_VideoOverlayControl_BLENDSRC_REGISTER	(1 << 4)
	#define PM3RD_VideoOverlayControl_KEY_COLOR		(0 << 5)
	#define PM3RD_VideoOverlayControl_KEY_ALPHA		(1 << 5)
#define PM3RD_VideoOverlayXStartLow				0x021
#define PM3RD_VideoOverlayXStartHigh				0x022
#define PM3RD_VideoOverlayYStartLow				0x023
#define PM3RD_VideoOverlayYStartHigh				0x024
#define PM3RD_VideoOverlayXEndLow				0x025
#define PM3RD_VideoOverlayXEndHigh				0x026
#define PM3RD_VideoOverlayYEndLow				0x027
#define PM3RD_VideoOverlayYEndHigh				0x028
#define PM3RD_VideoOverlayKeyR					0x029
#define PM3RD_VideoOverlayKeyG					0x02a
#define PM3RD_VideoOverlayKeyB					0x02b
#define PM3RD_VideoOverlayBlend					0x02c
	#define PM3RD_VideoOverlayBlend_FACTOR_0_PERCENT	(0 << 6)
	#define PM3RD_VideoOverlayBlend_FACTOR_25_PERCENT	(1 << 6)
	#define PM3RD_VideoOverlayBlend_FACTOR_75_PERCENT	(2 << 6)
	#define PM3RD_VideoOverlayBlend_FACTOR_100_PERCENT	(3 << 6)

#define PM3RD_DClkSetup1					0x1f0
#define PM3RD_DClkSetup2					0x1f1
#define PM3RD_KClkSetup1					0x1f2
#define PM3RD_KClkSetup2					0x1f3

#define PM3RD_DClkControl					0x200
	#define PM3RD_DClkControl_SOURCE_PLL			(0 << 4)
	#define PM3RD_DClkControl_SOURCE_VSA			(1 << 4)
	#define PM3RD_DClkControl_SOURCE_VSB			(2 << 4)
	#define PM3RD_DClkControl_SOURCE_EXT			(3 << 4)
	#define PM3RD_DClkControl_STATE_RUN			(2 << 2)
	#define PM3RD_DClkControl_STATE_HIGH			(1 << 2)
	#define PM3RD_DClkControl_STATE_LOW			(0 << 2)
	#define PM3RD_DClkControl_LOCKED			(1 << 1)
	#define PM3RD_DClkControl_NOT_LOCKED			(0 << 1)
	#define PM3RD_DClkControl_ENABLE			(1 << 0)
#define PM3RD_DClk0PreScale					0x201
#define PM3RD_DClk0FeedbackScale				0x202
#define PM3RD_DClk0PostScale					0x203
	#define PM3_REF_CLOCK					14318
#define PM3RD_DClk1PreScale					0x204
#define PM3RD_DClk1FeedbackScale				0x205
#define PM3RD_DClk1PostScale					0x206
#define PM3RD_DClk2PreScale					0x207
#define PM3RD_DClk2FeedbackScale				0x208
#define PM3RD_DClk2PostScale					0x209
#define PM3RD_DClk3PreScale					0x20a
#define PM3RD_DClk3FeedbackScale				0x20b
#define PM3RD_DClk3PostScale					0x20c
#define PM3RD_KClkControl					0x20d
	#define PM3RD_KClkControl_ENABLE			(1 << 0)
	#define PM3RD_KClkControl_NOT_LOCKED			(0 << 1)
	#define PM3RD_KClkControl_LOCKED			(1 << 1)
	#define PM3RD_KClkControl_STATE_LOW			(0 << 2)
	#define PM3RD_KClkControl_STATE_HIGH			(1 << 2)
	#define PM3RD_KClkControl_STATE_RUN			(2 << 2)
	#define PM3RD_KClkControl_STATE_LOW_POWER		(3 << 2)
	#define PM3RD_KClkControl_SOURCE_PCLK			(0 << 4)
	#define PM3RD_KClkControl_SOURCE_HALF_PCLK		(1 << 4)
	#define PM3RD_KClkControl_SOURCE_PLL			(2 << 4)
#define PM3RD_KClkPreScale					0x20e
#define PM3RD_KClkFeedbackScale					0x20f
#define PM3RD_KClkPostScale					0x210
#define PM3RD_MClkControl					0x211
	#define PM3RD_MClkControl_ENABLE			(1 << 0)
	#define PM3RD_MClkControl_NOT_LOCKED			(0 << 1)
	#define PM3RD_MClkControl_LOCKED			(1 << 1)
	#define PM3RD_MClkControl_STATE_LOW			(0 << 2)
	#define PM3RD_MClkControl_STATE_HIGH			(1 << 2)
	#define PM3RD_MClkControl_STATE_RUN			(2 << 2)
	#define PM3RD_MClkControl_STATE_LOW_POWER		(3 << 2)
	#define PM3RD_MClkControl_SOURCE_PCLK			(0 << 4)
	#define PM3RD_MClkControl_SOURCE_HALF_PCLK		(1 << 4)
	#define PM3RD_MClkControl_SOURCE_HALF_EXT		(3 << 4)
	#define PM3RD_MClkControl_SOURCE_EXT			(4 << 4)
	#define PM3RD_MClkControl_SOURCE_HALF_KCLK		(5 << 4)
	#define PM3RD_MClkControl_SOURCE_KCLK			(6 << 4)
#define PM3RD_MClkPreScale					0x212
#define PM3RD_MClkFeedbackScale					0x213
#define PM3RD_MClkPostScale					0x214
#define PM3RD_SClkControl					0x215
	#define PM3RD_SClkControl_ENABLE			(1 << 0)
	#define PM3RD_SClkControl_NOT_LOCKED			(0 << 1)
	#define PM3RD_SClkControl_LOCKED			(1 << 1)
	#define PM3RD_SClkControl_STATE_LOW			(0 << 2)
	#define PM3RD_SClkControl_STATE_HIGH			(1 << 2)
	#define PM3RD_SClkControl_STATE_RUN			(2 << 2)
	#define PM3RD_SClkControl_STATE_LOW_POWER		(3 << 2)
	#define PM3RD_SClkControl_SOURCE_PCLK			(0 << 4)
	#define PM3RD_SClkControl_SOURCE_HALF_PCLK		(1 << 4)
	#define PM3RD_SClkControl_SOURCE_HALF_EXT		(3 << 4)
	#define PM3RD_SClkControl_SOURCE_EXT			(4 << 4)
	#define PM3RD_SClkControl_SOURCE_HALF_KCLK		(5 << 4)
	#define PM3RD_SClkControl_SOURCE_KCLK			(6 << 4)
#define PM3RD_SClkPreScale					0x216
#define PM3RD_SClkFeedbackScale					0x217
#define PM3RD_SClkPostScale					0x218

#define PM3RD_CursorPalette(p)				(0x303 + (p))
#define PM3RD_CursorPattern(p)				(0x400 + (p))
/******************************************************
*  GLINT Permedia3 Video Streaming Registers (0x5000) *
*******************************************************/

#define PM3VSConfiguration					0x5800

/**********************************************
*  GLINT Permedia3 Core Registers (0x8000+)   *
***********************************************/
#define PM3AALineWidth						0x94c0
#define PM3AAPointsize						0x94a0
#define PM3AlphaBlendAlphaMode					0xafa8
#define PM3AlphaBlendAlphaModeAnd				0xad30
#define PM3AlphaBlendAlphaModeOr				0xad38
#define PM3AlphaBlendColorMode					0xafa0
#define PM3AlphaBlendColorModeAnd				0xacb0
#define PM3AlphaBlendColorModeOr				0xacb8
#define PM3AlphaDestColor					0xaf88
#define PM3AlphaSourceColor					0xaf80
#define PM3AlphaTestMode					0x8800
#define PM3AlphaTestModeAnd					0xabf0
#define PM3AlphaTestModeOr					0xabf8
#define PM3AntialiasMode					0x8808
#define PM3AntialiasModeAnd					0xac00
#define PM3AntialiasModeOr					0xac08
/* ... */
#define PM3BackgroundColor					0xb0c8
/* ... */
#define PM3ColorDDAMode						0x87e0
#define PM3ColorDDAModeAnd					0xabe0
#define PM3ColorDDAModeOr					0xabe8
#define PM3CommandInterrupt					0xa990
#define PM3ConstantColorDDA					0xafb0
	#define PM3ConstantColorDDA_R(r)		((r) & 0xff)
	#define PM3ConstantColorDDA_G(g)		(((g) & 0xff) << 8)
	#define PM3ConstantColorDDA_B(b)		(((b) & 0xff) << 16)
	#define PM3ConstantColorDDA_A(a)		(((a) & 0xff) << 24)
#define PM3ContextData						0x8dd0
#define PM3ContextDump						0x8dc0
#define PM3ContextRestore					0x8dc8
#define PM3Continue						0x8058
#define PM3ContinueNewDom					0x8048
#define PM3ContinueNewLine					0x8040
#define PM3ContinueNewSub					0x8050
#define PM3Count						0x8030
/* ... */
#define PM3DeltaControl						0x9350
#define PM3DeltaControlAnd					0xab20
#define PM3DeltaControlOr					0xab28
#define PM3DeltaMode						0x9300
#define PM3DeltaModeAnd						0xaad0
#define PM3DeltaModeOr						0xaad8
/* ... */
#define PM3DitherMode						0x8818
#define PM3DitherModeAnd					0xacd0
#define PM3DitherModeOr						0xacd8
/* ... */
#define PM3dXDom						0x8008
#define PM3dXSub						0x8018
#define PM3dY							0x8028
/* ... */
#define PM3FBBlockColor						0x8ac8
#define PM3FBBlockColor0					0xb060
#define PM3FBBlockColor1					0xb068
#define PM3FBBlockColor2					0xb070
#define PM3FBBlockColor3					0xb078
#define PM3FBBlockColorBack					0xb0a0
#define PM3FBBlockColorBack0					0xb080
#define PM3FBBlockColorBack1					0xb088
#define PM3FBBlockColorBack2					0xb090
#define PM3FBBlockColorBack3					0xb098
#define PM3FBColor						0x8a98
#define PM3FBDestReadBufferAddr0				0xae80
#define PM3FBDestReadBufferAddr1				0xae88
#define PM3FBDestReadBufferAddr2				0xae90
#define PM3FBDestReadBufferAddr3				0xae98
#define PM3FBDestReadBufferOffset0				0xaea0
#define PM3FBDestReadBufferOffset1				0xaea8
#define PM3FBDestReadBufferOffset2				0xaeb0
#define PM3FBDestReadBufferOffset3				0xaeb8
	#define PM3FBDestReadBufferOffset_XOffset(x)	((x) & 0xffff)
	#define PM3FBDestReadBufferOffset_YOffset(y)	(((y) & 0xffff) << 16)
#define PM3FBDestReadBufferWidth0				0xaec0
#define PM3FBDestReadBufferWidth1				0xaec8
#define PM3FBDestReadBufferWidth2				0xaed0
#define PM3FBDestReadBufferWidth3				0xaed8
	#define PM3FBDestReadBufferWidth_Width(w)	((w) & 0x0fff)

#define PM3FBDestReadEnables					0xaee8
#define PM3FBDestReadEnablesAnd					0xad20
#define PM3FBDestReadEnablesOr					0xad28
	#define PM3FBDestReadEnables_E(e)		((e) & 0xff)
	#define PM3FBDestReadEnables_E0				(1 << 0)
	#define PM3FBDestReadEnables_E1				(1 << 1)
	#define PM3FBDestReadEnables_E2				(1 << 2)
	#define PM3FBDestReadEnables_E3				(1 << 3)
	#define PM3FBDestReadEnables_E4				(1 << 4)
	#define PM3FBDestReadEnables_E5				(1 << 5)
	#define PM3FBDestReadEnables_E6				(1 << 6)
	#define PM3FBDestReadEnables_E7				(1 << 7)
	#define PM3FBDestReadEnables_R(r)		(((r) & 0xff) << 8)
	#define PM3FBDestReadEnables_R0				(1 << 8)
	#define PM3FBDestReadEnables_R1				(1 << 9)
	#define PM3FBDestReadEnables_R2				(1 << 10)
	#define PM3FBDestReadEnables_R3				(1 << 11)
	#define PM3FBDestReadEnables_R4				(1 << 12)
	#define PM3FBDestReadEnables_R5				(1 << 13)
	#define PM3FBDestReadEnables_R6				(1 << 14)
	#define PM3FBDestReadEnables_R7				(1 << 15)
	#define PM3FBDestReadEnables_ReferenceAlpha(a)	(((a) & 0xff) << 24)

#define PM3FBDestReadMode					0xaee0
#define PM3FBDestReadModeAnd					0xac90
#define PM3FBDestReadModeOr					0xac98
	#define PM3FBDestReadMode_ReadDisable			(0 << 0)
	#define PM3FBDestReadMode_ReadEnable			(1 << 0)
	#define PM3FBDestReadMode_StripePitch(sp)	(((sp) & 0x7) << 2)
	#define PM3FBDestReadMode_StripeHeight(sh)	(((sh) & 0x7) << 7)
	#define PM3FBDestReadMode_Enable0			(1 << 8)
	#define PM3FBDestReadMode_Enable1			(1 << 9)
	#define PM3FBDestReadMode_Enable2			(1 << 10)
	#define PM3FBDestReadMode_Enable3			(1 << 11)
	#define PM3FBDestReadMode_Layout0(l)		(((l) & 0x3) << 12)
	#define PM3FBDestReadMode_Layout1(l)		(((l) & 0x3) << 14)
	#define PM3FBDestReadMode_Layout2(l)		(((l) & 0x3) << 16)
	#define PM3FBDestReadMode_Layout3(l)		(((l) & 0x3) << 18)
	#define PM3FBDestReadMode_Origin0			(1 << 20)
	#define PM3FBDestReadMode_Origin1			(1 << 21)
	#define PM3FBDestReadMode_Origin2			(1 << 22)
	#define PM3FBDestReadMode_Origin3			(1 << 23)
	#define PM3FBDestReadMode_Blocking			(1 << 24)
	#define PM3FBDestReadMode_UseReadEnabled		(1 << 26)
	#define PM3FBDestReadMode_AlphaFiltering		(1 << 27)

#define PM3FBHardwareWriteMask					0x8ac0
#define PM3FBSoftwareWriteMask					0x8820
#define PM3FBData						0x8aa0
#define PM3FBSourceData						0x8aa8
#define PM3FBSourceReadBufferAddr				0xaf08
#define PM3FBSourceReadBufferOffset				0xaf10
	#define PM3FBSourceReadBufferOffset_XOffset(x)	((x) & 0xffff)
	#define PM3FBSourceReadBufferOffset_YOffset(y)	(((y) & 0xffff) << 16)
#define PM3FBSourceReadBufferWidth				0xaf18
	#define PM3FBSourceReadBufferWidth_Width(w)	((w) & 0x0fff)
#define PM3FBSourceReadMode					0xaf00
#define PM3FBSourceReadModeAnd					0xaca0
#define PM3FBSourceReadModeOr					0xaca8
	#define PM3FBSourceReadMode_ReadDisable			(0 << 0)
	#define PM3FBSourceReadMode_ReadEnable			(1 << 0)
	#define PM3FBSourceReadMode_StripePitch(sp)	(((sp) & 0x7) << 2)
	#define PM3FBSourceReadMode_StripeHeight(sh)	(((sh) & 0x7) << 7)
	#define PM3FBSourceReadMode_Layout(l)		(((l) & 0x3) << 8)
	#define PM3FBSourceReadMode_Origin			(1 << 10)
	#define PM3FBSourceReadMode_Blocking			(1 << 11)
	#define PM3FBSourceReadMode_UserTexelCoord		(1 << 13)
	#define PM3FBSourceReadMode_WrapXEnable			(1 << 14)
	#define PM3FBSourceReadMode_WrapYEnable			(1 << 15)
	#define PM3FBSourceReadMode_WrapX(w)		(((w) & 0xf) << 16)
	#define PM3FBSourceReadMode_WrapY(w)		(((w) & 0xf) << 20)
	#define PM3FBSourceReadMode_ExternalSourceData		(1 << 24)
#define PM3FBWriteBufferAddr0					0xb000
#define PM3FBWriteBufferAddr1					0xb008
#define PM3FBWriteBufferAddr2					0xb010
#define PM3FBWriteBufferAddr3					0xb018

#define PM3FBWriteBufferOffset0					0xb020
#define PM3FBWriteBufferOffset1					0xb028
#define PM3FBWriteBufferOffset2					0xb030
#define PM3FBWriteBufferOffset3					0xb038
	#define PM3FBWriteBufferOffset_XOffset(x)	((x) & 0xffff)
	#define PM3FBWriteBufferOffset_YOffset(y)	(((y) & 0xffff) << 16)

#define PM3FBWriteBufferWidth0					0xb040
#define PM3FBWriteBufferWidth1					0xb048
#define PM3FBWriteBufferWidth2					0xb050
#define PM3FBWriteBufferWidth3					0xb058
	#define PM3FBWriteBufferWidth_Width(w)		((w) & 0x0fff)

#define PM3FBWriteMode						0x8ab8
#define PM3FBWriteModeAnd					0xacf0
#define PM3FBWriteModeOr					0xacf8
	#define PM3FBWriteMode_WriteDisable			(0 << 0)
	#define PM3FBWriteMode_WriteEnable			(1 << 0)
	#define PM3FBWriteMode_Replicate			(1 << 4)
	#define PM3FBWriteMode_OpaqueSpan			(1 << 5)
	#define PM3FBWriteMode_StripePitch(p)		(((p) & 0x7) << 6)
	#define PM3FBWriteMode_StripeHeight(h)		(((h) & 0x7) << 9)
	#define PM3FBWriteMode_Enable0				(1 << 12)
	#define PM3FBWriteMode_Enable1				(1 << 13)
	#define PM3FBWriteMode_Enable2				(1 << 14)
	#define PM3FBWriteMode_Enable3				(1 << 15)
	#define PM3FBWriteMode_Layout0(l)		(((l) & 0x3) << 16)
	#define PM3FBWriteMode_Layout1(l)		(((l) & 0x3) << 18)
	#define PM3FBWriteMode_Layout2(l)		(((l) & 0x3) << 20)
	#define PM3FBWriteMode_Layout3(l)		(((l) & 0x3) << 22)
	#define PM3FBWriteMode_Origin0				(1 << 24)
	#define PM3FBWriteMode_Origin1				(1 << 25)
	#define PM3FBWriteMode_Origin2				(1 << 26)
	#define PM3FBWriteMode_Origin3				(1 << 27)
#define PM3ForegroundColor					0xb0c0
/* ... */
#define PM3GIDMode						0xb538
#define PM3GIDModeAnd						0xb5b0
#define PM3GIDModeOr						0xb5b8
/* ... */
#define PM3LBDestReadBufferAddr					0xb510
#define PM3LBDestReadBufferOffset				0xb518
#define PM3LBDestReadEnables					0xb508
#define PM3LBDestReadEnablesAnd					0xb590
#define PM3LBDestReadEnablesOr					0xb598
#define PM3LBDestReadMode					0xb500
#define PM3LBDestReadModeAnd					0xb580
#define PM3LBDestReadModeOr					0xb588
	#define PM3LBDestReadMode_Disable			(0 << 0)
	#define PM3LBDestReadMode_Enable			(1 << 0)
	#define PM3LBDestReadMode_StripePitch(p)	(((p) & 0x7) << 2)
	#define PM3LBDestReadMode_StripeHeight(h)	(((h) & 0x7) << 5)
	#define PM3LBDestReadMode_Layout			(1 << 8)
	#define PM3LBDestReadMode_Origin			(1 << 9)
	#define PM3LBDestReadMode_UserReadEnables		(1 << 10)
	#define PM3LBDestReadMode_Packed16			(1 << 11)
	#define PM3LBDestReadMode_Width(w)		(((w) & 0xfff) << 12)
#define PM3LBReadFormat						0x8888
	#define PM3LBReadFormat_DepthWidth(w)		(((w) & 0x3) << 0)
	#define PM3LBReadFormat_StencilWidth(w)		(((w) & 0xf) << 2)
	#define PM3LBReadFormat_StencilPosition(p)	(((p) & 0x1f) << 6)
	#define PM3LBReadFormat_FCPWidth(w)		(((w) & 0xf) << 11)
	#define PM3LBReadFormat_FCPPosition(p)		(((p) & 0x1f) << 15)
	#define PM3LBReadFormat_GIDWidth(w)		(((w) & 0x7) << 20)
	#define PM3LBReadFormat_GIDPosition(p)		(((p) & 0x1f) << 23)
#define PM3LBSourceReadBufferAddr				0xb528
#define PM3LBSourceReadBufferOffset				0xb530
#define PM3LBSourceReadMode					0xb520
#define PM3LBSourceReadModeAnd					0xb5a0
#define PM3LBSourceReadModeOr					0xb5a8
	#define PM3LBSourceReadMode_Enable			(1 << 0)
	#define PM3LBSourceReadMode_StripePitch(p)	(((p) & 0x7) << 2)
	#define PM3LBSourceReadMode_StripeHeight(h)	(((h) & 0x7) << 5)
	#define PM3LBSourceReadMode_Layout			(1 << 8)
	#define PM3LBSourceReadMode_Origin			(1 << 9)
	#define PM3LBSourceReadMode_Packed16			(1 << 10)
	#define PM3LBSourceReadMode_Width(w)		(((w) & 0xfff) << 11)
#define PM3LBStencil						0x88a8
#define PM3LBWriteBufferAddr					0xb540
#define PM3LBWriteBufferOffset					0xb548
#define PM3LBWriteFormat					0x88c8
	#define PM3LBWriteFormat_DepthWidth(w)		(((w) & 0x3) << 0)
	#define PM3LBWriteFormat_StencilWidth(w)	(((w) & 0xf) << 2)
	#define PM3LBWriteFormat_StencilPosition(p)	(((p) & 0x1f) << 6)
	#define PM3LBWriteFormat_GIDWidth(w)		(((w) & 0x7) << 20)
	#define PM3LBWriteFormat_GIDPosition(p)		(((p) & 0x1f) << 23)
#define PM3LBWriteMode						0x88c0
#define PM3LBWriteModeAnd					0xac80
#define PM3LBWriteModeOr					0xac88
	#define PM3LBWriteMode_WriteDisable			(0 << 0)
	#define PM3LBWriteMode_WriteEnable			(1 << 0)
	#define PM3LBWriteMode_StripePitch(p)		(((p) & 0x7) << 3)
	#define PM3LBWriteMode_StripeHeight(h)		(((h) & 0x7) << 6)
	#define PM3LBWriteMode_Layout				(1 << 9)
	#define PM3LBWriteMode_Origin				(1 << 10)
	#define PM3LBWriteMode_Packed16				(1 << 11)
	#define PM3LBWriteMode_Width(w)			(((w) & 0xfff) << 12)
/* ... */
#define PM3LineStippleMode					0x81a8
#define PM3LineStippleModeAnd					0xabc0
#define PM3LineStippleModeOr					0xabc8
#define PM3LoadLineStippleCounters				0x81b0
/* ... */
#define PM3LogicalOpMode					0x8828
#define PM3LogicalOpModeAnd					0xace0
#define PM3LogicalOpModeOr					0xace8
	#define PM3LogicalOpMode_Disable			(0 << 0)
	#define PM3LogicalOpMode_Enable				(1 << 0)
	#define PM3LogicalOpMode_LogicOp(op)		(((op) & 0xf) << 1)
	#define PM3LogicalOpMode_UseConstantWriteData_Disable	(0 << 5)
	#define PM3LogicalOpMode_UseConstantWriteData_Enable	(1 << 5)
	#define PM3LogicalOpMode_Background_Disable		(0 << 6)
	#define PM3LogicalOpMode_Background_Enable		(1 << 6)
	#define PM3LogicalOpMode_Background_LogicOp(op)	(((op) & 0xf) << 7)
	#define PM3LogicalOpMode_UseConstantSource_Disable	(0 << 11)
	#define PM3LogicalOpMode_UseConstantSource_Enable	(1 << 11)
/* ... */
#define PM3LUT							0x8e80
#define PM3LUTAddress						0x84d0
#define PM3LUTData						0x84c8
#define PM3LUTIndex						0x84c0
#define PM3LUTMode						0xb378
#define PM3LUTModeAnd						0xad70
#define PM3LUTModeOr						0xad78
#define PM3LUTTransfer						0x84d8
/* ... */
#define PM3PixelSize						0x80c0
	#define PM3PixelSize_GLOBAL_32BIT			(0 << 0)
	#define PM3PixelSize_GLOBAL_16BIT			(1 << 0)
	#define PM3PixelSize_GLOBAL_8BIT			(2 << 0)
	#define PM3PixelSize_RASTERIZER_32BIT			(0 << 2)
	#define PM3PixelSize_RASTERIZER_16BIT			(1 << 2)
	#define PM3PixelSize_RASTERIZER_8BIT			(2 << 2)
	#define PM3PixelSize_SCISSOR_AND_STIPPLE_32BIT		(0 << 4)
	#define PM3PixelSize_SCISSOR_AND_STIPPLE_16BIT		(1 << 4)
	#define PM3PixelSize_SCISSOR_AND_STIPPLE_8BIT		(2 << 4)
	#define PM3PixelSize_TEXTURE_32BIT			(0 << 6)
	#define PM3PixelSize_TEXTURE_16BIT			(1 << 6)
	#define PM3PixelSize_TEXTURE_8BIT			(2 << 6)
	#define PM3PixelSize_LUT_32BIT				(0 << 8)
	#define PM3PixelSize_LUT_16BIT				(1 << 8)
	#define PM3PixelSize_LUT_8BIT				(2 << 8)
	#define PM3PixelSize_FRAMEBUFFER_32BIT			(0 << 10)
	#define PM3PixelSize_FRAMEBUFFER_16BIT			(1 << 10)
	#define PM3PixelSize_FRAMEBUFFER_8BIT			(2 << 10)
	#define PM3PixelSize_LOGICAL_OP_32BIT			(0 << 12)
	#define PM3PixelSize_LOGICAL_OP_16BIT			(1 << 12)
	#define PM3PixelSize_LOGICAL_OP_8BIT			(2 << 12)
	#define PM3PixelSize_LOCALBUFFER_32BIT			(0 << 14)
	#define PM3PixelSize_LOCALBUFFER_16BIT			(1 << 14)
	#define PM3PixelSize_LOCALBUFFER_8BIT			(2 << 14)
	#define PM3PixelSize_SETUP_32BIT			(0 << 16)
	#define PM3PixelSize_SETUP_16BIT			(1 << 16)
	#define PM3PixelSize_SETUP_8BIT				(2 << 16)
	#define PM3PixelSize_GLOBAL				(0 << 31)
	#define PM3PixelSize_INDIVIDUAL				(1 << 31)
/* ... */
#define PM3Render						0x8038
	#define PM3Render_AreaStipple_Disable			(0 << 0)
	#define PM3Render_AreaStipple_Enable			(1 << 0)
	#define PM3Render_LineStipple_Disable			(0 << 1)
	#define PM3Render_LineStipple_Enable			(1 << 1)
	#define PM3Render_ResetLine_Disable			(0 << 2)
	#define PM3Render_ResetLine_Enable			(1 << 2)
	#define PM3Render_FastFill_Disable			(0 << 3)
	#define PM3Render_FastFill_Enable			(1 << 3)
	#define PM3Render_Primitive_Line			(0 << 6)
	#define PM3Render_Primitive_Trapezoid			(1 << 6)
	#define PM3Render_Primitive_Point			(2 << 6)
	#define PM3Render_Antialias_Disable			(0 << 8)
	#define PM3Render_Antialias_Enable			(1 << 8)
	#define PM3Render_Antialias_SubPixelRes_4x4		(0 << 9)
	#define PM3Render_Antialias_SubPixelRes_8x8		(1 << 9)
	#define PM3Render_UsePointTable_Disable			(0 << 10)
	#define PM3Render_UsePointTable_Enable			(1 << 10)
	#define PM3Render_SyncOnbitMask_Disable			(0 << 11)
	#define PM3Render_SyncOnBitMask_Enable			(1 << 11)
	#define PM3Render_SyncOnHostData_Disable		(0 << 12)
	#define PM3Render_SyncOnHostData_Enable			(1 << 12)
	#define PM3Render_Texture_Disable			(0 << 13)
	#define PM3Render_Texture_Enable			(1 << 13)
	#define PM3Render_Fog_Disable				(0 << 14)
	#define PM3Render_Fog_Enable				(1 << 14)
	#define PM3Render_Coverage_Disable			(0 << 15)
	#define PM3Render_Coverage_Enable			(1 << 15)
	#define PM3Render_SubPixelCorrection_Disable		(0 << 16)
	#define PM3Render_SubPixelCorrection_Enable		(1 << 16)
	#define PM3Render_SpanOperation_Disable			(0 << 18)
	#define PM3Render_SpanOperation_Enable			(1 << 18)
	#define PM3Render_FBSourceRead_Disable			(0 << 27)
	#define PM3Render_FBSourceRead_Enable			(1 << 27)
#define PM3RasterizerMode					0x80a0
#define PM3RasterizerModeAnd					0xaba0
#define PM3RasterizerModeOr					0xaba8
#define PM3RectangleHeight					0x94e0
#define PM3RepeatLine						0x9328
#define PM3ResetPickResult					0x8c20
#define PM3RLEMask						0x8c48
#define PM3RouterMode						0x8840
#define PM3RStart						0x8780
#define PM3S1Start						0x8400
#define PM3aveLineStippleCounters				0x81c0
#define PM3ScissorMaxXY						0x8190
#define PM3ScissorMinXY						0x8188
#define PM3ScissorMode						0x8180
#define PM3ScissorModeAnd					0xabb0
#define PM3ScissorModeOr					0xabb8
#define PM3ScreenSize						0x8198
#define PM3Security						0x8908
#define PM3SetLogicalTexturePage				0xb360
#define PM3SizeOfFramebuffer					0xb0a8
#define PM3SStart						0x8388
#define PM3StartXDom						0x8000
#define PM3StartXSub						0x8010
#define PM3StartY						0x8020
/* ... */
#define PM3SpanColorMask					0x8168
/* ... */
#define PM3TextureApplicationMode				0x8680
#define PM3TextureApplicationModeAnd				0xac50
#define PM3TextureApplicationModeOr				0xac58
#define PM3TextureBaseAddr					0x8500
#define PM3TextureCacheControl					0x8490
#define PM3TextureChromaLower0					0x84f0
#define PM3TextureChromaLower1					0x8608
#define PM3TextureChromaUpper0					0x84e8
#define PM3TextureChromaUpper1					0x8600
#define PM3TextureCompositeAlphaMode0				0xb310
#define PM3TextureCompositeAlphaMode0And			0xb390
#define PM3TextureCompositeAlphaMode0Or				0xb398
#define PM3TextureCompositeAlphaMode1				0xb320
#define PM3TextureCompositeAlphaMode1And			0xb3b0
#define PM3TextureCompositeAlphaMode1Or				0xb3b8
#define PM3TextureCompositeColorMode0				0xb308
#define PM3TextureCompositeColorMode0And			0xb380
#define PM3TextureCompositeColorMode0Or				0xb388
#define PM3TextureCompositeColorMode1				0xb318
#define PM3TextureCompositeColorMode1And			0xb3a0
#define PM3TextureCompositeColorMode1Or				0xb3a8
#define PM3TextureCompositeFactor0				0xb328
#define PM3TextureCompositeFactor1				0xb330
#define PM3TextureCompositeMode					0xb300
#define PM3TextureCoordMode					0x8380
#define PM3TextureCoordModeAnd					0xac20
#define PM3TextureCoordModeOr					0xac28
#define PM3TextureData						0x88e8
/*
#define PM3TextureDownloadControl				0x0108
*/
#define PM3TextureDownloadOffset				0x88f0
#define PM3TextureEnvColor					0x8688
#define PM3TextureFilterMode					0x84e0
#define PM3TextureFilterModeAnd					0xad50
#define PM3TextureFilterModeOr					0xad58
#define PM3TextureIndexMode0					0xb338
#define PM3TextureIndexMode0And					0xb3c0
#define PM3TextureIndexMode0Or					0xb3c8
#define PM3TextureIndexMode1					0xb340
#define PM3TextureIndexMode1And					0xb3d0
#define PM3TextureIndexMode1Or					0xb3d8
/* ... */
#define PM3TextureMapSize					0xb428
#define PM3TextureMapWidth0					0x8580
#define PM3TextureMapWidth1					0x8588
	#define PM3TextureMapWidth_Width(w)		(((w) & 0xfff) << 0)
	#define PM3TextureMapWidth_BorderLayout			(1 << 12)
	#define PM3TextureMapWidth_Layout_Linear		(0 << 13)
	#define PM3TextureMapWidth_Layout_Patch64		(1 << 13)
	#define PM3TextureMapWidth_Layout_Patch32_2		(2 << 13)
	#define PM3TextureMapWidth_Layout_Patch2		(3 << 13)
	#define PM3TextureMapWidth_HostTexture			(1 << 15)
#define PM3TextureReadMode0					0xb400
#define PM3TextureReadMode0And					0xac30
#define PM3TextureReadMode0Or					0xac38
#define PM3TextureReadMode1					0xb408
#define PM3TextureReadMode1And					0xad40
#define PM3TextureReadMode1Or					0xad48
/* ... */
#define PM3WaitForCompletion					0x80b8
#define PM3Window						0x8980
	#define PM3Window_ForceLBUpdate				(1 << 3)
	#define PM3Window_LBUpdateSource			(1 << 4)
	#define PM3Window_FrameCount(c)			(((c) & 0xff) << 9)
	#define PM3Window_StencilFCP				(1 << 17)
	#define PM3Window_DepthFCP				(1 << 18)
	#define PM3Window_OverrideWriteFiltering		(1 << 19)
#define PM3WindowAnd						0xab80
#define PM3WindowOr						0xab88
#define PM3WindowOrigin						0x81c8
#define PM3XBias						0x9480
#define PM3YBias						0x9488
#define PM3YLimits						0x80a8
#define PM3UVMode						0x8f00
#define PM3ZFogBias						0x86b8
#define PM3ZStart						0xadd8
#define PM3ZStartL						0x89b8
#define PM3ZStartU						0x89b0


/**********************************************
*  GLINT Permedia3 2D setup Unit	      *
***********************************************/
#define PM3Config2D						0xb618
	#define PM3Config2D_OpaqueSpan				(1 << 0)
	#define PM3Config2D_MultiRXBlit				(1 << 1)
	#define PM3Config2D_UserScissorEnable			(1 << 2)
	#define PM3Config2D_FBDestReadEnable			(1 << 3)
	#define PM3Config2D_AlphaBlendEnable			(1 << 4)
	#define PM3Config2D_DitherEnable			(1 << 5)
	#define PM3Config2D_ForegroundROPEnable			(1 << 6)
	#define PM3Config2D_ForegroundROP(rop)		(((rop) & 0xf) << 7)
	#define PM3Config2D_BackgroundROPEnable			(1 << 11)
	#define PM3Config2D_BackgroundROP(rop)		(((rop) & 0xf) << 12)
	#define PM3Config2D_UseConstantSource			(1 << 16)
	#define PM3Config2D_FBWriteEnable			(1 << 17)
	#define PM3Config2D_Blocking				(1 << 18)
	#define PM3Config2D_ExternalSourceData			(1 << 19)
	#define PM3Config2D_LUTModeEnable			(1 << 20)
#define PM3DownloadGlyphwidth					0xb658
	#define PM3DownloadGlyphwidth_GlyphWidth(gw)	((gw) & 0xffff)
#define PM3DownloadTarget					0xb650
	#define PM3DownloadTarget_TagName(tag)		((tag) & 0x1fff)
#define PM3GlyphData						0xb660
#define PM3GlyphPosition					0xb608
	#define PM3GlyphPosition_XOffset(x)		((x) & 0xffff)
	#define PM3GlyphPosition_YOffset(y)		(((y) & 0xffff) << 16)
#define PM3Packed4Pixels					0xb668
#define PM3Packed8Pixels					0xb630
#define PM3Packed16Pixels					0xb638
#define PM3RectanglePosition					0xb600
	#define PM3RectanglePosition_XOffset(x)		((x) & 0xffff)
	#define PM3RectanglePosition_YOffset(y)		(((y) & 0xffff) << 16)
#define PM3Render2D						0xb640
	#define PM3Render2D_Width(w)			((w) & 0x0fff)
	#define PM3Render2D_Operation_Normal			(0 << 12)
	#define PM3Render2D_Operation_SyncOnHostData		(1 << 12)
	#define PM3Render2D_Operation_SyncOnBitMask		(2 << 12)
	#define PM3Render2D_Operation_PatchOrderRendering	(3 << 12)
	#define PM3Render2D_FBSourceReadEnable			(1 << 14)
	#define PM3Render2D_SpanOperation			(1 << 15)
	#define PM3Render2D_Height(h)			(((h) & 0x0fff) << 16)
	#define PM3Render2D_XPositive				(1 << 28)
	#define PM3Render2D_YPositive				(1 << 29)
	#define PM3Render2D_AreaStippleEnable			(1 << 30)
	#define PM3Render2D_TextureEnable			(1 << 31)
#define PM3Render2DGlyph					0xb648
	#define PM3Render2DGlyph_Width(w)		((w) & 0x7f)
	#define PM3Render2DGlyph_Height(h)		(((h) & 0x7f) << 7)
	#define PM3Render2DGlyph_XOffset(x)		(((x) & 0x1ff) << 14)
	#define PM3Render2DGlyph_YOffset(y)		(((y) & 0x1ff) << 23)
#define PM3RenderPatchOffset					0xb610
	#define PM3RenderPatchOffset_XOffset(x)		((x) & 0xffff)
	#define PM3RenderPatchOffset_YOffset(y)		(((y) & 0xffff) << 16)
#define PM3RLCount						0xb678
	#define PM3RLCount_Count(c)			((c) & 0x0fff)
#define PM3RLData						0xb670

/**********************************************
*  GLINT Permedia3 Alias Register	     *
***********************************************/
#define PM3FillBackgroundColor					0x8330
#define PM3FillConfig2D0					0x8338
#define PM3FillConfig2D1					0x8360
	#define PM3FillConfig2D_OpaqueSpan			(1 << 0)
	#define PM3FillConfig2D_MultiRXBlit			(1 << 1)
	#define PM3FillConfig2D_UserScissorEnable		(1 << 2)
	#define PM3FillConfig2D_FBDestReadEnable		(1 << 3)
	#define PM3FillConfig2D_AlphaBlendEnable		(1 << 4)
	#define PM3FillConfig2D_DitherEnable			(1 << 5)
	#define PM3FillConfig2D_ForegroundROPEnable		(1 << 6)
	#define PM3FillConfig2D_ForegroundROP(rop)	(((rop) & 0xf) << 7)
	#define PM3FillConfig2D_BackgroundROPEnable		(1 << 11)
	#define PM3FillConfig2D_BackgroundROP(rop)	(((rop) & 0xf) << 12)
	#define PM3FillConfig2D_UseConstantSource		(1 << 16)
	#define PM3FillConfig2D_FBWriteEnable			(1 << 17)
	#define PM3FillConfig2D_Blocking			(1 << 18)
	#define PM3FillConfig2D_ExternalSourceData		(1 << 19)
	#define PM3FillConfig2D_LUTModeEnable			(1 << 20)
#define PM3FillFBDestReadBufferAddr				0x8310
#define PM3FillFBSourceReadBufferAddr				0x8308
#define PM3FillFBSourceReadBufferOffset				0x8340
	#define PM3FillFBSourceReadBufferOffset_XOffset(x) ((x) & 0xffff)
	#define PM3FillFBSourceReadBufferOffset_YOffset(y)	\
						(((y) & 0xffff) << 16)
#define PM3FillFBWriteBufferAddr				0x8300
#define PM3FillForegroundColor0					0x8328
#define PM3FillForegroundColor1					0x8358
#define PM3FillGlyphPosition					0x8368
	#define PM3FillGlyphPosition_XOffset(x)		((x) & 0xffff)
	#define PM3FillGlyphPosition_YOffset(y)		(((y) & 0xffff) << 16)
#define PM3FillRectanglePosition				0x8348
	#define PM3FillRectanglePosition_XOffset(x)	((x) & 0xffff)
	#define PM3FillRectanglePosition_YOffset(y)	(((y) & 0xffff) << 16)

/* a few more useful registers & regs value... */
#define PM3Sync							0x8c40
	#define PM3Sync_Tag					0x188
#define PM3FilterMode						0x8c00
	#define PM3FilterModeSync				0x400
#define PM3OutputFifo						0x2000
#define PM3StatisticMode					0x8c08
#define PM3AreaStippleMode					0x81a0
#define AreaStipplePattern_indexed(i)		(0x8200 + ((i) * 0x8))

#define PM3DepthMode						0x89a0
#define PM3StencilMode						0x8988
#define PM3StencilData						0x8990
#define PM3TextureReadMode					0x8670
#define PM3FogMode						0x8690
#define PM3ChromaTestMode					0x8f18
#define PM3YUVMode						0x8f00
#define PM3BitMaskPattern					0x8068

/* ***************************** */
/* ***** pm3fb IOCTL const ***** */
/* ***************************** */
#define PM3FBIO_RESETCHIP		0x504D33FF /* 'PM3\377' */

/* ***************************************** */
/* ***** pm3fb useful define and macro ***** */
/* ***************************************** */

/* fifo size in chip */
#define PM3_FIFO_SIZE						120
#define PM3_REGS_SIZE						0x10000
#define PM3_MAX_PIXCLOCK					300000

#endif /* PM3FB_H */
