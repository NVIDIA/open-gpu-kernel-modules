#define NVOC_KERNEL_HEAD_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_head_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x0145e6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHead;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_KernelHead(KernelHead*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelHead(KernelHead*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelHead(KernelHead*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelHead(KernelHead*, RmHalspecOwner* );
void __nvoc_dtor_KernelHead(KernelHead*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHead;

static const struct NVOC_RTTI __nvoc_rtti_KernelHead_KernelHead = {
    /*pClassDef=*/          &__nvoc_class_def_KernelHead,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHead,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHead_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHead, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelHead = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_KernelHead_KernelHead,
        &__nvoc_rtti_KernelHead_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHead = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelHead),
        /*classId=*/            classId(KernelHead),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelHead",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelHead,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelHead,
    /*pExportInfo=*/        &__nvoc_export_info_KernelHead
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHead = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_KernelHead(KernelHead *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelHead(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_KernelHead(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_KernelHead_fail_Object;
    __nvoc_init_dataField_KernelHead(pThis, pRmhalspecowner);

    status = __nvoc_kheadConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_KernelHead_fail__init;
    goto __nvoc_ctor_KernelHead_exit; // Success

__nvoc_ctor_KernelHead_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_KernelHead_fail_Object:
__nvoc_ctor_KernelHead_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelHead_1(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal function -- kheadProcessVblankCallbacks
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kheadProcessVblankCallbacks__ = &kheadProcessVblankCallbacks_IMPL;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kheadProcessVblankCallbacks__ = &kheadProcessVblankCallbacks_e426af;
        }
    }
    else if (0)
    {
    }

    // Hal function -- kheadResetPendingVblank
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kheadResetPendingVblank__ = &kheadResetPendingVblank_v04_00_KERNEL;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kheadResetPendingVblank__ = &kheadResetPendingVblank_e426af;
        }
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- kheadResetPendingVblankForKernel
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kheadResetPendingVblankForKernel__ = &kheadResetPendingVblankForKernel_v04_00_KERNEL;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kheadResetPendingVblankForKernel__ = &kheadResetPendingVblankForKernel_e426af;
        }
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- kheadReadPendingVblank
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kheadReadPendingVblank__ = &kheadReadPendingVblank_v04_00_KERNEL;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kheadReadPendingVblank__ = &kheadReadPendingVblank_92bfc3;
        }
    }
    else if (0)
    {
    }
}

void __nvoc_init_funcTable_KernelHead(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelHead_1(pThis, pRmhalspecowner);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_KernelHead(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelHead = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_KernelHead(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelHead(KernelHead **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelHead *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelHead));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelHead));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelHead);

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

    __nvoc_init_KernelHead(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelHead(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelHead_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelHead_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelHead(KernelHead **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelHead(ppThis, pParent, createFlags);

    return status;
}

