#define NVOC_KERNEL_GSP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_gsp_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x311d4e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

void __nvoc_init_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelGsp(KernelGsp*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_dtor_KernelGsp(KernelGsp*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsp;

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelGsp = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGsp,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGsp,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelCrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGsp = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGsp_KernelGsp,
        &__nvoc_rtti_KernelGsp_KernelFalcon,
        &__nvoc_rtti_KernelGsp_KernelCrashCatEngine,
        &__nvoc_rtti_KernelGsp_CrashCatEngine,
        &__nvoc_rtti_KernelGsp_IntrService,
        &__nvoc_rtti_KernelGsp_OBJENGSTATE,
        &__nvoc_rtti_KernelGsp_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGsp),
        /*classId=*/            classId(KernelGsp),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGsp",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGsp,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGsp,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGsp
};

// Down-thunk(s) to bridge KernelGsp methods from ancestors (if any)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // super^2
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data);    // super^2
void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // super^2
const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // super^2
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this);    // super^2
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset);    // super
void __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data);    // super
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr);    // super
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp);    // this
void __nvoc_down_thunk_KernelGsp_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceRecord pRecords[177]);    // this
NvU32 __nvoc_down_thunk_KernelGsp_intrservServiceInterrupt(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceServiceInterruptArguments *pParams);    // this
NV_STATUS __nvoc_down_thunk_KernelGsp_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp);    // this
void __nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem(struct KernelCrashCatEngine *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf);    // this

// 6 down-thunk(s) defined to bridge methods in KernelGsp from superclasses

// kgspConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp, ENGDESCRIPTOR arg3) {
    return kgspConstructEngine(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp) {
    return kgspStateInitLocked(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_KernelGsp_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceRecord pRecords[177]) {
    kgspRegisterIntrService(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService)), pRecords);
}

// kgspServiceInterrupt: virtual override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_KernelGsp_intrservServiceInterrupt(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceServiceInterruptArguments *pParams) {
    return kgspServiceInterrupt(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService)), pParams);
}

// kgspResetHw: virtual halified (4 hals) override (kflcn) base (kflcn) body
NV_STATUS __nvoc_down_thunk_KernelGsp_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp) {
    return kgspResetHw(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon)));
}

// kgspReadEmem: virtual halified (2 hals) override (kcrashcatEngine) base (kflcn) body
void __nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem(struct KernelCrashCatEngine *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf) {
    kgspReadEmem((struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset, size, pBuf);
}


// Up-thunk(s) to bridge KernelGsp methods to ancestors (if any)
NvBool __nvoc_up_thunk_KernelCrashCatEngine_kflcnConfigured(struct KernelFalcon *arg_this);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnload(struct KernelFalcon *arg_this);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnVprintf(struct KernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // super
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriRead(struct KernelFalcon *arg_this, NvU32 offset);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriWrite(struct KernelFalcon *arg_this, NvU32 offset, NvU32 data);    // super
void * __nvoc_up_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadDmem(struct KernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadEmem(struct KernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf);    // super
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets(struct KernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // super
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset(struct KernelFalcon *arg_this);    // super
void __nvoc_up_thunk_OBJENGSTATE_kgspInitMissing(struct OBJGPU *pGpu, struct KernelGsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePostLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePostUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kgspStateDestroy(struct OBJGPU *pGpu, struct KernelGsp *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kgspIsPresent(struct OBJGPU *pGpu, struct KernelGsp *pEngstate);    // this
NvBool __nvoc_up_thunk_IntrService_kgspClearInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceClearInterruptArguments *pParams);    // this
NV_STATUS __nvoc_up_thunk_IntrService_kgspServiceNotificationInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams);    // this
NvU32 __nvoc_up_thunk_KernelFalcon_kgspRegRead(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset);    // this
void __nvoc_up_thunk_KernelFalcon_kgspRegWrite(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset, NvU32 data);    // this
NvU32 __nvoc_up_thunk_KernelFalcon_kgspMaskDmemAddr(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 addr);    // this
NvBool __nvoc_up_thunk_KernelCrashCatEngine_kgspConfigured(struct KernelGsp *arg_this);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kgspUnload(struct KernelGsp *arg_this);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kgspVprintf(struct KernelGsp *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // this
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kgspPriRead(struct KernelGsp *arg_this, NvU32 offset);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kgspPriWrite(struct KernelGsp *arg_this, NvU32 offset, NvU32 data);    // this
void * __nvoc_up_thunk_KernelCrashCatEngine_kgspMapBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kgspUnmapBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kgspSyncBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kgspReadDmem(struct KernelGsp *arg_this, NvU32 offset, NvU32 size, void *pBuf);    // this
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_kgspGetScratchOffsets(struct KernelGsp *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // this
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kgspGetWFL0Offset(struct KernelGsp *arg_this);    // this

// 28 up-thunk(s) defined to bridge methods in KernelGsp to superclasses

// kgspInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgspInitMissing(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePostLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePostUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgspStateDestroy(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kgspIsPresent(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspClearInterrupt: virtual inherited (intrserv) base (intrserv)
NvBool __nvoc_up_thunk_IntrService_kgspClearInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService)), pParams);
}

// kgspServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
NV_STATUS __nvoc_up_thunk_IntrService_kgspServiceNotificationInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService)), pParams);
}

// kgspRegRead: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
NvU32 __nvoc_up_thunk_KernelFalcon_kgspRegRead(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon)), offset);
}

// kgspRegWrite: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
void __nvoc_up_thunk_KernelFalcon_kgspRegWrite(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon)), offset, data);
}

// kgspMaskDmemAddr: virtual halified (3 hals) inherited (kflcn) base (kflcn) body
NvU32 __nvoc_up_thunk_KernelFalcon_kgspMaskDmemAddr(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon)), addr);
}

// kgspConfigured: virtual inherited (kcrashcatEngine) base (kflcn)
NvBool __nvoc_up_thunk_KernelCrashCatEngine_kgspConfigured(struct KernelGsp *arg_this) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)));
}

// kgspUnload: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_kgspUnload(struct KernelGsp *arg_this) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)));
}

// kgspVprintf: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_kgspVprintf(struct KernelGsp *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), bReportStart, fmt, args);
}

// kgspPriRead: virtual inherited (kcrashcatEngine) base (kflcn)
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kgspPriRead(struct KernelGsp *arg_this, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset);
}

// kgspPriWrite: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_kgspPriWrite(struct KernelGsp *arg_this, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset, data);
}

// kgspMapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
void * __nvoc_up_thunk_KernelCrashCatEngine_kgspMapBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), pBufDesc);
}

// kgspUnmapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_kgspUnmapBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), pBufDesc);
}

// kgspSyncBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_kgspSyncBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), pBufDesc, offset, size);
}

// kgspReadDmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_kgspReadDmem(struct KernelGsp *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset, size, pBuf);
}

// kgspGetScratchOffsets: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_kgspGetScratchOffsets(struct KernelGsp *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), scratchGroupId);
}

// kgspGetWFL0Offset: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kgspGetWFL0Offset(struct KernelGsp *arg_this) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsp = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_KernelGsp(KernelGsp *pThis) {
    __nvoc_kgspDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // Hal field -- bPartitionedFmc
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bPartitionedFmc = NV_TRUE;
    }
    // default
    else
    {
        pThis->bPartitionedFmc = NV_FALSE;
    }

    // Hal field -- bScrubberUcodeSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bScrubberUcodeSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bScrubberUcodeSupported = NV_FALSE;
    }

    // Hal field -- fwHeapParamBaseSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->fwHeapParamBaseSize = (8 << 20);
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->fwHeapParamBaseSize = (14 << 20);
    }

    // Hal field -- bBootGspRmWithBoostClocks
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bBootGspRmWithBoostClocks = NV_TRUE;
    }
    // default
    else
    {
        pThis->bBootGspRmWithBoostClocks = NV_FALSE;
    }

    // Hal field -- ememPort
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->ememPort = 6;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->ememPort = 2;
    }
    // default
    else
    {
        pThis->ememPort = 0;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_IntrService;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_KernelFalcon;
    __nvoc_init_dataField_KernelGsp(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelGsp_exit; // Success

__nvoc_ctor_KernelGsp_fail_KernelFalcon:
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
__nvoc_ctor_KernelGsp_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelGsp_fail_OBJENGSTATE:
__nvoc_ctor_KernelGsp_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelGsp_1(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kgspConfigureFalcon -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_TU102;
        }
        else
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_GA102;
        }
    }

    // kgspIsDebugModeEnabled -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_86b752;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_TU102;
        }
        else
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_GA100;
        }
    }

    // kgspAllocBootArgs -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_TU102;
        }
        else
        {
            pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_GH100;
        }
    }

    // kgspFreeBootArgs -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_TU102;
        }
        else
        {
            pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_GH100;
        }
    }

    // kgspProgramLibosBootArgsAddr -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspProgramLibosBootArgsAddr__ = &kgspProgramLibosBootArgsAddr_f2d351;
    }
    else
    {
        pThis->__kgspProgramLibosBootArgsAddr__ = &kgspProgramLibosBootArgsAddr_TU102;
    }

    // kgspSetCmdQueueHead -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspSetCmdQueueHead__ = &kgspSetCmdQueueHead_5baef9;
    }
    else
    {
        pThis->__kgspSetCmdQueueHead__ = &kgspSetCmdQueueHead_TU102;
    }

    // kgspPrepareForBootstrap -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_TU102;
        }
        else
        {
            pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_GH100;
        }
    }

    // kgspBootstrap -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspBootstrap__ = &kgspBootstrap_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspBootstrap__ = &kgspBootstrap_TU102;
        }
        else
        {
            pThis->__kgspBootstrap__ = &kgspBootstrap_GH100;
        }
    }

    // kgspTeardown -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspTeardown__ = &kgspTeardown_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspTeardown__ = &kgspTeardown_TU102;
        }
        else
        {
            pThis->__kgspTeardown__ = &kgspTeardown_GH100;
        }
    }

    // kgspGetGspRmBootUcodeStorage -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GA102;
        }
        else
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GB100;
        }
    }

    // kgspGetBinArchiveGspRmBoot -- halified (8 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_AD102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GB202;
        }
        else
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GB100;
        }
    }

    // kgspGetBinArchiveConcatenatedFMCDesc -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_80f438;
        }
    }

    // kgspGetBinArchiveConcatenatedFMC -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_80f438;
        }
    }

    // kgspGetBinArchiveGspRmFmcGfwDebugSigned -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_80f438;
        }
    }

    // kgspGetBinArchiveGspRmFmcGfwProdSigned -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_80f438;
        }
    }

    // kgspGetBinArchiveGspRmCcFmcGfwProdSigned -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_80f438;
        }
    }

    // kgspCalculateFbLayout -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_TU102;
        }
        else
        {
            pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_GH100;
        }
    }

    // kgspGetNonWprHeapSize -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_ad951d;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_ed6b8b;
        }
        else
        {
            pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_d505ea;
        }
    }

    // kgspExecuteSequencerCommand -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_TU102;
        }
        else
        {
            pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_GA102;
        }
    }

    // kgspReadUcodeFuseVersion -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_b2b553;
        }
        else
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_GA100;
        }
    }

    // kgspResetHw -- virtual halified (4 hals) override (kflcn) base (kflcn) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspResetHw__ = &kgspResetHw_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspResetHw__ = &kgspResetHw_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspResetHw__ = &kgspResetHw_TU102;
        }
        else
        {
            pThis->__kgspResetHw__ = &kgspResetHw_GB100;
        }
    }

    // kgspHealthCheck -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspHealthCheck__ = &kgspHealthCheck_86b752;
    }
    else
    {
        pThis->__kgspHealthCheck__ = &kgspHealthCheck_TU102;
    }

    // kgspService -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspService__ = &kgspService_474d46;
    }
    else
    {
        pThis->__kgspService__ = &kgspService_TU102;
    }

    // kgspServiceFatalHwError -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspServiceFatalHwError__ = &kgspServiceFatalHwError_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspServiceFatalHwError__ = &kgspServiceFatalHwError_GB100;
        }
        // default
        else
        {
            pThis->__kgspServiceFatalHwError__ = &kgspServiceFatalHwError_b3696a;
        }
    }

    // kgspEccServiceEvent -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspEccServiceEvent__ = &kgspEccServiceEvent_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspEccServiceEvent__ = &kgspEccServiceEvent_GB100;
        }
        // default
        else
        {
            pThis->__kgspEccServiceEvent__ = &kgspEccServiceEvent_d44104;
        }
    }

    // kgspEccServiceUncorrError -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspEccServiceUncorrError__ = &kgspEccServiceUncorrError_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspEccServiceUncorrError__ = &kgspEccServiceUncorrError_GB100;
        }
        // default
        else
        {
            pThis->__kgspEccServiceUncorrError__ = &kgspEccServiceUncorrError_d44104;
        }
    }

    // kgspIsWpr2Up -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_86b752;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_TU102;
        }
        else
        {
            pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_GH100;
        }
    }

    // kgspGetFrtsSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_4a4dee;
        }
        else
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_TU102;
        }
    }

    // kgspGetPrescrubbedTopFbSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_e1e623;
        }
        else
        {
            pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_604eb7;
        }
    }

    // kgspExtractVbiosFromRom -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_TU102;
        }
        else
        {
            pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_395e98;
        }
    }

    // kgspPrepareForFwsecFrts -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareForFwsecFrts__ = &kgspPrepareForFwsecFrts_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspPrepareForFwsecFrts__ = &kgspPrepareForFwsecFrts_TU102;
        }
        else
        {
            pThis->__kgspPrepareForFwsecFrts__ = &kgspPrepareForFwsecFrts_5baef9;
        }
    }

    // kgspPrepareForFwsecSb -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareForFwsecSb__ = &kgspPrepareForFwsecSb_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspPrepareForFwsecSb__ = &kgspPrepareForFwsecSb_TU102;
        }
        else
        {
            pThis->__kgspPrepareForFwsecSb__ = &kgspPrepareForFwsecSb_395e98;
        }
    }

    // kgspExecuteFwsec -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteFwsec__ = &kgspExecuteFwsec_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteFwsec__ = &kgspExecuteFwsec_TU102;
        }
        else
        {
            pThis->__kgspExecuteFwsec__ = &kgspExecuteFwsec_5baef9;
        }
    }

    // kgspExecuteScrubberIfNeeded -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_AD102;
        }
        else
        {
            pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_5baef9;
        }
    }

    // kgspExecuteBooterLoad -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_TU102;
        }
        else
        {
            pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_5baef9;
        }
    }

    // kgspExecuteBooterUnloadIfNeeded -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_TU102;
        }
        else
        {
            pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_5baef9;
        }
    }

    // kgspExecuteHsFalcon -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_GA102;
        }
        else
        {
            pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_5baef9;
        }
    }

    // kgspWaitForProcessorSuspend -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspWaitForProcessorSuspend__ = &kgspWaitForProcessorSuspend_5baef9;
    }
    else
    {
        pThis->__kgspWaitForProcessorSuspend__ = &kgspWaitForProcessorSuspend_TU102;
    }

    // kgspPrepareSuspendResumeData -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareSuspendResumeData__ = &kgspPrepareSuspendResumeData_5baef9;
    }
    else
    {
        pThis->__kgspPrepareSuspendResumeData__ = &kgspPrepareSuspendResumeData_TU102;
    }

    // kgspFreeSuspendResumeData -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspFreeSuspendResumeData__ = &kgspFreeSuspendResumeData_f2d351;
    }
    else
    {
        pThis->__kgspFreeSuspendResumeData__ = &kgspFreeSuspendResumeData_TU102;
    }

    // kgspWaitForGfwBootOk -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_TU102;
        }
        else
        {
            pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_GH100;
        }
    }

    // kgspGetBinArchiveBooterLoadUcode -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU116;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_AD102;
        }
        else
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_80f438;
        }
    }

    // kgspGetBinArchiveBooterUnloadUcode -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU116;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_AD102;
        }
        else
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_80f438;
        }
    }

    // kgspGetMinWprHeapSizeMB -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_b2b553;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_7185bf;
        }
        else
        {
            pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_647ce6;
        }
    }

    // kgspGetMaxWprHeapSizeMB -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_b2b553;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_ad4e6a;
        }
        else
        {
            pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_e7e092;
        }
    }

    // kgspGetFwHeapParamOsCarveoutSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_b2b553;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_397f70;
        }
        else
        {
            pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_4b5307;
        }
    }

    // kgspInitVgpuPartitionLogging -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_395e98;
        }
        else
        {
            pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_IMPL;
        }
    }

    // kgspPreserveVgpuPartitionLogging -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPreserveVgpuPartitionLogging__ = &kgspPreserveVgpuPartitionLogging_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspPreserveVgpuPartitionLogging__ = &kgspPreserveVgpuPartitionLogging_395e98;
        }
        else
        {
            pThis->__kgspPreserveVgpuPartitionLogging__ = &kgspPreserveVgpuPartitionLogging_IMPL;
        }
    }

    // kgspFreeVgpuPartitionLogging -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_395e98;
        }
        else
        {
            pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_IMPL;
        }
    }

    // kgspGetSignatureSectionNamePrefix -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_789efb;
        }
    }

    // kgspSetupGspFmcArgs -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_GH100;
        }
        // default
        else
        {
            pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_5baef9;
        }
    }

    // kgspReadEmem -- virtual halified (2 hals) override (kcrashcatEngine) base (kflcn) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspReadEmem__ = &kgspReadEmem_366c4c;
    }
    else
    {
        pThis->__kgspReadEmem__ = &kgspReadEmem_TU102;
    }

    // kgspGetCrashcatSysmemBufferSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kgspGetCrashcatSysmemBufferSize__ = &kgspGetCrashcatSysmemBufferSize_b845ef;
    }
    // default
    else
    {
        pThis->__kgspGetCrashcatSysmemBufferSize__ = &kgspGetCrashcatSysmemBufferSize_4a4dee;
    }

    // kgspIssueNotifyOp -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspIssueNotifyOp__ = &kgspIssueNotifyOp_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspIssueNotifyOp__ = &kgspIssueNotifyOp_GH100;
        }
        // default
        else
        {
            pThis->__kgspIssueNotifyOp__ = &kgspIssueNotifyOp_5baef9;
        }
    }

    // kgspCheckGspRmCcCleanup -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspCheckGspRmCcCleanup__ = &kgspCheckGspRmCcCleanup_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B */ 
        {
            pThis->__kgspCheckGspRmCcCleanup__ = &kgspCheckGspRmCcCleanup_GH100;
        }
        // default
        else
        {
            pThis->__kgspCheckGspRmCcCleanup__ = &kgspCheckGspRmCcCleanup_5baef9;
        }
    }

    // kgspRegRead -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__kgspRegRead__ = &__nvoc_up_thunk_KernelFalcon_kgspRegRead;

    // kgspRegWrite -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__kgspRegWrite__ = &__nvoc_up_thunk_KernelFalcon_kgspRegWrite;

    // kgspMaskDmemAddr -- virtual halified (3 hals) inherited (kflcn) base (kflcn) body
    pThis->__kgspMaskDmemAddr__ = &__nvoc_up_thunk_KernelFalcon_kgspMaskDmemAddr;
} // End __nvoc_init_funcTable_KernelGsp_1 with approximately 189 basic block(s).


// Initialize vtable(s) for 87 virtual method(s).
void __nvoc_init_funcTable_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelGsp vtable = {
        .__kgspConstructEngine__ = &kgspConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGsp_engstateConstructEngine,    // virtual
        .__kgspStateInitLocked__ = &kgspStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelGsp_engstateStateInitLocked,    // virtual
        .__kgspRegisterIntrService__ = &kgspRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelGsp_intrservRegisterIntrService,    // virtual
        .__kgspServiceInterrupt__ = &kgspServiceInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelGsp_intrservServiceInterrupt,    // virtual
        .KernelFalcon.__kflcnResetHw__ = &__nvoc_down_thunk_KernelGsp_kflcnResetHw,    // pure virtual
        .KernelFalcon.__kflcnReadEmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnReadEmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineReadEmem__ = &__nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem,    // virtual halified (singleton optimized)
        .__kgspInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgspInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kgspStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kgspStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kgspStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kgspStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kgspStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__kgspStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kgspStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kgspStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__kgspStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kgspStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__kgspIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kgspIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
        .__kgspClearInterrupt__ = &__nvoc_up_thunk_IntrService_kgspClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
        .__kgspServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_kgspServiceNotificationInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineRegRead__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead,    // pure virtual
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineRegWrite__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite,    // pure virtual
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineMaskDmemAddr__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr,    // pure virtual
        .__kgspConfigured__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspConfigured,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnConfigured__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnConfigured,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineConfigured__ = &kcrashcatEngineConfigured_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
        .__kgspUnload__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspUnload,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnUnload__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnUnload,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineUnload__ = &kcrashcatEngineUnload_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
        .__kgspVprintf__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspVprintf,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnVprintf__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnVprintf,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineVprintf__ = &kcrashcatEngineVprintf_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
        .__kgspPriRead__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspPriRead,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnPriRead__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnPriRead,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEnginePriRead__ = &kcrashcatEnginePriRead_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
        .__kgspPriWrite__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspPriWrite,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnPriWrite__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnPriWrite,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEnginePriWrite__ = &kcrashcatEnginePriWrite_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
        .__kgspMapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspMapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnMapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineMapBufferDescriptor__ = &kcrashcatEngineMapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
        .__kgspUnmapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspUnmapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnUnmapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineUnmapBufferDescriptor__ = &kcrashcatEngineUnmapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
        .__kgspSyncBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspSyncBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnSyncBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineSyncBufferDescriptor__ = &kcrashcatEngineSyncBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
        .__kgspReadDmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspReadDmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnReadDmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnReadDmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineReadDmem__ = &kcrashcatEngineReadDmem_TU102,    // virtual halified (singleton optimized)
        .__kgspGetScratchOffsets__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspGetScratchOffsets,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnGetScratchOffsets__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineGetScratchOffsets__ = &kcrashcatEngineGetScratchOffsets_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
        .__kgspGetWFL0Offset__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspGetWFL0Offset,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
        .KernelFalcon.__kflcnGetWFL0Offset__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.__kcrashcatEngineGetWFL0Offset__ = &kcrashcatEngineGetWFL0Offset_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
        .KernelFalcon.KernelCrashCatEngine.CrashCatEngine.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_vtable = &vtable.IntrService;    // (intrserv) super
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine.__nvoc_vtable = &vtable.KernelFalcon.KernelCrashCatEngine.CrashCatEngine;    // (crashcatEngine) super^3
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_vtable = &vtable.KernelFalcon.KernelCrashCatEngine;    // (kcrashcatEngine) super^2
    pThis->__nvoc_base_KernelFalcon.__nvoc_vtable = &vtable.KernelFalcon;    // (kflcn) super
    pThis->__nvoc_vtable = &vtable;    // (kgsp) this

    // Initialize vtable(s) with 58 per-object function pointer(s).
    __nvoc_init_funcTable_KernelGsp_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelGsp = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_funcTable_KernelGsp(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGsp *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGsp), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGsp));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGsp);

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

    __nvoc_init_KernelGsp(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGsp(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGsp_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGsp_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGsp));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGsp(ppThis, pParent, createFlags);

    return status;
}

