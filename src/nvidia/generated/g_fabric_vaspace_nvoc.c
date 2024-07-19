#define NVOC_FABRIC_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_fabric_vaspace_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x8c8f3d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FABRIC_VASPACE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

void __nvoc_init_FABRIC_VASPACE(FABRIC_VASPACE*);
void __nvoc_init_funcTable_FABRIC_VASPACE(FABRIC_VASPACE*);
NV_STATUS __nvoc_ctor_FABRIC_VASPACE(FABRIC_VASPACE*);
void __nvoc_init_dataField_FABRIC_VASPACE(FABRIC_VASPACE*);
void __nvoc_dtor_FABRIC_VASPACE(FABRIC_VASPACE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_FABRIC_VASPACE;

static const struct NVOC_RTTI __nvoc_rtti_FABRIC_VASPACE_FABRIC_VASPACE = {
    /*pClassDef=*/          &__nvoc_class_def_FABRIC_VASPACE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_FABRIC_VASPACE,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_FABRIC_VASPACE_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FABRIC_VASPACE, __nvoc_base_OBJVASPACE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJVASPACE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FABRIC_VASPACE, __nvoc_base_OBJVASPACE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_FABRIC_VASPACE = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_FABRIC_VASPACE_FABRIC_VASPACE,
        &__nvoc_rtti_FABRIC_VASPACE_OBJVASPACE,
        &__nvoc_rtti_FABRIC_VASPACE_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_FABRIC_VASPACE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(FABRIC_VASPACE),
        /*classId=*/            classId(FABRIC_VASPACE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "FABRIC_VASPACE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_FABRIC_VASPACE,
    /*pCastInfo=*/          &__nvoc_castinfo_FABRIC_VASPACE,
    /*pExportInfo=*/        &__nvoc_export_info_FABRIC_VASPACE
};

// 10 down-thunk(s) defined to bridge methods in FABRIC_VASPACE from superclasses

// fabricvaspaceConstruct_: virtual override (vaspace) base (vaspace)
static NV_STATUS __nvoc_down_thunk_FABRIC_VASPACE_vaspaceConstruct_(struct OBJVASPACE *pFabricVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return fabricvaspaceConstruct_((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

// fabricvaspaceAlloc: virtual override (vaspace) base (vaspace)
static NV_STATUS __nvoc_down_thunk_FABRIC_VASPACE_vaspaceAlloc(struct OBJVASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return fabricvaspaceAlloc((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), size, align, rangeLo, rangeHi, pageSize, flags, pAddr);
}

// fabricvaspaceFree: virtual override (vaspace) base (vaspace)
static NV_STATUS __nvoc_down_thunk_FABRIC_VASPACE_vaspaceFree(struct OBJVASPACE *pFabricVAS, NvU64 vAddr) {
    return fabricvaspaceFree((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), vAddr);
}

// fabricvaspaceMap: virtual override (vaspace) base (vaspace)
static NV_STATUS __nvoc_down_thunk_FABRIC_VASPACE_vaspaceMap(struct OBJVASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return fabricvaspaceMap((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu, vaLo, vaHi, pTarget, flags);
}

// fabricvaspaceUnmap: virtual override (vaspace) base (vaspace)
static void __nvoc_down_thunk_FABRIC_VASPACE_vaspaceUnmap(struct OBJVASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    fabricvaspaceUnmap((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu, vaLo, vaHi);
}

// fabricvaspaceApplyDefaultAlignment: virtual override (vaspace) base (vaspace)
static NV_STATUS __nvoc_down_thunk_FABRIC_VASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pFabricVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return fabricvaspaceApplyDefaultAlignment((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

// fabricvaspaceGetVasInfo: virtual override (vaspace) base (vaspace)
static NV_STATUS __nvoc_down_thunk_FABRIC_VASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pFabricVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return fabricvaspaceGetVasInfo((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pParams);
}

// fabricvaspacePinRootPageDir: virtual override (vaspace) base (vaspace)
static NV_STATUS __nvoc_down_thunk_FABRIC_VASPACE_vaspacePinRootPageDir(struct OBJVASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    return fabricvaspacePinRootPageDir((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu);
}

// fabricvaspaceUnpinRootPageDir: virtual override (vaspace) base (vaspace)
static void __nvoc_down_thunk_FABRIC_VASPACE_vaspaceUnpinRootPageDir(struct OBJVASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    fabricvaspaceUnpinRootPageDir((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu);
}

// fabricvaspaceInvalidateTlb: virtual override (vaspace) base (vaspace)
static void __nvoc_down_thunk_FABRIC_VASPACE_vaspaceInvalidateTlb(struct OBJVASPACE *pFabricVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    fabricvaspaceInvalidateTlb((struct FABRIC_VASPACE *)(((unsigned char *) pFabricVAS) - __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu, type);
}


// 19 up-thunk(s) defined to bridge methods in FABRIC_VASPACE to superclasses

// fabricvaspaceIncAllocRefCnt: inline virtual inherited (vaspace) base (vaspace) body
static NV_STATUS __nvoc_up_thunk_OBJVASPACE_fabricvaspaceIncAllocRefCnt(struct FABRIC_VASPACE *pVAS, NvU64 vAddr) {
    return vaspaceIncAllocRefCnt((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), vAddr);
}

// fabricvaspaceGetVaStart: virtual inherited (vaspace) base (vaspace)
static NvU64 __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetVaStart(struct FABRIC_VASPACE *pVAS) {
    return vaspaceGetVaStart((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceGetVaLimit: virtual inherited (vaspace) base (vaspace)
static NvU64 __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetVaLimit(struct FABRIC_VASPACE *pVAS) {
    return vaspaceGetVaLimit((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceGetFlags: inline virtual inherited (vaspace) base (vaspace) body
static NvU32 __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetFlags(struct FABRIC_VASPACE *pVAS) {
    return vaspaceGetFlags((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceReserveMempool: inline virtual inherited (vaspace) base (vaspace) body
static NV_STATUS __nvoc_up_thunk_OBJVASPACE_fabricvaspaceReserveMempool(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return vaspaceReserveMempool((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu, pDevice, size, pageSizeLockMask, flags);
}

// fabricvaspaceGetHeap: inline virtual inherited (vaspace) base (vaspace) body
static OBJEHEAP * __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetHeap(struct FABRIC_VASPACE *pVAS) {
    return vaspaceGetHeap((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceGetMapPageSize: inline virtual inherited (vaspace) base (vaspace) body
static NvU64 __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetMapPageSize(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return vaspaceGetMapPageSize((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu, pMemBlock);
}

// fabricvaspaceGetBigPageSize: inline virtual inherited (vaspace) base (vaspace) body
static NvU64 __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetBigPageSize(struct FABRIC_VASPACE *pVAS) {
    return vaspaceGetBigPageSize((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceIsMirrored: inline virtual inherited (vaspace) base (vaspace) body
static NvBool __nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsMirrored(struct FABRIC_VASPACE *pVAS) {
    return vaspaceIsMirrored((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceIsFaultCapable: inline virtual inherited (vaspace) base (vaspace) body
static NvBool __nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsFaultCapable(struct FABRIC_VASPACE *pVAS) {
    return vaspaceIsFaultCapable((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceIsExternallyOwned: inline virtual inherited (vaspace) base (vaspace) body
static NvBool __nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsExternallyOwned(struct FABRIC_VASPACE *pVAS) {
    return vaspaceIsExternallyOwned((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceIsAtsEnabled: inline virtual inherited (vaspace) base (vaspace) body
static NvBool __nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsAtsEnabled(struct FABRIC_VASPACE *pVAS) {
    return vaspaceIsAtsEnabled((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset));
}

// fabricvaspaceGetPasid: inline virtual inherited (vaspace) base (vaspace) body
static NV_STATUS __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPasid(struct FABRIC_VASPACE *pVAS, NvU32 *pPasid) {
    return vaspaceGetPasid((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pPasid);
}

// fabricvaspaceGetPageDirBase: inline virtual inherited (vaspace) base (vaspace) body
static PMEMORY_DESCRIPTOR __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPageDirBase(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspaceGetPageDirBase((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu);
}

// fabricvaspaceGetKernelPageDirBase: inline virtual inherited (vaspace) base (vaspace) body
static PMEMORY_DESCRIPTOR __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetKernelPageDirBase(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspaceGetKernelPageDirBase((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu);
}

// fabricvaspaceGetPageTableInfo: inline virtual inherited (vaspace) base (vaspace) body
static NV_STATUS __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPageTableInfo(struct FABRIC_VASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return vaspaceGetPageTableInfo((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pParams);
}

// fabricvaspaceGetPteInfo: inline virtual inherited (vaspace) base (vaspace) body
static NV_STATUS __nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPteInfo(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return vaspaceGetPteInfo((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu, pParams, pPhysAddr);
}

// fabricvaspaceSetPteInfo: inline virtual inherited (vaspace) base (vaspace) body
static NV_STATUS __nvoc_up_thunk_OBJVASPACE_fabricvaspaceSetPteInfo(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return vaspaceSetPteInfo((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), pGpu, pParams);
}

// fabricvaspaceFreeV2: inline virtual inherited (vaspace) base (vaspace) body
static NV_STATUS __nvoc_up_thunk_OBJVASPACE_fabricvaspaceFreeV2(struct FABRIC_VASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return vaspaceFreeV2((struct OBJVASPACE *)(((unsigned char *) pVAS) + __nvoc_rtti_FABRIC_VASPACE_OBJVASPACE.offset), vAddr, pSize);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_FABRIC_VASPACE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJVASPACE(OBJVASPACE*);
void __nvoc_dtor_FABRIC_VASPACE(FABRIC_VASPACE *pThis) {
    __nvoc_fabricvaspaceDestruct(pThis);
    __nvoc_dtor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_FABRIC_VASPACE(FABRIC_VASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJVASPACE(OBJVASPACE* );
NV_STATUS __nvoc_ctor_FABRIC_VASPACE(FABRIC_VASPACE *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    if (status != NV_OK) goto __nvoc_ctor_FABRIC_VASPACE_fail_OBJVASPACE;
    __nvoc_init_dataField_FABRIC_VASPACE(pThis);
    goto __nvoc_ctor_FABRIC_VASPACE_exit; // Success

__nvoc_ctor_FABRIC_VASPACE_fail_OBJVASPACE:
__nvoc_ctor_FABRIC_VASPACE_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FABRIC_VASPACE_1(FABRIC_VASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // fabricvaspaceConstruct_ -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceConstruct___ = &fabricvaspaceConstruct__IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceConstruct___ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceConstruct_;

    // fabricvaspaceAlloc -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceAlloc__ = &fabricvaspaceAlloc_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceAlloc__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceAlloc;

    // fabricvaspaceFree -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceFree__ = &fabricvaspaceFree_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceFree__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceFree;

    // fabricvaspaceMap -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceMap__ = &fabricvaspaceMap_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceMap__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceMap;

    // fabricvaspaceUnmap -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceUnmap__ = &fabricvaspaceUnmap_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceUnmap__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceUnmap;

    // fabricvaspaceApplyDefaultAlignment -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceApplyDefaultAlignment__ = &fabricvaspaceApplyDefaultAlignment_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceApplyDefaultAlignment__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceApplyDefaultAlignment;

    // fabricvaspaceGetVasInfo -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceGetVasInfo__ = &fabricvaspaceGetVasInfo_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceGetVasInfo__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceGetVasInfo;

    // fabricvaspacePinRootPageDir -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspacePinRootPageDir__ = &fabricvaspacePinRootPageDir_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspacePinRootPageDir__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspacePinRootPageDir;

    // fabricvaspaceUnpinRootPageDir -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceUnpinRootPageDir__ = &fabricvaspaceUnpinRootPageDir_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceUnpinRootPageDir__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceUnpinRootPageDir;

    // fabricvaspaceInvalidateTlb -- virtual override (vaspace) base (vaspace)
    pThis->__fabricvaspaceInvalidateTlb__ = &fabricvaspaceInvalidateTlb_IMPL;
    pThis->__nvoc_base_OBJVASPACE.__vaspaceInvalidateTlb__ = &__nvoc_down_thunk_FABRIC_VASPACE_vaspaceInvalidateTlb;

    // fabricvaspaceIncAllocRefCnt -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceIncAllocRefCnt__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceIncAllocRefCnt;

    // fabricvaspaceGetVaStart -- virtual inherited (vaspace) base (vaspace)
    pThis->__fabricvaspaceGetVaStart__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetVaStart;

    // fabricvaspaceGetVaLimit -- virtual inherited (vaspace) base (vaspace)
    pThis->__fabricvaspaceGetVaLimit__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetVaLimit;

    // fabricvaspaceGetFlags -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetFlags__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetFlags;

    // fabricvaspaceReserveMempool -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceReserveMempool__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceReserveMempool;

    // fabricvaspaceGetHeap -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetHeap__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetHeap;

    // fabricvaspaceGetMapPageSize -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetMapPageSize__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetMapPageSize;

    // fabricvaspaceGetBigPageSize -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetBigPageSize__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetBigPageSize;

    // fabricvaspaceIsMirrored -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceIsMirrored__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsMirrored;

    // fabricvaspaceIsFaultCapable -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceIsFaultCapable__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsFaultCapable;

    // fabricvaspaceIsExternallyOwned -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceIsExternallyOwned__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsExternallyOwned;

    // fabricvaspaceIsAtsEnabled -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceIsAtsEnabled__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceIsAtsEnabled;

    // fabricvaspaceGetPasid -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetPasid__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPasid;

    // fabricvaspaceGetPageDirBase -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetPageDirBase__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPageDirBase;

    // fabricvaspaceGetKernelPageDirBase -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetKernelPageDirBase__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetKernelPageDirBase;

    // fabricvaspaceGetPageTableInfo -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetPageTableInfo__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPageTableInfo;

    // fabricvaspaceGetPteInfo -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceGetPteInfo__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceGetPteInfo;

    // fabricvaspaceSetPteInfo -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceSetPteInfo__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceSetPteInfo;

    // fabricvaspaceFreeV2 -- inline virtual inherited (vaspace) base (vaspace) body
    pThis->__fabricvaspaceFreeV2__ = &__nvoc_up_thunk_OBJVASPACE_fabricvaspaceFreeV2;
} // End __nvoc_init_funcTable_FABRIC_VASPACE_1 with approximately 39 basic block(s).


// Initialize vtable(s) for 29 virtual method(s).
void __nvoc_init_funcTable_FABRIC_VASPACE(FABRIC_VASPACE *pThis) {

    // Initialize vtable(s) with 29 per-object function pointer(s).
    __nvoc_init_funcTable_FABRIC_VASPACE_1(pThis);
}

void __nvoc_init_OBJVASPACE(OBJVASPACE*);
void __nvoc_init_FABRIC_VASPACE(FABRIC_VASPACE *pThis) {
    pThis->__nvoc_pbase_FABRIC_VASPACE = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJVASPACE = &pThis->__nvoc_base_OBJVASPACE;
    __nvoc_init_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    __nvoc_init_funcTable_FABRIC_VASPACE(pThis);
}

NV_STATUS __nvoc_objCreate_FABRIC_VASPACE(FABRIC_VASPACE **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    FABRIC_VASPACE *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(FABRIC_VASPACE), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(FABRIC_VASPACE));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_FABRIC_VASPACE);

    pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_FABRIC_VASPACE(pThis);
    status = __nvoc_ctor_FABRIC_VASPACE(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_FABRIC_VASPACE_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_FABRIC_VASPACE_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(FABRIC_VASPACE));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_FABRIC_VASPACE(FABRIC_VASPACE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_FABRIC_VASPACE(ppThis, pParent, createFlags);

    return status;
}

