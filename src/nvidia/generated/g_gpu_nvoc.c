#define NVOC_GPU_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7ef3cb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPU;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmHalspecOwner;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

void __nvoc_init_OBJGPU(OBJGPU*, NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev, RM_RUNTIME_VARIANT RmVariantHal_rmVariant, NvU32 DispIpHal_ipver);
void __nvoc_init_funcTable_OBJGPU(OBJGPU*);
NV_STATUS __nvoc_ctor_OBJGPU(OBJGPU*, NvU32 arg_gpuInstance);
void __nvoc_init_dataField_OBJGPU(OBJGPU*);
void __nvoc_dtor_OBJGPU(OBJGPU*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPU;

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_OBJGPU = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGPU,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPU,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPU, __nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_RmHalspecOwner = {
    /*pClassDef=*/          &__nvoc_class_def_RmHalspecOwner,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPU, __nvoc_base_RmHalspecOwner),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_OBJTRACEABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJTRACEABLE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPU, __nvoc_base_OBJTRACEABLE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGPU = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGPU_OBJGPU,
        &__nvoc_rtti_OBJGPU_OBJTRACEABLE,
        &__nvoc_rtti_OBJGPU_RmHalspecOwner,
        &__nvoc_rtti_OBJGPU_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPU = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGPU),
        /*classId=*/            classId(OBJGPU),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGPU",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPU,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGPU,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGPU
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPU = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RmHalspecOwner(RmHalspecOwner*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJGPU(OBJGPU *pThis) {
    __nvoc_gpuDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner);
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGPU(OBJGPU *pThis) {
    ChipHal *chipHal = &staticCast(pThis, RmHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &staticCast(pThis, RmHalspecOwner)->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    pThis->setProperty(pThis, PDB_PROP_GPU_IS_CONNECTED, ((NvBool)(0 == 0)));

    // NVOC Property Hal field -- PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_IS_UEFI
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_UEFI, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_ZERO_FB
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_ZERO_FB, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_MIG_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED, ((NvBool)(0 != 0)));
    }

    pThis->boardId = ~0;

    pThis->deviceInstance = 32;

    // Hal field -- isVirtual
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->isVirtual = ((NvBool)(0 != 0));
    }

    // Hal field -- isGspClient
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->isGspClient = ((NvBool)(0 == 0));
    }
    else if (0)
    {
    }

    pThis->bIsDebugModeEnabled = ((NvBool)(0 != 0));

    pThis->numOfMclkLockRequests = 0U;

    pThis->bUseRegisterAccessMap = !(0);

    pThis->boardInfo = ((void *)0);

    // Hal field -- bUnifiedMemorySpaceEnabled
    if (0)
    {
    }
    // default
    else
    {
        pThis->bUnifiedMemorySpaceEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bWarBug200577889SriovHeavyEnabled
    pThis->bWarBug200577889SriovHeavyEnabled = ((NvBool)(0 != 0));

    // Hal field -- bNeed4kPageIsolation
    if (0)
    {
    }
    // default
    else
    {
        pThis->bNeed4kPageIsolation = ((NvBool)(0 != 0));
    }

    // Hal field -- bInstLoc47bitPaWar
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bInstLoc47bitPaWar = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bInstLoc47bitPaWar = ((NvBool)(0 != 0));
    }

    // Hal field -- bIsBarPteInSysmemSupported
    if (0)
    {
    }
    // default
    else
    {
        pThis->bIsBarPteInSysmemSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bClientRmAllocatedCtxBuffer
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bClientRmAllocatedCtxBuffer = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bClientRmAllocatedCtxBuffer = ((NvBool)(0 != 0));
    }

    // Hal field -- bVidmemPreservationBrokenBug3172217
    if (0)
    {
    }
    // default
    else
    {
        pThis->bVidmemPreservationBrokenBug3172217 = ((NvBool)(0 != 0));
    }

    // Hal field -- bInstanceMemoryAlwaysCached
    if (0)
    {
    }
    // default
    else
    {
        pThis->bInstanceMemoryAlwaysCached = ((NvBool)(0 != 0));
    }

    pThis->bIsGeforce = ((NvBool)(0 == 0));

    // Hal field -- bComputePolicyTimesliceSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bComputePolicyTimesliceSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bComputePolicyTimesliceSupported = ((NvBool)(0 != 0));
    }
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RmHalspecOwner(RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE* );
NV_STATUS __nvoc_ctor_OBJGPU(OBJGPU *pThis, NvU32 arg_gpuInstance) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail_Object;
    status = __nvoc_ctor_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail_RmHalspecOwner;
    status = __nvoc_ctor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail_OBJTRACEABLE;
    __nvoc_init_dataField_OBJGPU(pThis);

    status = __nvoc_gpuConstruct(pThis, arg_gpuInstance);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail__init;
    goto __nvoc_ctor_OBJGPU_exit; // Success

__nvoc_ctor_OBJGPU_fail__init:
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
__nvoc_ctor_OBJGPU_fail_OBJTRACEABLE:
    __nvoc_dtor_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner);
__nvoc_ctor_OBJGPU_fail_RmHalspecOwner:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJGPU_fail_Object:
__nvoc_ctor_OBJGPU_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJGPU_1(OBJGPU *pThis) {
    ChipHal *chipHal = &staticCast(pThis, RmHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &staticCast(pThis, RmHalspecOwner)->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // Hal function -- gpuGetChildrenPresent
    if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000320UL) )) /* ChipHal: TU102 | TU116 | TU117 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000040UL) )) /* ChipHal: TU104 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_TU104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000080UL) )) /* ChipHal: TU106 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_TU106;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GA102;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- gpuGetClassDescriptorList
    if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000120UL) )) /* ChipHal: TU102 | TU116 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000040UL) )) /* ChipHal: TU104 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000080UL) )) /* ChipHal: TU106 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU106;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000200UL) )) /* ChipHal: TU117 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU117;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GA102;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- gpuFuseSupportsDisplay
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_GA100;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- gpuClearFbhubPoisonIntrForBug2924523
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__gpuClearFbhubPoisonIntrForBug2924523__ = &gpuClearFbhubPoisonIntrForBug2924523_GA100_KERNEL;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__gpuClearFbhubPoisonIntrForBug2924523__ = &gpuClearFbhubPoisonIntrForBug2924523_56cd7a;
        }
    }
    else if (0)
    {
#if 0
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__gpuClearFbhubPoisonIntrForBug2924523__ = &gpuClearFbhubPoisonIntrForBug2924523_GA100_PHYSICAL;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__gpuClearFbhubPoisonIntrForBug2924523__ = &gpuClearFbhubPoisonIntrForBug2924523_56cd7a;
        }
#endif
    }

    // Hal function -- gpuConstructDeviceInfoTable
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__gpuConstructDeviceInfoTable__ = &gpuConstructDeviceInfoTable_FWCLIENT;
        }
        // default
        else
        {
            pThis->__gpuConstructDeviceInfoTable__ = &gpuConstructDeviceInfoTable_56cd7a;
        }
    }
    else if (0)
    {
#if 0
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__gpuConstructDeviceInfoTable__ = &gpuConstructDeviceInfoTable_GA100;
        }
        // default
        else
        {
            pThis->__gpuConstructDeviceInfoTable__ = &gpuConstructDeviceInfoTable_56cd7a;
        }
#endif
    }

    // Hal function -- gpuGetFlaVasSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_GA100;
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_474d46;
    }
}

void __nvoc_init_funcTable_OBJGPU(OBJGPU *pThis) {
    __nvoc_init_funcTable_OBJGPU_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RmHalspecOwner(RmHalspecOwner*, NvU32, NvU32, NvU32, RM_RUNTIME_VARIANT, NvU32);
void __nvoc_init_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_OBJGPU(OBJGPU *pThis, NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev, RM_RUNTIME_VARIANT RmVariantHal_rmVariant, NvU32 DispIpHal_ipver) {
    pThis->__nvoc_pbase_OBJGPU = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    pThis->__nvoc_pbase_RmHalspecOwner = &pThis->__nvoc_base_RmHalspecOwner;
    pThis->__nvoc_pbase_OBJTRACEABLE = &pThis->__nvoc_base_OBJTRACEABLE;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, DispIpHal_ipver);
    __nvoc_init_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    __nvoc_init_funcTable_OBJGPU(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGPU(OBJGPU **ppThis, Dynamic *pParent, NvU32 createFlags, NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev, RM_RUNTIME_VARIANT RmVariantHal_rmVariant, NvU32 DispIpHal_ipver, NvU32 arg_gpuInstance) {
    NV_STATUS status;
    Object *pParentObj;
    OBJGPU *pThis;

    pThis = portMemAllocNonPaged(sizeof(OBJGPU));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(OBJGPU));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGPU);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJGPU(pThis, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, DispIpHal_ipver);
    status = __nvoc_ctor_OBJGPU(pThis, arg_gpuInstance);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGPU_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_OBJGPU_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPU(OBJGPU **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    NvU32 ChipHal_arch = va_arg(args, NvU32);
    NvU32 ChipHal_impl = va_arg(args, NvU32);
    NvU32 ChipHal_hidrev = va_arg(args, NvU32);
    RM_RUNTIME_VARIANT RmVariantHal_rmVariant = va_arg(args, RM_RUNTIME_VARIANT);
    NvU32 DispIpHal_ipver = va_arg(args, NvU32);
    NvU32 arg_gpuInstance = va_arg(args, NvU32);

    status = __nvoc_objCreate_OBJGPU(ppThis, pParent, createFlags, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, DispIpHal_ipver, arg_gpuInstance);

    return status;
}

