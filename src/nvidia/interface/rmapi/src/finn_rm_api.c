#include "finn_rm_api.h"
#include "ctrl/ctrl0000/ctrl0000nvd.h"
#include "ctrl/ctrl0080/ctrl0080dma.h"
#include "ctrl/ctrl0080/ctrl0080fb.h"
#include "ctrl/ctrl0080/ctrl0080fifo.h"
#include "ctrl/ctrl0080/ctrl0080gpu.h"
#include "ctrl/ctrl0080/ctrl0080gr.h"
#include "ctrl/ctrl0080/ctrl0080host.h"
#include "ctrl/ctrl0080/ctrl0080msenc.h"
#include "ctrl/ctrl2080/ctrl2080ce.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080i2c.h"
#include "ctrl/ctrl2080/ctrl2080nvd.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"
#include "ctrl/ctrl2080/ctrl2080rc.h"
#include "ctrl/ctrl402c.h"
#include "ctrl/ctrl83de/ctrl83dedebug.h"
#include "ctrl/ctrlb06f.h"

#include <stddef.h>
#if defined(NVRM) /* Kernel Mode */
#include "nvport/nvport.h"
#else /* User Mode */
#include <stdlib.h>
#include <string.h>
#endif


//
// This file was generated with FINN, an NVIDIA coding tool.
//

/*
 * Serialization helper macros. These make field copying code more readable.
 * Size is provided explicitly for cross-platform compatibility.
 */

// Allocate memory.
#if defined(FINN_MALLOC) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_MALLOC(size) portMemAllocNonPaged(size)

#else /* User Mode */
#define FINN_MALLOC(size) malloc(size)
#endif

// Free allocated memory.
#if defined(FINN_FREE) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_FREE(buf) portMemFree(buf)

#else /* User Mode */
#define FINN_FREE(buf) free(buf)
#endif

// Set memory region to all zeroes.
#if defined(FINN_MEMZERO) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_MEMZERO(buf, size) portMemSet(buf, 0, size)

#else /* User Mode */
#define FINN_MEMZERO(buf, size) memset(buf, 0, size)
#endif

// Copy nonoverlapping memory region.
#if defined(FINN_MEMCPY) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_MEMCPY(dst, src, size) portMemCopy(dst, size, src, size)

#else /* User Mode */
#define FINN_MEMCPY(dst, src, size) memcpy(dst, src, size)
#endif

// Report an error.
#if defined(FINN_ERROR) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_ERROR(err) /* No-op */

#else /* User Mode */
#define FINN_ERROR(err) /* No-op */
#endif

// Copy val into buf as type and increment buf by size.
#define FINN_COPY_TO_BUFFER(buf, val, type, size)                              \
    do {                                                                       \
        *((type *)(buf)) = (val);                                              \
        (buf) += (size);                                                       \
    } while(0)

// Copy buf into var as type and increment buf by size.
#define FINN_COPY_FROM_BUFFER(var, buf, type, size)                            \
    do {                                                                       \
        (var) = *((type *)(buf));                                              \
        (buf) += (size);                                                       \
    } while(0)

// Copy size bytes from src to dst and increment dst by size.
#define FINN_MEMCPY_TO_BUFFER(dst, src, size)                                  \
    do {                                                                       \
        FINN_MEMCPY((dst), (src), (size));               \
        (dst) += (size);                                                       \
    } while(0)

// Copy size bytes from src to dst and increment src by size.
#define FINN_MEMCPY_FROM_BUFFER(dst, src, size)                                \
    do {                                                                       \
        FINN_MEMCPY((dst), (src), (size));               \
        (src) += (size);                                                       \
    } while(0)

// Set ptr to buf as type and increment buf by size.
#define FINN_SET_PTR_TO_BUFFER(ptr, buf, type, size)                           \
    do {                                                                       \
        (ptr) = (type)(NvUPtr)(buf);                                           \
        (buf) += (size);                                                       \
    } while(0)

// Align a byte pointer up to the 8-byte boundary.
#define FINN_ALIGN_UP_BYTE_PTR(ptr)                                            \
    do {                                                                       \
        (ptr) = (NvU8 *)(((NvUPtr)(ptr) + 7) &~ 7);                            \
    } while(0)                                                                 \

NV_STATUS FinnRmApiSerializeInternal(NvU64 interface, NvU64 message, const char *src, char **dst, NvLength dst_size, NvBool seri_up);
NV_STATUS FinnRmApiDeserializeInternal(char * const *src, NvLength src_size, char *dst, NvLength dst_size, NvBool deser_up);

static NV_STATUS FinnNv01RootNvdSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01RootNvdDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_ROOT_NVD *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01RootNvdGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01RootNvdGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0DmaSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01Device0DmaDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_DMA *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0DmaGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01Device0DmaGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0FbSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01Device0FbDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_FB *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0FbGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01Device0FbGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0FifoSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01Device0FifoDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_FIFO *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0FifoGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01Device0FifoGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0GpuSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01Device0GpuDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0GpuGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01Device0GpuGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0GrSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01Device0GrDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_GR *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0GrGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01Device0GrGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0HostSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01Device0HostDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_HOST *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0HostGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01Device0HostGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0MsencSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv01Device0MsencDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_MSENC *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0MsencGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv01Device0MsencGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0CeSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0CeDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_CE *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0CeGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv20Subdevice0CeGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0GpuSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0GpuDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0GpuGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv20Subdevice0GpuGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0I2cSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0I2cDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_I2C *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0I2cGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv20Subdevice0I2cGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0NvdSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0NvdDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_NVD *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0NvdGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv20Subdevice0NvdGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0PerfSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0PerfDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_PERF *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0PerfGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv20Subdevice0PerfGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0RcSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0RcDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_RC *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0RcGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv20Subdevice0RcGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv40I2cI2cSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnNv40I2cI2cDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV40_I2C_I2C *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv40I2cI2cGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnNv40I2cI2cGetUnserializedSize(NvU64 message);
static NV_STATUS FinnGt200DebuggerDebugSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnGt200DebuggerDebugDeserialize(NvU8 **src, const NvU8 *src_max, FINN_GT200_DEBUGGER_DEBUG *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnGt200DebuggerDebugGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnGt200DebuggerDebugGetUnserializedSize(NvU64 message);
static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoDeserialize(NvU8 **src, const NvU8 *src_max, FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnMaxwellChannelGpfifoAGpfifoGetSerializedSize(NvU64 message, const NvP64 src);
static NvU64 FinnMaxwellChannelGpfifoAGpfifoGetUnserializedSize(NvU64 message);

static NV_STATUS Nv0000CtrlNvdGetDumpParamsSerialize(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0000CtrlNvdGetDumpParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0000_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0000CtrlNvdGetDumpParamsGetSerializedSize(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *src);
static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlDmaUpdatePde2PageTableParamsGetSerializedSize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *src);
static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlDmaUpdatePde2ParamsGetSerializedSize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *src);
static NV_STATUS Nv0080CtrlFbGetCapsParamsSerialize(const NV0080_CTRL_FB_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlFbGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FB_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlFbGetCapsParamsGetSerializedSize(const NV0080_CTRL_FB_GET_CAPS_PARAMS *src);
static NV_STATUS Nv0080CtrlFifoGetCapsParamsSerialize(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlFifoGetCapsParamsGetSerializedSize(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *src);
static NV_STATUS Nv0080CtrlFifoChannelSerialize(const NV0080_CTRL_FIFO_CHANNEL *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoChannelDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_CHANNEL *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlFifoChannelGetSerializedSize(const NV0080_CTRL_FIFO_CHANNEL *src);
static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsSerialize(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlFifoStartSelectedChannelsParamsGetSerializedSize(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *src);
static NV_STATUS Nv0080CtrlFifoGetChannellistParamsSerialize(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoGetChannellistParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlFifoGetChannellistParamsGetSerializedSize(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *src);
static NV_STATUS Nv0080CtrlGpuGetClasslistParamsSerialize(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlGpuGetClasslistParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlGpuGetClasslistParamsGetSerializedSize(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *src);
static NV_STATUS Nv0080CtrlGrGetCapsParamsSerialize(const NV0080_CTRL_GR_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlGrGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_GR_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlGrGetCapsParamsGetSerializedSize(const NV0080_CTRL_GR_GET_CAPS_PARAMS *src);
static NV_STATUS Nv0080CtrlHostGetCapsParamsSerialize(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlHostGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_HOST_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlHostGetCapsParamsGetSerializedSize(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *src);
static NV_STATUS Nv0080CtrlMsencGetCapsParamsSerialize(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv0080CtrlMsencGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_MSENC_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv0080CtrlMsencGetCapsParamsGetSerializedSize(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *src);
static NV_STATUS Nv2080CtrlCeGetCapsParamsSerialize(const NV2080_CTRL_CE_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv2080CtrlCeGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_CE_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv2080CtrlCeGetCapsParamsGetSerializedSize(const NV2080_CTRL_CE_GET_CAPS_PARAMS *src);
static NV_STATUS Nv2080CtrlGpuGetEnginesParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv2080CtrlGpuGetEnginesParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv2080CtrlGpuGetEnginesParamsGetSerializedSize(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *src);
static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv2080CtrlGpuGetEngineClasslistParamsGetSerializedSize(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *src);
static NV_STATUS Nv2080CtrlGpumonSamplesSerialize(const NV2080_CTRL_GPUMON_SAMPLES *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up, NvU64 interface, NvU64 message);
static NV_STATUS Nv2080CtrlGpumonSamplesDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_GPUMON_SAMPLES *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv2080CtrlGpumonSamplesGetSerializedSize(const NV2080_CTRL_GPUMON_SAMPLES *src);
static NV_STATUS Nv2080CtrlI2cAccessParamsSerialize(const NV2080_CTRL_I2C_ACCESS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv2080CtrlI2cAccessParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_I2C_ACCESS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv2080CtrlI2cAccessParamsGetSerializedSize(const NV2080_CTRL_I2C_ACCESS_PARAMS *src);
static NV_STATUS Nv2080CtrlNvdGetDumpParamsSerialize(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv2080CtrlNvdGetDumpParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv2080CtrlNvdGetDumpParamsGetSerializedSize(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *src);
static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsSerialize(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv2080CtrlRcReadVirtualMemParamsGetSerializedSize(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *src);
static NV_STATUS Nv402cCtrlI2cIndexedParamsSerialize(const NV402C_CTRL_I2C_INDEXED_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cIndexedParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_INDEXED_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cIndexedParamsGetSerializedSize(const NV402C_CTRL_I2C_INDEXED_PARAMS *src);
static NV_STATUS Nv402cCtrlI2cTransactionTypeValueToId(NvU8 **buf, const NvU8 *buf_max, NvU64 convert_size);
static NV_STATUS Nv402cCtrlI2cTransactionTypeIdtoValue(NvU8 **buf, const NvU8 *buf_max, NvU64 convert_size);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataSmbusQuickRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataI2cByteRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataI2cBlockRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataSmbusByteRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataSmbusWordRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataI2cBufferRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataSmbusBlockRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataSmbusProcessCallGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionDataReadEdidDdcGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *src);
static NV_STATUS Nv402cCtrlI2cTransactionDataSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType);
static NV_STATUS Nv402cCtrlI2cTransactionDataDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA *dst, NvLength dst_size, NvBool deser_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType);
static NvU64 Nv402cCtrlI2cTransactionDataGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA *src, NV402C_CTRL_I2C_TRANSACTION_TYPE transType);
static NV_STATUS Nv402cCtrlI2cTransactionParamsSerialize(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv402cCtrlI2cTransactionParamsGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *src);
static NV_STATUS Nv83deCtrlDebugReadMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv83deCtrlDebugReadMemoryParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv83deCtrlDebugReadMemoryParamsGetSerializedSize(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *src);
static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nv83deCtrlDebugWriteMemoryParamsGetSerializedSize(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *src);
static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsSerialize(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsDeserialize(NvU8 **src, const NvU8 *src_max, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nvb06fCtrlGetEngineCtxDataParamsGetSerializedSize(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *src);
static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsSerialize(const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up);
static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsDeserialize(NvU8 **src, const NvU8 *src_max, NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NvU64 Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsGetSerializedSize(const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *src);

NV_STATUS FinnRmApiSerializeUp(NvU64 interface, NvU64 message, const void *src, NvU8 **dst, NvLength dst_size)
{
    return FinnRmApiSerializeInternal(interface, message, (const char *) src, (char **) dst, dst_size / sizeof(NvU8), NV_TRUE);
}

NV_STATUS FinnRmApiSerializeDown(NvU64 interface, NvU64 message, const void *src, NvU8 **dst, NvLength dst_size)
{
    return FinnRmApiSerializeInternal(interface, message, (const char *) src, (char **) dst, dst_size / sizeof(NvU8), NV_FALSE);
}

NV_STATUS FinnRmApiDeserializeDown(NvU8 **src, NvLength src_size, void *dst, NvLength dst_size)
{
    return FinnRmApiDeserializeInternal((char **) src, src_size / sizeof(NvU8), (char *) dst, dst_size, NV_FALSE);
}

NV_STATUS FinnRmApiDeserializeUp(NvU8 * const *src, NvLength src_size, void *dst, NvLength dst_size)
{
    return FinnRmApiDeserializeInternal((char **) src, src_size / sizeof(NvU8), (char *) dst, dst_size, NV_TRUE);
}


NV_STATUS FinnRmApiSerializeInternal(NvU64 interface, NvU64 message, const char *src, char **dst, NvLength dst_size, NvBool seri_up)
{
    const char *dst_max = *dst + dst_size;

    // Input validation
    if (!src || !dst || !(*dst) || !dst_size)
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Forward to interface-specific routine
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return FinnNv01RootNvdSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return FinnNv01Device0DmaSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return FinnNv01Device0FbSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return FinnNv01Device0FifoSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return FinnNv01Device0GpuSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return FinnNv01Device0GrSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return FinnNv01Device0HostSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return FinnNv01Device0MsencSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return FinnNv20Subdevice0CeSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return FinnNv20Subdevice0GpuSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return FinnNv20Subdevice0I2cSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return FinnNv20Subdevice0NvdSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return FinnNv20Subdevice0PerfSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return FinnNv20Subdevice0RcSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return FinnNv40I2cI2cSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return FinnGt200DebuggerDebugSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return FinnMaxwellChannelGpfifoAGpfifoSerialize(message, src, (NvU8 **) dst, (const NvU8 *) dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


NV_STATUS FinnRmApiDeserializeInternal(char * const *src, NvLength src_size, char *dst, NvLength dst_size, NvBool deser_up)
{
    const char *src_max = *src + src_size;

    // Input validation
    if (!src || !(*src) || !src_size || !dst || !dst_size)
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (((NvU64*)(*src))[0] != FINN_SERIALIZATION_VERSION)
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    if (((NvU64*)(*src))[1] > src_size || ((NvU64 *)(*src))[1] < (4 * sizeof(NvU64)))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Forward to interface-specific routine
    switch (((NvU64 *)(*src))[2])
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return FinnNv01RootNvdDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_ROOT_NVD *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return FinnNv01Device0DmaDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_DEVICE_0_DMA *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return FinnNv01Device0FbDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_DEVICE_0_FB *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return FinnNv01Device0FifoDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_DEVICE_0_FIFO *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return FinnNv01Device0GpuDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_DEVICE_0_GPU *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return FinnNv01Device0GrDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_DEVICE_0_GR *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return FinnNv01Device0HostDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_DEVICE_0_HOST *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return FinnNv01Device0MsencDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV01_DEVICE_0_MSENC *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return FinnNv20Subdevice0CeDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV20_SUBDEVICE_0_CE *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return FinnNv20Subdevice0GpuDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV20_SUBDEVICE_0_GPU *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return FinnNv20Subdevice0I2cDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV20_SUBDEVICE_0_I2C *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return FinnNv20Subdevice0NvdDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV20_SUBDEVICE_0_NVD *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return FinnNv20Subdevice0PerfDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV20_SUBDEVICE_0_PERF *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return FinnNv20Subdevice0RcDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV20_SUBDEVICE_0_RC *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return FinnNv40I2cI2cDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_NV40_I2C_I2C *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return FinnGt200DebuggerDebugDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_GT200_DEBUGGER_DEBUG *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return FinnMaxwellChannelGpfifoAGpfifoDeserialize((NvU8 **) src, (const NvU8 *) src_max, (FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

NvU64 FinnRmApiGetSerializedSize(NvU64 interface, NvU64 message, const NvP64 src)
{
    // Input validation
    if (!src)
        return 0;

    // Forward to interface-specific routine
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return FinnNv01RootNvdGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return FinnNv01Device0DmaGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return FinnNv01Device0FbGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return FinnNv01Device0FifoGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return FinnNv01Device0GpuGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return FinnNv01Device0GrGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return FinnNv01Device0HostGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return FinnNv01Device0MsencGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return FinnNv20Subdevice0CeGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return FinnNv20Subdevice0GpuGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return FinnNv20Subdevice0I2cGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return FinnNv20Subdevice0NvdGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return FinnNv20Subdevice0PerfGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return FinnNv20Subdevice0RcGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return FinnNv40I2cI2cGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return FinnGt200DebuggerDebugGetSerializedSize(message, src);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return FinnMaxwellChannelGpfifoAGpfifoGetSerializedSize(message, src);
        default:
            return 0;
    }
}

NvU64 FinnRmApiGetUnserializedSize(NvU64 interface, NvU64 message)
{
    // Forward to interface-specific routine
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return FinnNv01RootNvdGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return FinnNv01Device0DmaGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return FinnNv01Device0FbGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return FinnNv01Device0FifoGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return FinnNv01Device0GpuGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return FinnNv01Device0GrGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return FinnNv01Device0HostGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return FinnNv01Device0MsencGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return FinnNv20Subdevice0CeGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return FinnNv20Subdevice0GpuGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return FinnNv20Subdevice0I2cGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return FinnNv20Subdevice0NvdGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return FinnNv20Subdevice0PerfGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return FinnNv20Subdevice0RcGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return FinnNv40I2cI2cGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return FinnGt200DebuggerDebugGetUnserializedSize(message);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return FinnMaxwellChannelGpfifoAGpfifoGetUnserializedSize(message);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01RootNvdSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv0000CtrlNvdGetDumpParamsSerialize((const NV0000_CTRL_NVD_GET_DUMP_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01RootNvdDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_ROOT_NVD *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv0000CtrlNvdGetDumpParamsDeserialize(src, src_max, (NV0000_CTRL_NVD_GET_DUMP_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01RootNvdGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv0000CtrlNvdGetDumpParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01RootNvdGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return sizeof(NV0000_CTRL_NVD_GET_DUMP_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0DmaSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return Nv0080CtrlDmaUpdatePde2ParamsSerialize((const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0DmaDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_DMA *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return Nv0080CtrlDmaUpdatePde2ParamsDeserialize(src, src_max, (NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0DmaGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return Nv0080CtrlDmaUpdatePde2ParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01Device0DmaGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0FbSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return Nv0080CtrlFbGetCapsParamsSerialize((const NV0080_CTRL_FB_GET_CAPS_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0FbDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_FB *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return Nv0080CtrlFbGetCapsParamsDeserialize(src, src_max, (NV0080_CTRL_FB_GET_CAPS_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0FbGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return Nv0080CtrlFbGetCapsParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01Device0FbGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_FB_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0FifoSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return Nv0080CtrlFifoGetCapsParamsSerialize((const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *) src, dst, dst_max, seri_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return Nv0080CtrlFifoStartSelectedChannelsParamsSerialize((const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *) src, dst, dst_max, seri_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return Nv0080CtrlFifoGetChannellistParamsSerialize((const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0FifoDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_FIFO *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return Nv0080CtrlFifoGetCapsParamsDeserialize(src, src_max, (NV0080_CTRL_FIFO_GET_CAPS_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return Nv0080CtrlFifoStartSelectedChannelsParamsDeserialize(src, src_max, (NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return Nv0080CtrlFifoGetChannellistParamsDeserialize(src, src_max, (NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0FifoGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return Nv0080CtrlFifoGetCapsParamsGetSerializedSize(NvP64_VALUE(src));
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return Nv0080CtrlFifoStartSelectedChannelsParamsGetSerializedSize(NvP64_VALUE(src));
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return Nv0080CtrlFifoGetChannellistParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01Device0FifoGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_FIFO_GET_CAPS_PARAMS);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return sizeof(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return sizeof(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0GpuSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return Nv0080CtrlGpuGetClasslistParamsSerialize((const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0GpuDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return Nv0080CtrlGpuGetClasslistParamsDeserialize(src, src_max, (NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0GpuGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return Nv0080CtrlGpuGetClasslistParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01Device0GpuGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0GrSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return Nv0080CtrlGrGetCapsParamsSerialize((const NV0080_CTRL_GR_GET_CAPS_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0GrDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_GR *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return Nv0080CtrlGrGetCapsParamsDeserialize(src, src_max, (NV0080_CTRL_GR_GET_CAPS_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0GrGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return Nv0080CtrlGrGetCapsParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01Device0GrGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_GR_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0HostSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return Nv0080CtrlHostGetCapsParamsSerialize((const NV0080_CTRL_HOST_GET_CAPS_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0HostDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_HOST *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return Nv0080CtrlHostGetCapsParamsDeserialize(src, src_max, (NV0080_CTRL_HOST_GET_CAPS_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0HostGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return Nv0080CtrlHostGetCapsParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01Device0HostGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_HOST_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0MsencSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return Nv0080CtrlMsencGetCapsParamsSerialize((const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0MsencDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV01_DEVICE_0_MSENC *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return Nv0080CtrlMsencGetCapsParamsDeserialize(src, src_max, (NV0080_CTRL_MSENC_GET_CAPS_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0MsencGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return Nv0080CtrlMsencGetCapsParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv01Device0MsencGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_MSENC_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0CeSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return Nv2080CtrlCeGetCapsParamsSerialize((const NV2080_CTRL_CE_GET_CAPS_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0CeDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_CE *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return Nv2080CtrlCeGetCapsParamsDeserialize(src, src_max, (NV2080_CTRL_CE_GET_CAPS_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0CeGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return Nv2080CtrlCeGetCapsParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv20Subdevice0CeGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0GpuSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return Nv2080CtrlGpuGetEnginesParamsSerialize((const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *) src, dst, dst_max, seri_up);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return Nv2080CtrlGpuGetEngineClasslistParamsSerialize((const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0GpuDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return Nv2080CtrlGpuGetEnginesParamsDeserialize(src, src_max, (NV2080_CTRL_GPU_GET_ENGINES_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return Nv2080CtrlGpuGetEngineClasslistParamsDeserialize(src, src_max, (NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0GpuGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return Nv2080CtrlGpuGetEnginesParamsGetSerializedSize(NvP64_VALUE(src));
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return Nv2080CtrlGpuGetEngineClasslistParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv20Subdevice0GpuGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0I2cSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return Nv2080CtrlI2cAccessParamsSerialize((const NV2080_CTRL_I2C_ACCESS_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0I2cDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_I2C *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return Nv2080CtrlI2cAccessParamsDeserialize(src, src_max, (NV2080_CTRL_I2C_ACCESS_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0I2cGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return Nv2080CtrlI2cAccessParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv20Subdevice0I2cGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0NvdSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv2080CtrlNvdGetDumpParamsSerialize((const NV2080_CTRL_NVD_GET_DUMP_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0NvdDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_NVD *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv2080CtrlNvdGetDumpParamsDeserialize(src, src_max, (NV2080_CTRL_NVD_GET_DUMP_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0NvdGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv2080CtrlNvdGetDumpParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv20Subdevice0NvdGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0PerfSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):
            return Nv2080CtrlGpumonSamplesSerialize((const NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM *) src, dst, dst_max, seri_up, FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF), FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM));
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0PerfDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_PERF *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):
            return Nv2080CtrlGpumonSamplesDeserialize(src, src_max, (NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0PerfGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):
            return Nv2080CtrlGpumonSamplesGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv20Subdevice0PerfGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):
            return sizeof(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0RcSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return Nv2080CtrlRcReadVirtualMemParamsSerialize((const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0RcDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV20_SUBDEVICE_0_RC *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return Nv2080CtrlRcReadVirtualMemParamsDeserialize(src, src_max, (NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0RcGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return Nv2080CtrlRcReadVirtualMemParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv20Subdevice0RcGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv40I2cI2cSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return Nv402cCtrlI2cIndexedParamsSerialize((const NV402C_CTRL_I2C_INDEXED_PARAMS *) src, dst, dst_max, seri_up);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return Nv402cCtrlI2cTransactionParamsSerialize((const NV402C_CTRL_I2C_TRANSACTION_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv40I2cI2cDeserialize(NvU8 **src, const NvU8 *src_max, FINN_NV40_I2C_I2C *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return Nv402cCtrlI2cIndexedParamsDeserialize(src, src_max, (NV402C_CTRL_I2C_INDEXED_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return Nv402cCtrlI2cTransactionParamsDeserialize(src, src_max, (NV402C_CTRL_I2C_TRANSACTION_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv40I2cI2cGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return Nv402cCtrlI2cIndexedParamsGetSerializedSize(NvP64_VALUE(src));
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return Nv402cCtrlI2cTransactionParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnNv40I2cI2cGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return sizeof(NV402C_CTRL_I2C_INDEXED_PARAMS);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return sizeof(NV402C_CTRL_I2C_TRANSACTION_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnGt200DebuggerDebugSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return Nv83deCtrlDebugReadMemoryParamsSerialize((const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *) src, dst, dst_max, seri_up);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return Nv83deCtrlDebugWriteMemoryParamsSerialize((const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnGt200DebuggerDebugDeserialize(NvU8 **src, const NvU8 *src_max, FINN_GT200_DEBUGGER_DEBUG *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return Nv83deCtrlDebugReadMemoryParamsDeserialize(src, src_max, (NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return Nv83deCtrlDebugWriteMemoryParamsDeserialize(src, src_max, (NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnGt200DebuggerDebugGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return Nv83deCtrlDebugReadMemoryParamsGetSerializedSize(NvP64_VALUE(src));
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return Nv83deCtrlDebugWriteMemoryParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnGt200DebuggerDebugGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return sizeof(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return sizeof(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoSerialize(NvU64 message, const char *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return Nvb06fCtrlGetEngineCtxDataParamsSerialize((const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *) src, dst, dst_max, seri_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS):
            return Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsSerialize((const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *) src, dst, dst_max, seri_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoDeserialize(NvU8 **src, const NvU8 *src_max, FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine
    switch (((NvU64 *)(*src))[3])
    {
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return Nvb06fCtrlGetEngineCtxDataParamsDeserialize(src, src_max, (NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS):
            return Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsDeserialize(src, src_max, (NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *) dst, dst_size, deser_up);
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnMaxwellChannelGpfifoAGpfifoGetSerializedSize(NvU64 message, const NvP64 src)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return Nvb06fCtrlGetEngineCtxDataParamsGetSerializedSize(NvP64_VALUE(src));
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS):
            return Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsGetSerializedSize(NvP64_VALUE(src));
        default:
            return 0;
    }
}

static NvU64 FinnMaxwellChannelGpfifoAGpfifoGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine
    switch (message)
    {
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return sizeof(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS):
            return sizeof(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS Nv0000CtrlNvdGetDumpParamsSerialize(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0000CtrlNvdGetDumpParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x6; // Interface ID
    header->message = 0x2; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->component, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->size, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->size < 0 || src->size > NV0000_CTRL_NVD_MAX_DUMP_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pBuffer);

    if (src->pBuffer)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pBuffer, (src->size));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0000CtrlNvdGetDumpParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0000_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0000_CTRL_NVD_GET_DUMP_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->component, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->size, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->size < 0 || dst->size > NV0000_CTRL_NVD_MAX_DUMP_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pBuffer, pos, (dst->size));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->size)) > src_max ||
                pos + ((dst->size)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pBuffer, pos, NvP64, (dst->size));
        }
    }
    else
    {
        if (!deser_up)
            dst->pBuffer = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0000CtrlNvdGetDumpParamsGetSerializedSize(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pBuffer)
    {
        size += (src->size);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlDmaUpdatePde2PageTableParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->physAddr, NvU64, 8);

    FINN_COPY_TO_BUFFER(pos, src->numEntries, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->aperture, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->physAddr, pos, NvU64, 8);

    FINN_COPY_FROM_BUFFER(dst->numEntries, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->aperture, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlDmaUpdatePde2PageTableParamsGetSerializedSize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 56;
}

static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlDmaUpdatePde2ParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8018; // Interface ID
    header->message = 0xf; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3f;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->pdeIndex, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->flags, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->hVASpace, NvHandle, 4);

    FINN_COPY_TO_BUFFER(pos, src->subDeviceId, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Bounded nested fields
    for (NvU64 i = 0; i < (NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE); ++i)
    {
        status = Nv0080CtrlDmaUpdatePde2PageTableParamsSerialize(&src->ptParams[i], &pos, dst_max, seri_up);
        if (status != NV_OK)
            goto exit;
    }

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pPdeBuffer);

    if (src->pPdeBuffer)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pPdeBuffer, 8);
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->pdeIndex, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->flags, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->hVASpace, pos, NvHandle, 4);

    FINN_COPY_FROM_BUFFER(dst->subDeviceId, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Bounded nested fields
    for (NvU64 i = 0; i < (NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE); ++i)
    {
        status = Nv0080CtrlDmaUpdatePde2PageTableParamsDeserialize(&pos, src_max, &dst->ptParams[i], dst_size, deser_up);
        if (status != NV_OK)
            goto exit;
    }

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pPdeBuffer, pos, 8);
        else
        {
            FINN_SET_PTR_TO_BUFFER(dst->pPdeBuffer, pos, NvP64, 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pPdeBuffer = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlDmaUpdatePde2ParamsGetSerializedSize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 168;

    // Add sizes that require runtime calculation
    size += 112;
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pPdeBuffer)
    {
        size += 8;
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlFbGetCapsParamsSerialize(const NV0080_CTRL_FB_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlFbGetCapsParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8013; // Interface ID
    header->message = 0x1; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->capsTblSize, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->capsTblSize < 0 || src->capsTblSize > NV0080_CTRL_FB_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->capsTbl);

    if (src->capsTbl)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->capsTbl, (src->capsTblSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlFbGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FB_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_FB_GET_CAPS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->capsTblSize, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->capsTblSize < 0 || dst->capsTblSize > NV0080_CTRL_FB_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->capsTbl, pos, (dst->capsTblSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->capsTblSize)) > src_max ||
                pos + ((dst->capsTblSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->capsTbl, pos, NvP64, (dst->capsTblSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlFbGetCapsParamsGetSerializedSize(const NV0080_CTRL_FB_GET_CAPS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->capsTbl)
    {
        size += (src->capsTblSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlFifoGetCapsParamsSerialize(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlFifoGetCapsParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8017; // Interface ID
    header->message = 0x1; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->capsTblSize, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->capsTblSize < 0 || src->capsTblSize > NV0080_CTRL_FIFO_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->capsTbl);

    if (src->capsTbl)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->capsTbl, (src->capsTblSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlFifoGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_FIFO_GET_CAPS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->capsTblSize, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->capsTblSize < 0 || dst->capsTblSize > NV0080_CTRL_FIFO_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->capsTbl, pos, (dst->capsTblSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->capsTblSize)) > src_max ||
                pos + ((dst->capsTblSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->capsTbl, pos, NvP64, (dst->capsTblSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlFifoGetCapsParamsGetSerializedSize(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->capsTbl)
    {
        size += (src->capsTblSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlFifoChannelSerialize(const NV0080_CTRL_FIFO_CHANNEL *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlFifoChannelGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x1;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->hChannel, NvHandle, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlFifoChannelDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_CHANNEL *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_FIFO_CHANNEL) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x1)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->hChannel, pos, NvHandle, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlFifoChannelGetSerializedSize(const NV0080_CTRL_FIFO_CHANNEL *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 48;
}

static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsSerialize(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlFifoStartSelectedChannelsParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8017; // Interface ID
    header->message = 0x5; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->fifoStartChannelListSize, NvU32, 4);

    FINN_MEMCPY_TO_BUFFER(pos, src->channelHandle, 32);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->fifoStartChannelList);

    if (src->fifoStartChannelList)
    {
        // Align
        pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

        for (NvU64 i = 0; i < (src->fifoStartChannelListSize); ++i)
        {
            status = Nv0080CtrlFifoChannelSerialize(&(((const NV0080_CTRL_FIFO_CHANNEL *) (NvP64_VALUE(src->fifoStartChannelList)))[i]), &pos, dst_max, seri_up);

            if (status != NV_OK)
                goto exit;
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->fifoStartChannelList)
            FINN_FREE(src->fifoStartChannelList);
    }


    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->fifoStartChannelListSize, pos, NvU32, 4);

    FINN_MEMCPY_FROM_BUFFER(dst->channelHandle, pos, 32);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        // Align
        pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

        // Caller must set up the pointers when deserializing down.
        if (!deser_up)
        {
            // Variable element size
            NvU64 element_size = Nv0080CtrlFifoChannelGetSerializedSize(NvP64_VALUE(dst->fifoStartChannelList));

            // Validate variable length buffer length
            if (element_size * (dst->fifoStartChannelListSize) < element_size ||
                pos + (element_size * (dst->fifoStartChannelListSize)) > src_max ||
                pos + (element_size * (dst->fifoStartChannelListSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            // FINN-generated code allocates memory and sets pointer when deserializing down.
            dst->fifoStartChannelList = FINN_MALLOC((sizeof(NV0080_CTRL_FIFO_CHANNEL) * (dst->fifoStartChannelListSize)));
            if (!dst->fifoStartChannelList)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->fifoStartChannelList, (sizeof(NV0080_CTRL_FIFO_CHANNEL) * (dst->fifoStartChannelListSize)));

        }

        // Otherwise the pointer must be provided.
        else if (!dst->fifoStartChannelList)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        for (NvU64 i = 0; i < (dst->fifoStartChannelListSize); ++i)
        {
            // Deserialize each element.
            status = Nv0080CtrlFifoChannelDeserialize(&pos, src_max, &(((NV0080_CTRL_FIFO_CHANNEL *) (NvP64_VALUE(dst->fifoStartChannelList)))[i]), sizeof(NV0080_CTRL_FIFO_CHANNEL), deser_up);
            if (status != NV_OK)
                goto exit;
        }
    }

    // Data is not present, set to NULL.
    else
    {
        if (!deser_up)
            dst->fifoStartChannelList = NULL;
    }


    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlFifoStartSelectedChannelsParamsGetSerializedSize(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 80;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->fifoStartChannelList)
    {
        // Alignment
        size = (size + 7) & ~7;
        size += Nv0080CtrlFifoChannelGetSerializedSize((const NV0080_CTRL_FIFO_CHANNEL *) src->fifoStartChannelList) * ((src->fifoStartChannelListSize));
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlFifoGetChannellistParamsSerialize(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlFifoGetChannellistParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8017; // Interface ID
    header->message = 0xd; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->numChannels, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pChannelHandleList);

    if (src->pChannelHandleList)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pChannelHandleList, (src->numChannels) * 4);
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Set data presence byte
    *(pos++) = !!(src->pChannelList);

    if (src->pChannelList)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pChannelList, (src->numChannels) * 4);
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlFifoGetChannellistParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->numChannels, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pChannelHandleList, pos, (dst->numChannels) * 4);
        else
        {
            // Validate variable length buffer length
            if ((dst->numChannels) * 4 < 4 ||
                pos + ((dst->numChannels) * 4) > src_max ||
                pos + ((dst->numChannels) * 4) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pChannelHandleList, pos, NvP64, (dst->numChannels) * 4);
        }
    }
    else
    {
        if (!deser_up)
            dst->pChannelHandleList = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pChannelList, pos, (dst->numChannels) * 4);
        else
        {
            // Validate variable length buffer length
            if ((dst->numChannels) * 4 < 4 ||
                pos + ((dst->numChannels) * 4) > src_max ||
                pos + ((dst->numChannels) * 4) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pChannelList, pos, NvP64, (dst->numChannels) * 4);
        }
    }
    else
    {
        if (!deser_up)
            dst->pChannelList = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlFifoGetChannellistParamsGetSerializedSize(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pChannelHandleList)
    {
        size += (src->numChannels) * 4;
    }

    // Add padding
    size = (size + 7) &~ 7;

    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pChannelList)
    {
        size += (src->numChannels) * 4;
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlGpuGetClasslistParamsSerialize(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlGpuGetClasslistParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8002; // Interface ID
    header->message = 0x1; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->numClasses, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->classList);

    if (src->classList)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->classList, (src->numClasses) * 4);
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlGpuGetClasslistParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->numClasses, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->classList, pos, (dst->numClasses) * 4);
        else
        {
            // Validate variable length buffer length
            if ((dst->numClasses) * 4 < 4 ||
                pos + ((dst->numClasses) * 4) > src_max ||
                pos + ((dst->numClasses) * 4) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->classList, pos, NvP64, (dst->numClasses) * 4);
        }
    }
    else
    {
        if (!deser_up)
            dst->classList = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlGpuGetClasslistParamsGetSerializedSize(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->classList)
    {
        size += (src->numClasses) * 4;
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlGrGetCapsParamsSerialize(const NV0080_CTRL_GR_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlGrGetCapsParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8011; // Interface ID
    header->message = 0x2; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->capsTblSize, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->capsTbl);

    if (src->capsTbl)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->capsTbl, (src->capsTblSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlGrGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_GR_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_GR_GET_CAPS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->capsTblSize, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->capsTbl, pos, (dst->capsTblSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->capsTblSize)) > src_max ||
                pos + ((dst->capsTblSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->capsTbl, pos, NvP64, (dst->capsTblSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlGrGetCapsParamsGetSerializedSize(const NV0080_CTRL_GR_GET_CAPS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->capsTbl)
    {
        size += (src->capsTblSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlHostGetCapsParamsSerialize(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlHostGetCapsParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x8014; // Interface ID
    header->message = 0x1; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->capsTblSize, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->capsTblSize < 0 || src->capsTblSize > NV0080_CTRL_HOST_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->capsTbl);

    if (src->capsTbl)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->capsTbl, (src->capsTblSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlHostGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_HOST_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_HOST_GET_CAPS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->capsTblSize, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->capsTblSize < 0 || dst->capsTblSize > NV0080_CTRL_HOST_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->capsTbl, pos, (dst->capsTblSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->capsTblSize)) > src_max ||
                pos + ((dst->capsTblSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->capsTbl, pos, NvP64, (dst->capsTblSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlHostGetCapsParamsGetSerializedSize(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->capsTbl)
    {
        size += (src->capsTblSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv0080CtrlMsencGetCapsParamsSerialize(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv0080CtrlMsencGetCapsParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x801b; // Interface ID
    header->message = 0x1; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->capsTblSize, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->capsTblSize < 0 || src->capsTblSize > NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->capsTbl);

    if (src->capsTbl)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->capsTbl, (src->capsTblSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv0080CtrlMsencGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV0080_CTRL_MSENC_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV0080_CTRL_MSENC_GET_CAPS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->capsTblSize, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->capsTblSize < 0 || dst->capsTblSize > NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->capsTbl, pos, (dst->capsTblSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->capsTblSize)) > src_max ||
                pos + ((dst->capsTblSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->capsTbl, pos, NvP64, (dst->capsTblSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv0080CtrlMsencGetCapsParamsGetSerializedSize(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->capsTbl)
    {
        size += (src->capsTblSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv2080CtrlCeGetCapsParamsSerialize(const NV2080_CTRL_CE_GET_CAPS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv2080CtrlCeGetCapsParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x20802a; // Interface ID
    header->message = 0x1; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->ceEngineType, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->capsTblSize, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->capsTblSize < 0 || src->capsTblSize > NV2080_CTRL_CE_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->capsTbl);

    if (src->capsTbl)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->capsTbl, (src->capsTblSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv2080CtrlCeGetCapsParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_CE_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->ceEngineType, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->capsTblSize, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->capsTblSize < 0 || dst->capsTblSize > NV2080_CTRL_CE_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->capsTbl, pos, (dst->capsTblSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->capsTblSize)) > src_max ||
                pos + ((dst->capsTblSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->capsTbl, pos, NvP64, (dst->capsTblSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv2080CtrlCeGetCapsParamsGetSerializedSize(const NV2080_CTRL_CE_GET_CAPS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->capsTbl)
    {
        size += (src->capsTblSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv2080CtrlGpuGetEnginesParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv2080CtrlGpuGetEnginesParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x208001; // Interface ID
    header->message = 0x23; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->engineCount, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->engineList);

    if (src->engineList)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->engineList, (src->engineCount) * 4);
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv2080CtrlGpuGetEnginesParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->engineCount, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->engineList, pos, (dst->engineCount) * 4);
        else
        {
            // Validate variable length buffer length
            if ((dst->engineCount) * 4 < 4 ||
                pos + ((dst->engineCount) * 4) > src_max ||
                pos + ((dst->engineCount) * 4) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->engineList, pos, NvP64, (dst->engineCount) * 4);
        }
    }
    else
    {
        if (!deser_up)
            dst->engineList = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv2080CtrlGpuGetEnginesParamsGetSerializedSize(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->engineList)
    {
        size += (src->engineCount) * 4;
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv2080CtrlGpuGetEngineClasslistParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x208001; // Interface ID
    header->message = 0x24; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->engineType, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->numClasses, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->classList);

    if (src->classList)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->classList, (src->numClasses) * 4);
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->engineType, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->numClasses, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->classList, pos, (dst->numClasses) * 4);
        else
        {
            // Validate variable length buffer length
            if ((dst->numClasses) * 4 < 4 ||
                pos + ((dst->numClasses) * 4) > src_max ||
                pos + ((dst->numClasses) * 4) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->classList, pos, NvP64, (dst->numClasses) * 4);
        }
    }
    else
    {
        if (!deser_up)
            dst->classList = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv2080CtrlGpuGetEngineClasslistParamsGetSerializedSize(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->classList)
    {
        size += (src->numClasses) * 4;
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv2080CtrlGpumonSamplesSerialize(const NV2080_CTRL_GPUMON_SAMPLES *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up, NvU64 interface, NvU64 message)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv2080CtrlGpumonSamplesGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = interface; // Interface ID
    header->message = message; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x1f;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->bufSize, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->count, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->tracker, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->type, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pSamples);

    if (src->pSamples)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pSamples, (src->bufSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv2080CtrlGpumonSamplesDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_GPUMON_SAMPLES *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV2080_CTRL_GPUMON_SAMPLES) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->bufSize, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->count, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->tracker, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->type, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pSamples, pos, (dst->bufSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->bufSize)) > src_max ||
                pos + ((dst->bufSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pSamples, pos, NvP64, (dst->bufSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->pSamples = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv2080CtrlGpumonSamplesGetSerializedSize(const NV2080_CTRL_GPUMON_SAMPLES *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 56;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pSamples)
    {
        size += (src->bufSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv2080CtrlI2cAccessParamsSerialize(const NV2080_CTRL_I2C_ACCESS_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv2080CtrlI2cAccessParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x208006; // Interface ID
    header->message = 0x10; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x1ff;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->token, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->cmd, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->port, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->flags, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->status, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->dataBuffSize, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->dataBuffSize < 0 || src->dataBuffSize > NV2080_CTRL_I2C_MAX_ENTRIES)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_TO_BUFFER(pos, src->speed, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->encrClientID, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->data);

    if (src->data)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->data, (src->dataBuffSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv2080CtrlI2cAccessParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_I2C_ACCESS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x1ff)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->token, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->cmd, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->port, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->flags, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->status, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->dataBuffSize, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->dataBuffSize < 0 || dst->dataBuffSize > NV2080_CTRL_I2C_MAX_ENTRIES)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_FROM_BUFFER(dst->speed, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->encrClientID, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->data, pos, (dst->dataBuffSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->dataBuffSize)) > src_max ||
                pos + ((dst->dataBuffSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->data, pos, NvP64, (dst->dataBuffSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->data = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv2080CtrlI2cAccessParamsGetSerializedSize(const NV2080_CTRL_I2C_ACCESS_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 72;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->data)
    {
        size += (src->dataBuffSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv2080CtrlNvdGetDumpParamsSerialize(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv2080CtrlNvdGetDumpParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x208024; // Interface ID
    header->message = 0x2; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->component, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->size, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pBuffer);

    if (src->pBuffer)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pBuffer, (src->size));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv2080CtrlNvdGetDumpParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->component, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->size, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pBuffer, pos, (dst->size));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->size)) > src_max ||
                pos + ((dst->size)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pBuffer, pos, NvP64, (dst->size));
        }
    }
    else
    {
        if (!deser_up)
            dst->pBuffer = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv2080CtrlNvdGetDumpParamsGetSerializedSize(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pBuffer)
    {
        size += (src->size);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsSerialize(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv2080CtrlRcReadVirtualMemParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x208022; // Interface ID
    header->message = 0x4; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0xf;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->virtAddress, NvU64, 8);

    FINN_COPY_TO_BUFFER(pos, src->hChannel, NvHandle, 4);

    FINN_COPY_TO_BUFFER(pos, src->bufferSize, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->bufferPtr);

    if (src->bufferPtr)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->bufferPtr, (src->bufferSize));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->virtAddress, pos, NvU64, 8);

    FINN_COPY_FROM_BUFFER(dst->hChannel, pos, NvHandle, 4);

    FINN_COPY_FROM_BUFFER(dst->bufferSize, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->bufferPtr, pos, (dst->bufferSize));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->bufferSize)) > src_max ||
                pos + ((dst->bufferSize)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->bufferPtr, pos, NvP64, (dst->bufferSize));
        }
    }
    else
    {
        if (!deser_up)
            dst->bufferPtr = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv2080CtrlRcReadVirtualMemParamsGetSerializedSize(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 56;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->bufferPtr)
    {
        size += (src->bufferSize);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cIndexedParamsSerialize(const NV402C_CTRL_I2C_INDEXED_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cIndexedParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x402c01; // Interface ID
    header->message = 0x2; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0xff;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->flags, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->indexLength, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->indexLength < 0 || src->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_TO_BUFFER(pos, src->messageLength, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->address, NvU16, 2);

    FINN_COPY_TO_BUFFER(pos, src->portId, NvU8, 1);

    FINN_COPY_TO_BUFFER(pos, src->bIsWrite, NvU8, 1);

    FINN_MEMCPY_TO_BUFFER(pos, src->index, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pMessage);

    if (src->pMessage)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pMessage, (src->messageLength));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cIndexedParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_INDEXED_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_INDEXED_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0xff)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->flags, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->indexLength, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->indexLength < 0 || dst->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_FROM_BUFFER(dst->messageLength, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->address, pos, NvU16, 2);

    FINN_COPY_FROM_BUFFER(dst->portId, pos, NvU8, 1);

    FINN_COPY_FROM_BUFFER(dst->bIsWrite, pos, NvU8, 1);

    FINN_MEMCPY_FROM_BUFFER(dst->index, pos, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pMessage, pos, (dst->messageLength));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->messageLength)) > src_max ||
                pos + ((dst->messageLength)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pMessage, pos, NvP64, (dst->messageLength));
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cIndexedParamsGetSerializedSize(const NV402C_CTRL_I2C_INDEXED_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 64;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pMessage)
    {
        size += (src->messageLength);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cTransactionTypeValueToId(NvU8 **buf, const NvU8 *buf_max, NvU64 convert_size)
{
    NV402C_CTRL_I2C_TRANSACTION_TYPE *pEnum = NULL;
    NvU8 *buf_end = *buf + convert_size;

    // Bounds checking before overwriting data
    if (buf_end > buf_max)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Convert each enum value to its corresponding ID.
    while (*buf < buf_end)
    {
        pEnum = (NV402C_CTRL_I2C_TRANSACTION_TYPE *)*buf;

        switch (*pEnum)
        {
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
                *pEnum = 0;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
                *pEnum = 1;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
                *pEnum = 2;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
                *pEnum = 3;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
                *pEnum = 4;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
                *pEnum = 5;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
                *pEnum = 6;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
                *pEnum = 7;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
                *pEnum = 8;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
                *pEnum = 9;
                break;
            case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
                *pEnum = 10;
                break;
            default:
            {
                FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
                return NV_ERR_INVALID_ARGUMENT;
            }
        }

        *buf += sizeof(NV402C_CTRL_I2C_TRANSACTION_TYPE);
    }

    return NV_OK;
}

static NV_STATUS Nv402cCtrlI2cTransactionTypeIdtoValue(NvU8 **buf, const NvU8 *buf_max, NvU64 convert_size)
{
    NvU32 *pID = NULL;
    NvU8 *buf_end = *buf + convert_size;

    // Bounds checking before overwriting data
    if (buf_end > buf_max)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Convert each ID to its corresponding enum value.
    while (*buf < buf_end)
    {
        pID = (NvU32 *)*buf;

        switch (*pID)
        {
            case 0:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW;
                break;
            case 1:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW;
                break;
            case 2:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW;
                break;
            case 3:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW;
                break;
            case 4:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW;
                break;
            case 5:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW;
                break;
            case 6:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW;
                break;
            case 7:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL;
                break;
            case 8:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL;
                break;
            case 9:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW;
                break;
            case 10:
                *pID = NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC;
                break;
            default:
            {
                FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
                return NV_ERR_INVALID_ARGUMENT;
            }
        }

        *buf += sizeof(NvU32);
    }

    return NV_OK;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataSmbusQuickRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->warFlags, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->warFlags, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataSmbusQuickRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 48;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataI2cByteRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x3;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    FINN_COPY_TO_BUFFER(pos, src->message, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    FINN_COPY_FROM_BUFFER(dst->message, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataI2cByteRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 48;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataI2cBlockRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->messageLength, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pMessage);

    if (src->pMessage)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pMessage, (src->messageLength));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->messageLength, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pMessage, pos, (dst->messageLength));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->messageLength)) > src_max ||
                pos + ((dst->messageLength)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pMessage, pos, NvP64, (dst->messageLength));
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataI2cBlockRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pMessage)
    {
        size += (src->messageLength);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataSmbusByteRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    FINN_COPY_TO_BUFFER(pos, src->registerAddress, NvU8, 1);

    FINN_COPY_TO_BUFFER(pos, src->message, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    FINN_COPY_FROM_BUFFER(dst->registerAddress, pos, NvU8, 1);

    FINN_COPY_FROM_BUFFER(dst->message, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataSmbusByteRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 48;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataSmbusWordRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->message, NvU16, 2);

    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    FINN_COPY_TO_BUFFER(pos, src->registerAddress, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->message, pos, NvU16, 2);

    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    FINN_COPY_FROM_BUFFER(dst->registerAddress, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataSmbusWordRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 48;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataI2cBufferRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x1f;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->warFlags, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->messageLength, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    FINN_COPY_TO_BUFFER(pos, src->registerAddress, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pMessage);

    if (src->pMessage)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pMessage, (src->messageLength));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->warFlags, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->messageLength, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    FINN_COPY_FROM_BUFFER(dst->registerAddress, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pMessage, pos, (dst->messageLength));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->messageLength)) > src_max ||
                pos + ((dst->messageLength)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pMessage, pos, NvP64, (dst->messageLength));
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataI2cBufferRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 56;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pMessage)
    {
        size += (src->messageLength);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataSmbusBlockRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0xf;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->messageLength, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    FINN_COPY_TO_BUFFER(pos, src->registerAddress, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pMessage);

    if (src->pMessage)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pMessage, (src->messageLength));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->messageLength, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    FINN_COPY_FROM_BUFFER(dst->registerAddress, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pMessage, pos, (dst->messageLength));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->messageLength)) > src_max ||
                pos + ((dst->messageLength)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pMessage, pos, NvP64, (dst->messageLength));
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataSmbusBlockRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pMessage)
    {
        size += (src->messageLength);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataSmbusProcessCallGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->writeMessage, NvU16, 2);

    FINN_COPY_TO_BUFFER(pos, src->readMessage, NvU16, 2);

    FINN_COPY_TO_BUFFER(pos, src->registerAddress, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->writeMessage, pos, NvU16, 2);

    FINN_COPY_FROM_BUFFER(dst->readMessage, pos, NvU16, 2);

    FINN_COPY_FROM_BUFFER(dst->registerAddress, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataSmbusProcessCallGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 48;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x1f;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->writeMessageLength, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->writeMessageLength < 0 || src->writeMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_TO_BUFFER(pos, src->readMessageLength, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->readMessageLength < 0 || src->readMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_TO_BUFFER(pos, src->registerAddress, NvU8, 1);

    FINN_MEMCPY_TO_BUFFER(pos, src->writeMessage, 32);

    FINN_MEMCPY_TO_BUFFER(pos, src->readMessage, 32);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->writeMessageLength, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->writeMessageLength < 0 || dst->writeMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_FROM_BUFFER(dst->readMessageLength, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->readMessageLength < 0 || dst->readMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_FROM_BUFFER(dst->registerAddress, pos, NvU8, 1);

    FINN_MEMCPY_FROM_BUFFER(dst->writeMessage, pos, 32);

    FINN_MEMCPY_FROM_BUFFER(dst->readMessage, pos, 32);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *src)
{
    // Suppress used-variable warnings.
    (void) src;

    // This struct is static and its size is known at compile time.
    return 120;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x3f;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->warFlags, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->indexLength, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (src->indexLength < 0 || src->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_TO_BUFFER(pos, src->messageLength, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->bWrite, NvBool, 1);

    FINN_MEMCPY_TO_BUFFER(pos, src->index, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pMessage);

    if (src->pMessage)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pMessage, (src->messageLength));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x3f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->warFlags, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->indexLength, pos, NvU32, 4);

    // Range validation, rewind buffer
    pos -= 4;

    if (dst->indexLength < 0 || dst->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    pos += 4;


    FINN_COPY_FROM_BUFFER(dst->messageLength, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->bWrite, pos, NvBool, 1);

    FINN_MEMCPY_FROM_BUFFER(dst->index, pos, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pMessage, pos, (dst->messageLength));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->messageLength)) > src_max ||
                pos + ((dst->messageLength)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pMessage, pos, NvP64, (dst->messageLength));
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 64;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pMessage)
    {
        size += (src->messageLength);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataReadEdidDdcGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0xf;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->messageLength, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->segmentNumber, NvU8, 1);

    FINN_COPY_TO_BUFFER(pos, src->registerAddress, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pMessage);

    if (src->pMessage)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pMessage, (src->messageLength));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->messageLength, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->segmentNumber, pos, NvU8, 1);

    FINN_COPY_FROM_BUFFER(dst->registerAddress, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pMessage, pos, (dst->messageLength));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->messageLength)) > src_max ||
                pos + ((dst->messageLength)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pMessage, pos, NvP64, (dst->messageLength));
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataReadEdidDdcGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pMessage)
    {
        size += (src->messageLength);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionDataGetSerializedSize(src, transType);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size

    //
    // Non-message type has no interface/message ID
    //

    // Field bitmasks
    header->fieldMask[0] = 0x7ff;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Field copying based on union selector
    switch (transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusQuickRwSerialize(&src->smbusQuickData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cByteRwSerialize(&src->i2cByteData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusByteRwSerialize(&src->smbusByteData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusWordRwSerialize(&src->smbusWordData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusProcessCallSerialize(&src->smbusProcessData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallSerialize(&src->smbusBlockProcessData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBlockRwSerialize(&src->i2cBlockData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBufferRwSerialize(&src->i2cBufferData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockRwSerialize(&src->smbusBlockData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwSerialize(&src->smbusMultibyteRegisterData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            status = Nv402cCtrlI2cTransactionDataReadEdidDdcSerialize(&src->edidData, &pos, dst_max, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
            FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
            goto exit;
        }
    }

    // Align
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionDataDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_DATA *dst, NvLength dst_size, NvBool deser_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7ff)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Field copying based on union selector
    switch (transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusQuickRwDeserialize(&pos, src_max, &dst->smbusQuickData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cByteRwDeserialize(&pos, src_max, &dst->i2cByteData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusByteRwDeserialize(&pos, src_max, &dst->smbusByteData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusWordRwDeserialize(&pos, src_max, &dst->smbusWordData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusProcessCallDeserialize(&pos, src_max, &dst->smbusProcessData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallDeserialize(&pos, src_max, &dst->smbusBlockProcessData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBlockRwDeserialize(&pos, src_max, &dst->i2cBlockData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBufferRwDeserialize(&pos, src_max, &dst->i2cBufferData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockRwDeserialize(&pos, src_max, &dst->smbusBlockData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwDeserialize(&pos, src_max, &dst->smbusMultibyteRegisterData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            status = Nv402cCtrlI2cTransactionDataReadEdidDdcDeserialize(&pos, src_max, &dst->edidData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
            FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
            goto exit;
        }
    }

    // Align
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionDataGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_DATA *src, NV402C_CTRL_I2C_TRANSACTION_TYPE transType)
{
    // Start with the header size
    NvU64 size = 40;

    // Calculate size based on union selector
    switch (transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        {
            size += 48;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        {
            size += 48;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        {
            size += 48;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        {
            size += 48;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        {
            size += 48;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        {
            size += 120;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            size += Nv402cCtrlI2cTransactionDataI2cBlockRwGetSerializedSize((const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *) &(src->i2cBlockData));
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            size += Nv402cCtrlI2cTransactionDataI2cBufferRwGetSerializedSize((const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *) &(src->i2cBufferData));
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            size += Nv402cCtrlI2cTransactionDataSmbusBlockRwGetSerializedSize((const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *) &(src->smbusBlockData));
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            size += Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwGetSerializedSize((const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *) &(src->smbusMultibyteRegisterData));
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            size += Nv402cCtrlI2cTransactionDataReadEdidDdcGetSerializedSize((const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *) &(src->edidData));
            break;
        }
        default:
        {
            break;
        }
    }

    // Add padding for alignment
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv402cCtrlI2cTransactionParamsSerialize(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv402cCtrlI2cTransactionParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x402c01; // Interface ID
    header->message = 0x5; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x1f;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->flags, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->transType, NV402C_CTRL_I2C_TRANSACTION_TYPE, 4);

    // Rewind buffer for conversion
    pos -= 4;

    status = Nv402cCtrlI2cTransactionTypeValueToId(&pos, dst_max, 4);
    if (status != NV_OK)
        goto exit;

    FINN_COPY_TO_BUFFER(pos, src->deviceAddress, NvU16, 2);

    FINN_COPY_TO_BUFFER(pos, src->portId, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    status = Nv402cCtrlI2cTransactionDataSerialize(&src->transData, &pos, dst_max, seri_up, src->transType);
    if (status != NV_OK)
        goto exit;

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv402cCtrlI2cTransactionParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV402C_CTRL_I2C_TRANSACTION_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->flags, pos, NvU32, 4);

    status = Nv402cCtrlI2cTransactionTypeIdtoValue(&pos, src_max, 4);
    if (status != NV_OK)
        goto exit;

    // Rewind buffer after conversion
    pos -= 4;

    FINN_COPY_FROM_BUFFER(dst->transType, pos, NV402C_CTRL_I2C_TRANSACTION_TYPE, 4);

    FINN_COPY_FROM_BUFFER(dst->deviceAddress, pos, NvU16, 2);

    FINN_COPY_FROM_BUFFER(dst->portId, pos, NvU8, 1);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    status = Nv402cCtrlI2cTransactionDataDeserialize(&pos, src_max, &dst->transData, dst_size, deser_up, dst->transType);
    if (status != NV_OK)
        goto exit;

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv402cCtrlI2cTransactionParamsGetSerializedSize(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 56;

    // Add sizes that require runtime calculation
    size += Nv402cCtrlI2cTransactionDataGetSerializedSize((const NV402C_CTRL_I2C_TRANSACTION_DATA *) &(src->transData),
            src->transType);
    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv83deCtrlDebugReadMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv83deCtrlDebugReadMemoryParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x83de03; // Interface ID
    header->message = 0x15; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0xf;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->offset, NvU64, 8);

    FINN_COPY_TO_BUFFER(pos, src->hMemory, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->length, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->buffer);

    if (src->buffer)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->buffer, (src->length));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv83deCtrlDebugReadMemoryParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->offset, pos, NvU64, 8);

    FINN_COPY_FROM_BUFFER(dst->hMemory, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->length, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->buffer, pos, (dst->length));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->length)) > src_max ||
                pos + ((dst->length)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->buffer, pos, NvP64, (dst->length));
        }
    }
    else
    {
        if (!deser_up)
            dst->buffer = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv83deCtrlDebugReadMemoryParamsGetSerializedSize(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 56;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->buffer)
    {
        size += (src->length);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nv83deCtrlDebugWriteMemoryParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0x83de03; // Interface ID
    header->message = 0x16; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0xf;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->offset, NvU64, 8);

    FINN_COPY_TO_BUFFER(pos, src->hMemory, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->length, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->buffer);

    if (src->buffer)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->buffer, (src->length));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsDeserialize(NvU8 **src, const NvU8 *src_max, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->offset, pos, NvU64, 8);

    FINN_COPY_FROM_BUFFER(dst->hMemory, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->length, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->buffer, pos, (dst->length));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->length)) > src_max ||
                pos + ((dst->length)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->buffer, pos, NvP64, (dst->length));
        }
    }
    else
    {
        if (!deser_up)
            dst->buffer = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nv83deCtrlDebugWriteMemoryParamsGetSerializedSize(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 56;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->buffer)
    {
        size += (src->length);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsSerialize(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nvb06fCtrlGetEngineCtxDataParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0xb06f01; // Interface ID
    header->message = 0xc; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x7;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_TO_BUFFER(pos, src->engineID, NvU32, 4);

    FINN_COPY_TO_BUFFER(pos, src->size, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Set data presence byte
    *(pos++) = !!(src->pEngineCtxBuff);

    if (src->pEngineCtxBuff)
    {
        FINN_MEMCPY_TO_BUFFER(pos, src->pEngineCtxBuff, (src->size));
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsDeserialize(NvU8 **src, const NvU8 *src_max, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Static size fields
    FINN_COPY_FROM_BUFFER(dst->engineID, pos, NvU32, 4);

    FINN_COPY_FROM_BUFFER(dst->size, pos, NvU32, 4);

    // Align after static size fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Unbounded fields
    // Check data presence byte
    if (*(pos++))
    {
        if (deser_up)
            FINN_MEMCPY_FROM_BUFFER(dst->pEngineCtxBuff, pos, (dst->size));
        else
        {
            // Validate variable length buffer length
            if (pos + ((dst->size)) > src_max ||
                pos + ((dst->size)) < pos)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            FINN_SET_PTR_TO_BUFFER(dst->pEngineCtxBuff, pos, NvP64, (dst->size));
        }
    }
    else
    {
        if (!deser_up)
            dst->pEngineCtxBuff = NULL;
    }

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nvb06fCtrlGetEngineCtxDataParamsGetSerializedSize(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 48;

    // Add sizes that require runtime calculation
    // Increment size to account for the data presence byte.
    ++size;

    // For non-NULL pointers, proceed to size calculation.
    if (src->pEngineCtxBuff)
    {
        size += (src->size);
    }

    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsSerialize(const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *src, NvU8 **dst, const NvU8 *dst_max, NvBool seri_up)
{
    NvU8 *pos = *dst;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NvU64 serializedSize = Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsGetSerializedSize(src);
    NV_STATUS status = NV_OK;

    // Validate buffer size
    if (pos + serializedSize > dst_max)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Clear destination buffer
    FINN_MEMZERO(pos, serializedSize);

    // Serialization header
    header->version = FINN_SERIALIZATION_VERSION; // Serialization version
    header->payloadSize = serializedSize; // Serialized size
    header->interface = 0xb06f01; // Interface ID
    header->message = 0xd; // Message ID

    // Field bitmasks
    header->fieldMask[0] = 0x1;

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Unbounded fields
    status = Nvb06fCtrlGetEngineCtxDataParamsSerialize(&src->params, &pos, dst_max, seri_up);
    if (status != NV_OK)
        goto exit;

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

exit:
    *dst = pos;
    return status;
}

static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsDeserialize(NvU8 **src, const NvU8 *src_max, NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    NvU8 *pos = *src;
    FINN_RM_API *header = (FINN_RM_API *)pos;
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    // and that the declared size fits within the source buffer
    if (sizeof(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS) > dst_size ||
        header->payloadSize < (sizeof(FINN_RM_API) + sizeof(NvU64)) ||
        pos + header->payloadSize > src_max ||
        pos + header->payloadSize < pos)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks. They must match the expected values for now
    if (header->fieldMask[0] != 0x1)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        pos = (NvU8 *) &header->fieldMask;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Jump past header
    pos += sizeof(FINN_RM_API) + (1 * sizeof(NvU64));

    // Unbounded fields
    status = Nvb06fCtrlGetEngineCtxDataParamsDeserialize(&pos, src_max, &dst->params, dst_size, deser_up);
    if (status != NV_OK)
        goto exit;

    // Align after unbounded fields
    pos = (NvU8*)(((NvU64)pos + 7) &~ 7);

    // Check that the declared size matches the serialization outcome
    if (header->payloadSize != (NvU64) (pos - *src))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        pos = (NvU8 *) &header->payloadSize;
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        goto exit;
    }

exit:
    *src = pos;
    return status;
}

static NvU64 Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsGetSerializedSize(const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *src)
{
    // Start with the portion of the size known at compile time.
    NvU64 size = 40;

    // Add sizes that require runtime calculation
    size += Nvb06fCtrlGetEngineCtxDataParamsGetSerializedSize((const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *) &(src->params));
    // Add padding
    size = (size + 7) &~ 7;

    return size;
}

