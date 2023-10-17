#define NVOC_KERNEL_FALCON_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_falcon_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb6b1af = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelFalcon;

static const struct NVOC_RTTI __nvoc_rtti_KernelFalcon_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelFalcon,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelFalcon_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelFalcon_KernelCrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelFalcon = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelFalcon_KernelFalcon,
        &__nvoc_rtti_KernelFalcon_KernelCrashCatEngine,
        &__nvoc_rtti_KernelFalcon_CrashCatEngine,
    },
};

// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  kflcnResetHw
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelFalcon),
        /*classId=*/            classId(KernelFalcon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelFalcon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelFalcon,
    /*pExportInfo=*/        &__nvoc_export_info_KernelFalcon
};

static NvU32 __nvoc_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) - __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), offset);
}

static void __nvoc_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) - __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), offset, data);
}

static NvU32 __nvoc_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) - __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), addr);
}

static void __nvoc_thunk_KernelCrashCatEngine_kflcnReadEmem(struct KernelFalcon *arg0, NvU64 offset, NvU64 size, void *pBuf) {
    kcrashcatEngineReadEmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), offset, size, pBuf);
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset(struct KernelFalcon *arg0) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset));
}

static const NvU32 *__nvoc_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets(struct KernelFalcon *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), scratchGroupId);
}

static void __nvoc_thunk_KernelCrashCatEngine_kflcnUnload(struct KernelFalcon *arg0) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset));
}

static NvBool __nvoc_thunk_KernelCrashCatEngine_kflcnConfigured(struct KernelFalcon *arg0) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset));
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_kflcnPriRead(struct KernelFalcon *arg0, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), offset);
}

static void __nvoc_thunk_KernelCrashCatEngine_kflcnVprintf(struct KernelFalcon *arg0, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), bReportStart, fmt, args);
}

static void __nvoc_thunk_KernelCrashCatEngine_kflcnPriWrite(struct KernelFalcon *arg0, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), offset, data);
}

static void __nvoc_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor(struct KernelFalcon *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), pBufDesc, offset, size);
}

static void *__nvoc_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor(struct KernelFalcon *arg0, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), pBufDesc);
}

static void __nvoc_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor(struct KernelFalcon *arg0, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), pBufDesc);
}

static void __nvoc_thunk_KernelCrashCatEngine_kflcnReadDmem(struct KernelFalcon *arg0, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelFalcon_KernelCrashCatEngine.offset), offset, size, pBuf);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelFalcon = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_KernelCrashCatEngine(KernelCrashCatEngine*);
void __nvoc_dtor_KernelFalcon(KernelFalcon *pThis) {
    __nvoc_dtor_KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
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
}

NV_STATUS __nvoc_ctor_KernelCrashCatEngine(KernelCrashCatEngine* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelFalcon_fail_KernelCrashCatEngine;
    __nvoc_init_dataField_KernelFalcon(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelFalcon_exit; // Success

__nvoc_ctor_KernelFalcon_fail_KernelCrashCatEngine:
__nvoc_ctor_KernelFalcon_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelFalcon_1(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal function -- kflcnRegRead
    pThis->__kflcnRegRead__ = &kflcnRegRead_TU102;

    // Hal function -- kflcnRegWrite
    pThis->__kflcnRegWrite__ = &kflcnRegWrite_TU102;

    // Hal function -- kflcnIsRiscvActive
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_TU102;
    }
    else
    {
        pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_GA10X;
    }

    // Hal function -- kflcnRiscvProgramBcr
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_f2d351;
    }
    else
    {
        pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_GA102;
    }

    // Hal function -- kflcnSwitchToFalcon
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_b3696a;
    }
    else
    {
        pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_GA10X;
    }

    pThis->__kflcnResetHw__ = NULL;

    // Hal function -- kflcnPreResetWait
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_GA10X;
    }
    // default
    else
    {
        pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_56cd7a;
    }

    // Hal function -- kflcnWaitForResetToFinish
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_TU102;
    }
    else
    {
        pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_GA102;
    }

    // Hal function -- kflcnReadIntrStatus
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnReadIntrStatus__ = &kflcnReadIntrStatus_TU102;
    }
    else
    {
        pThis->__kflcnReadIntrStatus__ = &kflcnReadIntrStatus_GA102;
    }

    // Hal function -- kflcnIntrRetrigger
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_b3696a;
    }
    else
    {
        pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_GA100;
    }

    // Hal function -- kflcnMaskImemAddr
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_TU102;
    }
    else
    {
        pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_GA100;
    }

    // Hal function -- kflcnMaskDmemAddr
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_TU102;
    }
    else
    {
        pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_GA100;
    }

    pThis->__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineRegRead__ = &__nvoc_thunk_KernelFalcon_kcrashcatEngineRegRead;

    pThis->__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineRegWrite__ = &__nvoc_thunk_KernelFalcon_kcrashcatEngineRegWrite;

    pThis->__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineMaskDmemAddr__ = &__nvoc_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr;

    pThis->__kflcnReadEmem__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnReadEmem;

    pThis->__kflcnGetWFL0Offset__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset;

    pThis->__kflcnGetScratchOffsets__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets;

    pThis->__kflcnUnload__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnUnload;

    pThis->__kflcnConfigured__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnConfigured;

    pThis->__kflcnPriRead__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnPriRead;

    pThis->__kflcnVprintf__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnVprintf;

    pThis->__kflcnPriWrite__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnPriWrite;

    pThis->__kflcnSyncBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor;

    pThis->__kflcnMapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor;

    pThis->__kflcnUnmapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor;

    pThis->__kflcnReadDmem__ = &__nvoc_thunk_KernelCrashCatEngine_kflcnReadDmem;
}

void __nvoc_init_funcTable_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelFalcon_1(pThis, pRmhalspecowner);
}

void __nvoc_init_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner* );
void __nvoc_init_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelFalcon = pThis;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelCrashCatEngine;
    __nvoc_init_KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine, pRmhalspecowner);
    __nvoc_init_funcTable_KernelFalcon(pThis, pRmhalspecowner);
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xabcf08 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner* );
void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon*);
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner* , struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig);
void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon*);
void __nvoc_dtor_GenericKernelFalcon(GenericKernelFalcon*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GenericKernelFalcon;

static const struct NVOC_RTTI __nvoc_rtti_GenericKernelFalcon_GenericKernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_GenericKernelFalcon,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GenericKernelFalcon,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GenericKernelFalcon_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericKernelFalcon_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericKernelFalcon_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericKernelFalcon_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GenericKernelFalcon = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_GenericKernelFalcon_GenericKernelFalcon,
        &__nvoc_rtti_GenericKernelFalcon_Object,
        &__nvoc_rtti_GenericKernelFalcon_IntrService,
        &__nvoc_rtti_GenericKernelFalcon_KernelFalcon,
        &__nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine,
        &__nvoc_rtti_GenericKernelFalcon_CrashCatEngine,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GenericKernelFalcon),
        /*classId=*/            classId(GenericKernelFalcon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GenericKernelFalcon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GenericKernelFalcon,
    /*pCastInfo=*/          &__nvoc_castinfo_GenericKernelFalcon,
    /*pExportInfo=*/        &__nvoc_export_info_GenericKernelFalcon
};

static NV_STATUS __nvoc_thunk_GenericKernelFalcon_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pGenKernFlcn) {
    return gkflcnResetHw(pGpu, (struct GenericKernelFalcon *)(((unsigned char *)pGenKernFlcn) - __nvoc_rtti_GenericKernelFalcon_KernelFalcon.offset));
}

static void __nvoc_thunk_GenericKernelFalcon_intrservRegisterIntrService(struct OBJGPU *arg0, struct IntrService *arg1, IntrServiceRecord arg2[168]) {
    gkflcnRegisterIntrService(arg0, (struct GenericKernelFalcon *)(((unsigned char *)arg1) - __nvoc_rtti_GenericKernelFalcon_IntrService.offset), arg2);
}

static NV_STATUS __nvoc_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt(struct OBJGPU *arg0, struct IntrService *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return gkflcnServiceNotificationInterrupt(arg0, (struct GenericKernelFalcon *)(((unsigned char *)arg1) - __nvoc_rtti_GenericKernelFalcon_IntrService.offset), arg2);
}

static void __nvoc_thunk_KernelCrashCatEngine_gkflcnReadEmem(struct GenericKernelFalcon *arg0, NvU64 offset, NvU64 size, void *pBuf) {
    kcrashcatEngineReadEmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), offset, size, pBuf);
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_gkflcnGetWFL0Offset(struct GenericKernelFalcon *arg0) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset));
}

static void __nvoc_thunk_KernelCrashCatEngine_gkflcnUnload(struct GenericKernelFalcon *arg0) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset));
}

static NvBool __nvoc_thunk_KernelCrashCatEngine_gkflcnConfigured(struct GenericKernelFalcon *arg0) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset));
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_gkflcnPriRead(struct GenericKernelFalcon *arg0, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), offset);
}

static const NvU32 *__nvoc_thunk_KernelCrashCatEngine_gkflcnGetScratchOffsets(struct GenericKernelFalcon *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), scratchGroupId);
}

static void __nvoc_thunk_KernelFalcon_gkflcnRegWrite(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_GenericKernelFalcon_KernelFalcon.offset), offset, data);
}

static NvU32 __nvoc_thunk_KernelFalcon_gkflcnMaskDmemAddr(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_GenericKernelFalcon_KernelFalcon.offset), addr);
}

static void __nvoc_thunk_KernelCrashCatEngine_gkflcnVprintf(struct GenericKernelFalcon *arg0, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), bReportStart, fmt, args);
}

static NvBool __nvoc_thunk_IntrService_gkflcnClearInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_GenericKernelFalcon_IntrService.offset), pParams);
}

static void __nvoc_thunk_KernelCrashCatEngine_gkflcnPriWrite(struct GenericKernelFalcon *arg0, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), offset, data);
}

static void *__nvoc_thunk_KernelCrashCatEngine_gkflcnMapBufferDescriptor(struct GenericKernelFalcon *arg0, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), pBufDesc);
}

static void __nvoc_thunk_KernelCrashCatEngine_gkflcnSyncBufferDescriptor(struct GenericKernelFalcon *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), pBufDesc, offset, size);
}

static NvU32 __nvoc_thunk_KernelFalcon_gkflcnRegRead(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_GenericKernelFalcon_KernelFalcon.offset), offset);
}

static void __nvoc_thunk_KernelCrashCatEngine_gkflcnUnmapBufferDescriptor(struct GenericKernelFalcon *arg0, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), pBufDesc);
}

static NvU32 __nvoc_thunk_IntrService_gkflcnServiceInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_GenericKernelFalcon_IntrService.offset), pParams);
}

static void __nvoc_thunk_KernelCrashCatEngine_gkflcnReadDmem(struct GenericKernelFalcon *arg0, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_GenericKernelFalcon_KernelCrashCatEngine.offset), offset, size, pBuf);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_GenericKernelFalcon = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_GenericKernelFalcon(GenericKernelFalcon *pThis) {
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner, struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_KernelFalcon;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_IntrService;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_Object;
    __nvoc_init_dataField_GenericKernelFalcon(pThis);

    status = __nvoc_gkflcnConstruct(pThis, arg_pGpu, arg_pFalconConfig);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail__init;
    goto __nvoc_ctor_GenericKernelFalcon_exit; // Success

__nvoc_ctor_GenericKernelFalcon_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_GenericKernelFalcon_fail_Object:
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
__nvoc_ctor_GenericKernelFalcon_fail_IntrService:
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
__nvoc_ctor_GenericKernelFalcon_fail_KernelFalcon:
__nvoc_ctor_GenericKernelFalcon_exit:

    return status;
}

static void __nvoc_init_funcTable_GenericKernelFalcon_1(GenericKernelFalcon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__gkflcnResetHw__ = &gkflcnResetHw_IMPL;

    pThis->__gkflcnRegisterIntrService__ = &gkflcnRegisterIntrService_IMPL;

    pThis->__gkflcnServiceNotificationInterrupt__ = &gkflcnServiceNotificationInterrupt_IMPL;

    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_thunk_GenericKernelFalcon_kflcnResetHw;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_GenericKernelFalcon_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt;

    pThis->__gkflcnReadEmem__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnReadEmem;

    pThis->__gkflcnGetWFL0Offset__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnGetWFL0Offset;

    pThis->__gkflcnUnload__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnUnload;

    pThis->__gkflcnConfigured__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnConfigured;

    pThis->__gkflcnPriRead__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnPriRead;

    pThis->__gkflcnGetScratchOffsets__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnGetScratchOffsets;

    pThis->__gkflcnRegWrite__ = &__nvoc_thunk_KernelFalcon_gkflcnRegWrite;

    pThis->__gkflcnMaskDmemAddr__ = &__nvoc_thunk_KernelFalcon_gkflcnMaskDmemAddr;

    pThis->__gkflcnVprintf__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnVprintf;

    pThis->__gkflcnClearInterrupt__ = &__nvoc_thunk_IntrService_gkflcnClearInterrupt;

    pThis->__gkflcnPriWrite__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnPriWrite;

    pThis->__gkflcnMapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnMapBufferDescriptor;

    pThis->__gkflcnSyncBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnSyncBufferDescriptor;

    pThis->__gkflcnRegRead__ = &__nvoc_thunk_KernelFalcon_gkflcnRegRead;

    pThis->__gkflcnUnmapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnUnmapBufferDescriptor;

    pThis->__gkflcnServiceInterrupt__ = &__nvoc_thunk_IntrService_gkflcnServiceInterrupt;

    pThis->__gkflcnReadDmem__ = &__nvoc_thunk_KernelCrashCatEngine_gkflcnReadDmem;
}

void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon *pThis) {
    __nvoc_init_funcTable_GenericKernelFalcon_1(pThis);
}

void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_Object(Object*);
void __nvoc_init_GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_GenericKernelFalcon = pThis;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_GenericKernelFalcon(pThis);
}

NV_STATUS __nvoc_objCreate_GenericKernelFalcon(GenericKernelFalcon **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig) {
    NV_STATUS status;
    Object *pParentObj;
    GenericKernelFalcon *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GenericKernelFalcon), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(GenericKernelFalcon));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GenericKernelFalcon);

    pThis->__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_GenericKernelFalcon(pThis, pRmhalspecowner);
    status = __nvoc_ctor_GenericKernelFalcon(pThis, pRmhalspecowner, arg_pGpu, arg_pFalconConfig);
    if (status != NV_OK) goto __nvoc_objCreate_GenericKernelFalcon_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GenericKernelFalcon_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GenericKernelFalcon));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GenericKernelFalcon(GenericKernelFalcon **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pGpu = va_arg(args, struct OBJGPU *);
    KernelFalconEngineConfig * arg_pFalconConfig = va_arg(args, KernelFalconEngineConfig *);

    status = __nvoc_objCreate_GenericKernelFalcon(ppThis, pParent, createFlags, arg_pGpu, arg_pFalconConfig);

    return status;
}

