#define NVOC_GPU_VASPACE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_vaspace_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xba5875 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGVASPACE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

// Forward declarations for OBJGVASPACE
void __nvoc_init__OBJVASPACE(OBJVASPACE*);
void __nvoc_init__OBJGVASPACE(OBJGVASPACE*);
void __nvoc_init_funcTable_OBJGVASPACE(OBJGVASPACE*);
NV_STATUS __nvoc_ctor_OBJGVASPACE(OBJGVASPACE*);
void __nvoc_init_dataField_OBJGVASPACE(OBJGVASPACE*);
void __nvoc_dtor_OBJGVASPACE(OBJGVASPACE*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJGVASPACE;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGVASPACE;

// Down-thunk(s) to bridge OBJGVASPACE methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceConstruct_(struct OBJVASPACE *pGVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceReserveMempool(struct OBJVASPACE *pGVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceAlloc(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceFree(struct OBJVASPACE *pVAS, NvU64 vAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceIncAllocRefCnt(struct OBJVASPACE *pVAS, NvU64 vAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceMap(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags);    // this
void __nvoc_down_thunk_OBJGVASPACE_vaspaceUnmap(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi);    // this
OBJEHEAP * __nvoc_down_thunk_OBJGVASPACE_vaspaceGetHeap(struct OBJVASPACE *pVAS);    // this
NvU64 __nvoc_down_thunk_OBJGVASPACE_vaspaceGetMapPageSize(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock);    // this
NvU64 __nvoc_down_thunk_OBJGVASPACE_vaspaceGetBigPageSize(struct OBJVASPACE *pVAS);    // this
NvU32 __nvoc_down_thunk_OBJGVASPACE_vaspaceGetFlags(struct OBJVASPACE *pVAS);    // this
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsMirrored(struct OBJVASPACE *pVAS);    // this
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsFaultCapable(struct OBJVASPACE *pVAS);    // this
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsExternallyOwned(struct OBJVASPACE *pVAS);    // this
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsAtsEnabled(struct OBJVASPACE *pVAS);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPasid(struct OBJVASPACE *pVAS, NvU32 *pPasid);    // this
PMEMORY_DESCRIPTOR __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPageDirBase(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu);    // this
PMEMORY_DESCRIPTOR __nvoc_down_thunk_OBJGVASPACE_vaspaceGetKernelPageDirBase(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspacePinRootPageDir(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu);    // this
void __nvoc_down_thunk_OBJGVASPACE_vaspaceUnpinRootPageDir(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu);    // this
void __nvoc_down_thunk_OBJGVASPACE_vaspaceInvalidateTlb(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPageTableInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPteInfo(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceSetPteInfo(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams);    // this
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceFreeV2(struct OBJVASPACE *pGVAS, NvU64 vAddr, NvU64 *pSize);    // this

// Up-thunk(s) to bridge OBJGVASPACE methods to ancestors (if any)
NvU64 __nvoc_up_thunk_OBJVASPACE_gvaspaceGetVaStart(struct OBJGVASPACE *pVAS);    // this
NvU64 __nvoc_up_thunk_OBJVASPACE_gvaspaceGetVaLimit(struct OBJGVASPACE *pVAS);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__OBJGVASPACE,
    /*pExportInfo=*/        &__nvoc_export_info__OBJGVASPACE
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJGVASPACE __nvoc_metadata__OBJGVASPACE = {
    .rtti.pClassDef = &__nvoc_class_def_OBJGVASPACE,    // (gvaspace) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGVASPACE,
    .rtti.offset    = 0,
    .metadata__OBJVASPACE.rtti.pClassDef = &__nvoc_class_def_OBJVASPACE,    // (vaspace) super
    .metadata__OBJVASPACE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJVASPACE.rtti.offset    = NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE),
    .metadata__OBJVASPACE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJVASPACE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJVASPACE.metadata__Object.rtti.offset    = NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE.__nvoc_base_Object),

    .vtable.__gvaspaceConstruct___ = &gvaspaceConstruct__IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceConstruct___ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceConstruct_,    // pure virtual
    .vtable.__gvaspaceReserveMempool__ = &gvaspaceReserveMempool_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceReserveMempool__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceReserveMempool,    // inline virtual body
    .vtable.__gvaspaceAlloc__ = &gvaspaceAlloc_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceAlloc__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceAlloc,    // pure virtual
    .vtable.__gvaspaceFree__ = &gvaspaceFree_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceFree__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceFree,    // pure virtual
    .vtable.__gvaspaceApplyDefaultAlignment__ = &gvaspaceApplyDefaultAlignment_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceApplyDefaultAlignment__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceApplyDefaultAlignment,    // pure virtual
    .vtable.__gvaspaceIncAllocRefCnt__ = &gvaspaceIncAllocRefCnt_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceIncAllocRefCnt__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceIncAllocRefCnt,    // inline virtual body
    .vtable.__gvaspaceMap__ = &gvaspaceMap_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceMap__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceMap,    // inline virtual body
    .vtable.__gvaspaceUnmap__ = &gvaspaceUnmap_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceUnmap__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceUnmap,    // inline virtual body
    .vtable.__gvaspaceGetHeap__ = &gvaspaceGetHeap_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetHeap__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetHeap,    // inline virtual body
    .vtable.__gvaspaceGetMapPageSize__ = &gvaspaceGetMapPageSize_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetMapPageSize__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetMapPageSize,    // inline virtual body
    .vtable.__gvaspaceGetBigPageSize__ = &gvaspaceGetBigPageSize_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetBigPageSize__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetBigPageSize,    // inline virtual body
    .vtable.__gvaspaceGetFlags__ = &gvaspaceGetFlags_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetFlags__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetFlags,    // inline virtual body
    .vtable.__gvaspaceIsMirrored__ = &gvaspaceIsMirrored_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceIsMirrored__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceIsMirrored,    // inline virtual body
    .vtable.__gvaspaceIsFaultCapable__ = &gvaspaceIsFaultCapable_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceIsFaultCapable__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceIsFaultCapable,    // inline virtual body
    .vtable.__gvaspaceIsExternallyOwned__ = &gvaspaceIsExternallyOwned_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceIsExternallyOwned__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceIsExternallyOwned,    // inline virtual body
    .vtable.__gvaspaceIsAtsEnabled__ = &gvaspaceIsAtsEnabled_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceIsAtsEnabled__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceIsAtsEnabled,    // inline virtual body
    .vtable.__gvaspaceGetPasid__ = &gvaspaceGetPasid_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetPasid__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetPasid,    // inline virtual body
    .vtable.__gvaspaceGetPageDirBase__ = &gvaspaceGetPageDirBase_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetPageDirBase__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetPageDirBase,    // inline virtual body
    .vtable.__gvaspaceGetKernelPageDirBase__ = &gvaspaceGetKernelPageDirBase_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetKernelPageDirBase__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetKernelPageDirBase,    // inline virtual body
    .vtable.__gvaspacePinRootPageDir__ = &gvaspacePinRootPageDir_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspacePinRootPageDir__ = &__nvoc_down_thunk_OBJGVASPACE_vaspacePinRootPageDir,    // inline virtual body
    .vtable.__gvaspaceUnpinRootPageDir__ = &gvaspaceUnpinRootPageDir_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceUnpinRootPageDir__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceUnpinRootPageDir,    // inline virtual body
    .vtable.__gvaspaceInvalidateTlb__ = &gvaspaceInvalidateTlb_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceInvalidateTlb__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceInvalidateTlb,    // virtual
    .vtable.__gvaspaceGetVasInfo__ = &gvaspaceGetVasInfo_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetVasInfo__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetVasInfo,    // pure virtual
    .vtable.__gvaspaceGetPageTableInfo__ = &gvaspaceGetPageTableInfo_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetPageTableInfo__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetPageTableInfo,    // inline virtual body
    .vtable.__gvaspaceGetPteInfo__ = &gvaspaceGetPteInfo_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetPteInfo__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceGetPteInfo,    // inline virtual body
    .vtable.__gvaspaceSetPteInfo__ = &gvaspaceSetPteInfo_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceSetPteInfo__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceSetPteInfo,    // inline virtual body
    .vtable.__gvaspaceFreeV2__ = &gvaspaceFreeV2_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceFreeV2__ = &__nvoc_down_thunk_OBJGVASPACE_vaspaceFreeV2,    // inline virtual body
    .vtable.__gvaspaceGetVaStart__ = &__nvoc_up_thunk_OBJVASPACE_gvaspaceGetVaStart,    // virtual inherited (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetVaStart__ = &vaspaceGetVaStart_IMPL,    // virtual
    .vtable.__gvaspaceGetVaLimit__ = &__nvoc_up_thunk_OBJVASPACE_gvaspaceGetVaLimit,    // virtual inherited (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetVaLimit__ = &vaspaceGetVaLimit_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJGVASPACE = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__OBJGVASPACE.rtti,    // [0]: (gvaspace) this
        &__nvoc_metadata__OBJGVASPACE.metadata__OBJVASPACE.rtti,    // [1]: (vaspace) super
        &__nvoc_metadata__OBJGVASPACE.metadata__OBJVASPACE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 27 down-thunk(s) defined to bridge methods in OBJGVASPACE from superclasses

// gvaspaceConstruct_: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceConstruct_(struct OBJVASPACE *pGVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return gvaspaceConstruct_((struct OBJGVASPACE *)(((unsigned char *) pGVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

// gvaspaceReserveMempool: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceReserveMempool(struct OBJVASPACE *pGVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return gvaspaceReserveMempool((struct OBJGVASPACE *)(((unsigned char *) pGVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pDevice, size, pageSizeLockMask, flags);
}

// gvaspaceAlloc: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceAlloc(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return gvaspaceAlloc((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

// gvaspaceFree: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceFree(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return gvaspaceFree((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), vAddr);
}

// gvaspaceApplyDefaultAlignment: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return gvaspaceApplyDefaultAlignment((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

// gvaspaceIncAllocRefCnt: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceIncAllocRefCnt(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return gvaspaceIncAllocRefCnt((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), vAddr);
}

// gvaspaceMap: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceMap(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return gvaspaceMap((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu, vaLo, vaHi, pTarget, flags);
}

// gvaspaceUnmap: virtual override (vaspace) base (vaspace)
void __nvoc_down_thunk_OBJGVASPACE_vaspaceUnmap(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    gvaspaceUnmap((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu, vaLo, vaHi);
}

// gvaspaceGetHeap: virtual override (vaspace) base (vaspace)
OBJEHEAP * __nvoc_down_thunk_OBJGVASPACE_vaspaceGetHeap(struct OBJVASPACE *pVAS) {
    return gvaspaceGetHeap((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceGetMapPageSize: virtual override (vaspace) base (vaspace)
NvU64 __nvoc_down_thunk_OBJGVASPACE_vaspaceGetMapPageSize(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return gvaspaceGetMapPageSize((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pMemBlock);
}

// gvaspaceGetBigPageSize: virtual override (vaspace) base (vaspace)
NvU64 __nvoc_down_thunk_OBJGVASPACE_vaspaceGetBigPageSize(struct OBJVASPACE *pVAS) {
    return gvaspaceGetBigPageSize((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceGetFlags: virtual override (vaspace) base (vaspace)
NvU32 __nvoc_down_thunk_OBJGVASPACE_vaspaceGetFlags(struct OBJVASPACE *pVAS) {
    return gvaspaceGetFlags((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceIsMirrored: virtual override (vaspace) base (vaspace)
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsMirrored(struct OBJVASPACE *pVAS) {
    return gvaspaceIsMirrored((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceIsFaultCapable: virtual override (vaspace) base (vaspace)
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsFaultCapable(struct OBJVASPACE *pVAS) {
    return gvaspaceIsFaultCapable((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceIsExternallyOwned: virtual override (vaspace) base (vaspace)
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsExternallyOwned(struct OBJVASPACE *pVAS) {
    return gvaspaceIsExternallyOwned((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceIsAtsEnabled: virtual override (vaspace) base (vaspace)
NvBool __nvoc_down_thunk_OBJGVASPACE_vaspaceIsAtsEnabled(struct OBJVASPACE *pVAS) {
    return gvaspaceIsAtsEnabled((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceGetPasid: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPasid(struct OBJVASPACE *pVAS, NvU32 *pPasid) {
    return gvaspaceGetPasid((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pPasid);
}

// gvaspaceGetPageDirBase: virtual override (vaspace) base (vaspace)
PMEMORY_DESCRIPTOR __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPageDirBase(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return gvaspaceGetPageDirBase((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
}

// gvaspaceGetKernelPageDirBase: virtual override (vaspace) base (vaspace)
PMEMORY_DESCRIPTOR __nvoc_down_thunk_OBJGVASPACE_vaspaceGetKernelPageDirBase(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return gvaspaceGetKernelPageDirBase((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
}

// gvaspacePinRootPageDir: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspacePinRootPageDir(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return gvaspacePinRootPageDir((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
}

// gvaspaceUnpinRootPageDir: virtual override (vaspace) base (vaspace)
void __nvoc_down_thunk_OBJGVASPACE_vaspaceUnpinRootPageDir(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    gvaspaceUnpinRootPageDir((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
}

// gvaspaceInvalidateTlb: virtual override (vaspace) base (vaspace)
void __nvoc_down_thunk_OBJGVASPACE_vaspaceInvalidateTlb(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    gvaspaceInvalidateTlb((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu, type);
}

// gvaspaceGetVasInfo: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return gvaspaceGetVasInfo((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pParams);
}

// gvaspaceGetPageTableInfo: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPageTableInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return gvaspaceGetPageTableInfo((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pParams);
}

// gvaspaceGetPteInfo: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceGetPteInfo(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return gvaspaceGetPteInfo((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pParams, pPhysAddr);
}

// gvaspaceSetPteInfo: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceSetPteInfo(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return gvaspaceSetPteInfo((struct OBJGVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pParams);
}

// gvaspaceFreeV2: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJGVASPACE_vaspaceFreeV2(struct OBJVASPACE *pGVAS, NvU64 vAddr, NvU64 *pSize) {
    return gvaspaceFreeV2((struct OBJGVASPACE *)(((unsigned char *) pGVAS) - NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)), vAddr, pSize);
}


// 2 up-thunk(s) defined to bridge methods in OBJGVASPACE to superclasses

// gvaspaceGetVaStart: virtual inherited (vaspace) base (vaspace)
NvU64 __nvoc_up_thunk_OBJVASPACE_gvaspaceGetVaStart(struct OBJGVASPACE *pVAS) {
    return vaspaceGetVaStart((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}

// gvaspaceGetVaLimit: virtual inherited (vaspace) base (vaspace)
NvU64 __nvoc_up_thunk_OBJVASPACE_gvaspaceGetVaLimit(struct OBJGVASPACE *pVAS) {
    return vaspaceGetVaLimit((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJGVASPACE, __nvoc_base_OBJVASPACE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGVASPACE = 
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

// Vtable initialization
static void __nvoc_init_funcTable_OBJGVASPACE_1(OBJGVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGVASPACE_1


// Initialize vtable(s) for 29 virtual method(s).
void __nvoc_init_funcTable_OBJGVASPACE(OBJGVASPACE *pThis) {
    __nvoc_init_funcTable_OBJGVASPACE_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJGVASPACE(OBJGVASPACE *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJVASPACE = &pThis->__nvoc_base_OBJVASPACE;    // (vaspace) super
    pThis->__nvoc_pbase_OBJGVASPACE = pThis;    // (gvaspace) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGVASPACE.metadata__OBJVASPACE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGVASPACE.metadata__OBJVASPACE;    // (vaspace) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJGVASPACE;    // (gvaspace) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJGVASPACE(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGVASPACE(OBJGVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJGVASPACE *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGVASPACE), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJGVASPACE));

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

    __nvoc_init__OBJGVASPACE(pThis);
    status = __nvoc_ctor_OBJGVASPACE(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGVASPACE_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGVASPACE_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGVASPACE));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGVASPACE(OBJGVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGVASPACE(ppThis, pParent, createFlags);

    return status;
}

