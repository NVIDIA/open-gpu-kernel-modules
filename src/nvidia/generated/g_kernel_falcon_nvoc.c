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

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelFalcon = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_KernelFalcon_KernelFalcon,
    },
};

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

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelFalcon = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_KernelFalcon(KernelFalcon *pThis) {
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

NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_KernelFalcon(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelFalcon_exit; // Success

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

    // Hal function -- kflcnIsRiscvActive
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_GA10X;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kflcnRiscvProgramBcr
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_f2d351;
        }
    }
    else if (0)
    {
    }

    // Hal function -- kflcnSwitchToFalcon
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_GA10X;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_b3696a;
        }
    }
    else if (0)
    {
    }

    pThis->__kflcnResetHw__ = NULL;

    // Hal function -- kflcnPreResetWait
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_GA10X;
        }
        // default
        else
        {
            pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_56cd7a;
        }
    }
    else if (0)
    {
    }

    // Hal function -- kflcnWaitForResetToFinish
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_TU102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    pThis->__kflcnIsEngineInReset__ = NULL;

    // Hal function -- kflcnReadIntrStatus
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnReadIntrStatus__ = &kflcnReadIntrStatus_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnReadIntrStatus__ = &kflcnReadIntrStatus_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kflcnIntrRetrigger
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_b3696a;
        }
    }
    else if (0)
    {
    }

    // Hal function -- kflcnMaskImemAddr
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_GA100;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kflcnMaskDmemAddr
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_GA100;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }
}

void __nvoc_init_funcTable_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelFalcon_1(pThis, pRmhalspecowner);
}

void __nvoc_init_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelFalcon = pThis;
    __nvoc_init_funcTable_KernelFalcon(pThis, pRmhalspecowner);
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xabcf08 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner* );
void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner* , struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig);
void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner* );
void __nvoc_dtor_GenericKernelFalcon(GenericKernelFalcon*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GenericKernelFalcon;

static const struct NVOC_RTTI __nvoc_rtti_GenericKernelFalcon_GenericKernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_GenericKernelFalcon,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GenericKernelFalcon,
    /*offset=*/             0,
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
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_GenericKernelFalcon_GenericKernelFalcon,
        &__nvoc_rtti_GenericKernelFalcon_Object,
        &__nvoc_rtti_GenericKernelFalcon_IntrService,
        &__nvoc_rtti_GenericKernelFalcon_KernelFalcon,
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

static NvBool __nvoc_thunk_GenericKernelFalcon_kflcnIsEngineInReset(struct OBJGPU *pGpu, struct KernelFalcon *pGenKernFlcn) {
    return gkflcnIsEngineInReset(pGpu, (struct GenericKernelFalcon *)(((unsigned char *)pGenKernFlcn) - __nvoc_rtti_GenericKernelFalcon_KernelFalcon.offset));
}

static void __nvoc_thunk_GenericKernelFalcon_intrservRegisterIntrService(struct OBJGPU *arg0, struct IntrService *arg1, IntrServiceRecord arg2[155]) {
    gkflcnRegisterIntrService(arg0, (struct GenericKernelFalcon *)(((unsigned char *)arg1) - __nvoc_rtti_GenericKernelFalcon_IntrService.offset), arg2);
}

static NV_STATUS __nvoc_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt(struct OBJGPU *arg0, struct IntrService *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return gkflcnServiceNotificationInterrupt(arg0, (struct GenericKernelFalcon *)(((unsigned char *)arg1) - __nvoc_rtti_GenericKernelFalcon_IntrService.offset), arg2);
}

static NvBool __nvoc_thunk_IntrService_gkflcnClearInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_GenericKernelFalcon_IntrService.offset), pParams);
}

static NvU32 __nvoc_thunk_IntrService_gkflcnServiceInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_GenericKernelFalcon_IntrService.offset), pParams);
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

void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_Object(Object* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner, struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_KernelFalcon;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_IntrService;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_Object;
    __nvoc_init_dataField_GenericKernelFalcon(pThis, pRmhalspecowner);

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

static void __nvoc_init_funcTable_GenericKernelFalcon_1(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    pThis->__gkflcnResetHw__ = &gkflcnResetHw_IMPL;

    pThis->__gkflcnIsEngineInReset__ = &gkflcnIsEngineInReset_IMPL;

    pThis->__gkflcnRegisterIntrService__ = &gkflcnRegisterIntrService_IMPL;

    pThis->__gkflcnServiceNotificationInterrupt__ = &gkflcnServiceNotificationInterrupt_IMPL;

    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_thunk_GenericKernelFalcon_kflcnResetHw;

    pThis->__nvoc_base_KernelFalcon.__kflcnIsEngineInReset__ = &__nvoc_thunk_GenericKernelFalcon_kflcnIsEngineInReset;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_GenericKernelFalcon_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt;

    pThis->__gkflcnClearInterrupt__ = &__nvoc_thunk_IntrService_gkflcnClearInterrupt;

    pThis->__gkflcnServiceInterrupt__ = &__nvoc_thunk_IntrService_gkflcnServiceInterrupt;
}

void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_GenericKernelFalcon_1(pThis, pRmhalspecowner);
}

void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_IntrService(IntrService*, RmHalspecOwner* );
void __nvoc_init_Object(Object*, RmHalspecOwner* );
void __nvoc_init_GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_GenericKernelFalcon = pThis;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    __nvoc_init_Object(&pThis->__nvoc_base_Object, pRmhalspecowner);
    __nvoc_init_funcTable_GenericKernelFalcon(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_GenericKernelFalcon(GenericKernelFalcon **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig) {
    NV_STATUS status;
    Object *pParentObj;
    GenericKernelFalcon *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(GenericKernelFalcon));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(GenericKernelFalcon));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GenericKernelFalcon);

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
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GenericKernelFalcon(GenericKernelFalcon **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pGpu = va_arg(args, struct OBJGPU *);
    KernelFalconEngineConfig * arg_pFalconConfig = va_arg(args, KernelFalconEngineConfig *);

    status = __nvoc_objCreate_GenericKernelFalcon(ppThis, pParent, createFlags, arg_pGpu, arg_pFalconConfig);

    return status;
}

