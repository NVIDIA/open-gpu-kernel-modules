#define NVOC_KERNEL_FALCON_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_falcon_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b6b1af = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

// Forward declarations for KernelFalcon
void __nvoc_init__KernelCrashCatEngine(KernelCrashCatEngine*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init__KernelFalcon(KernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelFalcon(KernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelFalcon(KernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelFalcon(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelFalcon;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFalcon;

// Down-thunk(s) to bridge KernelFalcon methods from ancestors (if any)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineResetLog(struct CrashCatEngine *arg_this);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvU64 errorId, const char *fmt, va_list args);    // super
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data);    // super
void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // super
const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // super
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this);    // super
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset);    // this
void __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data);    // this
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr);    // this

// Up-thunk(s) to bridge KernelFalcon methods to ancestors (if any)

// Class-specific details for KernelFalcon
// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  kflcnResetHw
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon = 
{
    .classInfo.size =               sizeof(KernelFalcon),
    .classInfo.classId =            classId(KernelFalcon),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelFalcon",
#endif
    .objCreatefn =        NULL,
    .pCastInfo =          &__nvoc_castinfo__KernelFalcon,
    .pExportInfo =        &__nvoc_export_info__KernelFalcon
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelFalcon __nvoc_metadata__KernelFalcon = {
    .rtti.pClassDef = &__nvoc_class_def_KernelFalcon,    // (kflcn) this
    .rtti.dtor      = &__nvoc_dtor_KernelFalcon,
    .rtti.offset    = 0,
    .metadata__KernelCrashCatEngine.rtti.pClassDef = &__nvoc_class_def_KernelCrashCatEngine,    // (kcrashcatEngine) super
    .metadata__KernelCrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelCrashCatEngine.rtti.offset    = NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine),
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.pClassDef = &__nvoc_class_def_CrashCatEngine,    // (crashcatEngine) super^2
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.offset    = NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),

    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegRead__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead,    // pure virtual
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegWrite__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite,    // pure virtual
    .vtable.__kflcnResetHw__ = NULL,    // pure virtual
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineMaskDmemAddr__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineResetLog__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineResetLog,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineReadEmem__ = &kcrashcatEngineReadEmem_f2d351,    // virtual halified (singleton optimized) body
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelFalcon = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelFalcon.rtti,    // [0]: (kflcn) this
        &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti,    // [1]: (kcrashcatEngine) super
        &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti,    // [2]: (crashcatEngine) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in KernelFalcon from superclasses

// kflcnRegRead: virtual halified (3 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) - NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset);
}

// kflcnRegWrite: virtual halified (3 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
void __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) - NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset, data);
}

// kflcnMaskDmemAddr: virtual halified (4 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) - NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), addr);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFalcon = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelFalcon object.
void __nvoc_dtor_KernelCrashCatEngine(Dynamic*);
void __nvoc_dtor_KernelFalcon(Dynamic* pThis) {

    KernelFalcon *__nvoc_this = (KernelFalcon *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_KernelCrashCatEngine((Dynamic *) &__nvoc_this->__nvoc_base_KernelCrashCatEngine);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_KernelFalcon(KernelFalcon *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->riscvMode = ((NvTristate)2);
}


// Construct KernelFalcon object.
NV_STATUS __nvoc_ctor_KernelCrashCatEngine(KernelCrashCatEngine *, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine, pGpuhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelFalcon_fail_KernelCrashCatEngine;

    // Initialize data fields.
    __nvoc_init_dataField_KernelFalcon(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelFalcon_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelFalcon_fail_KernelCrashCatEngine:
__nvoc_ctor_KernelFalcon_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelFalcon_1(KernelFalcon *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kflcnRegRead -- virtual halified (3 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnRegRead__ = &kflcnRegRead_TU102;
        }
    }

    // kflcnRegWrite -- virtual halified (3 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnRegWrite__ = &kflcnRegWrite_TU102;
        }
    }

    // kflcnRiscvRegRead -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnRiscvRegRead__ = &kflcnRiscvRegRead_TU102;
        }
    }

    // kflcnRiscvRegWrite -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnRiscvRegWrite__ = &kflcnRiscvRegWrite_TU102;
        }
    }

    // kflcnIsRiscvCpuEnabled -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnIsRiscvCpuEnabled__ = &kflcnIsRiscvCpuEnabled_TU102;
        }
    }

    // kflcnIsRiscvActive -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_TU102;
        }
        else
        {
            pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_GA102;
        }
    }

    // kflcnIsRiscvSelected -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnIsRiscvSelected__ = &kflcnIsRiscvSelected_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x05f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kflcnIsRiscvSelected__ = &kflcnIsRiscvSelected_GA102;
        }
        else
        {
            pThis->__kflcnIsRiscvSelected__ = &kflcnIsRiscvSelected_e661f0;
        }
    }

    // kflcnRiscvProgramBcr -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_GA102;
        }
    }

    // kflcnSwitchToFalcon -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_d44104;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x05f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_GA102;
        }
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kflcnReset -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnReset__ = &kflcnReset_TU102;
        }
    }

    // kflcnResetIntoRiscv -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnResetIntoRiscv__ = &kflcnResetIntoRiscv_TU102;
        }
        else
        {
            pThis->__kflcnResetIntoRiscv__ = &kflcnResetIntoRiscv_GA102;
        }
    }

    // kflcnStartCpu -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnStartCpu__ = &kflcnStartCpu_TU102;
        }
    }

    // kflcnDisableCtxReq -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnDisableCtxReq__ = &kflcnDisableCtxReq_TU102;
        }
    }

    // kflcnPreResetWait -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_GA102;
        }
        // default
        else
        {
            pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_ac1694;
        }
    }

    // kflcnWaitForResetToFinish -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_ac1694;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_TU102;
        }
        else
        {
            pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_GA102;
        }
    }

    // kflcnWaitForHalt -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnWaitForHalt__ = &kflcnWaitForHalt_TU102;
        }
    }

    // kflcnWaitForHaltRiscv -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnWaitForHaltRiscv__ = &kflcnWaitForHaltRiscv_GA102;
        }
    }

    // kflcnReadIntrStatus -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kflcnReadIntrStatus__ = &kflcnReadIntrStatus_TU102;
        }
    }

    // kflcnRiscvReadIntrStatus -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnRiscvReadIntrStatus__ = &kflcnRiscvReadIntrStatus_TU102;
        }
        else
        {
            pThis->__kflcnRiscvReadIntrStatus__ = &kflcnRiscvReadIntrStatus_GA102;
        }
    }

    // kflcnIntrRetrigger -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T238D | T264D */ 
        {
            pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_d44104;
        }
        else
        {
            pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_GA100;
        }
    }

    // kflcnMaskImemAddr -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_TU102;
        }
        else
        {
            pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_GA100;
        }
    }

    // kflcnMaskDmemAddr -- virtual halified (4 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_TU102;
        }
        else
        {
            pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_GA100;
        }
    }

    // kflcnRiscvIcdWaitForIdle -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdWaitForIdle__ = &kflcnRiscvIcdWaitForIdle_TU102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdWaitForIdle__ = &kflcnRiscvIcdWaitForIdle_395e98;
    }

    // kflcnRiscvIcdReadMem -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdReadMem__ = &kflcnRiscvIcdReadMem_TU102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdReadMem__ = &kflcnRiscvIcdReadMem_395e98;
    }

    // kflcnRiscvIcdReadReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdReadReg__ = &kflcnRiscvIcdReadReg_TU102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdReadReg__ = &kflcnRiscvIcdReadReg_395e98;
    }

    // kflcnRiscvIcdRcsr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdRcsr__ = &kflcnRiscvIcdRcsr_TU102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdRcsr__ = &kflcnRiscvIcdRcsr_395e98;
    }

    // kflcnRiscvIcdRstat -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdRstat__ = &kflcnRiscvIcdRstat_TU102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdRstat__ = &kflcnRiscvIcdRstat_395e98;
    }

    // kflcnRiscvIcdRpc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdRpc__ = &kflcnRiscvIcdRpc_TU102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdRpc__ = &kflcnRiscvIcdRpc_395e98;
    }

    // kflcnRiscvIcdHalt -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdHalt__ = &kflcnRiscvIcdHalt_TU102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdHalt__ = &kflcnRiscvIcdHalt_395e98;
    }

    // kflcnIcdReadCmdReg -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnIcdReadCmdReg__ = &kflcnIcdReadCmdReg_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnIcdReadCmdReg__ = &kflcnIcdReadCmdReg_GA102;
    }
    // default
    else
    {
        pThis->__kflcnIcdReadCmdReg__ = &kflcnIcdReadCmdReg_b2b553;
    }

    // kflcnRiscvIcdReadRdata -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnRiscvIcdReadRdata__ = &kflcnRiscvIcdReadRdata_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdReadRdata__ = &kflcnRiscvIcdReadRdata_GA102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdReadRdata__ = &kflcnRiscvIcdReadRdata_b2b553;
    }

    // kflcnRiscvIcdWriteAddress -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnRiscvIcdWriteAddress__ = &kflcnRiscvIcdWriteAddress_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnRiscvIcdWriteAddress__ = &kflcnRiscvIcdWriteAddress_GA102;
    }
    // default
    else
    {
        pThis->__kflcnRiscvIcdWriteAddress__ = &kflcnRiscvIcdWriteAddress_d44104;
    }

    // kflcnIcdWriteCmdReg -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnIcdWriteCmdReg__ = &kflcnIcdWriteCmdReg_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnIcdWriteCmdReg__ = &kflcnIcdWriteCmdReg_GA102;
    }
    // default
    else
    {
        pThis->__kflcnIcdWriteCmdReg__ = &kflcnIcdWriteCmdReg_d44104;
    }

    // kflcnCoreDumpPc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnCoreDumpPc__ = &kflcnCoreDumpPc_GA102;
    }
    // default
    else
    {
        pThis->__kflcnCoreDumpPc__ = &kflcnCoreDumpPc_395e98;
    }

    // kflcnDumpCoreRegs -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnDumpCoreRegs__ = &kflcnDumpCoreRegs_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060feUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnDumpCoreRegs__ = &kflcnDumpCoreRegs_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 */ 
    {
        pThis->__kflcnDumpCoreRegs__ = &kflcnDumpCoreRegs_GA102;
    }
    // default
    else
    {
        pThis->__kflcnDumpCoreRegs__ = &kflcnDumpCoreRegs_d44104;
    }

    // kflcnDumpTracepc -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kflcnDumpTracepc__ = &kflcnDumpTracepc_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnDumpTracepc__ = &kflcnDumpTracepc_GA102;
    }
    // default
    else
    {
        pThis->__kflcnDumpTracepc__ = &kflcnDumpTracepc_d44104;
    }

    // kflcnDumpPeripheralRegs -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnDumpPeripheralRegs__ = &kflcnDumpPeripheralRegs_TU102;
    }
    // default
    else
    {
        pThis->__kflcnDumpPeripheralRegs__ = &kflcnDumpPeripheralRegs_d44104;
    }

    // kflcnGetEccInterruptMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kflcnGetEccInterruptMask__ = &kflcnGetEccInterruptMask_GB100;
    }
    // default
    else
    {
        pThis->__kflcnGetEccInterruptMask__ = &kflcnGetEccInterruptMask_b2b553;
    }

    // kflcnGetFatalHwErrorStatus -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnGetFatalHwErrorStatus__ = &kflcnGetFatalHwErrorStatus_GB100;
    }
    // default
    else
    {
        pThis->__kflcnGetFatalHwErrorStatus__ = &kflcnGetFatalHwErrorStatus_395e98;
    }

    // kflcnFatalHwErrorCodeToString -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kflcnFatalHwErrorCodeToString__ = &kflcnFatalHwErrorCodeToString_GB100;
    }
    // default
    else
    {
        pThis->__kflcnFatalHwErrorCodeToString__ = &kflcnFatalHwErrorCodeToString_GA100;
    }
} // End __nvoc_init_funcTable_KernelFalcon_1 with approximately 119 basic block(s).


// Initialize vtable(s) for 41 virtual method(s).
void __nvoc_init_funcTable_KernelFalcon(KernelFalcon *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 40 per-object function pointer(s).
    __nvoc_init_funcTable_KernelFalcon_1(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelFalcon(KernelFalcon *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;    // (crashcatEngine) super^2
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelCrashCatEngine;    // (kcrashcatEngine) super
    pThis->__nvoc_pbase_KernelFalcon = pThis;    // (kflcn) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine, pGpuhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine;    // (crashcatEngine) super^2
    pThis->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine;    // (kcrashcatEngine) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelFalcon;    // (kflcn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelFalcon(pThis, pGpuhalspecowner, pRmhalspecowner);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__abcf08 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for GenericKernelFalcon
void __nvoc_init__KernelFalcon(KernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__Object(Object*);
void __nvoc_init__GenericKernelFalcon(GenericKernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon*);
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner, struct OBJGPU *pGpu, KernelFalconEngineConfig *pFalconConfig);
void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon*);
void __nvoc_dtor_GenericKernelFalcon(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GenericKernelFalcon;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GenericKernelFalcon;

// Down-thunk(s) to bridge GenericKernelFalcon methods from ancestors (if any)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineResetLog(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvU64 errorId, const char *fmt, va_list args);    // super^2
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
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pGenKernFlcn);    // this
void __nvoc_down_thunk_GenericKernelFalcon_intrservRegisterIntrService(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[320]);    // this
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3);    // this

// Up-thunk(s) to bridge GenericKernelFalcon methods to ancestors (if any)

// Class-specific details for GenericKernelFalcon
const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon = 
{
    .classInfo.size =               sizeof(GenericKernelFalcon),
    .classInfo.classId =            classId(GenericKernelFalcon),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GenericKernelFalcon",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GenericKernelFalcon,
    .pCastInfo =          &__nvoc_castinfo__GenericKernelFalcon,
    .pExportInfo =        &__nvoc_export_info__GenericKernelFalcon
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GenericKernelFalcon __nvoc_metadata__GenericKernelFalcon = {
    .rtti.pClassDef = &__nvoc_class_def_GenericKernelFalcon,    // (gkflcn) this
    .rtti.dtor      = &__nvoc_dtor_GenericKernelFalcon,
    .rtti.offset    = 0,
    .metadata__KernelFalcon.rtti.pClassDef = &__nvoc_class_def_KernelFalcon,    // (kflcn) super
    .metadata__KernelFalcon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon),
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.pClassDef = &__nvoc_class_def_KernelCrashCatEngine,    // (kcrashcatEngine) super^2
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.pClassDef = &__nvoc_class_def_CrashCatEngine,    // (crashcatEngine) super^3
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService),
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_Object),

    .metadata__KernelFalcon.vtable.__kflcnResetHw__ = &__nvoc_down_thunk_GenericKernelFalcon_kflcnResetHw,    // pure virtual
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_GenericKernelFalcon_intrservRegisterIntrService,    // virtual
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &__nvoc_down_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt,    // virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegRead__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegWrite__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineMaskDmemAddr__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineResetLog__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineResetLog,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineReadEmem__ = &kcrashcatEngineReadEmem_f2d351,    // virtual halified (singleton optimized) body
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &intrservServiceInterrupt_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GenericKernelFalcon = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GenericKernelFalcon.rtti,    // [0]: (gkflcn) this
        &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.rtti,    // [1]: (kflcn) super
        &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti,    // [2]: (kcrashcatEngine) super^2
        &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti,    // [3]: (crashcatEngine) super^3
        &__nvoc_metadata__GenericKernelFalcon.metadata__IntrService.rtti,    // [4]: (intrserv) super
        &__nvoc_metadata__GenericKernelFalcon.metadata__Object.rtti,    // [5]: (obj) super
    }
};

// 3 down-thunk(s) defined to bridge methods in GenericKernelFalcon from superclasses

// gkflcnResetHw: virtual override (kflcn) base (kflcn)
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pGenKernFlcn) {
    return gkflcnResetHw_IMPL(pGpu, (struct GenericKernelFalcon *)(((unsigned char *) pGenKernFlcn) - NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon)));
}

// gkflcnRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_GenericKernelFalcon_intrservRegisterIntrService(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[320]) {
    gkflcnRegisterIntrService_IMPL(arg1, (struct GenericKernelFalcon *)(((unsigned char *) arg_this) - NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService)), arg3);
}

// gkflcnServiceNotificationInterrupt: virtual override (intrserv) base (intrserv)
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return gkflcnServiceNotificationInterrupt_IMPL(arg1, (struct GenericKernelFalcon *)(((unsigned char *) arg_this) - NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService)), arg3);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__GenericKernelFalcon = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GenericKernelFalcon object.
void __nvoc_dtor_KernelFalcon(Dynamic*);
void __nvoc_dtor_IntrService(Dynamic*);
void __nvoc_dtor_Object(Dynamic*);
void __nvoc_dtor_GenericKernelFalcon(Dynamic* pThis) {

    GenericKernelFalcon *__nvoc_this = (GenericKernelFalcon *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_KernelFalcon((Dynamic *) &__nvoc_this->__nvoc_base_KernelFalcon);
    __nvoc_dtor_IntrService((Dynamic *) &__nvoc_this->__nvoc_base_IntrService);
    __nvoc_dtor_Object((Dynamic *) &__nvoc_this->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GenericKernelFalcon object.
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon *, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_IntrService(IntrService *);
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon *pGenKernFlcn, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner, struct OBJGPU *pGpu, KernelFalconEngineConfig *pFalconConfig) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_KernelFalcon(&pGenKernFlcn->__nvoc_base_KernelFalcon, pGpuhalspecowner, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_KernelFalcon;
    status = __nvoc_ctor_IntrService(&pGenKernFlcn->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_IntrService;
    status = __nvoc_ctor_Object(&pGenKernFlcn->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_GenericKernelFalcon(pGenKernFlcn);

    // Call the constructor for this class.
    status = __nvoc_gkflcnConstruct(pGenKernFlcn, pGpu, pFalconConfig);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail__init;
    goto __nvoc_ctor_GenericKernelFalcon_exit; // Success

    // Unwind on error.
__nvoc_ctor_GenericKernelFalcon_fail__init:
    __nvoc_dtor_Object((Dynamic *)&pGenKernFlcn->__nvoc_base_Object);
__nvoc_ctor_GenericKernelFalcon_fail_Object:
    __nvoc_dtor_IntrService((Dynamic *)&pGenKernFlcn->__nvoc_base_IntrService);
__nvoc_ctor_GenericKernelFalcon_fail_IntrService:
    __nvoc_dtor_KernelFalcon((Dynamic *)&pGenKernFlcn->__nvoc_base_KernelFalcon);
__nvoc_ctor_GenericKernelFalcon_fail_KernelFalcon:
__nvoc_ctor_GenericKernelFalcon_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GenericKernelFalcon_1(GenericKernelFalcon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GenericKernelFalcon_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon *pThis) {
    __nvoc_init_funcTable_GenericKernelFalcon_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GenericKernelFalcon(GenericKernelFalcon *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;    // (crashcatEngine) super^3
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;    // (kcrashcatEngine) super^2
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;    // (kflcn) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_GenericKernelFalcon = pThis;    // (gkflcn) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pGpuhalspecowner, pRmhalspecowner);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine;    // (crashcatEngine) super^3
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine;    // (kcrashcatEngine) super^2
    pThis->__nvoc_base_KernelFalcon.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon;    // (kflcn) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon;    // (gkflcn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GenericKernelFalcon(pThis);
}

NV_STATUS __nvoc_objCreate_GenericKernelFalcon(GenericKernelFalcon **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct OBJGPU *pGpu, KernelFalconEngineConfig *pFalconConfig)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GenericKernelFalcon *__nvoc_pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GenericKernelFalcon));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GenericKernelFalcon));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_GenericKernelFalcon_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_GenericKernelFalcon_cleanup);
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_GenericKernelFalcon_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GenericKernelFalcon(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_GenericKernelFalcon(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner, pGpu, pFalconConfig);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GenericKernelFalcon_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GenericKernelFalcon_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GenericKernelFalcon));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GenericKernelFalcon(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct OBJGPU *pGpu = va_arg(__nvoc_args, struct OBJGPU *);
    KernelFalconEngineConfig *pFalconConfig = va_arg(__nvoc_args, KernelFalconEngineConfig *);

    return __nvoc_objCreate_GenericKernelFalcon((GenericKernelFalcon **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pGpu, pFalconConfig);
}

