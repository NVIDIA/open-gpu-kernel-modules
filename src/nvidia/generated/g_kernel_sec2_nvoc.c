#define NVOC_KERNEL_SEC2_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_sec2_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2f36c9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

void __nvoc_init_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelSec2(KernelSec2*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_dtor_KernelSec2(KernelSec2*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSec2;

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelSec2 = {
    /*pClassDef=*/          &__nvoc_class_def_KernelSec2,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelSec2,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_IntrService),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelCrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_KernelFalcon),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelSec2 = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_KernelSec2_KernelSec2,
        &__nvoc_rtti_KernelSec2_KernelFalcon,
        &__nvoc_rtti_KernelSec2_KernelCrashCatEngine,
        &__nvoc_rtti_KernelSec2_CrashCatEngine,
        &__nvoc_rtti_KernelSec2_IntrService,
        &__nvoc_rtti_KernelSec2_OBJENGSTATE,
        &__nvoc_rtti_KernelSec2_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2 = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelSec2),
        /*classId=*/            classId(KernelSec2),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelSec2",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelSec2,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelSec2,
    /*pExportInfo=*/        &__nvoc_export_info_KernelSec2
};

// 5 down-thunk(s) defined to bridge methods in KernelSec2 from superclasses

// ksec2ConstructEngine: virtual halified (singleton optimized) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_KernelSec2_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelSec2, ENGDESCRIPTOR arg3) {
    return ksec2ConstructEngine(pGpu, (struct KernelSec2 *)(((unsigned char *) pKernelSec2) - __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg3);
}

// ksec2RegisterIntrService: virtual halified (singleton optimized) override (intrserv) base (intrserv) body
static void __nvoc_down_thunk_KernelSec2_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelSec2, IntrServiceRecord pRecords[175]) {
    ksec2RegisterIntrService(pGpu, (struct KernelSec2 *)(((unsigned char *) pKernelSec2) - __nvoc_rtti_KernelSec2_IntrService.offset), pRecords);
}

// ksec2ServiceNotificationInterrupt: virtual halified (singleton optimized) override (intrserv) base (intrserv)
static NV_STATUS __nvoc_down_thunk_KernelSec2_intrservServiceNotificationInterrupt(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return ksec2ServiceNotificationInterrupt(arg1, (struct KernelSec2 *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelSec2_IntrService.offset), arg3);
}

// ksec2ResetHw: virtual halified (2 hals) override (kflcn) base (kflcn) body
static NV_STATUS __nvoc_down_thunk_KernelSec2_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelSec2) {
    return ksec2ResetHw(pGpu, (struct KernelSec2 *)(((unsigned char *) pKernelSec2) - __nvoc_rtti_KernelSec2_KernelFalcon.offset));
}

// ksec2StateLoad: virtual halified (3 hals) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_KernelSec2_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelSec2, NvU32 arg3) {
    return ksec2StateLoad(pGpu, (struct KernelSec2 *)(((unsigned char *) pKernelSec2) - __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg3);
}


// 29 up-thunk(s) defined to bridge methods in KernelSec2 to superclasses

// ksec2InitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_ksec2InitMissing(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

// ksec2StatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StatePreInitLocked(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

// ksec2StatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

// ksec2StateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StateInitLocked(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

// ksec2StateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StateInitUnlocked(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

// ksec2StatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StatePreLoad(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg3);
}

// ksec2StatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StatePostLoad(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg3);
}

// ksec2StatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StatePreUnload(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg3);
}

// ksec2StateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StateUnload(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg3);
}

// ksec2StatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_ksec2StatePostUnload(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg3);
}

// ksec2StateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_ksec2StateDestroy(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

// ksec2IsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_ksec2IsPresent(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

// ksec2ClearInterrupt: virtual inherited (intrserv) base (intrserv)
static NvBool __nvoc_up_thunk_IntrService_ksec2ClearInterrupt(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_KernelSec2_IntrService.offset), pParams);
}

// ksec2ServiceInterrupt: virtual inherited (intrserv) base (intrserv)
static NvU32 __nvoc_up_thunk_IntrService_ksec2ServiceInterrupt(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_KernelSec2_IntrService.offset), pParams);
}

// ksec2RegRead: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
static NvU32 __nvoc_up_thunk_KernelFalcon_ksec2RegRead(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + __nvoc_rtti_KernelSec2_KernelFalcon.offset), offset);
}

// ksec2RegWrite: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
static void __nvoc_up_thunk_KernelFalcon_ksec2RegWrite(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + __nvoc_rtti_KernelSec2_KernelFalcon.offset), offset, data);
}

// ksec2MaskDmemAddr: virtual halified (3 hals) inherited (kflcn) base (kflcn) body
static NvU32 __nvoc_up_thunk_KernelFalcon_ksec2MaskDmemAddr(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + __nvoc_rtti_KernelSec2_KernelFalcon.offset), addr);
}

// ksec2Configured: virtual inherited (kcrashcatEngine) base (kflcn)
static NvBool __nvoc_up_thunk_KernelCrashCatEngine_ksec2Configured(struct KernelSec2 *arg_this) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset));
}

// ksec2Unload: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_ksec2Unload(struct KernelSec2 *arg_this) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset));
}

// ksec2Vprintf: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_ksec2Vprintf(struct KernelSec2 *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), bReportStart, fmt, args);
}

// ksec2PriRead: virtual inherited (kcrashcatEngine) base (kflcn)
static NvU32 __nvoc_up_thunk_KernelCrashCatEngine_ksec2PriRead(struct KernelSec2 *arg_this, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset);
}

// ksec2PriWrite: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_ksec2PriWrite(struct KernelSec2 *arg_this, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset, data);
}

// ksec2MapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
static void * __nvoc_up_thunk_KernelCrashCatEngine_ksec2MapBufferDescriptor(struct KernelSec2 *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), pBufDesc);
}

// ksec2UnmapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_ksec2UnmapBufferDescriptor(struct KernelSec2 *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), pBufDesc);
}

// ksec2SyncBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_ksec2SyncBufferDescriptor(struct KernelSec2 *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), pBufDesc, offset, size);
}

// ksec2ReadDmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_ksec2ReadDmem(struct KernelSec2 *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset, size, pBuf);
}

// ksec2ReadEmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_ksec2ReadEmem(struct KernelSec2 *arg_this, NvU64 offset, NvU64 size, void *pBuf) {
    kcrashcatEngineReadEmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset, size, pBuf);
}

// ksec2GetScratchOffsets: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
static const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_ksec2GetScratchOffsets(struct KernelSec2 *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), scratchGroupId);
}

// ksec2GetWFL0Offset: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
static NvU32 __nvoc_up_thunk_KernelCrashCatEngine_ksec2GetWFL0Offset(struct KernelSec2 *arg_this) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSec2 = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_KernelSec2(KernelSec2 *pThis) {
    __nvoc_ksec2Destruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_IntrService;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_KernelFalcon;
    __nvoc_init_dataField_KernelSec2(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelSec2_exit; // Success

__nvoc_ctor_KernelSec2_fail_KernelFalcon:
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
__nvoc_ctor_KernelSec2_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelSec2_fail_OBJENGSTATE:
__nvoc_ctor_KernelSec2_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelSec2_1(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // ksec2ConstructEngine -- virtual halified (singleton optimized) override (engstate) base (engstate) body
    pThis->__ksec2ConstructEngine__ = &ksec2ConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelSec2_engstateConstructEngine;

    // ksec2RegisterIntrService -- virtual halified (singleton optimized) override (intrserv) base (intrserv) body
    pThis->__ksec2RegisterIntrService__ = &ksec2RegisterIntrService_IMPL;
    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelSec2_intrservRegisterIntrService;

    // ksec2ServiceNotificationInterrupt -- virtual halified (singleton optimized) override (intrserv) base (intrserv)
    pThis->__ksec2ServiceNotificationInterrupt__ = &ksec2ServiceNotificationInterrupt_IMPL;
    pThis->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_down_thunk_KernelSec2_intrservServiceNotificationInterrupt;

    // ksec2ConfigureFalcon -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_TU102;
    }
    else
    {
        pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_GA102;
    }

    // ksec2ResetHw -- virtual halified (2 hals) override (kflcn) base (kflcn) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksec2ResetHw__ = &ksec2ResetHw_5baef9;
    }
    else
    {
        pThis->__ksec2ResetHw__ = &ksec2ResetHw_TU102;
    }
    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_down_thunk_KernelSec2_kflcnResetHw;

    // ksec2StateLoad -- virtual halified (3 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksec2StateLoad__ = &ksec2StateLoad_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__ksec2StateLoad__ = &ksec2StateLoad_GH100;
        }
        // default
        else
        {
            pThis->__ksec2StateLoad__ = &ksec2StateLoad_56cd7a;
        }
    }
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelSec2_engstateStateLoad;

    // ksec2ReadUcodeFuseVersion -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksec2ReadUcodeFuseVersion__ = &ksec2ReadUcodeFuseVersion_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__ksec2ReadUcodeFuseVersion__ = &ksec2ReadUcodeFuseVersion_b2b553;
        }
        else
        {
            pThis->__ksec2ReadUcodeFuseVersion__ = &ksec2ReadUcodeFuseVersion_GA100;
        }
    }

    // ksec2GetBinArchiveBlUcode -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksec2GetBinArchiveBlUcode__ = &ksec2GetBinArchiveBlUcode_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
        {
            pThis->__ksec2GetBinArchiveBlUcode__ = &ksec2GetBinArchiveBlUcode_80f438;
        }
        else
        {
            pThis->__ksec2GetBinArchiveBlUcode__ = &ksec2GetBinArchiveBlUcode_TU102;
        }
    }

    // ksec2GetGenericBlUcode -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksec2GetGenericBlUcode__ = &ksec2GetGenericBlUcode_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
        {
            pThis->__ksec2GetGenericBlUcode__ = &ksec2GetGenericBlUcode_5baef9;
        }
        else
        {
            pThis->__ksec2GetGenericBlUcode__ = &ksec2GetGenericBlUcode_TU102;
        }
    }

    // ksec2GetBinArchiveSecurescrubUcode -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksec2GetBinArchiveSecurescrubUcode__ = &ksec2GetBinArchiveSecurescrubUcode_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__ksec2GetBinArchiveSecurescrubUcode__ = &ksec2GetBinArchiveSecurescrubUcode_AD10X;
        }
        else
        {
            pThis->__ksec2GetBinArchiveSecurescrubUcode__ = &ksec2GetBinArchiveSecurescrubUcode_80f438;
        }
    }

    // ksec2InitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2InitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2InitMissing;

    // ksec2StatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StatePreInitLocked;

    // ksec2StatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StatePreInitUnlocked;

    // ksec2StateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StateInitLocked;

    // ksec2StateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StateInitUnlocked;

    // ksec2StatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StatePreLoad;

    // ksec2StatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StatePostLoad;

    // ksec2StatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StatePreUnload;

    // ksec2StateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StateUnload;

    // ksec2StatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StatePostUnload;

    // ksec2StateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2StateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2StateDestroy;

    // ksec2IsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__ksec2IsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_ksec2IsPresent;

    // ksec2ClearInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__ksec2ClearInterrupt__ = &__nvoc_up_thunk_IntrService_ksec2ClearInterrupt;

    // ksec2ServiceInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__ksec2ServiceInterrupt__ = &__nvoc_up_thunk_IntrService_ksec2ServiceInterrupt;

    // ksec2RegRead -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__ksec2RegRead__ = &__nvoc_up_thunk_KernelFalcon_ksec2RegRead;

    // ksec2RegWrite -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__ksec2RegWrite__ = &__nvoc_up_thunk_KernelFalcon_ksec2RegWrite;

    // ksec2MaskDmemAddr -- virtual halified (3 hals) inherited (kflcn) base (kflcn) body
    pThis->__ksec2MaskDmemAddr__ = &__nvoc_up_thunk_KernelFalcon_ksec2MaskDmemAddr;

    // ksec2Configured -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2Configured__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2Configured;

    // ksec2Unload -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2Unload__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2Unload;

    // ksec2Vprintf -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2Vprintf__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2Vprintf;

    // ksec2PriRead -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2PriRead__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2PriRead;

    // ksec2PriWrite -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2PriWrite__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2PriWrite;

    // ksec2MapBufferDescriptor -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2MapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2MapBufferDescriptor;

    // ksec2UnmapBufferDescriptor -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2UnmapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2UnmapBufferDescriptor;

    // ksec2SyncBufferDescriptor -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2SyncBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2SyncBufferDescriptor;

    // ksec2ReadDmem -- virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2ReadDmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2ReadDmem;

    // ksec2ReadEmem -- virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2ReadEmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2ReadEmem;

    // ksec2GetScratchOffsets -- virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2GetScratchOffsets__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2GetScratchOffsets;

    // ksec2GetWFL0Offset -- virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    pThis->__ksec2GetWFL0Offset__ = &__nvoc_up_thunk_KernelCrashCatEngine_ksec2GetWFL0Offset;
} // End __nvoc_init_funcTable_KernelSec2_1 with approximately 61 basic block(s).


// Initialize vtable(s) for 39 virtual method(s).
void __nvoc_init_funcTable_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 39 per-object function pointer(s).
    __nvoc_init_funcTable_KernelSec2_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelSec2 = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_funcTable_KernelSec2(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelSec2(KernelSec2 **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelSec2 *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelSec2), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelSec2));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelSec2);

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_KernelSec2(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelSec2(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelSec2_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelSec2_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelSec2));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelSec2(KernelSec2 **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelSec2(ppThis, pParent, createFlags);

    return status;
}

