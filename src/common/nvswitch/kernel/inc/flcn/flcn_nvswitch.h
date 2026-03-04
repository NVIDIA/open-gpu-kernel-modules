/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _FLCN_NVSWITCH_H_
#define _FLCN_NVSWITCH_H_

#include "flcn/flcnrtosdebug_nvswitch.h"         // <TODO - HEADER CLEANUP>
#include "flcnifcmn.h"
#include "flcn/flcnqueue_nvswitch.h"

#include "flcn/haldefs_flcn_nvswitch.h"
#include "common_nvswitch.h"

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: FLCN_NVSWITCH.H                                                   *
*       Defines and structures used for the Falcon Object. The Falcon       *
*       object is the base object for all Falcon-derived engines.           *
\***************************************************************************/

/*!
 * Compares an unit id against the values in the unit_id enumeration and
 * verifies that the id is valid.  It is expected that the id is specified
 * as an unsigned integer.
 */
#define  RM_FLCN_UNIT_ID_IS_VALID(pQeueInfo, id)                                       \
             ((id) < (pQeueInfo)->maxUnitId)


/*!
 * Verifies that the given queue identifier is a valid command queue id.  It
 * is expected that the id is specified as an unsigned integer.
 */
#define  RM_FLCN_QUEUEID_IS_COMMAND_QUEUE(pQeueInfo, id)       \
             ((id)  <= (pQeueInfo)->maxCmdQueueIndex)

/*!
 * Define a sequence descriptor that may be used during initialization that
 * represents an invalid sequence descriptor (one in which will never be
 * assigned when a sequence/command is submitted).
 */
#define FLCN_INVALID_SEQ_DESC    NV_U32_MAX

/*!
 * Define a event descriptor that may be used during initialization that
 * represents an invalid event descriptor (one in which will never be assigned
 * when a event is registered).
 */
#define FLCN_INVALID_EVT_DESC    NV_U32_MAX

/*!
 * Defines the alignment/granularity of falcon memory blocks
 */
#define FLCN_BLK_ALIGNMENT (256)

/*!
 * Defines the required address/offset alignment for all DMEM accesses
 */
#define FLCN_DMEM_ACCESS_ALIGNMENT (4)

typedef struct FLCN_EVENT_INFO FLCN_EVENT_INFO, *PFLCN_EVENT_INFO;

/*!
 * @brief Tracks all information for each client that has registered for a
 *        specific type of event-notification.
 */
struct FLCN_EVENT_INFO
{
    /*!
     * A unique identifier given to each event info instance to provide a
     * fast way to identify and track an event registration.
     */
    NvU32                  evtDesc;

    /*!
     * An identifier that describes the type of event the client wants
     * notification of.
     */
    NvU32                  unitId;

    /*!
     * The client's pre-allocated message buffer.  This is the buffer that
     * the message data will be written to when extracted from the Message
     * Queue.  This buffer must be sufficiently sized to hold the largest
     * possible event for type 'unitId'.
     */
    union RM_FLCN_MSG     *pMsg;

    /*! The client function to be called when the event triggers. */
    FlcnQMgrClientCallback pCallback;

    /*!
     * Any client-specified private parameters that must be provided in the
     * callback function.
     */
    void                  *pCallbackParams;

    /*!
     * Client's are tracked as a linked list.  This is a pointer to the next
     * client in the list.  The ordering of this list implies no association
     * between the clients.
     */
    FLCN_EVENT_INFO     *pNext;
};

/*!
 * @brief Enumeration for each discrete command state.
 */
typedef enum FLCN_CMD_STATE
{
    /*!
     * Indicates the the command does not have a state.  Commands/sequences
     * that have never been submitted while possess this state.
     */
    FLCN_CMD_STATE_NONE = 0,

    /*! Indicates that the command is being processed by the FLCN.     */
    FLCN_CMD_STATE_RUNNING,

    /*! Indicates that the command has finished execution on the FLCN. */
    FLCN_CMD_STATE_DONE
} FLCN_CMD_STATE;


typedef struct FALCON_EXTERNAL_CONFIG
{
    NvBool bResetInPmc;                 // If TRUE, Reset Falcon using PMC Enable
    NvU32 riscvRegisterBase;            // Falcon's RISCV base offset.
    NvU32 fbifBase;                     // Falcon's FB Interface base.
    NvU32 blkcgBase;                    // Falcon's BLKCG base.
} FALCON_EXTERNAL_CONFIG, *PFALCON_EXTERNAL_CONFIG;

typedef struct
{
    NvU8                maxUnitId;                  //<! Maximum valid Unit Id
    NvU8                initEventUnitId;            //<! INIT event unit id
    NvU8                cmdQHeadSize;               //<! Command Queue Head Size
    NvU8                cmdQTailSize;               //<! Command Queue Tail size
    NvU8                msgQHeadSize;               //<! Message Queue Head Size
    NvU8                msgQTailSize;               //<! Message Queue Tail Size
    NvU32               maxCmdQueueIndex;           //<! Maximum command queue Index
    NvU32               maxMsgSize;                 //<! Maximum valid MSG size
    NvU32               cmdQHeadBaseAddress;        //<! Base Register Address of Command Queue Head.
    NvU32               cmdQHeadStride;             //<! Stride used to access indexed Command Queue Head registers.
    NvU32               cmdQTailBaseAddress;        //<! Base Register Address of Command Queue Tail.
    NvU32               cmdQTailStride;             //<! Stride used to access indexed Command Queue Tail registers.
    NvU32               msgQHeadBaseAddress;        //<! Base Register Address of Message Queue Head.
    NvU32               msgQHeadStride;             //<! Stride used to access indexed Message Queue Head registers.
    NvU32               msgQTailBaseAddress;        //<! Base Register Address of Message Queue Tail.
    NvU32               msgQTailStride;             //<! Stride used to access indexed Message Queue Head registers.
    FLCNQUEUE          *pQueues;                    //<! Queues allocated

    /*!
     * A linked-list of event information structures tracking all clients that
     * have registered for event notification.
     */
    PFLCN_EVENT_INFO pEventInfo;

    /*!
     * Each time a client registers for an event notification, an internal
     * data structure is created and attached to the event-info list.  Each
     * structure will have a unique identifier/descriptor assigned that will
     * be used to track and manage the registration.  This variable keeps track
     * of the next descriptor that will be assigned at any given time.
     */
    NvU32               nextEvtDesc;

    /*!
     * Similar to 'nextEvtDesc' keeps track of the command descriptor that
     * will be assigned to the next queued command.
     */
    NvU32               nextSeqDesc;

    /*!
     * Keeps track the latest used sequence number. We always search the free
     * sequence starting from the next to the latest used sequence since it is
     * the most possible free sequence if we consume the sequence in serial.
     */
    NvU32               latestUsedSeqNum;

} FALCON_QUEUE_INFO,
*PFALCON_QUEUE_INFO;


/*!
 * Data specific Falcon debugging features.
 */
typedef struct
{
    NvU32           dumpEngineTag;        // NVDUMP_COMPONENT_ENG_xyz.
    NvU32           pbFalconId;           // Protobuf falcon ID.  RTOS_FLCN_xyz.
    NvU16           debugInfoDmemOffset;  // DMEM address of the Falcon's
                                          // DEBUG_INFO structure.
    NvBool          bCrashed;             // Falcon has crashed at least once
                                          // since RM was initialized.
    NvBool          bCallbackTriggered;   // Flag indicating that callback
                                          // was actually called.
} FLCN_DEBUG, *PFLCN_DEBUG;

struct FLCNABLE;

typedef struct ENGINE_DESCRIPTOR_TYPE
{
    NvU32   base;
    NvBool  initialized;
} ENGINE_DESCRIPTOR_TYPE, *PENGINE_DESCRIPTOR_TYPE;

typedef enum ENGINE_TAG
{
    ENG_TAG_INVALID,
    ENG_TAG_SOE,
    ENG_TAG_END_INVALID
} ENGINE_TAG, *PENGINE_TAG;

typedef struct FLCN
{
    // pointer to our function table - should always be the first thing in any object
    flcn_hal *pHal;

    // we don't have a parent class, so we go straight to our members
    const char             *name;

    ENGINE_DESCRIPTOR_TYPE engDescUc;
    ENGINE_DESCRIPTOR_TYPE engDescBc;

    FALCON_EXTERNAL_CONFIG extConfig;

    //
    // State variables
    //
    NvBool                bConstructed;

    /*! The FLCN is ready to accept work from the RM. */
    NvBool                 bOSReady;

    /*! This Falcon will have queue support */
    NvBool                bQueuesEnabled;
    NvU8                  numQueues;         //<! Number of queues constructed
    NvU32                 numSequences;      //<! Number of sequences constructed

    FLCN_DEBUG            debug;             //<! Data specific to debugging
    NvU8                  coreRev;           //<! Core revision.  0x51 is 5.1.
    NvU8                  securityModel;     //<! Follows _FALCON_HWCFG1_SECURITY_MODEL_xyz
    // Replacement for a PDB Property: PDB_PROP_FLCN_SUPPORTS_DMEM_APERTURES
    NvBool                supportsDmemApertures;

    // We need to save a pointer to the FLCNABLE interface
    struct FLCNABLE      *pFlcnable;

    ENGINE_TAG            engineTag;

    PFALCON_QUEUE_INFO    pQueueInfo;

    /*!
     * Determines whether to use EMEM in place of DMEM for RM queues and
     * the RM managed heap. EMEM is a memory region outside of the core engine
     * of some falcons which allows for RM access even when the falcon is
     * locked down in HS mode. This is required so that engines like SEC2
     * can receive new commands from RM without blocking.
     */
    NvBool                bEmemEnabled;

    /*! HW arch that is enabled and running on corresponding uproc engine. */
    NvU32                 engArch;

    /*! Flcn debug buffer object */

} FLCN, *PFLCN;

// hal functions

// OBJECT Interfaces
NV_STATUS flcnQueueReadData(struct nvswitch_device *, PFLCN, NvU32 queueId, void *pData, NvBool bMsg);
NV_STATUS flcnQueueCmdWrite(struct nvswitch_device *, PFLCN, NvU32 queueId, union RM_FLCN_CMD *pCmd, struct NVSWITCH_TIMEOUT *pTimeout);
NV_STATUS flcnQueueCmdCancel(struct nvswitch_device *, PFLCN, NvU32 seqDesc);
NV_STATUS flcnQueueCmdPostNonBlocking(struct nvswitch_device *, PFLCN, union RM_FLCN_CMD *pCmd, union RM_FLCN_MSG *pMsg, void *pPayload, NvU32 queueIdLogical, FlcnQMgrClientCallback pCallback, void *pCallbackParams, NvU32 *pSeqDesc, struct NVSWITCH_TIMEOUT *pTimeout);
NV_STATUS flcnQueueCmdPostBlocking(struct nvswitch_device *, PFLCN, union RM_FLCN_CMD *pCmd, union RM_FLCN_MSG *pMsg, void *pPayload, NvU32 queueIdLogical, NvU32 *pSeqDesc, struct NVSWITCH_TIMEOUT *pTimeout);
NV_STATUS flcnQueueCmdWait(struct nvswitch_device *, PFLCN, NvU32, struct NVSWITCH_TIMEOUT *pTimeout);
NvU8 flcnCoreRevisionGet(struct nvswitch_device *, PFLCN);
void flcnMarkNotReady(struct nvswitch_device *, PFLCN);
NV_STATUS flcnCmdQueueHeadGet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 *pHead);
NV_STATUS flcnMsgQueueHeadGet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 *pHead);
NV_STATUS flcnCmdQueueTailGet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 *pTail);
NV_STATUS flcnMsgQueueTailGet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 *pTail);
NV_STATUS flcnCmdQueueHeadSet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 head);
NV_STATUS flcnMsgQueueHeadSet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 head);
NV_STATUS flcnCmdQueueTailSet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 tail);
NV_STATUS flcnMsgQueueTailSet(struct nvswitch_device *, PFLCN, FLCNQUEUE *pQueue, NvU32 tail);
PFLCN_QMGR_SEQ_INFO flcnQueueSeqInfoFind(struct nvswitch_device *, PFLCN, NvU32 seqDesc);
PFLCN_QMGR_SEQ_INFO flcnQueueSeqInfoAcq(struct nvswitch_device *, PFLCN);
void flcnQueueSeqInfoRel(struct nvswitch_device *, PFLCN, PFLCN_QMGR_SEQ_INFO pSeqInfo);
void flcnQueueSeqInfoStateInit(struct nvswitch_device *, PFLCN);
void flcnQueueSeqInfoCancelAll(struct nvswitch_device *, PFLCN);
NV_STATUS flcnQueueSeqInfoFree(struct nvswitch_device *, PFLCN, PFLCN_QMGR_SEQ_INFO);
NV_STATUS flcnQueueEventRegister(struct nvswitch_device *, PFLCN, NvU32 unitId, NvU8 *pMsg, FlcnQMgrClientCallback pCallback, void *pParams, NvU32 *pEvtDesc);
NV_STATUS flcnQueueEventUnregister(struct nvswitch_device *, PFLCN, NvU32 evtDesc);
NV_STATUS flcnQueueEventHandle(struct nvswitch_device *, PFLCN, union RM_FLCN_MSG *pMsg, NV_STATUS evtStatus);
NV_STATUS flcnQueueResponseHandle(struct nvswitch_device *, PFLCN, union RM_FLCN_MSG *pMsg);
NvU32 flcnQueueCmdStatus(struct nvswitch_device *, PFLCN, NvU32 seqDesc);
NV_STATUS flcnDmemCopyFrom(struct nvswitch_device *, PFLCN, NvU32 src, NvU8 *pDst, NvU32 sizeBytes, NvU8 port);
NV_STATUS flcnDmemCopyTo(struct nvswitch_device *, PFLCN, NvU32 dst, NvU8 *pSrc, NvU32 sizeBytes, NvU8 port);
void flcnPostDiscoveryInit(struct nvswitch_device *, PFLCN);
void flcnDbgInfoDmemOffsetSet(struct nvswitch_device *, PFLCN, NvU16 debugInfoDmemOffset);

// HAL Interfaces
NV_STATUS   flcnConstruct_HAL                           (struct nvswitch_device *, PFLCN);
void        flcnDestruct_HAL                            (struct nvswitch_device *, PFLCN);
NvU32       flcnRegRead_HAL                             (struct nvswitch_device *, PFLCN, NvU32 offset);
void        flcnRegWrite_HAL                            (struct nvswitch_device *, PFLCN, NvU32 offset, NvU32 data);
const char *flcnGetName_HAL                             (struct nvswitch_device *, PFLCN);
NvU8        flcnReadCoreRev_HAL                         (struct nvswitch_device *, PFLCN);
void        flcnGetCoreInfo_HAL                         (struct nvswitch_device *, PFLCN);
NV_STATUS   flcnDmemTransfer_HAL                        (struct nvswitch_device *, PFLCN, NvU32 src, NvU8 *pDst, NvU32 sizeBytes, NvU8 port, NvBool bCopyFrom);
void        flcnIntrRetrigger_HAL                       (struct nvswitch_device *, PFLCN);
NvBool      flcnAreEngDescsInitialized_HAL              (struct nvswitch_device *, PFLCN);
NV_STATUS   flcnWaitForResetToFinish_HAL                (struct nvswitch_device *, PFLCN);
void        flcnDbgInfoCapturePcTrace_HAL               (struct nvswitch_device *, PFLCN);
void        flcnDbgInfoCaptureRiscvPcTrace_HAL          (struct nvswitch_device *, PFLCN);
NvU32       flcnDmemSize_HAL                            (struct nvswitch_device *, struct FLCN *);
NvU32       flcnSetImemAddr_HAL                         (struct nvswitch_device *, struct FLCN *, NvU32 dst);
void        flcnImemCopyTo_HAL                          (struct nvswitch_device *, struct FLCN *, NvU32 dst, NvU8 *pSrc, NvU32 sizeBytes, NvBool bSecure, NvU32 tag, NvU8 port);
NvU32       flcnSetDmemAddr_HAL                         (struct nvswitch_device *, struct FLCN *, NvU32 dst);
NvU32       flcnRiscvRegRead_HAL                        (struct nvswitch_device *, PFLCN, NvU32 offset);
void        flcnRiscvRegWrite_HAL                       (struct nvswitch_device *, PFLCN, NvU32 offset, NvU32 data);
NV_STATUS   flcnDebugBufferInit_HAL                     (struct nvswitch_device *, struct FLCN *, NvU32 debugBufferMaxSize, NvU32 writeRegAddr, NvU32 readRegAddr);
NV_STATUS   flcnDebugBufferDestroy_HAL                  (struct nvswitch_device *, struct FLCN *);
NV_STATUS   flcnDebugBufferDisplay_HAL                  (struct nvswitch_device *, struct FLCN *);
NvBool      flcnDebugBufferIsEmpty_HAL                  (struct nvswitch_device *, struct FLCN *);

// Falcon core revision / subversion definitions.
#define NV_FLCN_CORE_REV_3_0    0x30  // 3.0 - Core revision 3 subversion 0.
#define NV_FLCN_CORE_REV_4_0    0x40  // 4.0 - Core revision 4 subversion 0.
#define NV_FLCN_CORE_REV_4_1    0x41  // 4.1 - Core revision 4 subversion 1.
#define NV_FLCN_CORE_REV_5_0    0x50  // 5.0 - Core revision 5 subversion 0.
#define NV_FLCN_CORE_REV_5_1    0x51  // 5.1 - Core revision 5 subversion 1.
#define NV_FLCN_CORE_REV_6_0    0x60  // 6.0 - Core revision 6 subversion 0.

//
// Convert Falcon core rev/subver to the IP version format that can be recognized
// by the chip-config dynamic HAL infra.
//
#define NV_FLCN_CORE_REV_TO_IP_VER(coreVer)                       \
    (DRF_NUM(_PFALCON, _IP_VER, _MAJOR, ((coreVer >> 4) & 0xf)) | \
     DRF_NUM(_PFALCON, _IP_VER, _MINOR, (coreVer & 0xf)))

#define NV_PFALCON_IP_VER_MINOR                                                     23:16
#define NV_PFALCON_IP_VER_MAJOR                                                     31:24

// Some mailbox defines (should be shared with MSDEC OS)
#define NV_FALCON_MAILBOX0_MSDECOS_STATUS                   11:0
#define NV_FALCON_MAILBOX0_MSDECOS_INVALID_METHOD_MTHDCNT   19:12
#define NV_FALCON_MAILBOX0_MSDECOS_INVALID_METHOD_MTHDID    31:20
#define NV_FALCON_MAILBOX1_MSDECOS_INVALID_METHOD_MTHDDATA  31:0

PFLCN flcnAllocNew(void);
NvlStatus flcnInit(nvswitch_device *device, PFLCN pFlcn, NvU32 pci_device_id);
void flcnDestroy(nvswitch_device *device, FLCN *pFlcn);

/*!
 * The HW arch (e.g. FALCON or FALCON + RISCV) that can be actively enabled and
 * running on an uproc engine.
 */
#define NV_UPROC_ENGINE_ARCH_DEFAULT        (0x0)
#define NV_UPROC_ENGINE_ARCH_FALCON         (0x1)
#define NV_UPROC_ENGINE_ARCH_FALCON_RISCV   (0x2)

/*!
 * Hepler macro to check what HW arch is enabled and running on an uproc engine.
 */
#define UPROC_ENG_ARCH_FALCON(pFlcn)        (pFlcn->engArch == NV_UPROC_ENGINE_ARCH_FALCON)
#define UPROC_ENG_ARCH_FALCON_RISCV(pFlcn)  (pFlcn->engArch == NV_UPROC_ENGINE_ARCH_FALCON_RISCV)

// Falcon Register index
#define NV_FALCON_REG_R0                       (0)
#define NV_FALCON_REG_R1                       (1)
#define NV_FALCON_REG_R2                       (2)
#define NV_FALCON_REG_R3                       (3)
#define NV_FALCON_REG_R4                       (4)
#define NV_FALCON_REG_R5                       (5)
#define NV_FALCON_REG_R6                       (6)
#define NV_FALCON_REG_R7                       (7)
#define NV_FALCON_REG_R8                       (8)
#define NV_FALCON_REG_R9                       (9)
#define NV_FALCON_REG_R10                      (10)
#define NV_FALCON_REG_R11                      (11)
#define NV_FALCON_REG_R12                      (12)
#define NV_FALCON_REG_R13                      (13)
#define NV_FALCON_REG_R14                      (14)
#define NV_FALCON_REG_R15                      (15)
#define NV_FALCON_REG_IV0                      (16)
#define NV_FALCON_REG_IV1                      (17)
#define NV_FALCON_REG_UNDEFINED                (18)
#define NV_FALCON_REG_EV                       (19)
#define NV_FALCON_REG_SP                       (20)
#define NV_FALCON_REG_PC                       (21)
#define NV_FALCON_REG_IMB                      (22)
#define NV_FALCON_REG_DMB                      (23)
#define NV_FALCON_REG_CSW                      (24)
#define NV_FALCON_REG_CCR                      (25)
#define NV_FALCON_REG_SEC                      (26)
#define NV_FALCON_REG_CTX                      (27)
#define NV_FALCON_REG_EXCI                     (28)
#define NV_FALCON_REG_RSVD0                    (29)
#define NV_FALCON_REG_RSVD1                    (30)
#define NV_FALCON_REG_RSVD2                    (31)

#define NV_FALCON_REG_SIZE                     (32)

#define FALC_REG(x)                            NV_FALCON_REG_##x


#endif // _FLCN_NVSWITCH_H_

/*!
 * Defines the Falcon IMEM block-size (as a power-of-2).
 */
#define FALCON_IMEM_BLKSIZE2 (8)

/*!
 * Defines the Falcon DMEM block-size (as a power-of-2).
 */
#define FALCON_DMEM_BLKSIZE2 (8)

