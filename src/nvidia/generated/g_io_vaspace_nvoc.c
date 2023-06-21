#define NVOC_IO_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_io_vaspace_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x28ed9c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJIOVASPACE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

void __nvoc_init_OBJIOVASPACE(OBJIOVASPACE*);
void __nvoc_init_funcTable_OBJIOVASPACE(OBJIOVASPACE*);
NV_STATUS __nvoc_ctor_OBJIOVASPACE(OBJIOVASPACE*);
void __nvoc_init_dataField_OBJIOVASPACE(OBJIOVASPACE*);
void __nvoc_dtor_OBJIOVASPACE(OBJIOVASPACE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJIOVASPACE;

static const struct NVOC_RTTI __nvoc_rtti_OBJIOVASPACE_OBJIOVASPACE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJIOVASPACE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJIOVASPACE,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJIOVASPACE_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJIOVASPACE_OBJVASPACE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJVASPACE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJIOVASPACE = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_OBJIOVASPACE_OBJIOVASPACE,
        &__nvoc_rtti_OBJIOVASPACE_OBJVASPACE,
        &__nvoc_rtti_OBJIOVASPACE_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJIOVASPACE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJIOVASPACE),
        /*classId=*/            classId(OBJIOVASPACE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJIOVASPACE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJIOVASPACE,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJIOVASPACE,
    /*pExportInfo=*/        &__nvoc_export_info_OBJIOVASPACE
};

static NV_STATUS __nvoc_thunk_OBJIOVASPACE_vaspaceConstruct_(struct OBJVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return iovaspaceConstruct_((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

static NV_STATUS __nvoc_thunk_OBJIOVASPACE_vaspaceAlloc(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return iovaspaceAlloc((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

static NV_STATUS __nvoc_thunk_OBJIOVASPACE_vaspaceFree(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return iovaspaceFree((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), vAddr);
}

static NV_STATUS __nvoc_thunk_OBJIOVASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return iovaspaceApplyDefaultAlignment((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

static NV_STATUS __nvoc_thunk_OBJIOVASPACE_vaspaceIncAllocRefCnt(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return iovaspaceIncAllocRefCnt((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), vAddr);
}

static NvU64 __nvoc_thunk_OBJIOVASPACE_vaspaceGetVaStart(struct OBJVASPACE *pVAS) {
    return iovaspaceGetVaStart((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NvU64 __nvoc_thunk_OBJIOVASPACE_vaspaceGetVaLimit(struct OBJVASPACE *pVAS) {
    return iovaspaceGetVaLimit((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NV_STATUS __nvoc_thunk_OBJIOVASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return iovaspaceGetVasInfo((struct OBJIOVASPACE *)(((unsigned char *)pVAS) - __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pParams);
}

static NvBool __nvoc_thunk_OBJVASPACE_iovaspaceIsMirrored(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsMirrored((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJVASPACE_iovaspaceIsExternallyOwned(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsExternallyOwned((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJVASPACE_iovaspaceIsInternalVaRestricted(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsInternalVaRestricted((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NvU32 __nvoc_thunk_OBJVASPACE_iovaspaceGetFlags(struct OBJIOVASPACE *pVAS) {
    return vaspaceGetFlags((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJVASPACE_iovaspaceIsAtsEnabled(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsAtsEnabled((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NvU64 __nvoc_thunk_OBJVASPACE_iovaspaceGetBigPageSize(struct OBJIOVASPACE *pVAS) {
    return vaspaceGetBigPageSize((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspaceGetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return vaspaceGetPteInfo((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu, pParams, pPhysAddr);
}

static PMEMORY_DESCRIPTOR __nvoc_thunk_OBJVASPACE_iovaspaceGetPageDirBase(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspaceGetPageDirBase((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu);
}

static PMEMORY_DESCRIPTOR __nvoc_thunk_OBJVASPACE_iovaspaceGetKernelPageDirBase(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspaceGetKernelPageDirBase((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu);
}

static NvU64 __nvoc_thunk_OBJVASPACE_iovaspaceGetMapPageSize(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return vaspaceGetMapPageSize((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu, pMemBlock);
}

static struct OBJEHEAP *__nvoc_thunk_OBJVASPACE_iovaspaceGetHeap(struct OBJIOVASPACE *pVAS) {
    return vaspaceGetHeap((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static NvBool __nvoc_thunk_OBJVASPACE_iovaspaceIsFaultCapable(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsFaultCapable((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset));
}

static void __nvoc_thunk_OBJVASPACE_iovaspaceUnmap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    vaspaceUnmap((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu, vaLo, vaHi);
}

static void __nvoc_thunk_OBJVASPACE_iovaspaceInvalidateTlb(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    vaspaceInvalidateTlb((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu, type);
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspacePinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspacePinRootPageDir((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu);
}

static void __nvoc_thunk_OBJVASPACE_iovaspaceUnpinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    vaspaceUnpinRootPageDir((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspaceSetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return vaspaceSetPteInfo((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu, pParams);
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspaceFreeV2(struct OBJIOVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return vaspaceFreeV2((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), vAddr, pSize);
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspaceGetPasid(struct OBJIOVASPACE *pVAS, NvU32 *pPasid) {
    return vaspaceGetPasid((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pPasid);
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspaceGetPageTableInfo(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return vaspaceGetPageTableInfo((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspaceReserveMempool(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NvHandle hClient, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return vaspaceReserveMempool((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu, hClient, size, pageSizeLockMask, flags);
}

static NV_STATUS __nvoc_thunk_OBJVASPACE_iovaspaceMap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return vaspaceMap((struct OBJVASPACE *)(((unsigned char *)pVAS) + __nvoc_rtti_OBJIOVASPACE_OBJVASPACE.offset), pGpu, vaLo, vaHi, pTarget, flags);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJIOVASPACE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJVASPACE(OBJVASPACE*);
void __nvoc_dtor_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    __nvoc_iovaspaceDestruct(pThis);
    __nvoc_dtor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJVASPACE(OBJVASPACE* );
NV_STATUS __nvoc_ctor_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    if (status != NV_OK) goto __nvoc_ctor_OBJIOVASPACE_fail_OBJVASPACE;
    __nvoc_init_dataField_OBJIOVASPACE(pThis);
    goto __nvoc_ctor_OBJIOVASPACE_exit; // Success

__nvoc_ctor_OBJIOVASPACE_fail_OBJVASPACE:
__nvoc_ctor_OBJIOVASPACE_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJIOVASPACE_1(OBJIOVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__iovaspaceConstruct___ = &iovaspaceConstruct__IMPL;

    pThis->__iovaspaceAlloc__ = &iovaspaceAlloc_IMPL;

    pThis->__iovaspaceFree__ = &iovaspaceFree_IMPL;

    pThis->__iovaspaceApplyDefaultAlignment__ = &iovaspaceApplyDefaultAlignment_IMPL;

    pThis->__iovaspaceIncAllocRefCnt__ = &iovaspaceIncAllocRefCnt_IMPL;

    pThis->__iovaspaceGetVaStart__ = &iovaspaceGetVaStart_IMPL;

    pThis->__iovaspaceGetVaLimit__ = &iovaspaceGetVaLimit_IMPL;

    pThis->__iovaspaceGetVasInfo__ = &iovaspaceGetVasInfo_IMPL;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceConstruct___ = &__nvoc_thunk_OBJIOVASPACE_vaspaceConstruct_;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceAlloc__ = &__nvoc_thunk_OBJIOVASPACE_vaspaceAlloc;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceFree__ = &__nvoc_thunk_OBJIOVASPACE_vaspaceFree;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceApplyDefaultAlignment__ = &__nvoc_thunk_OBJIOVASPACE_vaspaceApplyDefaultAlignment;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceIncAllocRefCnt__ = &__nvoc_thunk_OBJIOVASPACE_vaspaceIncAllocRefCnt;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetVaStart__ = &__nvoc_thunk_OBJIOVASPACE_vaspaceGetVaStart;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetVaLimit__ = &__nvoc_thunk_OBJIOVASPACE_vaspaceGetVaLimit;

    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetVasInfo__ = &__nvoc_thunk_OBJIOVASPACE_vaspaceGetVasInfo;

    pThis->__iovaspaceIsMirrored__ = &__nvoc_thunk_OBJVASPACE_iovaspaceIsMirrored;

    pThis->__iovaspaceIsExternallyOwned__ = &__nvoc_thunk_OBJVASPACE_iovaspaceIsExternallyOwned;

    pThis->__iovaspaceIsInternalVaRestricted__ = &__nvoc_thunk_OBJVASPACE_iovaspaceIsInternalVaRestricted;

    pThis->__iovaspaceGetFlags__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetFlags;

    pThis->__iovaspaceIsAtsEnabled__ = &__nvoc_thunk_OBJVASPACE_iovaspaceIsAtsEnabled;

    pThis->__iovaspaceGetBigPageSize__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetBigPageSize;

    pThis->__iovaspaceGetPteInfo__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetPteInfo;

    pThis->__iovaspaceGetPageDirBase__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetPageDirBase;

    pThis->__iovaspaceGetKernelPageDirBase__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetKernelPageDirBase;

    pThis->__iovaspaceGetMapPageSize__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetMapPageSize;

    pThis->__iovaspaceGetHeap__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetHeap;

    pThis->__iovaspaceIsFaultCapable__ = &__nvoc_thunk_OBJVASPACE_iovaspaceIsFaultCapable;

    pThis->__iovaspaceUnmap__ = &__nvoc_thunk_OBJVASPACE_iovaspaceUnmap;

    pThis->__iovaspaceInvalidateTlb__ = &__nvoc_thunk_OBJVASPACE_iovaspaceInvalidateTlb;

    pThis->__iovaspacePinRootPageDir__ = &__nvoc_thunk_OBJVASPACE_iovaspacePinRootPageDir;

    pThis->__iovaspaceUnpinRootPageDir__ = &__nvoc_thunk_OBJVASPACE_iovaspaceUnpinRootPageDir;

    pThis->__iovaspaceSetPteInfo__ = &__nvoc_thunk_OBJVASPACE_iovaspaceSetPteInfo;

    pThis->__iovaspaceFreeV2__ = &__nvoc_thunk_OBJVASPACE_iovaspaceFreeV2;

    pThis->__iovaspaceGetPasid__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetPasid;

    pThis->__iovaspaceGetPageTableInfo__ = &__nvoc_thunk_OBJVASPACE_iovaspaceGetPageTableInfo;

    pThis->__iovaspaceReserveMempool__ = &__nvoc_thunk_OBJVASPACE_iovaspaceReserveMempool;

    pThis->__iovaspaceMap__ = &__nvoc_thunk_OBJVASPACE_iovaspaceMap;
}

void __nvoc_init_funcTable_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    __nvoc_init_funcTable_OBJIOVASPACE_1(pThis);
}

void __nvoc_init_OBJVASPACE(OBJVASPACE*);
void __nvoc_init_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    pThis->__nvoc_pbase_OBJIOVASPACE = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJVASPACE = &pThis->__nvoc_base_OBJVASPACE;
    __nvoc_init_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    __nvoc_init_funcTable_OBJIOVASPACE(pThis);
}

NV_STATUS __nvoc_objCreate_OBJIOVASPACE(OBJIOVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJIOVASPACE *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJIOVASPACE), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(OBJIOVASPACE));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJIOVASPACE);

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

    __nvoc_init_OBJIOVASPACE(pThis);
    status = __nvoc_ctor_OBJIOVASPACE(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJIOVASPACE_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJIOVASPACE_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJIOVASPACE));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJIOVASPACE(OBJIOVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJIOVASPACE(ppThis, pParent, createFlags);

    return status;
}

