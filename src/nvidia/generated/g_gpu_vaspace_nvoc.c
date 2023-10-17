#define NVOC_GPU_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_vaspace_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xba5875 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGVASPACE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

void __nvoc_init_OBJGVASPACE(OBJGVASPACE*);
void __nvoc_init_funcTable_OBJGVASPACE(OBJGVASPACE*);
NV_STATUS __nvoc_ctor_OBJGVASPACE(OBJGVASPACE*);
void __nvoc_init_dataField_OBJGVASPACE(OBJGVASPACE*);
void __nvoc_dtor_OBJGVASPACE(OBJGVASPACE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGVASPACE;

static const struct NVOC_RTTI __nvoc_rtti_OBJGVASPACE_OBJGVASPACE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGVASPACE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGVASPACE,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGVASPACE_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGVASPACE_OBJVASPACE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJVASPACE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGVASPACE = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGVASPACE_OBJGVASPACE,
        &__nvoc_rtti_OBJGVASPACE_OBJVASPACE,
        &__nvoc_rtti_OBJGVASPACE_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGVASPACE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGVASPACE),
        /*classId=*/            classId(OBJGVASPACE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGVASPACE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGVASPACE,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGVASPACE,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGVASPACE
};

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceConstruct_(struct OBJVASPACE *pGVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return gvaspaceConstruct_((struct OBJGVASPACE *)(((unsigned char *)pGVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceReserveMempool(struct OBJVASPACE *pGVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return gvaspaceReserveMempool((struct OBJGVASPACE *)(((unsigned char *)pGVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu, pDevice, size, pageSizeLockMask, flags);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceAlloc(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return gvaspaceAlloc((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceFree(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return gvaspaceFree((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), vAddr);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return gvaspaceApplyDefaultAlignment((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceIncAllocRefCnt(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return gvaspaceIncAllocRefCnt((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), vAddr);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceMap(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return gvaspaceMap((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu, vaLo, vaHi, pTarget, flags);
}

static void __nvoc_thunk_OBJGVASPACE_vaspaceUnmap(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    gvaspaceUnmap((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu, vaLo, vaHi);
}

static struct OBJEHEAP *__nvoc_thunk_OBJGVASPACE_vaspaceGetHeap(struct OBJVASPACE *pVAS) {
    return gvaspaceGetHeap((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NvU64 __nvoc_thunk_OBJGVASPACE_vaspaceGetMapPageSize(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return gvaspaceGetMapPageSize((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu, pMemBlock);
}

static NvU64 __nvoc_thunk_OBJGVASPACE_vaspaceGetBigPageSize(struct OBJVASPACE *pVAS) {
    return gvaspaceGetBigPageSize((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NvU32 __nvoc_thunk_OBJGVASPACE_vaspaceGetFlags(struct OBJVASPACE *pVAS) {
    return gvaspaceGetFlags((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJGVASPACE_vaspaceIsMirrored(struct OBJVASPACE *pVAS) {
    return gvaspaceIsMirrored((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJGVASPACE_vaspaceIsFaultCapable(struct OBJVASPACE *pVAS) {
    return gvaspaceIsFaultCapable((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJGVASPACE_vaspaceIsExternallyOwned(struct OBJVASPACE *pVAS) {
    return gvaspaceIsExternallyOwned((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJGVASPACE_vaspaceIsAtsEnabled(struct OBJVASPACE *pVAS) {
    return gvaspaceIsAtsEnabled((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceGetPasid(struct OBJVASPACE *pVAS, NvU32 *pPasid) {
    return gvaspaceGetPasid((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pPasid);
}

static PMEMORY_DESCRIPTOR __nvoc_thunk_OBJGVASPACE_vaspaceGetPageDirBase(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return gvaspaceGetPageDirBase((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu);
}

static PMEMORY_DESCRIPTOR __nvoc_thunk_OBJGVASPACE_vaspaceGetKernelPageDirBase(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return gvaspaceGetKernelPageDirBase((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspacePinRootPageDir(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return gvaspacePinRootPageDir((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu);
}

static void __nvoc_thunk_OBJGVASPACE_vaspaceUnpinRootPageDir(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    gvaspaceUnpinRootPageDir((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu);
}

static void __nvoc_thunk_OBJGVASPACE_vaspaceInvalidateTlb(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    gvaspaceInvalidateTlb((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu, type);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return gvaspaceGetVasInfo((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceGetPageTableInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return gvaspaceGetPageTableInfo((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceGetPteInfo(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return gvaspaceGetPteInfo((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu, pParams, pPhysAddr);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceSetPteInfo(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return gvaspaceSetPteInfo((struct OBJGVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), pGpu, pParams);
}

static NV_STATUS __nvoc_thunk_OBJGVASPACE_vaspaceFreeV2(struct OBJVASPACE *pGVAS, NvU64 vAddr, NvU64 *pSize) {
    return gvaspaceFreeV2((struct OBJGVASPACE *)(((unsigned char *)pGVAS) - __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset), vAddr, pSize);
}

static NvU64 __nvoc_thunk_OBJVASPACE_gvaspaceGetVaLimit(struct OBJGVASPACE *pVAS) {
    return vaspaceGetVaLimit((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

static NvU64 __nvoc_thunk_OBJVASPACE_gvaspaceGetVaStart(struct OBJGVASPACE *pVAS) {
    return vaspaceGetVaStart((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJGVASPACE_OBJVASPACE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGVASPACE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJVASPACE(OBJVASPACE*);
void __nvoc_dtor_OBJGVASPACE(OBJGVASPACE *pThis) {
    __nvoc_gvaspaceDestruct(pThis);
    __nvoc_dtor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGVASPACE(OBJGVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJVASPACE(OBJVASPACE* );
NV_STATUS __nvoc_ctor_OBJGVASPACE(OBJGVASPACE *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    if (status != NV_OK) goto __nvoc_ctor_OBJGVASPACE_fail_OBJVASPACE;
    __nvoc_init_dataField_OBJGVASPACE(pThis);
    goto __nvoc_ctor_OBJGVASPACE_exit; // Success

__nvoc_ctor_OBJGVASPACE_fail_OBJVASPACE:
__nvoc_ctor_OBJGVASPACE_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJGVASPACE_1(OBJGVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__gvaspaceConstruct___ = &gvaspaceConstruct__IMPL;

    pThis->__gvaspaceReserveMempool__ = &gvaspaceReserveMempool_IMPL;

    pThis->__gvaspaceAlloc__ = &gvaspaceAlloc_IMPL;

    pThis->__gvaspaceFree__ = &gvaspaceFree_IMPL;

    pThis->__gvaspaceApplyDefaultAlignment__ = &gvaspaceApplyDefaultAlignment_IMPL;

    pThis->__gvaspaceIncAllocRefCnt__ = &gvaspaceIncAllocRefCnt_IMPL;

    pThis->__gvaspaceMap__ = &gvaspaceMap_IMPL;

    pThis->__gvaspaceUnmap__ = &gvaspaceUnmap_IMPL;

    pThis->__gvaspaceGetHeap__ = &gvaspaceGetHeap_IMPL;

    pThis->__gvaspaceGetMapPageSize__ = &gvaspaceGetMapPageSize_IMPL;

    pThis->__gvaspaceGetBigPageSize__ = &gvaspaceGetBigPageSize_IMPL;

    pThis->__gvaspaceGetFlags__ = &gvaspaceGetFlags_IMPL;

    pThis->__gvaspaceIsMirrored__ = &gvaspaceIsMirrored_IMPL;

    pThis->__gvaspaceIsFaultCapable__ = &gvaspaceIsFaultCapable_IMPL;

    pThis->__gvaspaceIsExternallyOwned__ = &gvaspaceIsExternallyOwned_IMPL;

    pThis->__gvaspaceIsAtsEnabled__ = &gvaspaceIsAtsEnabled_IMPL;

    pThis->__gvaspaceGetPasid__ = &gvaspaceGetPasid_IMPL;

    pThis->__gvaspaceGetPageDirBase__ = &gvaspaceGetPageDirBase_IMPL;

    pThis->__gvaspaceGetKernelPageDirBase__ = &gvaspaceGetKernelPageDirBase_IMPL;

    pThis->__gvaspacePinRootPageDir__ = &gvaspacePinRootPageDir_IMPL;

    pThis->__gvaspaceUnpinRootPageDir__ = &gvaspaceUnpinRootPageDir_IMPL;

    pThis->__gvaspaceInvalidateTlb__ = &gvaspaceInvalidateTlb_IMPL;

    pThis->__gvaspaceGetVasInfo__ = &gvaspaceGetVasInfo_IMPL;

    pThis->__gvaspaceGetPageTableInfo__ = &gvaspaceGetPageTableInfo_IMPL;

    pThis->__gvaspaceGetPteInfo__ = &gvaspaceGetPteInfo_IMPL;

    pThis->__gvaspaceSetPteInfo__ = &gvaspaceSetPteInfo_IMPL;

    pThis->__gvaspaceFreeV2__ = &gvaspaceFreeV2_IMPL;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceConstruct___ = &__nvoc_thunk_OBJGVASPACE_vaspaceConstruct_;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceReserveMempool__ = &__nvoc_thunk_OBJGVASPACE_vaspaceReserveMempool;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceAlloc__ = &__nvoc_thunk_OBJGVASPACE_vaspaceAlloc;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceFree__ = &__nvoc_thunk_OBJGVASPACE_vaspaceFree;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceApplyDefaultAlignment__ = &__nvoc_thunk_OBJGVASPACE_vaspaceApplyDefaultAlignment;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceIncAllocRefCnt__ = &__nvoc_thunk_OBJGVASPACE_vaspaceIncAllocRefCnt;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceMap__ = &__nvoc_thunk_OBJGVASPACE_vaspaceMap;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceUnmap__ = &__nvoc_thunk_OBJGVASPACE_vaspaceUnmap;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetHeap__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetHeap;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetMapPageSize__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetMapPageSize;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetBigPageSize__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetBigPageSize;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetFlags__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetFlags;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceIsMirrored__ = &__nvoc_thunk_OBJGVASPACE_vaspaceIsMirrored;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceIsFaultCapable__ = &__nvoc_thunk_OBJGVASPACE_vaspaceIsFaultCapable;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceIsExternallyOwned__ = &__nvoc_thunk_OBJGVASPACE_vaspaceIsExternallyOwned;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceIsAtsEnabled__ = &__nvoc_thunk_OBJGVASPACE_vaspaceIsAtsEnabled;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetPasid__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetPasid;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetPageDirBase__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetPageDirBase;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetKernelPageDirBase__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetKernelPageDirBase;

    pThis->__nvoc_base_OBJVASPACE.__vaspacePinRootPageDir__ = &__nvoc_thunk_OBJGVASPACE_vaspacePinRootPageDir;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceUnpinRootPageDir__ = &__nvoc_thunk_OBJGVASPACE_vaspaceUnpinRootPageDir;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceInvalidateTlb__ = &__nvoc_thunk_OBJGVASPACE_vaspaceInvalidateTlb;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetVasInfo__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetVasInfo;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetPageTableInfo__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetPageTableInfo;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetPteInfo__ = &__nvoc_thunk_OBJGVASPACE_vaspaceGetPteInfo;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceSetPteInfo__ = &__nvoc_thunk_OBJGVASPACE_vaspaceSetPteInfo;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceFreeV2__ = &__nvoc_thunk_OBJGVASPACE_vaspaceFreeV2;

    pThis->__gvaspaceGetVaLimit__ = &__nvoc_thunk_OBJVASPACE_gvaspaceGetVaLimit;

    pThis->__gvaspaceGetVaStart__ = &__nvoc_thunk_OBJVASPACE_gvaspaceGetVaStart;
}

void __nvoc_init_funcTable_OBJGVASPACE(OBJGVASPACE *pThis) {
    __nvoc_init_funcTable_OBJGVASPACE_1(pThis);
}

void __nvoc_init_OBJVASPACE(OBJVASPACE*);
void __nvoc_init_OBJGVASPACE(OBJGVASPACE *pThis) {
    pThis->__nvoc_pbase_OBJGVASPACE = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJVASPACE = &pThis->__nvoc_base_OBJVASPACE;
    __nvoc_init_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    __nvoc_init_funcTable_OBJGVASPACE(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGVASPACE(OBJGVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJGVASPACE *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGVASPACE), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(OBJGVASPACE));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGVASPACE);

    pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJGVASPACE(pThis);
    status = __nvoc_ctor_OBJGVASPACE(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGVASPACE_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGVASPACE_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGVASPACE));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGVASPACE(OBJGVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGVASPACE(ppThis, pParent, createFlags);

    return status;
}

