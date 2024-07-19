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
    DispIpHal *dispIpHal = &pRmhalspecowner->dispIpHal;
    const unsigned long dispIpHal_HalVarIdx = (unsigned long)dispIpHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(dispIpHal);
    PORT_UNREFERENCED_VARIABLE(dispIpHal_HalVarIdx);
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

// Vtable initialization
static void __nvoc_init_funcTable_KernelHead_1(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    DispIpHal *dispIpHal = &pRmhalspecowner->dispIpHal;
    const unsigned long dispIpHal_HalVarIdx = (unsigned long)dispIpHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(dispIpHal);
    PORT_UNREFERENCED_VARIABLE(dispIpHal_HalVarIdx);

    // kheadReadVblankIntrEnable -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ))
    {
        pThis->__kheadReadVblankIntrEnable__ = &kheadReadVblankIntrEnable_KERNEL;
    }
    // default
    else
    {
        pThis->__kheadReadVblankIntrEnable__ = &kheadReadVblankIntrEnable_108313;
    }

    // kheadGetDisplayInitialized -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ))
    {
        pThis->__kheadGetDisplayInitialized__ = &kheadGetDisplayInitialized_KERNEL;
    }
    // default
    else
    {
        pThis->__kheadGetDisplayInitialized__ = &kheadGetDisplayInitialized_108313;
    }

    // kheadWriteVblankIntrEnable -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ))
    {
        pThis->__kheadWriteVblankIntrEnable__ = &kheadWriteVblankIntrEnable_KERNEL;
    }
    // default
    else
    {
        pThis->__kheadWriteVblankIntrEnable__ = &kheadWriteVblankIntrEnable_e426af;
    }

    // kheadProcessVblankCallbacks -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ))
    {
        pThis->__kheadProcessVblankCallbacks__ = &kheadProcessVblankCallbacks_IMPL;
    }
    // default
    else
    {
        pThis->__kheadProcessVblankCallbacks__ = &kheadProcessVblankCallbacks_e426af;
    }

    // kheadResetPendingVblank -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ))
    {
        pThis->__kheadResetPendingVblank__ = &kheadResetPendingVblank_KERNEL;
    }
    // default
    else
    {
        pThis->__kheadResetPendingVblank__ = &kheadResetPendingVblank_e426af;
    }

    // kheadReadPendingVblank -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kheadReadPendingVblank__ = &kheadReadPendingVblank_ceaee8;
    }
    else
    {
        pThis->__kheadReadPendingVblank__ = &kheadReadPendingVblank_v03_00;
    }

    // kheadGetLoadVCounter -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kheadGetLoadVCounter__ = &kheadGetLoadVCounter_4a4dee;
    }
    else
    {
        pThis->__kheadGetLoadVCounter__ = &kheadGetLoadVCounter_v03_00;
    }

    // kheadReadPendingRgLineIntr -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kheadReadPendingRgLineIntr__ = &kheadReadPendingRgLineIntr_4a4dee;
    }
    else
    {
        pThis->__kheadReadPendingRgLineIntr__ = &kheadReadPendingRgLineIntr_v03_00;
    }

    // kheadResetRgLineIntrMask -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kheadResetRgLineIntrMask__ = &kheadResetRgLineIntrMask_b3696a;
    }
    else
    {
        pThis->__kheadResetRgLineIntrMask__ = &kheadResetRgLineIntrMask_v03_00;
    }

    // kheadProcessRgLineCallbacks -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ))
    {
        pThis->__kheadProcessRgLineCallbacks__ = &kheadProcessRgLineCallbacks_KERNEL;
    }
    // default
    else
    {
        pThis->__kheadProcessRgLineCallbacks__ = &kheadProcessRgLineCallbacks_ca557d;
    }
} // End __nvoc_init_funcTable_KernelHead_1 with approximately 20 basic block(s).


// Initialize vtable(s) for 10 virtual method(s).
void __nvoc_init_funcTable_KernelHead(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 10 per-object function pointer(s).
    __nvoc_init_funcTable_KernelHead_1(pThis, pRmhalspecowner);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_KernelHead(KernelHead *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelHead = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_KernelHead(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelHead(KernelHead **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelHead *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelHead), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelHead));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelHead);

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelHead_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelHead));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelHead(KernelHead **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelHead(ppThis, pParent, createFlags);

    return status;
}

