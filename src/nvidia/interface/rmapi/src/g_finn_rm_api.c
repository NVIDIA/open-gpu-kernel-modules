#include "g_finn_rm_api.h"
#include "ctrl/ctrl0000/ctrl0000nvd.h"
#include "ctrl/ctrl0080/ctrl0080dma.h"
#include "ctrl/ctrl0080/ctrl0080fb.h"
#include "ctrl/ctrl0080/ctrl0080fifo.h"
#include "ctrl/ctrl0080/ctrl0080gpu.h"
#include "ctrl/ctrl0080/ctrl0080gr.h"
#include "ctrl/ctrl0080/ctrl0080host.h"
#include "ctrl/ctrl0080/ctrl0080msenc.h"
#include "ctrl/ctrl2080/ctrl2080bios.h"
#include "ctrl/ctrl2080/ctrl2080ce.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080i2c.h"
#include "ctrl/ctrl2080/ctrl2080nvd.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"
#include "ctrl/ctrl2080/ctrl2080rc.h"
#include "ctrl/ctrl2080/ctrl2080ucodefuzzer.h"
#include "ctrl/ctrl208f/ctrl208fgpu.h"
#include "ctrl/ctrl402c.h"
#include "ctrl/ctrl83de/ctrl83dedebug.h"
#include "ctrl/ctrlb06f.h"

#if defined(NVRM) /* Kernel Mode */
#include <stdint.h>
#include <stddef.h>
#include "nvport/nvport.h"
#elif defined(NV_LIBOS) /* LIBOS */
#include <nvstdint.h>
#include "nvport/nvport.h"
#elif defined(USE_CUSTOM_MALLOC) /* OpenGL */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
extern void *rmapi_import_malloc(size_t size);
extern void rmapi_import_free(void *ptr);
#else /* Default */
#include <stdint.h>
#include <stddef.h>
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

#elif defined(NV_LIBOS) /* LIBOS */
#define FINN_MALLOC(size) portMemAllocNonPaged(size)

#elif defined(USE_CUSTOM_MALLOC) /* OpenGL */
#define FINN_MALLOC(size) rmapi_import_malloc(size)

#else /* Default */
#define FINN_MALLOC(size) malloc(size)
#endif

// Free allocated memory.
#if defined(FINN_FREE) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_FREE(buf) portMemFree(buf)

#elif defined(NV_LIBOS) /* LIBOS */
#define FINN_FREE(buf) portMemFree(buf)

#elif defined(USE_CUSTOM_MALLOC) /* OpenGL */
#define FINN_FREE(buf) rmapi_import_free(buf)

#else /* Default */
#define FINN_FREE(buf) free(buf)
#endif

// Set memory region to all zeroes.
#if defined(FINN_MEMZERO) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_MEMZERO(buf, size) portMemSet(buf, 0, size)

#elif defined(NV_LIBOS) /* LIBOS */
#define FINN_MEMZERO(buf, size) portMemSet(buf, 0, size)

#elif defined(USE_CUSTOM_MALLOC) /* OpenGL */
#define FINN_MEMZERO(buf, size) memset(buf, 0, size)

#else /* Default */
#define FINN_MEMZERO(buf, size) memset(buf, 0, size)
#endif

// Copy nonoverlapping memory region.
#if defined(FINN_MEMCPY) /* Use override from Makefile */
#elif defined(NVRM) /* Kernel Mode */
#define FINN_MEMCPY(dst, src, size) portMemCopy(dst, size, src, size)

#elif defined(NV_LIBOS) /* LIBOS */
#define FINN_MEMCPY(dst, src, size) portMemCopy(dst, size, src, size)

#elif defined(USE_CUSTOM_MALLOC) /* OpenGL */
#define FINN_MEMCPY(dst, src, size) memcpy(dst, src, size)

#else /* Default */
#define FINN_MEMCPY(dst, src, size) memcpy(dst, src, size)
#endif

// Report an error.
#if defined(FINN_ERROR) /* Use override from Makefile */
#else /* Default */
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
    // Accumulator does not have enough to satisfy the request.
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
        // Furthermore, if `bit_size == 64`, then `bp->remaining_bit_count` must
        // have been zero when subtracted above.  That's how we know that there is
        // no boundary crossing.
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
static inline void finn_open_buffer_for_write(finn_bit_pump_for_write *bp, uint64_t *sod, const uint64_t *eob)
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



static NV_STATUS finnSerializeRoot_FINN_RM_API(NvU64 interface, NvU64 message, const char *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRoot_FINN_RM_API(NvU64 interface, NvU64 message, finn_bit_pump_for_read *bp, char *api, NvLength dst_size, NvBool deser_up);
#if (defined(NVRM))
NvBool finnBadEnum_NV402C_CTRL_I2C_TRANSACTION_TYPE(NV402C_CTRL_I2C_TRANSACTION_TYPE value);
#endif // (defined(NVRM))

static NV_STATUS finnSerializeInterface_FINN_GT200_DEBUGGER_DEBUG(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_GT200_DEBUGGER_DEBUG(NvU64 message, finn_bit_pump_for_read *bp, FINN_GT200_DEBUGGER_DEBUG *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_GT200_DEBUGGER_DEBUG(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(NvU64 message, finn_bit_pump_for_read *bp, FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_DMA(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_DMA(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_DMA *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_DMA(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_FB(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_FB(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FB *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_FB(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_FIFO(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_FIFO(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FIFO *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_FIFO(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_GPU(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_GPU(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GPU *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_GPU(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_GR(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_GR(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GR *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_GR(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_HOST(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_HOST(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_HOST *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_HOST(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_MSENC(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_MSENC(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_MSENC *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_MSENC(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV01_ROOT_NVD(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV01_ROOT_NVD(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_ROOT_NVD *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_ROOT_NVD(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_BIOS(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_BIOS(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_BIOS *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_BIOS(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_CE(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_CE(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_CE *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_CE(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_GPU(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_GPU(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_GPU *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_GPU(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_I2C(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_I2C(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_I2C *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_I2C(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_NVD(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_NVD(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_NVD *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_NVD(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_PERF(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_PERF(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_PERF *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_PERF(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_RC(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_RC(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_RC *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_RC(NvU64 message);

static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_DIAG_GPU(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_DIAG_GPU(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_DIAG_GPU *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_DIAG_GPU(NvU64 message);
static NV_STATUS finnSerializeInterface_FINN_NV40_I2C_I2C(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeInterface_FINN_NV40_I2C_I2C(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV40_I2C_I2C *api_intf, NvLength api_size, NvBool deser_up);
static NvU64 finnUnserializedInterfaceSize_FINN_NV40_I2C_I2C(NvU64 message);
#if (defined(NVRM))
static NV_STATUS finnSerializeMessage_NV0000_CTRL_NVD_GET_DUMP_PARAMS(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0000_CTRL_NVD_GET_DUMP_PARAMS(finn_bit_pump_for_read *bp, NV0000_CTRL_NVD_GET_DUMP_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV0080_CTRL_FB_GET_CAPS_PARAMS(const NV0080_CTRL_FB_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FB_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FB_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV0080_CTRL_FIFO_GET_CAPS_PARAMS(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FIFO_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *api, NvLength api_size, NvBool deser_up);
#endif // (defined(NVRM))

static NV_STATUS finnSerializeMessage_NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *api, NvLength api_size, NvBool deser_up);
#if (defined(NVRM))
static NV_STATUS finnSerializeMessage_NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV0080_CTRL_GR_GET_CAPS_PARAMS(const NV0080_CTRL_GR_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_GR_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_GR_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV0080_CTRL_HOST_GET_CAPS_PARAMS(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_HOST_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_HOST_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV0080_CTRL_MSENC_GET_CAPS_PARAMS(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_MSENC_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_MSENC_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up);
#endif // (defined(NVRM))

#if (defined(NVRM))
static NV_STATUS finnSerializeMessage_NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS(const NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV2080_CTRL_CE_GET_CAPS_PARAMS(const NV2080_CTRL_CE_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_CE_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_CE_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV2080_CTRL_GPU_GET_ENGINES_PARAMS(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_GPU_GET_ENGINES_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV2080_CTRL_I2C_ACCESS_PARAMS(const NV2080_CTRL_I2C_ACCESS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_I2C_ACCESS_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_I2C_ACCESS_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV2080_CTRL_NVD_GET_DUMP_PARAMS(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_NVD_GET_DUMP_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_NVD_GET_DUMP_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *api, NvLength api_size, NvBool deser_up);
#endif // (defined(NVRM))

#if (defined(NVRM))
static NV_STATUS finnSerializeMessage_NV402C_CTRL_I2C_INDEXED_PARAMS(const NV402C_CTRL_I2C_INDEXED_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV402C_CTRL_I2C_INDEXED_PARAMS(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_INDEXED_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV402C_CTRL_I2C_TRANSACTION_PARAMS(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV402C_CTRL_I2C_TRANSACTION_PARAMS(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS(const NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS(finn_bit_pump_for_read *bp, NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS(finn_bit_pump_for_read *bp, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS(const NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS(finn_bit_pump_for_read *bp, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *api, NvLength api_size, NvBool deser_up);
#endif // (defined(NVRM))

static NV_STATUS finnSerializeRecord_NV0080_CTRL_FIFO_CHANNEL(const NV0080_CTRL_FIFO_CHANNEL *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV0080_CTRL_FIFO_CHANNEL(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_CHANNEL *api, NvLength api_size, NvBool deser_up);
#if (defined(NVRM))
static NV_STATUS finnSerializeRecord_NV2080_CTRL_GPUMON_SAMPLES(const NV2080_CTRL_GPUMON_SAMPLES *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV2080_CTRL_GPUMON_SAMPLES(finn_bit_pump_for_read *bp, NV2080_CTRL_GPUMON_SAMPLES *api, NvLength api_size, NvBool deser_up);
#endif // (defined(NVRM))

#if (defined(NVRM))
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *api, NvLength api_size, NvBool deser_up);
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up);
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *api, NvLength api_size, NvBool deser_up);
#endif // (defined(NVRM))

#if (defined(NVRM))
static NV_STATUS finnSerializeUnion_NV402C_CTRL_I2C_TRANSACTION_DATA(const NV402C_CTRL_I2C_TRANSACTION_DATA *api, finn_bit_pump_for_write *bp, NvBool seri_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType);
static NV_STATUS finnDeserializeUnion_NV402C_CTRL_I2C_TRANSACTION_DATA(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA *api, NvLength dst_size, NvBool deser_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType);

#endif // (defined(NVRM))

// Serialize this API.
NV_STATUS finnSerializeInternal_FINN_RM_API(NvU64 interface, NvU64 message, const char *api, char *dst, NvLength dst_size, NvBool seri_up)
{
    // Header
    FINN_RM_API *header;

    // Buffer end
    // `char` is the C-standrd unit of measure for `sizeof`.
    const char *dst_end;

    // Bit pump is used to fill the buffer with serialized data.
    finn_bit_pump_for_write bp;

    // Error code returned from serialization
    NV_STATUS error_code;

    // Input validation
    // Null pointers are not permitted.
    // Buffer must begin on an 8-byte boundary.
    if (!api || !dst || !dst_size || (uintptr_t) dst & 0x7u)
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Header is at the start of the buffer.
    header = (FINN_RM_API *) dst;

    // Buffer must end on an 8-byte boundary, so round down.
    dst_end = (const char *) ((uintptr_t) (dst + dst_size) & ~ (uintptr_t) 0x7);

    // Set header data.
    header->version = FINN_SERIALIZATION_VERSION;
    header->payloadSize = 0;        // Zero until completed successfully
    header->interface = interface;
    header->message = message;

    // Advance past header.
    dst += sizeof(FINN_RM_API);

    // Open the bit pump.
    finn_open_buffer_for_write(&bp, (uint64_t *) dst, (const uint64_t *) dst_end);

    // Call the serializer.
    error_code = finnSerializeRoot_FINN_RM_API(interface, message, api, &bp, seri_up);

    // Close the bit pump.
    finn_close_buffer_for_write(&bp);

    // Payload size in bytes
    if (error_code == NV_OK)
        header->payloadSize = (NvU64) (((const char *) bp.buffer_position) - ((const char *) header));

    // Done
    return error_code;
}

// Serialize this API to the bit pump.
static NV_STATUS finnSerializeRoot_FINN_RM_API(NvU64 interface, NvU64 message, const char *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize the specified interface.
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return finnSerializeInterface_FINN_NV01_ROOT_NVD(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return finnSerializeInterface_FINN_NV01_DEVICE_0_DMA(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return finnSerializeInterface_FINN_NV01_DEVICE_0_FB(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return finnSerializeInterface_FINN_NV01_DEVICE_0_FIFO(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return finnSerializeInterface_FINN_NV01_DEVICE_0_GPU(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return finnSerializeInterface_FINN_NV01_DEVICE_0_GR(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return finnSerializeInterface_FINN_NV01_DEVICE_0_HOST(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return finnSerializeInterface_FINN_NV01_DEVICE_0_MSENC(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_BIOS):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_0_BIOS(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_0_CE(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_0_GPU(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_0_I2C(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_0_NVD(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_0_PERF(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_0_RC(message, api, bp, seri_up);

        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_DIAG_GPU):
            return finnSerializeInterface_FINN_NV20_SUBDEVICE_DIAG_GPU(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return finnSerializeInterface_FINN_NV40_I2C_I2C(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return finnSerializeInterface_FINN_GT200_DEBUGGER_DEBUG(message, api, bp, seri_up);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return finnSerializeInterface_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(message, api, bp, seri_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
} // end finnSerializeRoot_FINN_RM_API


// Deserialize this API.
NV_STATUS finnDeserializeInternal_FINN_RM_API(const char *src, NvLength src_size, char *api, NvLength api_size, NvBool deser_up)
{
    // Header
    const FINN_RM_API *header;

    // End of data
    const char *src_max;

    // Bit pump is used to read the serialized data.
    finn_bit_pump_for_read bp;

    // Error code
    NV_STATUS status;

    // Input validation
    // Null pointers are not permitted.
    // Buffer must begin on an 8-byte boundary.
    if (!src || !src_size || !api || !api_size || (uintptr_t) src & 0x7u)
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Header data comes first.
    header = (const FINN_RM_API *) src;

    // Check the version.
    if (header->version != FINN_SERIALIZATION_VERSION)
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Set src_max for buffer bounds checking.
    src_max = src + src_size;

    // Check that source buffer is large enough.
    if (sizeof(FINN_RM_API) > src_size ||
        header->payloadSize > src_size ||
        header->payloadSize < sizeof(FINN_RM_API))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Open the bit pump, skipping past the header.
    finn_open_buffer_for_read(&bp, (const uint64_t *) (src + sizeof(FINN_RM_API)), (const uint64_t *) (src_max));

    // Dispatch to interface-specific routine
    status = finnDeserializeRoot_FINN_RM_API(header->interface, header->message, &bp, api, api_size, deser_up);
    // Nothing more to do if there was an error.
    if (status != NV_OK)
        return status;

    // Check that the declared size matches the serialization outcome.
    if (header->payloadSize != (NvU64) (((const char *) bp.buffer_position) - ((const char *) header)))
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // All good
    return NV_OK;
}


// Deserialize this API from the bit pump.
static NV_STATUS finnDeserializeRoot_FINN_RM_API(NvU64 interface, NvU64 message, finn_bit_pump_for_read *bp, char *api, NvLength api_size, NvBool deser_up)
{
    // Deserialize the specified interface.
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return finnDeserializeInterface_FINN_NV01_ROOT_NVD(message, bp, (FINN_NV01_ROOT_NVD *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return finnDeserializeInterface_FINN_NV01_DEVICE_0_DMA(message, bp, (FINN_NV01_DEVICE_0_DMA *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return finnDeserializeInterface_FINN_NV01_DEVICE_0_FB(message, bp, (FINN_NV01_DEVICE_0_FB *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return finnDeserializeInterface_FINN_NV01_DEVICE_0_FIFO(message, bp, (FINN_NV01_DEVICE_0_FIFO *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return finnDeserializeInterface_FINN_NV01_DEVICE_0_GPU(message, bp, (FINN_NV01_DEVICE_0_GPU *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return finnDeserializeInterface_FINN_NV01_DEVICE_0_GR(message, bp, (FINN_NV01_DEVICE_0_GR *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return finnDeserializeInterface_FINN_NV01_DEVICE_0_HOST(message, bp, (FINN_NV01_DEVICE_0_HOST *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return finnDeserializeInterface_FINN_NV01_DEVICE_0_MSENC(message, bp, (FINN_NV01_DEVICE_0_MSENC *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_BIOS):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_BIOS(message, bp, (FINN_NV20_SUBDEVICE_0_BIOS *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_CE(message, bp, (FINN_NV20_SUBDEVICE_0_CE *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_GPU(message, bp, (FINN_NV20_SUBDEVICE_0_GPU *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_I2C(message, bp, (FINN_NV20_SUBDEVICE_0_I2C *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_NVD(message, bp, (FINN_NV20_SUBDEVICE_0_NVD *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_PERF(message, bp, (FINN_NV20_SUBDEVICE_0_PERF *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_RC(message, bp, (FINN_NV20_SUBDEVICE_0_RC *) api, api_size, deser_up);

        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_DIAG_GPU):
            return finnDeserializeInterface_FINN_NV20_SUBDEVICE_DIAG_GPU(message, bp, (FINN_NV20_SUBDEVICE_DIAG_GPU *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return finnDeserializeInterface_FINN_NV40_I2C_I2C(message, bp, (FINN_NV40_I2C_I2C *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return finnDeserializeInterface_FINN_GT200_DEBUGGER_DEBUG(message, bp, (FINN_GT200_DEBUGGER_DEBUG *) api, api_size, deser_up);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return finnDeserializeInterface_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(message, bp, (FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *) api, api_size, deser_up);

        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
} // end finnDeserializeRoot_FINN_RM_API


// Size of the serialized format for this interface/message
NvU64 FinnRmApiGetSerializedSize(NvU64 interface, NvU64 message, const NvP64 src)
{
    // Bit pump with writing disabled.
    finn_bit_pump_for_write bp;
    finn_open_buffer_for_write(&bp, (uint64_t *) 0, (const uint64_t *) 0);

    // Call the serializer with write-suppressed bit pump.
    // The size is the same in bith directions (up/down).
    // Eeturn zero on error to indicate that this API is not serialized by FINN.
    if (finnSerializeRoot_FINN_RM_API(interface, message, (const char *) NvP64_VALUE(src), &bp, 0) != NV_OK)
        return 0;

    // Close the bit pump.
    finn_close_buffer_for_write(&bp);

    // Add the header size in bytes to the amount of data serialzied.
    // `buffer_position` is the payload size (not really the buffer position).
    return (NvU64) NV_PTR_TO_NvP64(bp.buffer_position) + sizeof(FINN_RM_API);
}


// Size of the unserialized format for this interface/message
NvU64 FinnRmApiGetUnserializedSize(NvU64 interface, NvU64 message)
{
    // Forward to message-specific routine.
    switch (interface)
    {
        case FINN_INTERFACE_ID(FINN_NV01_ROOT_NVD):
            return finnUnserializedInterfaceSize_FINN_NV01_ROOT_NVD(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_DMA):
            return finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_DMA(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FB):
            return finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_FB(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_FIFO):
            return finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_FIFO(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GPU):
            return finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_GPU(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_GR):
            return finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_GR(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_HOST):
            return finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_HOST(message);
        case FINN_INTERFACE_ID(FINN_NV01_DEVICE_0_MSENC):
            return finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_MSENC(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_BIOS):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_BIOS(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_CE):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_CE(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_GPU):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_GPU(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_I2C):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_I2C(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_NVD):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_NVD(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_PERF):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_PERF(message);
        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_0_RC):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_RC(message);

        case FINN_INTERFACE_ID(FINN_NV20_SUBDEVICE_DIAG_GPU):
            return finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_DIAG_GPU(message);
        case FINN_INTERFACE_ID(FINN_NV40_I2C_I2C):
            return finnUnserializedInterfaceSize_FINN_NV40_I2C_I2C(message);
        case FINN_INTERFACE_ID(FINN_GT200_DEBUGGER_DEBUG):
            return finnUnserializedInterfaceSize_FINN_GT200_DEBUGGER_DEBUG(message);
        case FINN_INTERFACE_ID(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO):
            return finnUnserializedInterfaceSize_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(message);

        // Zero indicates an unsupported interface (or message).
        default:
            return 0;
    }
} // end FINN_RM_APIGetUnserializedSize
#if (defined(NVRM))
// Validate the enum value.
NvBool finnBadEnum_NV402C_CTRL_I2C_TRANSACTION_TYPE(NV402C_CTRL_I2C_TRANSACTION_TYPE value)
{   switch(value)
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
            return NV_FALSE;

        default:
            return NV_TRUE;
    }
}

#endif // (defined(NVRM))

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_GT200_DEBUGGER_DEBUG(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 2 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return finnSerializeMessage_NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS((const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *) api_intf, bp, seri_up);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return finnSerializeMessage_NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS((const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_GT200_DEBUGGER_DEBUG(NvU64 message, finn_bit_pump_for_read *bp, FINN_GT200_DEBUGGER_DEBUG *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 2 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return finnDeserializeMessage_NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS(bp, (NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *) api_intf, api_size, deser_up);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return finnDeserializeMessage_NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS(bp, (NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_GT200_DEBUGGER_DEBUG(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS):
            return sizeof(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS);
        case FINN_MESSAGE_ID(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS):
            return sizeof(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 3 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return finnSerializeMessage_NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS((const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *) api_intf, bp, seri_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS):
            return finnSerializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS((const NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *) api_intf, bp, seri_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS):
            return finnSerializeMessage_NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS((const NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(NvU64 message, finn_bit_pump_for_read *bp, FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 3 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return finnDeserializeMessage_NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS(bp, (NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *) api_intf, api_size, deser_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS):
            return finnDeserializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS(bp, (NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *) api_intf, api_size, deser_up);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS):
            return finnDeserializeMessage_NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS(bp, (NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS):
            return sizeof(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS);
        case FINN_MESSAGE_ID(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS):
            return sizeof(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS);
        case FINN_MESSAGE_ID(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS):
            return sizeof(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_DMA(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS((const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_DMA(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_DMA *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS(bp, (NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_DMA(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS):
            return sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_FB(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_FB_GET_CAPS_PARAMS((const NV0080_CTRL_FB_GET_CAPS_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_FB(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FB *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_FB_GET_CAPS_PARAMS(bp, (NV0080_CTRL_FB_GET_CAPS_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_FB(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FB_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_FB_GET_CAPS_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_FIFO(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 3 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_FIFO_GET_CAPS_PARAMS((const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))

        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS((const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *) api_intf, bp, seri_up);
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS((const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_FIFO(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_FIFO *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 3 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_FIFO_GET_CAPS_PARAMS(bp, (NV0080_CTRL_FIFO_GET_CAPS_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))

        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS(bp, (NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *) api_intf, api_size, deser_up);
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS(bp, (NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_FIFO(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_FIFO_GET_CAPS_PARAMS);
#endif // (defined(NVRM))

        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS):
            return sizeof(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS);
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS):
            return sizeof(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_GPU(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS((const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_GPU(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GPU *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS(bp, (NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_GPU(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS):
            return sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_GR(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_GR_GET_CAPS_PARAMS((const NV0080_CTRL_GR_GET_CAPS_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_GR(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_GR *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_GR_GET_CAPS_PARAMS(bp, (NV0080_CTRL_GR_GET_CAPS_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_GR(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_GR_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_GR_GET_CAPS_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_HOST(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_HOST_GET_CAPS_PARAMS((const NV0080_CTRL_HOST_GET_CAPS_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_HOST(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_HOST *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_HOST_GET_CAPS_PARAMS(bp, (NV0080_CTRL_HOST_GET_CAPS_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_HOST(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_HOST_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_HOST_GET_CAPS_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_DEVICE_0_MSENC(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return finnSerializeMessage_NV0080_CTRL_MSENC_GET_CAPS_PARAMS((const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_DEVICE_0_MSENC(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_DEVICE_0_MSENC *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return finnDeserializeMessage_NV0080_CTRL_MSENC_GET_CAPS_PARAMS(bp, (NV0080_CTRL_MSENC_GET_CAPS_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_DEVICE_0_MSENC(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0080_CTRL_MSENC_GET_CAPS_PARAMS):
            return sizeof(NV0080_CTRL_MSENC_GET_CAPS_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV01_ROOT_NVD(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return finnSerializeMessage_NV0000_CTRL_NVD_GET_DUMP_PARAMS((const NV0000_CTRL_NVD_GET_DUMP_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV01_ROOT_NVD(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV01_ROOT_NVD *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return finnDeserializeMessage_NV0000_CTRL_NVD_GET_DUMP_PARAMS(bp, (NV0000_CTRL_NVD_GET_DUMP_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV01_ROOT_NVD(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV0000_CTRL_NVD_GET_DUMP_PARAMS):
            return sizeof(NV0000_CTRL_NVD_GET_DUMP_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_BIOS(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 2 messages in this interface.
    switch (message)
    {

#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS):
            return finnSerializeMessage_NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS((const NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_BIOS(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_BIOS *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 2 messages in this interface.
    switch (message)
    {

#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS):
            return finnDeserializeMessage_NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS(bp, (NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_BIOS(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {

#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS):
            return sizeof(NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_CE(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return finnSerializeMessage_NV2080_CTRL_CE_GET_CAPS_PARAMS((const NV2080_CTRL_CE_GET_CAPS_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_CE(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_CE *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return finnDeserializeMessage_NV2080_CTRL_CE_GET_CAPS_PARAMS(bp, (NV2080_CTRL_CE_GET_CAPS_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_CE(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_CE_GET_CAPS_PARAMS):
            return sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_GPU(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 2 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return finnSerializeMessage_NV2080_CTRL_GPU_GET_ENGINES_PARAMS((const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *) api_intf, bp, seri_up);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return finnSerializeMessage_NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS((const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_GPU(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_GPU *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 2 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return finnDeserializeMessage_NV2080_CTRL_GPU_GET_ENGINES_PARAMS(bp, (NV2080_CTRL_GPU_GET_ENGINES_PARAMS *) api_intf, api_size, deser_up);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return finnDeserializeMessage_NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS(bp, (NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_GPU(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINES_PARAMS):
            return sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS);
        case FINN_MESSAGE_ID(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS):
            return sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_I2C(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return finnSerializeMessage_NV2080_CTRL_I2C_ACCESS_PARAMS((const NV2080_CTRL_I2C_ACCESS_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_I2C(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_I2C *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return finnDeserializeMessage_NV2080_CTRL_I2C_ACCESS_PARAMS(bp, (NV2080_CTRL_I2C_ACCESS_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_I2C(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_I2C_ACCESS_PARAMS):
            return sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_NVD(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return finnSerializeMessage_NV2080_CTRL_NVD_GET_DUMP_PARAMS((const NV2080_CTRL_NVD_GET_DUMP_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_NVD(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_NVD *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return finnDeserializeMessage_NV2080_CTRL_NVD_GET_DUMP_PARAMS(bp, (NV2080_CTRL_NVD_GET_DUMP_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_NVD(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_NVD_GET_DUMP_PARAMS):
            return sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_PERF(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):    /* alias */
            return finnSerializeRecord_NV2080_CTRL_GPUMON_SAMPLES((const NV2080_CTRL_GPUMON_SAMPLES *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_PERF(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_PERF *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):    /* alias */
            return finnDeserializeRecord_NV2080_CTRL_GPUMON_SAMPLES(bp, (NV2080_CTRL_GPUMON_SAMPLES *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_PERF(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM):    /* alias */
            return sizeof(NV2080_CTRL_GPUMON_SAMPLES);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_0_RC(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return finnSerializeMessage_NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS((const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_0_RC(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_0_RC *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 1 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return finnDeserializeMessage_NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS(bp, (NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_0_RC(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS):
            return sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV20_SUBDEVICE_DIAG_GPU(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 2 messages in this interface.
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


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV20_SUBDEVICE_DIAG_GPU(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV20_SUBDEVICE_DIAG_GPU *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 2 messages in this interface.
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


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV20_SUBDEVICE_DIAG_GPU(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

// Serialize this interface.
static NV_STATUS finnSerializeInterface_FINN_NV40_I2C_I2C(NvU64 message, const char *api_intf, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize one of 2 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return finnSerializeMessage_NV402C_CTRL_I2C_INDEXED_PARAMS((const NV402C_CTRL_I2C_INDEXED_PARAMS *) api_intf, bp, seri_up);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return finnSerializeMessage_NV402C_CTRL_I2C_TRANSACTION_PARAMS((const NV402C_CTRL_I2C_TRANSACTION_PARAMS *) api_intf, bp, seri_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Deserialize this interface.
static NV_STATUS finnDeserializeInterface_FINN_NV40_I2C_I2C(NvU64 message, finn_bit_pump_for_read *bp, FINN_NV40_I2C_I2C *api_intf, NvLength api_size, NvBool deser_up)
{
    // Deerialize one of 2 messages in this interface.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return finnDeserializeMessage_NV402C_CTRL_I2C_INDEXED_PARAMS(bp, (NV402C_CTRL_I2C_INDEXED_PARAMS *) api_intf, api_size, deser_up);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return finnDeserializeMessage_NV402C_CTRL_I2C_TRANSACTION_PARAMS(bp, (NV402C_CTRL_I2C_TRANSACTION_PARAMS *) api_intf, api_size, deser_up);
#endif // (defined(NVRM))


        // Everything else is unsupported.
        default:
        {
            FINN_ERROR(NV_ERR_NOT_SUPPORTED);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
}


// Size of the unserialized format for this interface/message
static NvU64 finnUnserializedInterfaceSize_FINN_NV40_I2C_I2C(NvU64 message)
{
    // Forward to message-specific routine.
    switch (message)
    {
#if (defined(NVRM))
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_INDEXED_PARAMS):
            return sizeof(NV402C_CTRL_I2C_INDEXED_PARAMS);
        case FINN_MESSAGE_ID(NV402C_CTRL_I2C_TRANSACTION_PARAMS):
            return sizeof(NV402C_CTRL_I2C_TRANSACTION_PARAMS);
#endif // (defined(NVRM))


        // Zero indicates an unsupported message (or interface).
        default:
            return 0;
    }
}

#if (defined(NVRM))

// Serialize each of the 3 field(s).
// 2 out of 2 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnSerializeMessage_NV0000_CTRL_NVD_GET_DUMP_PARAMS(const NV0000_CTRL_NVD_GET_DUMP_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `component`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->component, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `size`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->size > NV0000_CTRL_NVD_MAX_DUMP_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->size, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pBuffer), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pBuffer)
    {
        // Serialize each element in `pBuffer`.
        {
            NvLength i;
            for (i = 0; i < (api->size); ++i)
            {
                // Serialize field-presence indicator for `pBuffer[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pBuffer)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pBuffer)
            FINN_FREE(NvP64_VALUE(api->pBuffer));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
// 2 out of 2 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnDeserializeMessage_NV0000_CTRL_NVD_GET_DUMP_PARAMS(finn_bit_pump_for_read *bp, NV0000_CTRL_NVD_GET_DUMP_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0000_CTRL_NVD_GET_DUMP_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `component`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->component = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `size`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->size = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->size > NV0000_CTRL_NVD_MAX_DUMP_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check data-presence (nonnull pointer) indicator for `pBuffer`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->size) * (sizeof(NvU8) /*pBuffer[i]*/) /*pBuffer*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pBuffer = NV_PTR_TO_NvP64(FINN_MALLOC((api->size) * (sizeof(NvU8) /*pBuffer[i]*/) /*pBuffer*/));
            if (!api->pBuffer)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pBuffer), (api->size) * (sizeof(NvU8) /*pBuffer[i]*/) /*pBuffer*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pBuffer)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pBuffer`.
        {
            NvLength i;
            for (i = 0; i < (api->size); ++i)
            {
                // Check field-presence indicator for `pBuffer[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pBuffer)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pBuffer = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 6 field(s).
// 2 out of 4 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS(const NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `pdeIndex`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->pdeIndex, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `flags`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->flags, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `hVASpace`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->hVASpace, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `subDeviceId`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->subDeviceId, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize each element in `ptParams`.
    {
        NvLength i;
        for (i = 0; i < NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE; ++i)
        {
            // Serialize field-presence indicator for `ptParams[i]`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 3 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS((api->ptParams+(i)), bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

        }
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pPdeBuffer), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pPdeBuffer)
    {
        // Serialize each element in `pPdeBuffer`.
        {
            NvLength i;
            for (i = 0; i < 1; ++i)
            {
                // Serialize field-presence indicator for `pPdeBuffer[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 64-bit NvU64 primitive.
                if (finn_write_buffer(bp, ((*(NvU64 (*) [1])(NvP64_VALUE(api->pPdeBuffer))))[i], 64))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pPdeBuffer)
            FINN_FREE(NvP64_VALUE(api->pPdeBuffer));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 6 field(s).
// 2 out of 4 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `pdeIndex`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->pdeIndex = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `flags`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->flags = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `hVASpace`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->hVASpace = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `subDeviceId`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->subDeviceId = (NvU32) finn_read_buffer(bp, 32);

    // Deserialize each element in `ptParams`.
    {
        NvLength i;
        for (i = 0; i < NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE; ++i)
        {
            // Check field-presence indicator for `ptParams[i]`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 3 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS(bp, (api->ptParams+(i)), sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS), deser_up);
                if (status != NV_OK)
                    return status;
            }

        }
    }

    // Check data-presence (nonnull pointer) indicator for `pPdeBuffer`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if (1 * (sizeof(NvU64) /*pPdeBuffer[i]*/) /*pPdeBuffer*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pPdeBuffer = NV_PTR_TO_NvP64(FINN_MALLOC(1 * (sizeof(NvU64) /*pPdeBuffer[i]*/) /*pPdeBuffer*/));
            if (!api->pPdeBuffer)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pPdeBuffer), 1 * (sizeof(NvU64) /*pPdeBuffer[i]*/) /*pPdeBuffer*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pPdeBuffer)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pPdeBuffer`.
        {
            NvLength i;
            for (i = 0; i < 1; ++i)
            {
                // Check field-presence indicator for `pPdeBuffer[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 64-bit NvU64 primitive.
                ((*(NvU64 (*) [1])(NvP64_VALUE(api->pPdeBuffer))))[i] = (NvU64) finn_read_buffer(bp, 64);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pPdeBuffer = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_FB_GET_CAPS_PARAMS(const NV0080_CTRL_FB_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `capsTblSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_FB_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->capsTblSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->capsTbl), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->capsTbl)
    {
        // Serialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Serialize field-presence indicator for `capsTbl[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->capsTbl)
            FINN_FREE(NvP64_VALUE(api->capsTbl));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FB_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FB_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_FB_GET_CAPS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `capsTblSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->capsTblSize = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_FB_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check data-presence (nonnull pointer) indicator for `capsTbl`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->capsTbl = NV_PTR_TO_NvP64(FINN_MALLOC((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/));
            if (!api->capsTbl)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->capsTbl), (api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->capsTbl)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Check field-presence indicator for `capsTbl[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->capsTbl = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_FIFO_GET_CAPS_PARAMS(const NV0080_CTRL_FIFO_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `capsTblSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_FIFO_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->capsTblSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->capsTbl), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->capsTbl)
    {
        // Serialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Serialize field-presence indicator for `capsTbl[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->capsTbl)
            FINN_FREE(NvP64_VALUE(api->capsTbl));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FIFO_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_FIFO_GET_CAPS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `capsTblSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->capsTblSize = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_FIFO_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check data-presence (nonnull pointer) indicator for `capsTbl`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->capsTbl = NV_PTR_TO_NvP64(FINN_MALLOC((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/));
            if (!api->capsTbl)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->capsTbl), (api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->capsTbl)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Check field-presence indicator for `capsTbl[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->capsTbl = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS(const NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `numChannels`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->numChannels, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pChannelHandleList), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pChannelHandleList)
    {
        // Serialize each element in `pChannelHandleList`.
        {
            NvLength i;
            for (i = 0; i < (api->numChannels); ++i)
            {
                // Serialize field-presence indicator for `pChannelHandleList[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 32-bit NvU32 primitive.
                if (finn_write_buffer(bp, ((NvU32 *)(NvP64_VALUE(api->pChannelHandleList)))[i], 32))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pChannelHandleList)
            FINN_FREE(NvP64_VALUE(api->pChannelHandleList));
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pChannelList), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pChannelList)
    {
        // Serialize each element in `pChannelList`.
        {
            NvLength i;
            for (i = 0; i < (api->numChannels); ++i)
            {
                // Serialize field-presence indicator for `pChannelList[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 32-bit NvU32 primitive.
                if (finn_write_buffer(bp, ((NvU32 *)(NvP64_VALUE(api->pChannelList)))[i], 32))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pChannelList)
            FINN_FREE(NvP64_VALUE(api->pChannelList));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `numChannels`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->numChannels = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pChannelHandleList`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->numChannels) * (sizeof(NvU32) /*pChannelHandleList[i]*/) /*pChannelHandleList*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pChannelHandleList = NV_PTR_TO_NvP64(FINN_MALLOC((api->numChannels) * (sizeof(NvU32) /*pChannelHandleList[i]*/) /*pChannelHandleList*/));
            if (!api->pChannelHandleList)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pChannelHandleList), (api->numChannels) * (sizeof(NvU32) /*pChannelHandleList[i]*/) /*pChannelHandleList*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pChannelHandleList)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pChannelHandleList`.
        {
            NvLength i;
            for (i = 0; i < (api->numChannels); ++i)
            {
                // Check field-presence indicator for `pChannelHandleList[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 32-bit NvU32 primitive.
                ((NvU32 *)(NvP64_VALUE(api->pChannelHandleList)))[i] = (NvU32) finn_read_buffer(bp, 32);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pChannelHandleList = NV_PTR_TO_NvP64(NULL);
    }

    // Check data-presence (nonnull pointer) indicator for `pChannelList`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->numChannels) * (sizeof(NvU32) /*pChannelList[i]*/) /*pChannelList*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pChannelList = NV_PTR_TO_NvP64(FINN_MALLOC((api->numChannels) * (sizeof(NvU32) /*pChannelList[i]*/) /*pChannelList*/));
            if (!api->pChannelList)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pChannelList), (api->numChannels) * (sizeof(NvU32) /*pChannelList[i]*/) /*pChannelList*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pChannelList)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pChannelList`.
        {
            NvLength i;
            for (i = 0; i < (api->numChannels); ++i)
            {
                // Check field-presence indicator for `pChannelList[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 32-bit NvU32 primitive.
                ((NvU32 *)(NvP64_VALUE(api->pChannelList)))[i] = (NvU32) finn_read_buffer(bp, 32);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pChannelList = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}

#endif // (defined(NVRM))


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS(const NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `fifoStartChannelListCount`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->fifoStartChannelListCount, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize each element in `channelHandle`.
    {
        NvLength i;
        for (i = 0; i < 8; ++i)
        {
            // Serialize field-presence indicator for `channelHandle[i]`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Serialize 32-bit NvU32 primitive.
            if (finn_write_buffer(bp, (api->channelHandle)[i], 32))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

        }
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->fifoStartChannelList), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->fifoStartChannelList)
    {
        // Serialize each element in `fifoStartChannelList`.
        {
            NvLength i;
            for (i = 0; i < (api->fifoStartChannelListCount); ++i)
            {
                // Serialize field-presence indicator for `fifoStartChannelList[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Record has 1 field(s) to be serialized.
                {
                    NV_STATUS status = finnSerializeRecord_NV0080_CTRL_FIFO_CHANNEL(((NV0080_CTRL_FIFO_CHANNEL *)(NvP64_VALUE(api->fifoStartChannelList))+(i)), bp, seri_up);
                    if (status != NV_OK)
                        return status;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->fifoStartChannelList)
            FINN_FREE(NvP64_VALUE(api->fifoStartChannelList));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `fifoStartChannelListCount`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->fifoStartChannelListCount = (NvU32) finn_read_buffer(bp, 32);

    // Deserialize each element in `channelHandle`.
    {
        NvLength i;
        for (i = 0; i < 8; ++i)
        {
            // Check field-presence indicator for `channelHandle[i]`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Deserialize 32-bit NvU32 primitive.
            (api->channelHandle)[i] = (NvU32) finn_read_buffer(bp, 32);

        }
    }

    // Check data-presence (nonnull pointer) indicator for `fifoStartChannelList`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->fifoStartChannelListCount) * (sizeof(NV0080_CTRL_FIFO_CHANNEL) /*fifoStartChannelList[i]*/) /*fifoStartChannelList*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->fifoStartChannelList = NV_PTR_TO_NvP64(FINN_MALLOC((api->fifoStartChannelListCount) * (sizeof(NV0080_CTRL_FIFO_CHANNEL) /*fifoStartChannelList[i]*/) /*fifoStartChannelList*/));
            if (!api->fifoStartChannelList)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->fifoStartChannelList), (api->fifoStartChannelListCount) * (sizeof(NV0080_CTRL_FIFO_CHANNEL) /*fifoStartChannelList[i]*/) /*fifoStartChannelList*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->fifoStartChannelList)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `fifoStartChannelList`.
        {
            NvLength i;
            for (i = 0; i < (api->fifoStartChannelListCount); ++i)
            {
                // Check field-presence indicator for `fifoStartChannelList[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Record has 1 field(s) to be deserialized.
                {
                    NV_STATUS status = finnDeserializeRecord_NV0080_CTRL_FIFO_CHANNEL(bp, ((NV0080_CTRL_FIFO_CHANNEL *)(NvP64_VALUE(api->fifoStartChannelList))+(i)), sizeof(NV0080_CTRL_FIFO_CHANNEL), deser_up);
                    if (status != NV_OK)
                        return status;
                }

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->fifoStartChannelList = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}

#if (defined(NVRM))

// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS(const NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `numClasses`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->numClasses, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->classList), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->classList)
    {
        // Serialize each element in `classList`.
        {
            NvLength i;
            for (i = 0; i < (api->numClasses); ++i)
            {
                // Serialize field-presence indicator for `classList[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 32-bit NvU32 primitive.
                if (finn_write_buffer(bp, ((NvU32 *)(NvP64_VALUE(api->classList)))[i], 32))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->classList)
            FINN_FREE(NvP64_VALUE(api->classList));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `numClasses`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->numClasses = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `classList`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->numClasses) * (sizeof(NvU32) /*classList[i]*/) /*classList*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->classList = NV_PTR_TO_NvP64(FINN_MALLOC((api->numClasses) * (sizeof(NvU32) /*classList[i]*/) /*classList*/));
            if (!api->classList)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->classList), (api->numClasses) * (sizeof(NvU32) /*classList[i]*/) /*classList*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->classList)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `classList`.
        {
            NvLength i;
            for (i = 0; i < (api->numClasses); ++i)
            {
                // Check field-presence indicator for `classList[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 32-bit NvU32 primitive.
                ((NvU32 *)(NvP64_VALUE(api->classList)))[i] = (NvU32) finn_read_buffer(bp, 32);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->classList = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_GR_GET_CAPS_PARAMS(const NV0080_CTRL_GR_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `capsTblSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->capsTblSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->capsTbl), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->capsTbl)
    {
        // Serialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Serialize field-presence indicator for `capsTbl[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->capsTbl)
            FINN_FREE(NvP64_VALUE(api->capsTbl));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_GR_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_GR_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_GR_GET_CAPS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `capsTblSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->capsTblSize = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `capsTbl`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->capsTbl = NV_PTR_TO_NvP64(FINN_MALLOC((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/));
            if (!api->capsTbl)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->capsTbl), (api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->capsTbl)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Check field-presence indicator for `capsTbl[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->capsTbl = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_HOST_GET_CAPS_PARAMS(const NV0080_CTRL_HOST_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `capsTblSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_HOST_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->capsTblSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->capsTbl), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->capsTbl)
    {
        // Serialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Serialize field-presence indicator for `capsTbl[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->capsTbl)
            FINN_FREE(NvP64_VALUE(api->capsTbl));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_HOST_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_HOST_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_HOST_GET_CAPS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `capsTblSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->capsTblSize = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_HOST_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check data-presence (nonnull pointer) indicator for `capsTbl`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->capsTbl = NV_PTR_TO_NvP64(FINN_MALLOC((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/));
            if (!api->capsTbl)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->capsTbl), (api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->capsTbl)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Check field-presence indicator for `capsTbl[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->capsTbl = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeMessage_NV0080_CTRL_MSENC_GET_CAPS_PARAMS(const NV0080_CTRL_MSENC_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `capsTblSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->capsTblSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->capsTbl), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->capsTbl)
    {
        // Serialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Serialize field-presence indicator for `capsTbl[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->capsTbl)
            FINN_FREE(NvP64_VALUE(api->capsTbl));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeMessage_NV0080_CTRL_MSENC_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_MSENC_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_MSENC_GET_CAPS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `capsTblSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->capsTblSize = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->capsTblSize > NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check data-presence (nonnull pointer) indicator for `capsTbl`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->capsTbl = NV_PTR_TO_NvP64(FINN_MALLOC((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/));
            if (!api->capsTbl)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->capsTbl), (api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->capsTbl)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Check field-presence indicator for `capsTbl[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->capsTbl = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}

#endif // (defined(NVRM))

#if (defined(NVRM))

// Serialize each of the 8 field(s).
// 3 out of 7 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnSerializeMessage_NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS(const NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `globType`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 16-bit NvU16 primitive.
    if (finn_write_buffer(bp, api->globType, 16))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `globIndex`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->globIndex, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `globSource`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 16-bit NvU16 primitive.
    if (finn_write_buffer(bp, api->globSource, 16))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `retBufOffset`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->retBufOffset, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `retSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->retSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `totalObjSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->totalObjSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `errorCode`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->errorCode, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->retBuf), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->retBuf)
    {
        // Serialize each element in `retBuf`.
        {
            NvLength i;
            for (i = 0; i < (api->retSize); ++i)
            {
                // Serialize field-presence indicator for `retBuf[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->retBuf)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->retBuf)
            FINN_FREE(NvP64_VALUE(api->retBuf));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 8 field(s).
// 3 out of 7 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `globType`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 16-bit NvU16 primitive.
    api->globType = (NvU16) finn_read_buffer(bp, 16);

    // Check field-presence indicator for `globIndex`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->globIndex = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `globSource`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 16-bit NvU16 primitive.
    api->globSource = (NvU16) finn_read_buffer(bp, 16);

    // Check field-presence indicator for `retBufOffset`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->retBufOffset = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `retSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->retSize = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `totalObjSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->totalObjSize = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `errorCode`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->errorCode = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `retBuf`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->retSize) * (sizeof(NvU8) /*retBuf[i]*/) /*retBuf*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->retBuf = NV_PTR_TO_NvP64(FINN_MALLOC((api->retSize) * (sizeof(NvU8) /*retBuf[i]*/) /*retBuf*/));
            if (!api->retBuf)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->retBuf), (api->retSize) * (sizeof(NvU8) /*retBuf[i]*/) /*retBuf*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->retBuf)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `retBuf`.
        {
            NvLength i;
            for (i = 0; i < (api->retSize); ++i)
            {
                // Check field-presence indicator for `retBuf[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->retBuf)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->retBuf = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeMessage_NV2080_CTRL_CE_GET_CAPS_PARAMS(const NV2080_CTRL_CE_GET_CAPS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `ceEngineType`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->ceEngineType, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `capsTblSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->capsTblSize > NV2080_CTRL_CE_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->capsTblSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->capsTbl), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->capsTbl)
    {
        // Serialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Serialize field-presence indicator for `capsTbl[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->capsTbl)
            FINN_FREE(NvP64_VALUE(api->capsTbl));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_CE_GET_CAPS_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_CE_GET_CAPS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `ceEngineType`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->ceEngineType = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `capsTblSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->capsTblSize = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->capsTblSize > NV2080_CTRL_CE_CAPS_TBL_SIZE)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check data-presence (nonnull pointer) indicator for `capsTbl`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->capsTbl = NV_PTR_TO_NvP64(FINN_MALLOC((api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/));
            if (!api->capsTbl)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->capsTbl), (api->capsTblSize) * (sizeof(NvU8) /*capsTbl[i]*/) /*capsTbl*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->capsTbl)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `capsTbl`.
        {
            NvLength i;
            for (i = 0; i < (api->capsTblSize); ++i)
            {
                // Check field-presence indicator for `capsTbl[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->capsTbl)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->capsTbl = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeMessage_NV2080_CTRL_GPU_GET_ENGINES_PARAMS(const NV2080_CTRL_GPU_GET_ENGINES_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `engineCount`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->engineCount, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->engineList), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->engineList)
    {
        // Serialize each element in `engineList`.
        {
            NvLength i;
            for (i = 0; i < (api->engineCount); ++i)
            {
                // Serialize field-presence indicator for `engineList[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 32-bit NvU32 primitive.
                if (finn_write_buffer(bp, ((NvU32 *)(NvP64_VALUE(api->engineList)))[i], 32))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->engineList)
            FINN_FREE(NvP64_VALUE(api->engineList));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_GPU_GET_ENGINES_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `engineCount`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->engineCount = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `engineList`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->engineCount) * (sizeof(NvU32) /*engineList[i]*/) /*engineList*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->engineList = NV_PTR_TO_NvP64(FINN_MALLOC((api->engineCount) * (sizeof(NvU32) /*engineList[i]*/) /*engineList*/));
            if (!api->engineList)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->engineList), (api->engineCount) * (sizeof(NvU32) /*engineList[i]*/) /*engineList*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->engineList)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `engineList`.
        {
            NvLength i;
            for (i = 0; i < (api->engineCount); ++i)
            {
                // Check field-presence indicator for `engineList[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 32-bit NvU32 primitive.
                ((NvU32 *)(NvP64_VALUE(api->engineList)))[i] = (NvU32) finn_read_buffer(bp, 32);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->engineList = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeMessage_NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS(const NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `engineType`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->engineType, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `numClasses`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->numClasses, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->classList), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->classList)
    {
        // Serialize each element in `classList`.
        {
            NvLength i;
            for (i = 0; i < (api->numClasses); ++i)
            {
                // Serialize field-presence indicator for `classList[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 32-bit NvU32 primitive.
                if (finn_write_buffer(bp, ((NvU32 *)(NvP64_VALUE(api->classList)))[i], 32))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->classList)
            FINN_FREE(NvP64_VALUE(api->classList));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `engineType`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->engineType = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `numClasses`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->numClasses = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `classList`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->numClasses) * (sizeof(NvU32) /*classList[i]*/) /*classList*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->classList = NV_PTR_TO_NvP64(FINN_MALLOC((api->numClasses) * (sizeof(NvU32) /*classList[i]*/) /*classList*/));
            if (!api->classList)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->classList), (api->numClasses) * (sizeof(NvU32) /*classList[i]*/) /*classList*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->classList)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `classList`.
        {
            NvLength i;
            for (i = 0; i < (api->numClasses); ++i)
            {
                // Check field-presence indicator for `classList[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 32-bit NvU32 primitive.
                ((NvU32 *)(NvP64_VALUE(api->classList)))[i] = (NvU32) finn_read_buffer(bp, 32);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->classList = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 9 field(s).
// 4 out of 8 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnSerializeMessage_NV2080_CTRL_I2C_ACCESS_PARAMS(const NV2080_CTRL_I2C_ACCESS_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `token`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->token, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `cmd`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->cmd, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `port`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->port, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `flags`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->flags, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `status`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->status, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `dataBuffSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->dataBuffSize > NV2080_CTRL_I2C_MAX_ENTRIES)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->dataBuffSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `speed`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->speed, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `encrClientID`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->encrClientID, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->data), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->data)
    {
        // Serialize each element in `data`.
        {
            NvLength i;
            for (i = 0; i < (api->dataBuffSize); ++i)
            {
                // Serialize field-presence indicator for `data[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->data)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->data)
            FINN_FREE(NvP64_VALUE(api->data));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 9 field(s).
// 4 out of 8 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_I2C_ACCESS_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_I2C_ACCESS_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `token`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->token = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `cmd`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->cmd = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `port`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->port = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `flags`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->flags = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `status`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->status = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `dataBuffSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->dataBuffSize = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->dataBuffSize > NV2080_CTRL_I2C_MAX_ENTRIES)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check field-presence indicator for `speed`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->speed = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `encrClientID`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->encrClientID = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `data`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->dataBuffSize) * (sizeof(NvU8) /*data[i]*/) /*data*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->data = NV_PTR_TO_NvP64(FINN_MALLOC((api->dataBuffSize) * (sizeof(NvU8) /*data[i]*/) /*data*/));
            if (!api->data)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->data), (api->dataBuffSize) * (sizeof(NvU8) /*data[i]*/) /*data*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->data)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `data`.
        {
            NvLength i;
            for (i = 0; i < (api->dataBuffSize); ++i)
            {
                // Check field-presence indicator for `data[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->data)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->data = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
// 2 out of 2 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnSerializeMessage_NV2080_CTRL_NVD_GET_DUMP_PARAMS(const NV2080_CTRL_NVD_GET_DUMP_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `component`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->component, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `size`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->size, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pBuffer), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pBuffer)
    {
        // Serialize each element in `pBuffer`.
        {
            NvLength i;
            for (i = 0; i < (api->size); ++i)
            {
                // Serialize field-presence indicator for `pBuffer[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pBuffer)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pBuffer)
            FINN_FREE(NvP64_VALUE(api->pBuffer));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
// 2 out of 2 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_NVD_GET_DUMP_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_NVD_GET_DUMP_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `component`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->component = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `size`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->size = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pBuffer`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->size) * (sizeof(NvU8) /*pBuffer[i]*/) /*pBuffer*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pBuffer = NV_PTR_TO_NvP64(FINN_MALLOC((api->size) * (sizeof(NvU8) /*pBuffer[i]*/) /*pBuffer*/));
            if (!api->pBuffer)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pBuffer), (api->size) * (sizeof(NvU8) /*pBuffer[i]*/) /*pBuffer*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pBuffer)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pBuffer`.
        {
            NvLength i;
            for (i = 0; i < (api->size); ++i)
            {
                // Check field-presence indicator for `pBuffer[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pBuffer)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pBuffer = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 4 field(s).
// 1 out of 3 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnSerializeMessage_NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS(const NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `hChannel`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->hChannel, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `virtAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 64-bit NvU64 primitive.
    if (finn_write_buffer(bp, api->virtAddress, 64))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `bufferSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->bufferSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->bufferPtr), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->bufferPtr)
    {
        // Serialize each element in `bufferPtr`.
        {
            NvLength i;
            for (i = 0; i < (api->bufferSize); ++i)
            {
                // Serialize field-presence indicator for `bufferPtr[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->bufferPtr)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->bufferPtr)
            FINN_FREE(NvP64_VALUE(api->bufferPtr));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 4 field(s).
// 1 out of 3 independent field(s) are reordered to be before 1 dependent field(s).
static NV_STATUS finnDeserializeMessage_NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS(finn_bit_pump_for_read *bp, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `hChannel`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->hChannel = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `virtAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 64-bit NvU64 primitive.
    api->virtAddress = (NvU64) finn_read_buffer(bp, 64);

    // Check field-presence indicator for `bufferSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->bufferSize = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `bufferPtr`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->bufferSize) * (sizeof(NvU8) /*bufferPtr[i]*/) /*bufferPtr*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->bufferPtr = NV_PTR_TO_NvP64(FINN_MALLOC((api->bufferSize) * (sizeof(NvU8) /*bufferPtr[i]*/) /*bufferPtr*/));
            if (!api->bufferPtr)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->bufferPtr), (api->bufferSize) * (sizeof(NvU8) /*bufferPtr[i]*/) /*bufferPtr*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->bufferPtr)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `bufferPtr`.
        {
            NvLength i;
            for (i = 0; i < (api->bufferSize); ++i)
            {
                // Check field-presence indicator for `bufferPtr[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->bufferPtr)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->bufferPtr = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}

#endif // (defined(NVRM))

#if (defined(NVRM))

// Serialize each of the 8 field(s).
// 1 out of 6 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnSerializeMessage_NV402C_CTRL_I2C_INDEXED_PARAMS(const NV402C_CTRL_I2C_INDEXED_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `portId`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->portId, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `bIsWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->bIsWrite, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `address`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 16-bit NvU16 primitive.
    if (finn_write_buffer(bp, api->address, 16))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `flags`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->flags, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `indexLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->indexLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `messageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->messageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize each element in `index`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_INDEX_LENGTH_MAX; ++i)
        {
            // Serialize field-presence indicator for `index[i]`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Serialize 8-bit NvU8 primitive.
            if (finn_write_buffer(bp, (api->index)[i], 8))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

        }
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pMessage), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pMessage)
    {
        // Serialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Serialize field-presence indicator for `pMessage[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pMessage)
            FINN_FREE(NvP64_VALUE(api->pMessage));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 8 field(s).
// 1 out of 6 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnDeserializeMessage_NV402C_CTRL_I2C_INDEXED_PARAMS(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_INDEXED_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_INDEXED_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `portId`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->portId = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `bIsWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->bIsWrite = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `address`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 16-bit NvU16 primitive.
    api->address = (NvU16) finn_read_buffer(bp, 16);

    // Check field-presence indicator for `flags`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->flags = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `indexLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->indexLength = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check field-presence indicator for `messageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->messageLength = (NvU32) finn_read_buffer(bp, 32);

    // Deserialize each element in `index`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_INDEX_LENGTH_MAX; ++i)
        {
            // Check field-presence indicator for `index[i]`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Deserialize 8-bit NvU8 primitive.
            (api->index)[i] = (NvU8) finn_read_buffer(bp, 8);

        }
    }

    // Check data-presence (nonnull pointer) indicator for `pMessage`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pMessage = NV_PTR_TO_NvP64(FINN_MALLOC((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/));
            if (!api->pMessage)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pMessage), (api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pMessage)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Check field-presence indicator for `pMessage[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pMessage = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 5 field(s).
static NV_STATUS finnSerializeMessage_NV402C_CTRL_I2C_TRANSACTION_PARAMS(const NV402C_CTRL_I2C_TRANSACTION_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `portId`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->portId, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `flags`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->flags, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `deviceAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 16-bit NvU16 primitive.
    if (finn_write_buffer(bp, api->deviceAddress, 16))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `transType`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Validate the enum value.
    if (finnBadEnum_NV402C_CTRL_I2C_TRANSACTION_TYPE(api->transType))
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Serialize NV402C_CTRL_I2C_TRANSACTION_TYPE enum.
    if (finn_write_buffer(bp, api->transType, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `transData`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // `transData` has 11 cases to serialize.
    {
        NV_STATUS status = finnSerializeUnion_NV402C_CTRL_I2C_TRANSACTION_DATA(&api->transData, bp, seri_up, api->transType);
        if (status != NV_OK)
            return status;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 5 field(s).
static NV_STATUS finnDeserializeMessage_NV402C_CTRL_I2C_TRANSACTION_PARAMS(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `portId`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->portId = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `flags`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->flags = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `deviceAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 16-bit NvU16 primitive.
    api->deviceAddress = (NvU16) finn_read_buffer(bp, 16);

    // Check field-presence indicator for `transType`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize NV402C_CTRL_I2C_TRANSACTION_TYPE enum.
    api->transType = (NV402C_CTRL_I2C_TRANSACTION_TYPE) finn_read_buffer(bp, 32);

    // Validate the enum value.
    if (finnBadEnum_NV402C_CTRL_I2C_TRANSACTION_TYPE(api->transType))
    {
        FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Check field-presence indicator for `transData`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // `transData` has 11 cases to deserialize.
    {
        NV_STATUS status = finnDeserializeUnion_NV402C_CTRL_I2C_TRANSACTION_DATA(bp, &api->transData, api_size, deser_up, api->transType);
        if (status != NV_OK)
            return status;
    }

    // Done
    return NV_OK;
}


// Serialize each of the 4 field(s).
static NV_STATUS finnSerializeMessage_NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS(const NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `hMemory`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->hMemory, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `length`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->length, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `offset`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 64-bit NvU64 primitive.
    if (finn_write_buffer(bp, api->offset, 64))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->buffer), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->buffer)
    {
        // Serialize each element in `buffer`.
        {
            NvLength i;
            for (i = 0; i < (api->length); ++i)
            {
                // Serialize field-presence indicator for `buffer[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->buffer)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->buffer)
            FINN_FREE(NvP64_VALUE(api->buffer));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 4 field(s).
static NV_STATUS finnDeserializeMessage_NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `hMemory`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->hMemory = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `length`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->length = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `offset`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 64-bit NvU64 primitive.
    api->offset = (NvU64) finn_read_buffer(bp, 64);

    // Check data-presence (nonnull pointer) indicator for `buffer`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->length) * (sizeof(NvU8) /*buffer[i]*/) /*buffer*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->buffer = NV_PTR_TO_NvP64(FINN_MALLOC((api->length) * (sizeof(NvU8) /*buffer[i]*/) /*buffer*/));
            if (!api->buffer)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->buffer), (api->length) * (sizeof(NvU8) /*buffer[i]*/) /*buffer*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->buffer)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `buffer`.
        {
            NvLength i;
            for (i = 0; i < (api->length); ++i)
            {
                // Check field-presence indicator for `buffer[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->buffer)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->buffer = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 4 field(s).
static NV_STATUS finnSerializeMessage_NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS(const NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `hMemory`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->hMemory, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `length`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->length, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `offset`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 64-bit NvU64 primitive.
    if (finn_write_buffer(bp, api->offset, 64))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->buffer), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->buffer)
    {
        // Serialize each element in `buffer`.
        {
            NvLength i;
            for (i = 0; i < (api->length); ++i)
            {
                // Serialize field-presence indicator for `buffer[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->buffer)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->buffer)
            FINN_FREE(NvP64_VALUE(api->buffer));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 4 field(s).
static NV_STATUS finnDeserializeMessage_NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS(finn_bit_pump_for_read *bp, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `hMemory`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->hMemory = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `length`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->length = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `offset`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 64-bit NvU64 primitive.
    api->offset = (NvU64) finn_read_buffer(bp, 64);

    // Check data-presence (nonnull pointer) indicator for `buffer`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->length) * (sizeof(NvU8) /*buffer[i]*/) /*buffer*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->buffer = NV_PTR_TO_NvP64(FINN_MALLOC((api->length) * (sizeof(NvU8) /*buffer[i]*/) /*buffer*/));
            if (!api->buffer)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->buffer), (api->length) * (sizeof(NvU8) /*buffer[i]*/) /*buffer*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->buffer)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `buffer`.
        {
            NvLength i;
            for (i = 0; i < (api->length); ++i)
            {
                // Check field-presence indicator for `buffer[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->buffer)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->buffer = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 1 field(s).
static NV_STATUS finnSerializeMessage_NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS(const NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `params`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Message has 3 field(s) to be serialized.
    {
        NV_STATUS status = finnSerializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS(&api->params, bp, seri_up);
        if (status != NV_OK)
            return status;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 1 field(s).
static NV_STATUS finnDeserializeMessage_NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS(finn_bit_pump_for_read *bp, NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `params`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Message has 3 field(s) to be deserialized.
    {
        NV_STATUS status = finnDeserializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS(bp, &api->params, sizeof(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS), deser_up);
        if (status != NV_OK)
            return status;
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeMessage_NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS(const NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `engineID`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->engineID, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `size`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->size, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pEngineCtxBuff), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pEngineCtxBuff)
    {
        // Serialize each element in `pEngineCtxBuff`.
        {
            NvLength i;
            for (i = 0; i < (api->size); ++i)
            {
                // Serialize field-presence indicator for `pEngineCtxBuff[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pEngineCtxBuff)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pEngineCtxBuff)
            FINN_FREE(NvP64_VALUE(api->pEngineCtxBuff));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeMessage_NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS(finn_bit_pump_for_read *bp, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `engineID`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->engineID = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `size`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->size = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pEngineCtxBuff`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->size) * (sizeof(NvU8) /*pEngineCtxBuff[i]*/) /*pEngineCtxBuff*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pEngineCtxBuff = NV_PTR_TO_NvP64(FINN_MALLOC((api->size) * (sizeof(NvU8) /*pEngineCtxBuff[i]*/) /*pEngineCtxBuff*/));
            if (!api->pEngineCtxBuff)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pEngineCtxBuff), (api->size) * (sizeof(NvU8) /*pEngineCtxBuff[i]*/) /*pEngineCtxBuff*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pEngineCtxBuff)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pEngineCtxBuff`.
        {
            NvLength i;
            for (i = 0; i < (api->size); ++i)
            {
                // Check field-presence indicator for `pEngineCtxBuff[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pEngineCtxBuff)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pEngineCtxBuff = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS(const NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `engineID`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->engineID, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `size`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->size, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize each element in `engineCtxBuff`.
    {
        NvLength i;
        for (i = 0; i < NVB06F_CTRL_ENGINE_CTX_BUFFER_SIZE_MAX; ++i)
        {
            // Serialize field-presence indicator for `engineCtxBuff[i]`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Serialize 8-bit NvU8 primitive.
            if (finn_write_buffer(bp, (api->engineCtxBuff)[i], 8))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

        }
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeMessage_NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS(finn_bit_pump_for_read *bp, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `engineID`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->engineID = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `size`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->size = (NvU32) finn_read_buffer(bp, 32);

    // Deserialize each element in `engineCtxBuff`.
    {
        NvLength i;
        for (i = 0; i < NVB06F_CTRL_ENGINE_CTX_BUFFER_SIZE_MAX; ++i)
        {
            // Check field-presence indicator for `engineCtxBuff[i]`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Deserialize 8-bit NvU8 primitive.
            (api->engineCtxBuff)[i] = (NvU8) finn_read_buffer(bp, 8);

        }
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeRecord_NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS(const NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `physAddr`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 64-bit NvU64 primitive.
    if (finn_write_buffer(bp, api->physAddr, 64))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `numEntries`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->numEntries, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `aperture`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->aperture, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeRecord_NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS(finn_bit_pump_for_read *bp, NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `physAddr`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 64-bit NvU64 primitive.
    api->physAddr = (NvU64) finn_read_buffer(bp, 64);

    // Check field-presence indicator for `numEntries`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->numEntries = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `aperture`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->aperture = (NvU32) finn_read_buffer(bp, 32);

    // Done
    return NV_OK;
}

#endif // (defined(NVRM))


// Serialize each of the 1 field(s).
static NV_STATUS finnSerializeRecord_NV0080_CTRL_FIFO_CHANNEL(const NV0080_CTRL_FIFO_CHANNEL *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `hChannel`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->hChannel, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 1 field(s).
static NV_STATUS finnDeserializeRecord_NV0080_CTRL_FIFO_CHANNEL(finn_bit_pump_for_read *bp, NV0080_CTRL_FIFO_CHANNEL *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV0080_CTRL_FIFO_CHANNEL) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `hChannel`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->hChannel = (NvU32) finn_read_buffer(bp, 32);

    // Done
    return NV_OK;
}

#if (defined(NVRM))

// Serialize each of the 5 field(s).
static NV_STATUS finnSerializeRecord_NV2080_CTRL_GPUMON_SAMPLES(const NV2080_CTRL_GPUMON_SAMPLES *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `type`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->type, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `bufSize`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->bufSize, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `count`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->count, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `tracker`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->tracker, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pSamples), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pSamples)
    {
        // Serialize each element in `pSamples`.
        {
            NvLength i;
            for (i = 0; i < (api->bufSize); ++i)
            {
                // Serialize field-presence indicator for `pSamples[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pSamples)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pSamples)
            FINN_FREE(NvP64_VALUE(api->pSamples));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 5 field(s).
static NV_STATUS finnDeserializeRecord_NV2080_CTRL_GPUMON_SAMPLES(finn_bit_pump_for_read *bp, NV2080_CTRL_GPUMON_SAMPLES *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV2080_CTRL_GPUMON_SAMPLES) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `type`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->type = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `bufSize`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->bufSize = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `count`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->count = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `tracker`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->tracker = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pSamples`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->bufSize) * (sizeof(NvU8) /*pSamples[i]*/) /*pSamples*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pSamples = NV_PTR_TO_NvP64(FINN_MALLOC((api->bufSize) * (sizeof(NvU8) /*pSamples[i]*/) /*pSamples*/));
            if (!api->pSamples)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pSamples), (api->bufSize) * (sizeof(NvU8) /*pSamples[i]*/) /*pSamples*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pSamples)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pSamples`.
        {
            NvLength i;
            for (i = 0; i < (api->bufSize); ++i)
            {
                // Check field-presence indicator for `pSamples[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pSamples)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pSamples = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}

#endif // (defined(NVRM))

#if (defined(NVRM))

// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `messageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->messageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pMessage), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pMessage)
    {
        // Serialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Serialize field-presence indicator for `pMessage[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pMessage)
            FINN_FREE(NvP64_VALUE(api->pMessage));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `messageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->messageLength = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pMessage`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pMessage = NV_PTR_TO_NvP64(FINN_MALLOC((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/));
            if (!api->pMessage)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pMessage), (api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pMessage)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Check field-presence indicator for `pMessage[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pMessage = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 5 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `warFlags`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->warFlags, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `registerAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->registerAddress, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `messageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->messageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pMessage), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pMessage)
    {
        // Serialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Serialize field-presence indicator for `pMessage[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pMessage)
            FINN_FREE(NvP64_VALUE(api->pMessage));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 5 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `warFlags`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->warFlags = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `registerAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->registerAddress = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `messageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->messageLength = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pMessage`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pMessage = NV_PTR_TO_NvP64(FINN_MALLOC((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/));
            if (!api->pMessage)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pMessage), (api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pMessage)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Check field-presence indicator for `pMessage[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pMessage = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `message`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->message, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `message`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->message = (NvU8) finn_read_buffer(bp, 8);

    // Done
    return NV_OK;
}


// Serialize each of the 4 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC(const NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `segmentNumber`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->segmentNumber, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `registerAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->registerAddress, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `messageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->messageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pMessage), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pMessage)
    {
        // Serialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Serialize field-presence indicator for `pMessage[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pMessage)
            FINN_FREE(NvP64_VALUE(api->pMessage));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 4 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `segmentNumber`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->segmentNumber = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `registerAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->registerAddress = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `messageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->messageLength = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pMessage`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pMessage = NV_PTR_TO_NvP64(FINN_MALLOC((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/));
            if (!api->pMessage)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pMessage), (api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pMessage)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Check field-presence indicator for `pMessage[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pMessage = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 5 field(s).
// 1 out of 3 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `registerAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->registerAddress, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `writeMessageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->writeMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->writeMessageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `readMessageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->readMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->readMessageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize each element in `writeMessage`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX; ++i)
        {
            // Serialize field-presence indicator for `writeMessage[i]`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Serialize 8-bit NvU8 primitive.
            if (finn_write_buffer(bp, (api->writeMessage)[i], 8))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

        }
    }

    // Serialize each element in `readMessage`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX; ++i)
        {
            // Serialize field-presence indicator for `readMessage[i]`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Serialize 8-bit NvU8 primitive.
            if (finn_write_buffer(bp, (api->readMessage)[i], 8))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

        }
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 5 field(s).
// 1 out of 3 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `registerAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->registerAddress = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `writeMessageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->writeMessageLength = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->writeMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check field-presence indicator for `readMessageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->readMessageLength = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->readMessageLength > NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Deserialize each element in `writeMessage`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX; ++i)
        {
            // Check field-presence indicator for `writeMessage[i]`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Deserialize 8-bit NvU8 primitive.
            (api->writeMessage)[i] = (NvU8) finn_read_buffer(bp, 8);

        }
    }

    // Deserialize each element in `readMessage`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_BLOCK_PROCESS_PROTOCOL_MAX; ++i)
        {
            // Check field-presence indicator for `readMessage[i]`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Deserialize 8-bit NvU8 primitive.
            (api->readMessage)[i] = (NvU8) finn_read_buffer(bp, 8);

        }
    }

    // Done
    return NV_OK;
}


// Serialize each of the 4 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `registerAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->registerAddress, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `messageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->messageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pMessage), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pMessage)
    {
        // Serialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Serialize field-presence indicator for `pMessage[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pMessage)
            FINN_FREE(NvP64_VALUE(api->pMessage));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 4 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `registerAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->registerAddress = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `messageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->messageLength = (NvU32) finn_read_buffer(bp, 32);

    // Check data-presence (nonnull pointer) indicator for `pMessage`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pMessage = NV_PTR_TO_NvP64(FINN_MALLOC((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/));
            if (!api->pMessage)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pMessage), (api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pMessage)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Check field-presence indicator for `pMessage[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pMessage = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `registerAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->registerAddress, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `message`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->message, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `registerAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->registerAddress = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `message`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->message = (NvU8) finn_read_buffer(bp, 8);

    // Done
    return NV_OK;
}


// Serialize each of the 6 field(s).
// 1 out of 4 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `warFlags`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->warFlags, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `indexLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Reject source data if it is out of range.
    if (api->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->indexLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `messageLength`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->messageLength, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize each element in `index`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_INDEX_LENGTH_MAX; ++i)
        {
            // Serialize field-presence indicator for `index[i]`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Serialize 8-bit NvU8 primitive.
            if (finn_write_buffer(bp, (api->index)[i], 8))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

        }
    }

    // Serialize data-presence (nonnull pointer) indicator.
    if (finn_write_buffer(bp, !!(api->pMessage), 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Skip if pointer is null.
    if (api->pMessage)
    {
        // Serialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Serialize field-presence indicator for `pMessage[i]`.
                if (finn_write_buffer(bp, 1, 1))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

                // Serialize 8-bit NvU8 primitive.
                if (finn_write_buffer(bp, ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i], 8))
                {
                    FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                    return NV_ERR_BUFFER_TOO_SMALL;
                }

            }
        }

        // Free memory that was allocated during downward deserialization.
        if (seri_up && api->pMessage)
            FINN_FREE(NvP64_VALUE(api->pMessage));
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 6 field(s).
// 1 out of 4 independent field(s) are reordered to be before 2 dependent field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `warFlags`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->warFlags = (NvU32) finn_read_buffer(bp, 32);

    // Check field-presence indicator for `indexLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->indexLength = (NvU32) finn_read_buffer(bp, 32);

    // Reject deserialized data if it is out of range.
    if (api->indexLength > NV402C_CTRL_I2C_INDEX_LENGTH_MAX)
    {
        FINN_ERROR(NV_ERR_OUT_OF_RANGE);
        return NV_ERR_OUT_OF_RANGE;
    }

    // Check field-presence indicator for `messageLength`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->messageLength = (NvU32) finn_read_buffer(bp, 32);

    // Deserialize each element in `index`.
    {
        NvLength i;
        for (i = 0; i < NV402C_CTRL_I2C_INDEX_LENGTH_MAX; ++i)
        {
            // Check field-presence indicator for `index[i]`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Deserialize 8-bit NvU8 primitive.
            (api->index)[i] = (NvU8) finn_read_buffer(bp, 8);

        }
    }

    // Check data-presence (nonnull pointer) indicator for `pMessage`.
    if (finn_read_buffer(bp, 1))
    {
        // Allocate memory and set pointer when deserializing down.
        // (Calling code is expected to do so when deserializing up.)
        if (!deser_up)
        {
            // The data-presence indicator would have been false
            // if there were no data to deserialize.
            if ((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/ < 1)
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            api->pMessage = NV_PTR_TO_NvP64(FINN_MALLOC((api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/));
            if (!api->pMessage)
            {
                FINN_ERROR(NV_ERR_NO_MEMORY);
                return NV_ERR_NO_MEMORY;
            }

            FINN_MEMZERO(NvP64_VALUE(api->pMessage), (api->messageLength) * (sizeof(NvU8) /*pMessage[i]*/) /*pMessage*/);
        }

        // Otherwise the pointer must be provided by caller.
        else if (!api->pMessage)
        {
            FINN_ERROR(NV_ERR_INVALID_POINTER);
            return NV_ERR_INVALID_POINTER;
        }

        // Deserialize each element in `pMessage`.
        {
            NvLength i;
            for (i = 0; i < (api->messageLength); ++i)
            {
                // Check field-presence indicator for `pMessage[i]`.
                if (!finn_read_buffer(bp, 1))
                {
                    FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                    return NV_ERR_LIB_RM_VERSION_MISMATCH;
                }

                // Deserialize 8-bit NvU8 primitive.
                ((NvU8 *)(NvP64_VALUE(api->pMessage)))[i] = (NvU8) finn_read_buffer(bp, 8);

            }
        }

    }

    // Nullify pointer only if FINN manages memory allocation.
    else
    {
        if (!deser_up)
            api->pMessage = NV_PTR_TO_NvP64(NULL);
    }

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `registerAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->registerAddress, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `writeMessage`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 16-bit NvU16 primitive.
    if (finn_write_buffer(bp, api->writeMessage, 16))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `readMessage`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 16-bit NvU16 primitive.
    if (finn_write_buffer(bp, api->readMessage, 16))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `registerAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->registerAddress = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `writeMessage`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 16-bit NvU16 primitive.
    api->writeMessage = (NvU16) finn_read_buffer(bp, 16);

    // Check field-presence indicator for `readMessage`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 16-bit NvU16 primitive.
    api->readMessage = (NvU16) finn_read_buffer(bp, 16);

    // Done
    return NV_OK;
}


// Serialize each of the 2 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `warFlags`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 32-bit NvU32 primitive.
    if (finn_write_buffer(bp, api->warFlags, 32))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 2 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `warFlags`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 32-bit NvU32 primitive.
    api->warFlags = (NvU32) finn_read_buffer(bp, 32);

    // Done
    return NV_OK;
}


// Serialize each of the 3 field(s).
static NV_STATUS finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW(const NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *api, finn_bit_pump_for_write *bp, NvBool seri_up)
{
    // Serialize field-presence indicator for `bWrite`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 1-bit NvBool primitive.
    if (finn_write_buffer(bp, api->bWrite, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `registerAddress`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 8-bit NvU8 primitive.
    if (finn_write_buffer(bp, api->registerAddress, 8))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize field-presence indicator for `message`.
    if (finn_write_buffer(bp, 1, 1))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Serialize 16-bit NvU16 primitive.
    if (finn_write_buffer(bp, api->message, 16))
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }


    // Done
    return NV_OK;
}


// Deserialize each of the 3 field(s).
static NV_STATUS finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW *api, NvLength api_size, NvBool deser_up)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check field-presence indicator for `bWrite`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 1-bit NvBool primitive.
    api->bWrite = (NvBool) finn_read_buffer(bp, 1);

    // Check field-presence indicator for `registerAddress`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 8-bit NvU8 primitive.
    api->registerAddress = (NvU8) finn_read_buffer(bp, 8);

    // Check field-presence indicator for `message`.
    if (!finn_read_buffer(bp, 1))
    {
        FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
        return NV_ERR_LIB_RM_VERSION_MISMATCH;
    }

    // Deserialize 16-bit NvU16 primitive.
    api->message = (NvU16) finn_read_buffer(bp, 16);

    // Done
    return NV_OK;
}

#endif // (defined(NVRM))

#if (defined(NVRM))

// Serialize selected field from 11 possible values.
static NV_STATUS finnSerializeUnion_NV402C_CTRL_I2C_TRANSACTION_DATA(const NV402C_CTRL_I2C_TRANSACTION_DATA *api, finn_bit_pump_for_write *bp, NvBool seri_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType)
{
    switch(transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        {
            // Serialize field-presence indicator for `api->smbusQuickData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 2 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW(&api->smbusQuickData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        {
            // Serialize field-presence indicator for `api->i2cByteData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 2 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW(&api->i2cByteData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            // Serialize field-presence indicator for `api->i2cBlockData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 3 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW(&api->i2cBlockData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            // Serialize field-presence indicator for `api->i2cBufferData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 5 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW(&api->i2cBufferData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        {
            // Serialize field-presence indicator for `api->smbusByteData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 3 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW(&api->smbusByteData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        {
            // Serialize field-presence indicator for `api->smbusWordData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 3 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW(&api->smbusWordData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            // Serialize field-presence indicator for `api->smbusBlockData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 4 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW(&api->smbusBlockData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        {
            // Serialize field-presence indicator for `api->smbusProcessData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 3 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL(&api->smbusProcessData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        {
            // Serialize field-presence indicator for `api->smbusBlockProcessData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 5 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL(&api->smbusBlockProcessData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            // Serialize field-presence indicator for `api->smbusMultibyteRegisterData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 6 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW(&api->smbusMultibyteRegisterData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            // Serialize field-presence indicator for `api->edidData`.
            if (finn_write_buffer(bp, 1, 1))
            {
                FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            // Record has 4 field(s) to be serialized.
            {
                NV_STATUS status = finnSerializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC(&api->edidData, bp, seri_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        default:
        {
            FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
}


// Deserialize selected field from 11 possible values.
static NV_STATUS finnDeserializeUnion_NV402C_CTRL_I2C_TRANSACTION_DATA(finn_bit_pump_for_read *bp, NV402C_CTRL_I2C_TRANSACTION_DATA *api, NvLength api_size, NvBool deser_up, NV402C_CTRL_I2C_TRANSACTION_TYPE transType)
{
    // Check that the destination struct fits within the destination buffer.
    if (sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA) > api_size)
    {
        FINN_ERROR(NV_ERR_BUFFER_TOO_SMALL);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    switch(transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW:
        {
            // Check field-presence indicator for `api->smbusQuickData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 2 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW(bp, &api->smbusQuickData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_QUICK_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BYTE_RW:
        {
            // Check field-presence indicator for `api->i2cByteData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 2 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW(bp, &api->i2cByteData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BYTE_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            // Check field-presence indicator for `api->i2cBlockData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 3 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW(bp, &api->i2cBlockData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BLOCK_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            // Check field-presence indicator for `api->i2cBufferData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 5 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW(bp, &api->i2cBufferData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_I2C_BUFFER_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW:
        {
            // Check field-presence indicator for `api->smbusByteData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 3 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW(bp, &api->smbusByteData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BYTE_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_WORD_RW:
        {
            // Check field-presence indicator for `api->smbusWordData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 3 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW(bp, &api->smbusWordData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_WORD_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            // Check field-presence indicator for `api->smbusBlockData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 4 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW(bp, &api->smbusBlockData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_PROCESS_CALL:
        {
            // Check field-presence indicator for `api->smbusProcessData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 3 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL(bp, &api->smbusProcessData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_PROCESS_CALL), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_PROCESS_CALL:
        {
            // Check field-presence indicator for `api->smbusBlockProcessData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 5 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL(bp, &api->smbusBlockProcessData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_BLOCK_PROCESS_CALL), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            // Check field-presence indicator for `api->smbusMultibyteRegisterData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 6 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW(bp, &api->smbusMultibyteRegisterData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            // Check field-presence indicator for `api->edidData`.
            if (!finn_read_buffer(bp, 1))
            {
                FINN_ERROR(NV_ERR_LIB_RM_VERSION_MISMATCH);
                return NV_ERR_LIB_RM_VERSION_MISMATCH;
            }

            // Record has 4 field(s) to be deserialized.
            {
                NV_STATUS status = finnDeserializeRecord_NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC(bp, &api->edidData, sizeof(NV402C_CTRL_I2C_TRANSACTION_DATA_READ_EDID_DDC), deser_up);
                if (status != NV_OK)
                    return status;
            }

            return NV_OK;  // Success
        }

        default:
        {
            FINN_ERROR(NV_ERR_INVALID_ARGUMENT);
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
}


#endif // (defined(NVRM))
