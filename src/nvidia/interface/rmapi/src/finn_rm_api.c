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
#include "ctrl/ctrl208f/ctrl208fgpu.h"
#include "ctrl/ctrl402c.h"
#include "ctrl/ctrl83de/ctrl83dedebug.h"
#include "ctrl/ctrlb06f.h"

#include <stdint.h>
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



//
// The purpose of the bit pump is to ensure 64-bit aligned access to the
// buffer while enabling arbitrary bits to be read/written.
//
typedef struct finn_bit_pump_for_read finn_bit_pump_for_read;

struct finn_bit_pump_for_read
{
    uint64_t accumulator;               // Bits not yet read from the data buffer
    uint64_t checksum;                  // Checksum of data
    const uint64_t *buffer_position;    // Next word within data buffer to be read
    const uint64_t *end_of_data;        // End of data within buffer
    uint8_t remaining_bit_count;        // Number of bits remaining in the accumulator
};


//
// Initialize bit pump for reading from the buffer.
//
// WARNING: The buffer start is assumed to be 64-bit aligned for optimal performance.
// `sod` (start of data) and `eod` (end of data) must be multiples of 64 bits
// since this logic is optimized for a 64-bit word size.  Caller must check both
// `sod` and `eod`.
//
// `eod` points to the 64-bit word after the data (like most C++ `std` iterators).
//
static inline void finn_open_buffer_for_read(finn_bit_pump_for_read *bp, const uint64_t *sod, const uint64_t *eod)
{
    bp->accumulator         = 0U;
    bp->checksum            = 0U;
    bp->buffer_position     = sod;
    bp->end_of_data         = eod;
    bp->remaining_bit_count = 0U;
}


//
// Read the next several bits.
//
// `bit_size` must be in range of 0 to 64 inclusive; no check is made.
// When `bit_size` is zero, an unsupported use-case, it works as expected by
// returning zero without advancing the pointer.
//
static uint64_t finn_read_buffer(finn_bit_pump_for_read *bp, uint8_t bit_size)
{
    // Value to be deserialized and returned
    uint64_t value;

    // Boundary crossing
    // Accumulator does not have enough to satisfy the request,
    if (bit_size > bp->remaining_bit_count)
    {
        // Number of bits not yet satisfied
        bit_size -= bp->remaining_bit_count;

        // Shift the bits we have into place.
        value = bp->accumulator;

        // Return zeroes for unsatisfied bits (if any) at end of data.
        if (bp->buffer_position >= bp->end_of_data)
        bp->accumulator = 0U;

        // Read the next word from the buffer.
        else
        bp->accumulator = *(bp->buffer_position++);

        // Update the checksum.
        bp->checksum = ((bp->checksum << 1) ^ (bp->checksum & 1U)) ^ bp->accumulator;

        //
        // This is the special case where we are reading an entire 64-bit word
        // without crossing a boundary (when the accumulator is empty).  The
        // accumulator remains empty on exit.
        //
        // The bitwise operations in the normal flow do not work in this case.
        // Shifts are not well-defined in C when the right operand exceeds the
        // size of the left operand.  Also, the right operand of the bitwise-and
        // would exceed the 64-bit capacity.  However, the needed logic is simple.
        //
        // 64 is the largest legal value for `bit_size`, so `>=` is equivalent to `==`.
        //
        if (bit_size >= 64)
        {
            // The value is the entire word.
            value = bp->accumulator;

            // Discard the consumed data from the accumulator.
            bp->accumulator = 0U;

            // Under the assumption that `bit_size` is never larger than 64,
            // `bit_size == 64` implies `bp->remaining_bit_count == 0` because
            // of the above `bit_size -= bp->remaining_bit_count`.  As such, there
            // is no need to do `bp->remaining_bit_count = 64U - bit_size`.

            // Done
            return value;
        }

        // OR in the bits since this was a boundary crossing.
        // Shift it over by the number of bits we get from the prior word.
        value |= (bp->accumulator
            & (((uint64_t) 1U << bit_size) - 1U))
        << bp->remaining_bit_count;

        // Logic below subtracts off the bits consumed in the accumulator.
        bp->remaining_bit_count = 64U;
    }

    else
    {
        // The accumulator has enough to satisfy the request.
        value = bp->accumulator & (((uint64_t) 1U << bit_size) - 1U);
    }

    // Discard the consumed bits from the accumulator.
    bp->accumulator >>= bit_size;

    // Keep track of the remaining available bits in the accumulator.
    bp->remaining_bit_count -= bit_size;

    // Done
    return value;
}


// Close the read buffer.
// Postcondition:  `bp->checksum` is updated to end-of-data.
static inline void finn_close_buffer_for_read(finn_bit_pump_for_read *bp)
{
    // No need to update the bit pump buffer position,
    // so use a local for optimal performance.
    const uint64_t *p = bp->buffer_position;

    // Apply any unread words to the checksum.
    while (p < bp->end_of_data)
    bp->checksum = ((bp->checksum << 1U) ^ (bp->checksum & 1U)) ^ (*(p++));
}


typedef struct finn_bit_pump_for_write finn_bit_pump_for_write;

struct finn_bit_pump_for_write
{
    uint64_t accumulator;           // Bits not yet written to the data buffer
    uint64_t checksum;              // Checksum of data
    uint64_t *buffer_position;      // Next word within the data buffer to be written
    const uint64_t *end_of_buffer;  // End of buffer (which may be after end of data)
    uint8_t empty_bit_count;        // Number of available bits in the accumulator
};


//
// Initialize bit pump for writing to the buffer.
//
// In the general case for writing to the bit pump:
//
// WARNING: The buffer start is assumed to be 64-bit aligned for optimal performance.
// `sod` (start of data) and `eob` (end of buffer) must be multiples of 64 bits
// since this logic is optimized for a 64-bit word size.    Caller must check both
// `sod` and `eod`.
//
// `eob` points to the 64-bit word after the buffer, an illegal access.
//
//
// Special case to get the serialized size without writing to the buffer:
// Both `sod` and `eob` are null.
// When closed, `bp->buffer_position` contains the byte count.
//
static inline void finn_open_buffer_for_write(finn_bit_pump_for_write *bp, uint64_t *sod, uint64_t *eob)
{
    bp->accumulator      = 0U;
    bp->buffer_position  = sod;
    bp->end_of_buffer    = eob;
    bp->checksum         = 0U;
    bp->empty_bit_count  = 64U;
}

//
// Write several bits to the buffer.
//
// `bit_size` must be in range of 1 to 64 inclusive; no check is made.
// `value` must not have more 1 bits than specified by `bit_size`.
// In other words, bits that are left of `bit_size` must be 0s; no check is made.
//
// Return value is nonzero if the end of buffer is reached, an error.
//
// `bp->end_of_buffer` is null to disable writing to the buffer.
//
static int finn_write_buffer(finn_bit_pump_for_write *bp, uint64_t value, uint8_t bit_size)
{
    // Boundary crossing:  Accumulator does not have enough to satisfy the request,
    if (bit_size >= bp->empty_bit_count)
    {
        // Number of bits not yet satisfied
        bit_size -= bp->empty_bit_count;

        // OR as many bits as will fit into the accumulator.
        bp->accumulator |= value << (64U - bp->empty_bit_count);

        // Discard these bits by setting them to 0s.
        // CAUTION: `value` may be unchanged when `bp->empty_bit_count` is 64
        // depending on the processor/ISA.
        value >>= bp->empty_bit_count;

        // Write the word to the buffer unless writes are disabled.
        if (bp->end_of_buffer)
        {
            *bp->buffer_position = bp->accumulator;
        }

        // Advance to the next word in the buffer.
        bp->buffer_position++;

        // Update the checksum.
        bp->checksum = ((bp->checksum << 1) ^ (bp->checksum & 1U)) ^ bp->accumulator;

        // Re-initialize the accumulator and the bits filled.
        bp->accumulator = 0U;
        bp->empty_bit_count = 64U;
    }

    // OR the data into the accumulator.
    // When `bit_size` and `bp->empty_bit_count` are both 64 above, `bit_size`
    // is assigned zero, but `value` may be unchanged.  Check `bit_size` here so
    // that stale `value` is not ORed into the accumulator again.
    if (bit_size)
    {
        bp->accumulator |= (value << (64U - bp->empty_bit_count));
    }

    // Advance the bit count
    bp->empty_bit_count -= bit_size;

    // Return nonzero on buffer overrun.
    return bp->end_of_buffer && bp->buffer_position >= bp->end_of_buffer && bit_size;
}


//
// Close the write buffer and compute the checksum.
//
// Do NOT call this function if `finn_write_buffer` returned nonzero; no check is made.
//
// In the general case for writing to the bit pump:
//
// Postcondition: `bp->buffer_position` points to the word after the end of the data,
// which can be used to calculate the data size in 64-bit words by subtracting from
// `bp->end_of_buffer`.  Buffer data at and after this point is set to zeroes.
//
// Special case to get the serialized size without writing to the buffer:
// Postcondition: ``bp->buffer_position` contains the byte count.
//
// All cases:
// Postcondition: `bp->checksum` contains the checksum of words written to the buffer.
//
static inline void finn_close_buffer_for_write(finn_bit_pump_for_write *bp)
{
    uint64_t *p;

    // The accumulator is not empty.
    if (bp->empty_bit_count < 64U)
    {
        // Update the buffer with the last word.
        if (bp->end_of_buffer)
        {
            *bp->buffer_position = bp->accumulator;
        }

        // Advance to the next word to get an accurate word count.
        bp->buffer_position++;

        // Update the checksum.
        bp->checksum = ((bp->checksum << 1U) ^ (bp->checksum & 1U)) ^ bp->accumulator;
    }

    // Zero out the rest of the buffer.
    for (p = bp->buffer_position; p < bp->end_of_buffer; ++p)
    {
        *p = 0u;
    }
}


static NV_STATUS FinnRmApiSerializeInternal(NvU64 interface, NvU64 message, const char *src, char **dst, size_t dst_size, NvBool seri_up);
static NV_STATUS FinnRmApiSerializeInterface(NvU64 interface, NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnRmApiDeserializeInternal(char **src, NvLength src_size, char *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS FinnRmApiDeserializeInterface(NvU64 interface, NvU64 message, finn_bit_pump_for_read *bp, char *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS FinnNv01RootNvdSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01RootNvdDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_ROOT_NVD *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01RootNvdGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0DmaSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01Device0DmaDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_DMA *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0DmaGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0FbSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01Device0FbDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FB *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0FbGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0FifoSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01Device0FifoDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FIFO *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0FifoGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0GpuSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01Device0GpuDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0GpuGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0GrSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01Device0GrDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GR *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0GrGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0HostSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01Device0HostDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_HOST *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0HostGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv01Device0MsencSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv01Device0MsencDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_MSENC *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv01Device0MsencGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0CeSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0CeDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_CE *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0CeGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0GpuSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0GpuDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0GpuGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0I2cSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0I2cDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_I2C *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0I2cGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0NvdSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0NvdDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_NVD *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0NvdGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0PerfSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0PerfDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_PERF *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0PerfGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20Subdevice0RcSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv20Subdevice0RcDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_RC *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20Subdevice0RcGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv20SubdeviceDiagGpuSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv20SubdeviceDiagGpuDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_DIAG_GPU *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv20SubdeviceDiagGpuGetUnserializedSize(NvU64 message);
static NV_STATUS FinnNv40I2cI2cSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnNv40I2cI2cDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV40_I2C_I2C *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnNv40I2cI2cGetUnserializedSize(NvU64 message);
static NV_STATUS FinnGt200DebuggerDebugSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnGt200DebuggerDebugDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_GT200_DEBUGGER_DEBUG *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnGt200DebuggerDebugGetUnserializedSize(NvU64 message);
static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *dst, NvLength dst_size, NvBool deser_up);
static NvU64 FinnMaxwellChannelGpfifoAGpfifoGetUnserializedSize(NvU64 message);

static NV_STATUS Nv0000CtrlNvdGetDumpParamsSerialize(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0000CtrlNvdGetDumpParamsDeserialize(finn_bit_pump_for_read *bp, NV0000_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlFbGetCapsParamsSerialize(const NV0080_CTRL_FB_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlFbGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FB_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlFifoGetCapsParamsSerialize(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlFifoChannelSerialize(const NV0080_CTRL_FIFO_CHANNEL *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoChannelDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_CHANNEL *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsSerialize(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlFifoGetChannellistParamsSerialize(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlFifoGetChannellistParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlGpuGetClasslistParamsSerialize(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlGpuGetClasslistParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlGrGetCapsParamsSerialize(const NV0080_CTRL_GR_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlGrGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_GR_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlHostGetCapsParamsSerialize(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlHostGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_HOST_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv0080CtrlMsencGetCapsParamsSerialize(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv0080CtrlMsencGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_MSENC_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv2080CtrlCeGetCapsParamsSerialize(const NV2080_CTRL_CE_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv2080CtrlCeGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_CE_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv2080CtrlGpuGetEnginesParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv2080CtrlGpuGetEnginesParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv2080CtrlGpumonSamplesSerialize(const NV2080_CTRL_GPUMON_SAMPLES *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv2080CtrlGpumonSamplesDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_GPUMON_SAMPLES *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv2080CtrlI2cAccessParamsSerialize(const NV2080_CTRL_I2C_ACCESS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv2080CtrlI2cAccessParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_I2C_ACCESS_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv2080CtrlNvdGetDumpParamsSerialize(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv2080CtrlNvdGetDumpParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsSerialize(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *dst, NvLength dst_size, NvBool deser_up);

static NV_STATUS Nv402cCtrlI2cIndexedParamsSerialize(const NV402C_CTRL_I2C_INDEXED_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cIndexedParamsDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_INDEXED_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionTypeCheckEnum(NV402C_CTRL_I2C_TRANSACTION_TYPE id);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv402cCtrlI2cTransactionDataSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA *src, finn_bit_pump_for_write *bp, NvBool seri_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType);
static NV_STATUS Nv402cCtrlI2cTransactionDataDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA *dst, NvLength dst_size, NvBool deser_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType);
static NV_STATUS Nv402cCtrlI2cTransactionParamsSerialize(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv402cCtrlI2cTransactionParamsDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv83deCtrlDebugReadMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv83deCtrlDebugReadMemoryParamsDeserialize(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsDeserialize(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsSerialize(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsSerialize(const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nvb06fCtrlSaveEngineCtxDataParamsSerialize(const NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nvb06fCtrlSaveEngineCtxDataParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *dst, NvLength dst_size, NvBool deser_up);
static NV_STATUS Nvb06fCtrlCmdRestoreEngineCtxDataFinnParamsSerialize(const NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS Nvb06fCtrlCmdRestoreEngineCtxDataFinnParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *dst, NvLength dst_size, NvBool deser_up);

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


static NV_STATUS FinnRmApiSerializeInternal(NvU64 interface, NvU64 message, const char *src, char **dst, size_t dst_size, NvBool seri_up)
{
    // Header
    FINN_RM_API *header;

    // Buffer end
    // `char` is the C-standrd unit of measure for `sizeof` and `size_t`.
    const char *dst_end;

    // Bit pump is used to fill the buffer with serialized data.
    finn_bit_pump_for_write bp;

    // Error code returned from serialization
    NV_STATUS error_code;

    // Input validation
    // Null pointers are not permitted.
    // Buffer must begin on an 8-byte boundary.
    if (!src || !dst || !(*dst) || !dst_size || (uintptr_t) dst & 0x7u)
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Header is at the start of the buffer.
    header = (FINN_RM_API *) *dst;

    // Buffer must end on an 8-byte boundary, so round down.
    dst_end = (const char *) ((uintptr_t) (*dst + dst_size) & ~ (uintptr_t) 0x7);

    // Set header data.
    header->version = FINN_SERIALIZATION_VERSION;
    header->payloadSize = 0;        // Zero until completed successfully
    header->interface = interface;
    header->message = message;

    // Advance past header.
    (*dst) += sizeof(FINN_RM_API);

    // Open the bit pump.
    finn_open_buffer_for_write(&bp, (uint64_t *) *dst, (uint64_t *) dst_end);

    // Call the serializer.
    error_code = FinnRmApiSerializeInterface(interface, message, src, &bp, seri_up);

    // Close the bit pump.
    finn_close_buffer_for_write(&bp);

    // Payload size in bytes
    if (error_code == NV_OK)
        header->payloadSize = (NvU64) (((const char *) bp.buffer_position) - ((const char *) header));

    // Indicate the ending location.
    *dst = (char *) bp.buffer_position;

    // Done
    return error_code;
}


static NV_STATUS FinnRmApiSerializeInterface(NvU64 interface, NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{


    // Forward to interface-specific serialize routine
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return FinnNv01RootNvdSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return FinnNv01Device0DmaSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return FinnNv01Device0FbSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return FinnNv01Device0FifoSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return FinnNv01Device0GpuSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return FinnNv01Device0GrSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return FinnNv01Device0HostSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return FinnNv01Device0MsencSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return FinnNv20Subdevice0CeSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return FinnNv20Subdevice0GpuSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return FinnNv20Subdevice0I2cSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return FinnNv20Subdevice0NvdSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return FinnNv20Subdevice0PerfSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return FinnNv20Subdevice0RcSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_DIAG_GPU):
            return FinnNv20SubdeviceDiagGpuSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return FinnNv40I2cI2cSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return FinnGt200DebuggerDebugSerializeMessage(message, src, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return FinnMaxwellChannelGpfifoAGpfifoSerializeMessage(message, src, bp, seri_up);

        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


static NV_STATUS FinnRmApiDeserializeInternal(char **src, NvLength src_size, char *dst, NvLength dst_size, NvBool deser_up)
{
    // Header
    FINN_RM_API *header;

    // End of data
    const char *src_max;

    // Bit pump is used to read the serialized data.
    finn_bit_pump_for_read bp;

    // Error code
    NV_STATUS status;

    // Input validation
    // Null pointers are not permitted.
    // Buffer must begin on an 8-byte boundary.
    if (!src || !(*src) || !src_size || !dst || !dst_size || (uintptr_t) *src & 0x7u)
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Header data comes first.
    header = (FINN_RM_API *) *src;

    // Check the version.
    if (header->version != FINN_SERIALIZATION_VERSION)
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Set src_max for buffer bounds checking.
    src_max = *src + src_size;

    // Check that source buffer is large enough.
    if (sizeof(FINN_RM_API) > src_size ||
        header->payloadSize > src_size ||
        header->payloadSize < sizeof(FINN_RM_API) ||
        *src + header->payloadSize > src_max ||
        *src + header->payloadSize < *src)
    {
        *src = (char *) &header->payloadSize;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Open the bit punp, skipping past the header.
    finn_open_buffer_for_read(&bp, (const uint64_t *) (*src + sizeof(FINN_RM_API)), (const uint64_t *) (src_max));

    // Dispatch to interface-specific routine
    status = FinnRmApiDeserializeInterface(header->interface, header->message, &bp, dst, dst_size, deser_up);

    // Update the buffer position, error or not.
    *(src) = (char *) bp.buffer_position;

    // Nothing more to do if there was an error.
    if (status != NV_OK)
        return status;

    // Update the checksum.
    finn_close_buffer_for_read(&bp);

    // TODO: Check the checksum

    // Check that the declared size matches the serialization outcome.
    if (header->payloadSize != (NvU64) (((const char *) bp.buffer_position) - ((const char *) header)))
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // All good
    return NV_OK;
}


static NV_STATUS FinnRmApiDeserializeInterface(NvU64 interface, NvU64 message, finn_bit_pump_for_read *bp, char *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to interface-specific routine
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return FinnNv01RootNvdDeserializeMessage(message, bp, (FINN_NV01_ROOT_NVD *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return FinnNv01Device0DmaDeserializeMessage(message, bp, (FINN_NV01_DEVICE_0_DMA *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return FinnNv01Device0FbDeserializeMessage(message, bp, (FINN_NV01_DEVICE_0_FB *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return FinnNv01Device0FifoDeserializeMessage(message, bp, (FINN_NV01_DEVICE_0_FIFO *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return FinnNv01Device0GpuDeserializeMessage(message, bp, (FINN_NV01_DEVICE_0_GPU *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return FinnNv01Device0GrDeserializeMessage(message, bp, (FINN_NV01_DEVICE_0_GR *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return FinnNv01Device0HostDeserializeMessage(message, bp, (FINN_NV01_DEVICE_0_HOST *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return FinnNv01Device0MsencDeserializeMessage(message, bp, (FINN_NV01_DEVICE_0_MSENC *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return FinnNv20Subdevice0CeDeserializeMessage(message, bp, (FINN_NV20_SUBDEVICE_0_CE *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return FinnNv20Subdevice0GpuDeserializeMessage(message, bp, (FINN_NV20_SUBDEVICE_0_GPU *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return FinnNv20Subdevice0I2cDeserializeMessage(message, bp, (FINN_NV20_SUBDEVICE_0_I2C *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return FinnNv20Subdevice0NvdDeserializeMessage(message, bp, (FINN_NV20_SUBDEVICE_0_NVD *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return FinnNv20Subdevice0PerfDeserializeMessage(message, bp, (FINN_NV20_SUBDEVICE_0_PERF *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return FinnNv20Subdevice0RcDeserializeMessage(message, bp, (FINN_NV20_SUBDEVICE_0_RC *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_DIAG_GPU):
            return FinnNv20SubdeviceDiagGpuDeserializeMessage(message, bp, (FINN_NV20_SUBDEVICE_DIAG_GPU *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return FinnNv40I2cI2cDeserializeMessage(message, bp, (FINN_NV40_I2C_I2C *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return FinnGt200DebuggerDebugDeserializeMessage(message, bp, (FINN_GT200_DEBUGGER_DEBUG *) dst, dst_size, deser_up);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return FinnMaxwellChannelGpfifoAGpfifoDeserializeMessage(message, bp, (FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *) dst, dst_size, deser_up);

        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


NvU64 FinnRmApiGetSerializedSize(NvU64 interface, NvU64 message, const NvP64 src)
{
    // Bit pump with writing disabled.
    finn_bit_pump_for_write bp;
    finn_open_buffer_for_write(&bp, (uint64_t *) 0, (uint64_t *) 0);

    // Call the serializer with write-suppressed bit pump.
    // The size is the same in bith directions (up/down).
    // Eeturn zero on error to indicate that this API is not serialized by FINN.
    if (FinnRmApiSerializeInterface(interface, message, (const char *) src, &bp, 0) != NV_OK)
    return 0;

    // Close the bit pump.
    finn_close_buffer_for_write(&bp);

    // Add the header size in bytes to the amount of data serialzied.
    // `buffer_position` is the payload size (not really the buffer position).
    return (NvU64) bp.buffer_position + sizeof(FINN_RM_API);
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
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_DIAG_GPU):
            return FinnNv20SubdeviceDiagGpuGetUnserializedSize(message);
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

static NV_STATUS FinnNv01RootNvdSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv0000CtrlNvdGetDumpParamsSerialize((const NV0000_CTRL_NVD_GET_DUMP_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01RootNvdDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_ROOT_NVD *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv0000CtrlNvdGetDumpParamsDeserialize(bp, (NV0000_CTRL_NVD_GET_DUMP_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01RootNvdGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return sizeof(NV0000_CTRL_NVD_GET_DUMP_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0DmaSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return Nv0080CtrlDmaUpdatePde2ParamsSerialize((const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0DmaDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_DMA *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return Nv0080CtrlDmaUpdatePde2ParamsDeserialize(bp, (NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0DmaGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0FbSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return Nv0080CtrlFbGetCapsParamsSerialize((const NV0080_CTRL_FB_GET_CAPS_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0FbDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FB *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return Nv0080CtrlFbGetCapsParamsDeserialize(bp, (NV0080_CTRL_FB_GET_CAPS_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0FbGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_FB_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0FifoSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return Nv0080CtrlFifoGetCapsParamsSerialize((const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return Nv0080CtrlFifoStartSelectedChannelsParamsSerialize((const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return Nv0080CtrlFifoGetChannellistParamsSerialize((const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0FifoDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FIFO *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return Nv0080CtrlFifoGetCapsParamsDeserialize(bp, (NV0080_CTRL_FIFO_GET_CAPS_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return Nv0080CtrlFifoStartSelectedChannelsParamsDeserialize(bp, (NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return Nv0080CtrlFifoGetChannellistParamsDeserialize(bp, (NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0FifoGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
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

static NV_STATUS FinnNv01Device0GpuSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return Nv0080CtrlGpuGetClasslistParamsSerialize((const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0GpuDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return Nv0080CtrlGpuGetClasslistParamsDeserialize(bp, (NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0GpuGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0GrSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return Nv0080CtrlGrGetCapsParamsSerialize((const NV0080_CTRL_GR_GET_CAPS_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0GrDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GR *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return Nv0080CtrlGrGetCapsParamsDeserialize(bp, (NV0080_CTRL_GR_GET_CAPS_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0GrGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_GR_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0HostSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return Nv0080CtrlHostGetCapsParamsSerialize((const NV0080_CTRL_HOST_GET_CAPS_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0HostDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_HOST *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return Nv0080CtrlHostGetCapsParamsDeserialize(bp, (NV0080_CTRL_HOST_GET_CAPS_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0HostGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_HOST_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv01Device0MsencSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return Nv0080CtrlMsencGetCapsParamsSerialize((const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv01Device0MsencDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_MSENC *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return Nv0080CtrlMsencGetCapsParamsDeserialize(bp, (NV0080_CTRL_MSENC_GET_CAPS_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv01Device0MsencGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_MSENC_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0CeSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return Nv2080CtrlCeGetCapsParamsSerialize((const NV2080_CTRL_CE_GET_CAPS_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0CeDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_CE *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return Nv2080CtrlCeGetCapsParamsDeserialize(bp, (NV2080_CTRL_CE_GET_CAPS_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0CeGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0GpuSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return Nv2080CtrlGpuGetEnginesParamsSerialize((const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return Nv2080CtrlGpuGetEngineClasslistParamsSerialize((const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0GpuDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_GPU *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return Nv2080CtrlGpuGetEnginesParamsDeserialize(bp, (NV2080_CTRL_GPU_GET_ENGINES_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return Nv2080CtrlGpuGetEngineClasslistParamsDeserialize(bp, (NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0GpuGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
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

static NV_STATUS FinnNv20Subdevice0I2cSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return Nv2080CtrlI2cAccessParamsSerialize((const NV2080_CTRL_I2C_ACCESS_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0I2cDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_I2C *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return Nv2080CtrlI2cAccessParamsDeserialize(bp, (NV2080_CTRL_I2C_ACCESS_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0I2cGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0NvdSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv2080CtrlNvdGetDumpParamsSerialize((const NV2080_CTRL_NVD_GET_DUMP_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0NvdDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_NVD *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return Nv2080CtrlNvdGetDumpParamsDeserialize(bp, (NV2080_CTRL_NVD_GET_DUMP_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0NvdGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0PerfSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):
            return Nv2080CtrlGpumonSamplesSerialize((const NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0PerfDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_PERF *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):
            return Nv2080CtrlGpumonSamplesDeserialize(bp, (NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0PerfGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):
            return sizeof(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20Subdevice0RcSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return Nv2080CtrlRcReadVirtualMemParamsSerialize((const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20Subdevice0RcDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_RC *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return Nv2080CtrlRcReadVirtualMemParamsDeserialize(bp, (NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20Subdevice0RcGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS FinnNv20SubdeviceDiagGpuSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv20SubdeviceDiagGpuDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_DIAG_GPU *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv20SubdeviceDiagGpuGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        default:
            return 0;
    }
}

static NV_STATUS FinnNv40I2cI2cSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return Nv402cCtrlI2cIndexedParamsSerialize((const NV402C_CTRL_I2C_INDEXED_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return Nv402cCtrlI2cTransactionParamsSerialize((const NV402C_CTRL_I2C_TRANSACTION_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnNv40I2cI2cDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV40_I2C_I2C *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return Nv402cCtrlI2cIndexedParamsDeserialize(bp, (NV402C_CTRL_I2C_INDEXED_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return Nv402cCtrlI2cTransactionParamsDeserialize(bp, (NV402C_CTRL_I2C_TRANSACTION_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnNv40I2cI2cGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
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

static NV_STATUS FinnGt200DebuggerDebugSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return Nv83deCtrlDebugReadMemoryParamsSerialize((const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return Nv83deCtrlDebugWriteMemoryParamsSerialize((const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnGt200DebuggerDebugDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_GT200_DEBUGGER_DEBUG *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return Nv83deCtrlDebugReadMemoryParamsDeserialize(bp, (NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return Nv83deCtrlDebugWriteMemoryParamsDeserialize(bp, (NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnGt200DebuggerDebugGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
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

static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoSerializeMessage(NvU64 message, const char *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return Nvb06fCtrlGetEngineCtxDataParamsSerialize((const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS):
            return Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsSerialize((const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS):
            return Nvb06fCtrlSaveEngineCtxDataParamsSerialize((const NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *) src, bp, seri_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS):
            return Nvb06fCtrlCmdRestoreEngineCtxDataFinnParamsSerialize((const NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *) src, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NV_STATUS FinnMaxwellChannelGpfifoAGpfifoDeserializeMessage(NvU64 message, finn_bit_pump_for_read *bp, FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *dst, NvLength dst_size, NvBool deser_up)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return Nvb06fCtrlGetEngineCtxDataParamsDeserialize(bp, (NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS):
            return Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsDeserialize(bp, (NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS):
            return Nvb06fCtrlSaveEngineCtxDataParamsDeserialize(bp, (NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *) dst, dst_size, deser_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS):
            return Nvb06fCtrlCmdRestoreEngineCtxDataFinnParamsDeserialize(bp, (NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *) dst, dst_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

static NvU64 FinnMaxwellChannelGpfifoAGpfifoGetUnserializedSize(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return sizeof(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS):
            return sizeof(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS);
        case FINN_MESSAGE_ID(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS):
            return sizeof(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS):
            return sizeof(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS);
        default:
            return 0;
    }
}

static NV_STATUS Nv0000CtrlNvdGetDumpParamsSerialize(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->component
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->component, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    if (src->size > NV0000_CTRL_NVD_MAX_DUMP_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->size, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pBuffer), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pBuffer)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->size); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pBuffer))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pBuffer)
            FINN_FREE(src->pBuffer);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0000CtrlNvdGetDumpParamsDeserialize(finn_bit_pump_for_read *bp, NV0000_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0000_CTRL_NVD_GET_DUMP_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->component = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->component


    // Deserialize 4-byte NvU32 object.
    dst->size = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->size > NV0000_CTRL_NVD_MAX_DUMP_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pBuffer = FINN_MALLOC((dst->size));
            if (!dst->pBuffer)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pBuffer, (dst->size));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pBuffer)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->size); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pBuffer))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pBuffer = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->physAddr
    // Deserialize 8-byte NvU64 object.
    if (finn_write_buffer(bp, src->physAddr, 8 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->numEntries
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->numEntries, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->aperture
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->aperture, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlDmaUpdatePde2PageTableParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 8-byte NvU64 object.
    dst->physAddr = (NvU64) finn_read_buffer(bp, 8 * 8);
    // No range check for dst->physAddr


    // Deserialize 4-byte NvU32 object.
    dst->numEntries = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->numEntries


    // Deserialize 4-byte NvU32 object.
    dst->aperture = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->aperture


exit:
    return status;
}


static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsSerialize(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3f;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->pdeIndex
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->pdeIndex, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->flags
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->flags, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->hVASpace
    // Deserialize 4-byte NvHandle object.
    if (finn_write_buffer(bp, src->hVASpace, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->subDeviceId
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->subDeviceId, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Bounded nested fields
    for (NvU64 i = 0; i < (NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE); ++i)
    {
        status = Nv0080CtrlDmaUpdatePde2PageTableParamsSerialize(&src->ptParams[i], bp, seri_up);
        if (status != NV_OK)
            goto exit;
    }

    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pPdeBuffer), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pPdeBuffer)
    {

        // Serialize each 8-byte NvU64 element.
        for (NvU64 j = 0; j < 1; ++j)
        {
            if (finn_write_buffer(bp, ((NvU64 *) NvP64_VALUE(src->pPdeBuffer))[j], 8 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pPdeBuffer)
            FINN_FREE(src->pPdeBuffer);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlDmaUpdatePde2ParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->pdeIndex = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->pdeIndex


    // Deserialize 4-byte NvU32 object.
    dst->flags = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->flags


    // Deserialize 4-byte NvHandle object.
    dst->hVASpace = (NvHandle) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->hVASpace


    // Deserialize 4-byte NvU32 object.
    dst->subDeviceId = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->subDeviceId


    // Bounded nested fields
    for (NvU64 i = 0; i < (NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE); ++i)
    {
        status = Nv0080CtrlDmaUpdatePde2PageTableParamsDeserialize(bp, &dst->ptParams[i], dst_size, deser_up);
        if (status != NV_OK)
            goto exit;
    }

    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pPdeBuffer = FINN_MALLOC(8);
            if (!dst->pPdeBuffer)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pPdeBuffer, 8);
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pPdeBuffer)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 8-byte NvU64 element.
        for (NvU64 j = 0; j < 1; ++j)
        {
            ((NvU64 *) NvP64_VALUE(dst->pPdeBuffer))[j] = (NvU64) finn_read_buffer(bp, 8 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pPdeBuffer = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlFbGetCapsParamsSerialize(const NV0080_CTRL_FB_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    if (src->capsTblSize > NV0080_CTRL_FB_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->capsTblSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->capsTbl), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->capsTbl)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->capsTblSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->capsTbl))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->capsTbl)
            FINN_FREE(src->capsTbl);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlFbGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FB_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_FB_GET_CAPS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->capsTblSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->capsTblSize > NV0080_CTRL_FB_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->capsTbl = FINN_MALLOC((dst->capsTblSize));
            if (!dst->capsTbl)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->capsTbl, (dst->capsTblSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->capsTbl)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->capsTblSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->capsTbl))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoGetCapsParamsSerialize(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    if (src->capsTblSize > NV0080_CTRL_FIFO_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->capsTblSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->capsTbl), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->capsTbl)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->capsTblSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->capsTbl))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->capsTbl)
            FINN_FREE(src->capsTbl);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_FIFO_GET_CAPS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->capsTblSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->capsTblSize > NV0080_CTRL_FIFO_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->capsTbl = FINN_MALLOC((dst->capsTblSize));
            if (!dst->capsTbl)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->capsTbl, (dst->capsTblSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->capsTbl)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->capsTblSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->capsTbl))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoChannelSerialize(const NV0080_CTRL_FIFO_CHANNEL *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->hChannel
    // Deserialize 4-byte NvHandle object.
    if (finn_write_buffer(bp, src->hChannel, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoChannelDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_CHANNEL *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_FIFO_CHANNEL) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvHandle object.
    dst->hChannel = (NvHandle) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->hChannel


exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsSerialize(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->fifoStartChannelListCount
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->fifoStartChannelListCount, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    for (NvU64 i = 0; i < (8); ++i)
    {
        // No range check for src->channelHandle[i]
        // Deserialize 4-byte NvHandle object.
        if (finn_write_buffer(bp, src->channelHandle[i], 4 * 8))
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
            goto exit;
        }
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->fifoStartChannelList), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->fifoStartChannelList)
    {
        for (NvU64 i = 0; i < (src->fifoStartChannelListCount); ++i)
        {
            status = Nv0080CtrlFifoChannelSerialize(&(((const NV0080_CTRL_FIFO_CHANNEL *) (NvP64_VALUE(src->fifoStartChannelList)))[i]), bp, seri_up);

            if (status != NV_OK)
                goto exit;
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->fifoStartChannelList)
            FINN_FREE(src->fifoStartChannelList);
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoStartSelectedChannelsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->fifoStartChannelListCount = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->fifoStartChannelListCount


    for (NvU64 i = 0; i < (8); ++i)
    {
        // Deserialize 4-byte NvHandle object.
        dst->channelHandle[i] = (NvHandle) finn_read_buffer(bp, 4 * 8);
        // No range check for dst->channelHandle[i]
    }


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {
        // Caller must set up the pointers when deserializing down.
        if (!deser_up)
        {
            // Data-presence indicator should be false if empty.
            // Check for integer overflow in the element size variable.
            if ((dst->fifoStartChannelListCount) < 1 ||
                (sizeof(NV0080_CTRL_FIFO_CHANNEL) * (dst->fifoStartChannelListCount)) < sizeof(NV0080_CTRL_FIFO_CHANNEL))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }

            // Allocate memory and set pointer when deserializing down.
            // (Calling cods is expected to do so when deserializing up.)
            dst->fifoStartChannelList = FINN_MALLOC((sizeof(NV0080_CTRL_FIFO_CHANNEL) * (dst->fifoStartChannelListCount)));
            if (!dst->fifoStartChannelList)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->fifoStartChannelList, (sizeof(NV0080_CTRL_FIFO_CHANNEL) * (dst->fifoStartChannelListCount)));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->fifoStartChannelList)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        for (NvU64 i = 0; i < (dst->fifoStartChannelListCount); ++i)
        {
            // Deserialize each element.
            status = Nv0080CtrlFifoChannelDeserialize(bp, &(((NV0080_CTRL_FIFO_CHANNEL *) (NvP64_VALUE(dst->fifoStartChannelList)))[i]), sizeof(NV0080_CTRL_FIFO_CHANNEL), deser_up);
            if (status != NV_OK)
                goto exit;
        }
    }

    // Data is not present; set to NULL.
    else
    {
        if (!deser_up)
            dst->fifoStartChannelList = NULL;
    }


exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoGetChannellistParamsSerialize(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->numChannels
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->numChannels, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pChannelHandleList), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pChannelHandleList)
    {

        // Serialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (src->numChannels); ++j)
        {
            if (finn_write_buffer(bp, ((NvU32 *) NvP64_VALUE(src->pChannelHandleList))[j], 4 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pChannelHandleList)
            FINN_FREE(src->pChannelHandleList);
    }

    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pChannelList), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pChannelList)
    {

        // Serialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (src->numChannels); ++j)
        {
            if (finn_write_buffer(bp, ((NvU32 *) NvP64_VALUE(src->pChannelList))[j], 4 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pChannelList)
            FINN_FREE(src->pChannelList);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlFifoGetChannellistParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->numChannels = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->numChannels


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pChannelHandleList = FINN_MALLOC((dst->numChannels) * 4);
            if (!dst->pChannelHandleList)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pChannelHandleList, (dst->numChannels) * 4);
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pChannelHandleList)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (dst->numChannels); ++j)
        {
            ((NvU32 *) NvP64_VALUE(dst->pChannelHandleList))[j] = (NvU32) finn_read_buffer(bp, 4 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pChannelHandleList = NULL;
    }

    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pChannelList = FINN_MALLOC((dst->numChannels) * 4);
            if (!dst->pChannelList)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pChannelList, (dst->numChannels) * 4);
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pChannelList)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (dst->numChannels); ++j)
        {
            ((NvU32 *) NvP64_VALUE(dst->pChannelList))[j] = (NvU32) finn_read_buffer(bp, 4 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pChannelList = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlGpuGetClasslistParamsSerialize(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->numClasses
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->numClasses, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->classList), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->classList)
    {

        // Serialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (src->numClasses); ++j)
        {
            if (finn_write_buffer(bp, ((NvU32 *) NvP64_VALUE(src->classList))[j], 4 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->classList)
            FINN_FREE(src->classList);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlGpuGetClasslistParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->numClasses = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->numClasses


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->classList = FINN_MALLOC((dst->numClasses) * 4);
            if (!dst->classList)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->classList, (dst->numClasses) * 4);
        }

        // Otherwise the pointer must be provided.
        else if (!dst->classList)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (dst->numClasses); ++j)
        {
            ((NvU32 *) NvP64_VALUE(dst->classList))[j] = (NvU32) finn_read_buffer(bp, 4 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->classList = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlGrGetCapsParamsSerialize(const NV0080_CTRL_GR_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->capsTblSize
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->capsTblSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->capsTbl), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->capsTbl)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->capsTblSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->capsTbl))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->capsTbl)
            FINN_FREE(src->capsTbl);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlGrGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_GR_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_GR_GET_CAPS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->capsTblSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->capsTblSize


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->capsTbl = FINN_MALLOC((dst->capsTblSize));
            if (!dst->capsTbl)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->capsTbl, (dst->capsTblSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->capsTbl)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->capsTblSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->capsTbl))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlHostGetCapsParamsSerialize(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    if (src->capsTblSize > NV0080_CTRL_HOST_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->capsTblSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->capsTbl), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->capsTbl)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->capsTblSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->capsTbl))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->capsTbl)
            FINN_FREE(src->capsTbl);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlHostGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_HOST_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_HOST_GET_CAPS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->capsTblSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->capsTblSize > NV0080_CTRL_HOST_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->capsTbl = FINN_MALLOC((dst->capsTblSize));
            if (!dst->capsTbl)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->capsTbl, (dst->capsTblSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->capsTbl)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->capsTblSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->capsTbl))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv0080CtrlMsencGetCapsParamsSerialize(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    if (src->capsTblSize > NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->capsTblSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->capsTbl), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->capsTbl)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->capsTblSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->capsTbl))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->capsTbl)
            FINN_FREE(src->capsTbl);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv0080CtrlMsencGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV0080_CTRL_MSENC_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV0080_CTRL_MSENC_GET_CAPS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->capsTblSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->capsTblSize > NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->capsTbl = FINN_MALLOC((dst->capsTblSize));
            if (!dst->capsTbl)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->capsTbl, (dst->capsTblSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->capsTbl)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->capsTblSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->capsTbl))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv2080CtrlCeGetCapsParamsSerialize(const NV2080_CTRL_CE_GET_CAPS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->ceEngineType
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->ceEngineType, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    if (src->capsTblSize > NV2080_CTRL_CE_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->capsTblSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->capsTbl), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->capsTbl)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->capsTblSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->capsTbl))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->capsTbl)
            FINN_FREE(src->capsTbl);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv2080CtrlCeGetCapsParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_CE_GET_CAPS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->ceEngineType = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->ceEngineType


    // Deserialize 4-byte NvU32 object.
    dst->capsTblSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->capsTblSize > NV2080_CTRL_CE_CAPS_TBL_SIZE)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->capsTbl = FINN_MALLOC((dst->capsTblSize));
            if (!dst->capsTbl)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->capsTbl, (dst->capsTblSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->capsTbl)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->capsTblSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->capsTbl))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->capsTbl = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv2080CtrlGpuGetEnginesParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->engineCount
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->engineCount, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->engineList), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->engineList)
    {

        // Serialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (src->engineCount); ++j)
        {
            if (finn_write_buffer(bp, ((NvU32 *) NvP64_VALUE(src->engineList))[j], 4 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->engineList)
            FINN_FREE(src->engineList);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv2080CtrlGpuGetEnginesParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->engineCount = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->engineCount


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->engineList = FINN_MALLOC((dst->engineCount) * 4);
            if (!dst->engineList)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->engineList, (dst->engineCount) * 4);
        }

        // Otherwise the pointer must be provided.
        else if (!dst->engineList)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (dst->engineCount); ++j)
        {
            ((NvU32 *) NvP64_VALUE(dst->engineList))[j] = (NvU32) finn_read_buffer(bp, 4 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->engineList = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsSerialize(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->engineType
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->engineType, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->numClasses
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->numClasses, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->classList), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->classList)
    {

        // Serialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (src->numClasses); ++j)
        {
            if (finn_write_buffer(bp, ((NvU32 *) NvP64_VALUE(src->classList))[j], 4 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->classList)
            FINN_FREE(src->classList);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv2080CtrlGpuGetEngineClasslistParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->engineType = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->engineType


    // Deserialize 4-byte NvU32 object.
    dst->numClasses = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->numClasses


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->classList = FINN_MALLOC((dst->numClasses) * 4);
            if (!dst->classList)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->classList, (dst->numClasses) * 4);
        }

        // Otherwise the pointer must be provided.
        else if (!dst->classList)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 4-byte NvU32 element.
        for (NvU64 j = 0; j < (dst->numClasses); ++j)
        {
            ((NvU32 *) NvP64_VALUE(dst->classList))[j] = (NvU32) finn_read_buffer(bp, 4 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->classList = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv2080CtrlGpumonSamplesSerialize(const NV2080_CTRL_GPUMON_SAMPLES *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1f;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->bufSize
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->bufSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->count
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->count, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->tracker
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->tracker, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->type
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->type, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pSamples), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pSamples)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->bufSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pSamples))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pSamples)
            FINN_FREE(src->pSamples);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv2080CtrlGpumonSamplesDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_GPUMON_SAMPLES *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV2080_CTRL_GPUMON_SAMPLES) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->bufSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->bufSize


    // Deserialize 4-byte NvU32 object.
    dst->count = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->count


    // Deserialize 4-byte NvU32 object.
    dst->tracker = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->tracker


    // Deserialize 1-byte NvU8 object.
    dst->type = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->type


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pSamples = FINN_MALLOC((dst->bufSize));
            if (!dst->pSamples)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pSamples, (dst->bufSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pSamples)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->bufSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pSamples))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pSamples = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv2080CtrlI2cAccessParamsSerialize(const NV2080_CTRL_I2C_ACCESS_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1ff;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->token
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->token, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->cmd
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->cmd, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->port
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->port, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->flags
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->flags, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->status
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->status, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    if (src->dataBuffSize > NV2080_CTRL_I2C_MAX_ENTRIES)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->dataBuffSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->speed
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->speed, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->encrClientID
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->encrClientID, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->data), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->data)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->dataBuffSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->data))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->data)
            FINN_FREE(src->data);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv2080CtrlI2cAccessParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_I2C_ACCESS_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1ff)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->token = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->token


    // Deserialize 4-byte NvU32 object.
    dst->cmd = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->cmd


    // Deserialize 4-byte NvU32 object.
    dst->port = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->port


    // Deserialize 4-byte NvU32 object.
    dst->flags = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->flags


    // Deserialize 4-byte NvU32 object.
    dst->status = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->status


    // Deserialize 4-byte NvU32 object.
    dst->dataBuffSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->dataBuffSize > NV2080_CTRL_I2C_MAX_ENTRIES)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Deserialize 4-byte NvU32 object.
    dst->speed = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->speed


    // Deserialize 4-byte NvU32 object.
    dst->encrClientID = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->encrClientID


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->data = FINN_MALLOC((dst->dataBuffSize));
            if (!dst->data)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->data, (dst->dataBuffSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->data)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->dataBuffSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->data))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->data = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv2080CtrlNvdGetDumpParamsSerialize(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->component
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->component, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->size
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->size, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pBuffer), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pBuffer)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->size); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pBuffer))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pBuffer)
            FINN_FREE(src->pBuffer);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv2080CtrlNvdGetDumpParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_NVD_GET_DUMP_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->component = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->component


    // Deserialize 4-byte NvU32 object.
    dst->size = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->size


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pBuffer = FINN_MALLOC((dst->size));
            if (!dst->pBuffer)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pBuffer, (dst->size));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pBuffer)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->size); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pBuffer))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pBuffer = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsSerialize(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0xf;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->virtAddress
    // Deserialize 8-byte NvU64 object.
    if (finn_write_buffer(bp, src->virtAddress, 8 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->hChannel
    // Deserialize 4-byte NvHandle object.
    if (finn_write_buffer(bp, src->hChannel, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bufferSize
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->bufferSize, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->bufferPtr), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->bufferPtr)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->bufferSize); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->bufferPtr))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->bufferPtr)
            FINN_FREE(src->bufferPtr);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv2080CtrlRcReadVirtualMemParamsDeserialize(finn_bit_pump_for_read *bp, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 8-byte NvU64 object.
    dst->virtAddress = (NvU64) finn_read_buffer(bp, 8 * 8);
    // No range check for dst->virtAddress


    // Deserialize 4-byte NvHandle object.
    dst->hChannel = (NvHandle) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->hChannel


    // Deserialize 4-byte NvU32 object.
    dst->bufferSize = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->bufferSize


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->bufferPtr = FINN_MALLOC((dst->bufferSize));
            if (!dst->bufferPtr)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->bufferPtr, (dst->bufferSize));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->bufferPtr)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->bufferSize); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->bufferPtr))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->bufferPtr = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cIndexedParamsSerialize(const NV402C_CTRL_I2C_INDEXED_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0xff;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->flags
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->flags, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    if (src->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->indexLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->messageLength
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->messageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->address
    // Deserialize 2-byte NvU16 object.
    if (finn_write_buffer(bp, src->address, 2 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->portId
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->portId, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bIsWrite
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->bIsWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_INDEX_LENGTH_MAX); ++i)
    {
        // No range check for src->index[i]
        // Deserialize 1-byte NvU8 object.
        if (finn_write_buffer(bp, src->index[i], 1 * 8))
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
            goto exit;
        }
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pMessage), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pMessage)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->messageLength); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pMessage))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pMessage)
            FINN_FREE(src->pMessage);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cIndexedParamsDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_INDEXED_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_INDEXED_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0xff)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->flags = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->flags


    // Deserialize 4-byte NvU32 object.
    dst->indexLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Deserialize 4-byte NvU32 object.
    dst->messageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->messageLength


    // Deserialize 2-byte NvU16 object.
    dst->address = (NvU16) finn_read_buffer(bp, 2 * 8);
    // No range check for dst->address


    // Deserialize 1-byte NvU8 object.
    dst->portId = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->portId


    // Deserialize 1-byte NvU8 object.
    dst->bIsWrite = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bIsWrite


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_INDEX_LENGTH_MAX); ++i)
    {
        // Deserialize 1-byte NvU8 object.
        dst->index[i] = (NvU8) finn_read_buffer(bp, 1 * 8);
        // No range check for dst->index[i]
    }


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pMessage = FINN_MALLOC((dst->messageLength));
            if (!dst->pMessage)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pMessage, (dst->messageLength));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pMessage)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->messageLength); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pMessage))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionTypeCheckEnum(NV402C_CTRL_I2C_TRANSACTION_TYPE id)
{
    switch (id)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
            return NV_OK; // okay
    }

    FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
    return NV_ERR_INVALID_ARGUMENT;
}
static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->warFlags
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->warFlags, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusQuickRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->warFlags = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->warFlags


    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->message
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->message, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataI2cByteRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


    // Deserialize 1-byte NvU8 object.
    dst->message = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->message


exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->messageLength
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->messageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pMessage), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pMessage)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->messageLength); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pMessage))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pMessage)
            FINN_FREE(src->pMessage);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBlockRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->messageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->messageLength


    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pMessage = FINN_MALLOC((dst->messageLength));
            if (!dst->pMessage)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pMessage, (dst->messageLength));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pMessage)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->messageLength); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pMessage))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->registerAddress
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->registerAddress, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->message
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->message, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusByteRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


    // Deserialize 1-byte NvU8 object.
    dst->registerAddress = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->registerAddress


    // Deserialize 1-byte NvU8 object.
    dst->message = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->message


exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->message
    // Deserialize 2-byte NvU16 object.
    if (finn_write_buffer(bp, src->message, 2 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->registerAddress
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->registerAddress, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusWordRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 2-byte NvU16 object.
    dst->message = (NvU16) finn_read_buffer(bp, 2 * 8);
    // No range check for dst->message


    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


    // Deserialize 1-byte NvU8 object.
    dst->registerAddress = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->registerAddress


exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1f;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->warFlags
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->warFlags, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->messageLength
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->messageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->registerAddress
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->registerAddress, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pMessage), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pMessage)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->messageLength); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pMessage))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pMessage)
            FINN_FREE(src->pMessage);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataI2cBufferRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->warFlags = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->warFlags


    // Deserialize 4-byte NvU32 object.
    dst->messageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->messageLength


    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


    // Deserialize 1-byte NvU8 object.
    dst->registerAddress = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->registerAddress


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pMessage = FINN_MALLOC((dst->messageLength));
            if (!dst->pMessage)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pMessage, (dst->messageLength));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pMessage)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->messageLength); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pMessage))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0xf;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->messageLength
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->messageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->registerAddress
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->registerAddress, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pMessage), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pMessage)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->messageLength); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pMessage))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pMessage)
            FINN_FREE(src->pMessage);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->messageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->messageLength


    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


    // Deserialize 1-byte NvU8 object.
    dst->registerAddress = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->registerAddress


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pMessage = FINN_MALLOC((dst->messageLength));
            if (!dst->pMessage)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pMessage, (dst->messageLength));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pMessage)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->messageLength); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pMessage))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->writeMessage
    // Deserialize 2-byte NvU16 object.
    if (finn_write_buffer(bp, src->writeMessage, 2 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->readMessage
    // Deserialize 2-byte NvU16 object.
    if (finn_write_buffer(bp, src->readMessage, 2 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->registerAddress
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->registerAddress, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusProcessCallDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 2-byte NvU16 object.
    dst->writeMessage = (NvU16) finn_read_buffer(bp, 2 * 8);
    // No range check for dst->writeMessage


    // Deserialize 2-byte NvU16 object.
    dst->readMessage = (NvU16) finn_read_buffer(bp, 2 * 8);
    // No range check for dst->readMessage


    // Deserialize 1-byte NvU8 object.
    dst->registerAddress = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->registerAddress


exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1f;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    if (src->writeMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->writeMessageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    if (src->readMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->readMessageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->registerAddress
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->registerAddress, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX); ++i)
    {
        // No range check for src->writeMessage[i]
        // Deserialize 1-byte NvU8 object.
        if (finn_write_buffer(bp, src->writeMessage[i], 1 * 8))
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
            goto exit;
        }
    }


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX); ++i)
    {
        // No range check for src->readMessage[i]
        // Deserialize 1-byte NvU8 object.
        if (finn_write_buffer(bp, src->readMessage[i], 1 * 8))
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
            goto exit;
        }
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->writeMessageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->writeMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Deserialize 4-byte NvU32 object.
    dst->readMessageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->readMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Deserialize 1-byte NvU8 object.
    dst->registerAddress = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->registerAddress


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX); ++i)
    {
        // Deserialize 1-byte NvU8 object.
        dst->writeMessage[i] = (NvU8) finn_read_buffer(bp, 1 * 8);
        // No range check for dst->writeMessage[i]
    }


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX); ++i)
    {
        // Deserialize 1-byte NvU8 object.
        dst->readMessage[i] = (NvU8) finn_read_buffer(bp, 1 * 8);
        // No range check for dst->readMessage[i]
    }


exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x3f;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->warFlags
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->warFlags, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    if (src->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }

    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->indexLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->messageLength
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->messageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->bWrite
    // Deserialize 1-byte NvBool object.
    if (finn_write_buffer(bp, src->bWrite, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_INDEX_LENGTH_MAX); ++i)
    {
        // No range check for src->index[i]
        // Deserialize 1-byte NvU8 object.
        if (finn_write_buffer(bp, src->index[i], 1 * 8))
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
            goto exit;
        }
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pMessage), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pMessage)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->messageLength); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pMessage))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pMessage)
            FINN_FREE(src->pMessage);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x3f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->warFlags = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->warFlags


    // Deserialize 4-byte NvU32 object.
    dst->indexLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    if (dst->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        status = NV_ERR_OUT_OF_RANGE;
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        goto exit;
    }



    // Deserialize 4-byte NvU32 object.
    dst->messageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->messageLength


    // Deserialize 1-byte NvBool object.
    dst->bWrite = (NvBool) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->bWrite


    for (NvU64 i = 0; i < (NV402C_CTRL_I2C_INDEX_LENGTH_MAX); ++i)
    {
        // Deserialize 1-byte NvU8 object.
        dst->index[i] = (NvU8) finn_read_buffer(bp, 1 * 8);
        // No range check for dst->index[i]
    }


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pMessage = FINN_MALLOC((dst->messageLength));
            if (!dst->pMessage)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pMessage, (dst->messageLength));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pMessage)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->messageLength); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pMessage))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0xf;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->messageLength
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->messageLength, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->segmentNumber
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->segmentNumber, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->registerAddress
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->registerAddress, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pMessage), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pMessage)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->messageLength); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pMessage))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pMessage)
            FINN_FREE(src->pMessage);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataReadEdidDdcDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->messageLength = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->messageLength


    // Deserialize 1-byte NvU8 object.
    dst->segmentNumber = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->segmentNumber


    // Deserialize 1-byte NvU8 object.
    dst->registerAddress = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->registerAddress


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pMessage = FINN_MALLOC((dst->messageLength));
            if (!dst->pMessage)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pMessage, (dst->messageLength));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pMessage)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->messageLength); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pMessage))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pMessage = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataSerialize(const NV402C_CTRL_I2C_TRANSACTION_DATA *src, finn_bit_pump_for_write *bp, NvBool seri_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7ff;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Field copying based on union selector
    switch (transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusQuickRwSerialize(&src->smbusQuickData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cByteRwSerialize(&src->i2cByteData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusByteRwSerialize(&src->smbusByteData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusWordRwSerialize(&src->smbusWordData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusProcessCallSerialize(&src->smbusProcessData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallSerialize(&src->smbusBlockProcessData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBlockRwSerialize(&src->i2cBlockData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBufferRwSerialize(&src->i2cBufferData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockRwSerialize(&src->smbusBlockData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwSerialize(&src->smbusMultibyteRegisterData, bp, seri_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            status = Nv402cCtrlI2cTransactionDataReadEdidDdcSerialize(&src->edidData, bp, seri_up);
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

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionDataDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA *dst, NvLength dst_size, NvBool deser_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7ff)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Field copying based on union selector
    switch (transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusQuickRwDeserialize(bp, &dst->smbusQuickData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cByteRwDeserialize(bp, &dst->i2cByteData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusByteRwDeserialize(bp, &dst->smbusByteData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusWordRwDeserialize(bp, &dst->smbusWordData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusProcessCallDeserialize(bp, &dst->smbusProcessData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockProcessCallDeserialize(bp, &dst->smbusBlockProcessData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBlockRwDeserialize(bp, &dst->i2cBlockData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            status = Nv402cCtrlI2cTransactionDataI2cBufferRwDeserialize(bp, &dst->i2cBufferData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusBlockRwDeserialize(bp, &dst->smbusBlockData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            status = Nv402cCtrlI2cTransactionDataSmbusMultibyteRegisterBlockRwDeserialize(bp, &dst->smbusMultibyteRegisterData, dst_size, deser_up);
            if (status != NV_OK)
                goto exit;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            status = Nv402cCtrlI2cTransactionDataReadEdidDdcDeserialize(bp, &dst->edidData, dst_size, deser_up);
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

exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionParamsSerialize(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1f;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->flags
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->flags, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->transType
    status = Nv402cCtrlI2cTransactionTypeCheckEnum(src->transType);
    if (status != NV_OK)
        goto exit;

    // Deserialize 4-byte NV402C_CTRL_I2C_TRANSACTION_TYPE object.
    if (finn_write_buffer(bp, src->transType, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->deviceAddress
    // Deserialize 2-byte NvU16 object.
    if (finn_write_buffer(bp, src->deviceAddress, 2 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->portId
    // Deserialize 1-byte NvU8 object.
    if (finn_write_buffer(bp, src->portId, 1 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    status = Nv402cCtrlI2cTransactionDataSerialize(&src->transData, bp, seri_up, src->transType);
    if (status != NV_OK)
        goto exit;

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv402cCtrlI2cTransactionParamsDeserialize(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1f)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->flags = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->flags


    // Deserialize 4-byte NV402C_CTRL_I2C_TRANSACTION_TYPE object.
    dst->transType = (NV402C_CTRL_I2C_TRANSACTION_TYPE) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->transType
    status = Nv402cCtrlI2cTransactionTypeCheckEnum(dst->transType);
    if (status != NV_OK)
        goto exit;



    // Deserialize 2-byte NvU16 object.
    dst->deviceAddress = (NvU16) finn_read_buffer(bp, 2 * 8);
    // No range check for dst->deviceAddress


    // Deserialize 1-byte NvU8 object.
    dst->portId = (NvU8) finn_read_buffer(bp, 1 * 8);
    // No range check for dst->portId


    // Unbounded fields
    status = Nv402cCtrlI2cTransactionDataDeserialize(bp, &dst->transData, dst_size, deser_up, dst->transType);
    if (status != NV_OK)
        goto exit;

exit:
    return status;
}


static NV_STATUS Nv83deCtrlDebugReadMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0xf;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->offset
    // Deserialize 8-byte NvU64 object.
    if (finn_write_buffer(bp, src->offset, 8 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->hMemory
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->hMemory, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->length
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->length, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->buffer), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->buffer)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->length); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->buffer))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->buffer)
            FINN_FREE(src->buffer);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv83deCtrlDebugReadMemoryParamsDeserialize(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 8-byte NvU64 object.
    dst->offset = (NvU64) finn_read_buffer(bp, 8 * 8);
    // No range check for dst->offset


    // Deserialize 4-byte NvU32 object.
    dst->hMemory = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->hMemory


    // Deserialize 4-byte NvU32 object.
    dst->length = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->length


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->buffer = FINN_MALLOC((dst->length));
            if (!dst->buffer)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->buffer, (dst->length));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->buffer)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->length); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->buffer))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->buffer = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsSerialize(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0xf;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->offset
    // Deserialize 8-byte NvU64 object.
    if (finn_write_buffer(bp, src->offset, 8 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->hMemory
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->hMemory, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->length
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->length, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->buffer), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->buffer)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->length); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->buffer))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->buffer)
            FINN_FREE(src->buffer);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nv83deCtrlDebugWriteMemoryParamsDeserialize(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0xf)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 8-byte NvU64 object.
    dst->offset = (NvU64) finn_read_buffer(bp, 8 * 8);
    // No range check for dst->offset


    // Deserialize 4-byte NvU32 object.
    dst->hMemory = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->hMemory


    // Deserialize 4-byte NvU32 object.
    dst->length = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->length


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->buffer = FINN_MALLOC((dst->length));
            if (!dst->buffer)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->buffer, (dst->length));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->buffer)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->length); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->buffer))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->buffer = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsSerialize(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->engineID
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->engineID, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->size
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->size, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // Unbounded fields
    // Set data-presence indicator.
    if (finn_write_buffer(bp, !!(src->pEngineCtxBuff), 1))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Skip if pointer is null.
    if (src->pEngineCtxBuff)
    {

        // Serialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (src->size); ++j)
        {
            if (finn_write_buffer(bp, ((NvU8 *) NvP64_VALUE(src->pEngineCtxBuff))[j], 1 * 8))
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                goto exit;
            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && src->pEngineCtxBuff)
            FINN_FREE(src->pEngineCtxBuff);
    }

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nvb06fCtrlGetEngineCtxDataParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->engineID = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->engineID


    // Deserialize 4-byte NvU32 object.
    dst->size = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->size


    // Unbounded fields
    // Skip if data is not present (null pointer).
    if (finn_read_buffer(bp, 1))
    {

        // Allocate memory and set pointer when deserializing down.
        // (Calling cods is expected to do so when deserializing up.)
        if (!deser_up)
        {
            dst->pEngineCtxBuff = FINN_MALLOC((dst->size));
            if (!dst->pEngineCtxBuff)
            {
                status = NV_ERR_NO_MEMORY;
                FINN_ERROR(NV_ERR_NO_MEMORY);
                goto exit;
            }

            FINN_MEMZERO(dst->pEngineCtxBuff, (dst->size));
        }

        // Otherwise the pointer must be provided.
        else if (!dst->pEngineCtxBuff)
        {
            status = NV_ERR_INVALID_POINTER;
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            goto exit;
        }

        // Deserialize each 1-byte NvU8 element.
        for (NvU64 j = 0; j < (dst->size); ++j)
        {
            ((NvU8 *) NvP64_VALUE(dst->pEngineCtxBuff))[j] = (NvU8) finn_read_buffer(bp, 1 * 8);
        }
    }
    else
    {
        if (!deser_up)
            dst->pEngineCtxBuff = NULL;
    }

exit:
    return status;
}


static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsSerialize(const NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Unbounded fields
    status = Nvb06fCtrlGetEngineCtxDataParamsSerialize(&src->params, bp, seri_up);
    if (status != NV_OK)
        goto exit;

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nvb06fCtrlCmdMigrateEngineCtxDataFinnParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA_FINN_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Unbounded fields
    status = Nvb06fCtrlGetEngineCtxDataParamsDeserialize(bp, &dst->params, dst_size, deser_up);
    if (status != NV_OK)
        goto exit;

exit:
    return status;
}


static NV_STATUS Nvb06fCtrlSaveEngineCtxDataParamsSerialize(const NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x7;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Statically sized fields
    // No range check for src->engineID
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->engineID, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    // No range check for src->size
    // Deserialize 4-byte NvU32 object.
    if (finn_write_buffer(bp, src->size, 4 * 8))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }


    for (NvU64 i = 0; i < (NVB06F_CTRL_ENGINE_CTX_BUFFER_SIZE_MAX); ++i)
    {
        // No range check for src->engineCtxBuff[i]
        // Deserialize 1-byte NvU8 object.
        if (finn_write_buffer(bp, src->engineCtxBuff[i], 1 * 8))
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
            goto exit;
        }
    }


    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nvb06fCtrlSaveEngineCtxDataParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x7)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Statically sized fields
    // Deserialize 4-byte NvU32 object.
    dst->engineID = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->engineID


    // Deserialize 4-byte NvU32 object.
    dst->size = (NvU32) finn_read_buffer(bp, 4 * 8);
    // No range check for dst->size


    for (NvU64 i = 0; i < (NVB06F_CTRL_ENGINE_CTX_BUFFER_SIZE_MAX); ++i)
    {
        // Deserialize 1-byte NvU8 object.
        dst->engineCtxBuff[i] = (NvU8) finn_read_buffer(bp, 1 * 8);
        // No range check for dst->engineCtxBuff[i]
    }


exit:
    return status;
}


static NV_STATUS Nvb06fCtrlCmdRestoreEngineCtxDataFinnParamsSerialize(const NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *src, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Local variables
    NV_STATUS status = NV_OK;
    uint64_t field_presence_mask;

    // Field bitmasks
    field_presence_mask = 0x1;

    // Write field-presence indicators.
    if (finn_write_buffer(bp, field_presence_mask, 64))
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Bounded nested fields
    status = Nvb06fCtrlSaveEngineCtxDataParamsSerialize(&src->params, bp, seri_up);
    if (status != NV_OK)
        goto exit;

    goto exit; // Suppress potential not-used warning
exit:
    return status;
}


static NV_STATUS Nvb06fCtrlCmdRestoreEngineCtxDataFinnParamsDeserialize(finn_bit_pump_for_read *bp, NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *dst, NvLength dst_size, NvBool deser_up)
{
    // Local variables
    NV_STATUS status = NV_OK;

    // Check that the destination struct fits within the destination buffer
    if (sizeof(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS) > dst_size)
    {
        status = NV_ERR_BUFFER_TOO_SMALL;
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        goto exit;
    }

    // Validate the field bitmasks, which must match the expected values for now.
    if (finn_read_buffer(bp, 64) != 0x1)
    {
        status = NV_ERR_LIB_RM_VERSION_MISMATCH;
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        goto exit;
    }

    // Bounded nested fields
    status = Nvb06fCtrlSaveEngineCtxDataParamsDeserialize(bp, &dst->params, dst_size, deser_up);
    if (status != NV_OK)
        goto exit;

exit:
    return status;
}


