#define NVOC_IO_VASPACE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_io_vaspace_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__28ed9c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJIOVASPACE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

// Forward declarations for OBJIOVASPACE
void __nvoc_init__OBJVASPACE(OBJVASPACE*);
void __nvoc_init__OBJIOVASPACE(OBJIOVASPACE*);
void __nvoc_init_funcTable_OBJIOVASPACE(OBJIOVASPACE*);
NV_STATUS __nvoc_ctor_OBJIOVASPACE(OBJIOVASPACE*);
void __nvoc_init_dataField_OBJIOVASPACE(OBJIOVASPACE*);
void __nvoc_dtor_OBJIOVASPACE(OBJIOVASPACE*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJIOVASPACE;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJIOVASPACE;

// Down-thunk(s) to bridge OBJIOVASPACE methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceConstruct_(struct OBJVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceAlloc(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceFree(struct OBJVASPACE *pVAS, NvU64 vAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceIncAllocRefCnt(struct OBJVASPACE *pVAS, NvU64 vAddr);    // this
NvU64 __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaStart(struct OBJVASPACE *pVAS);    // this
NvU64 __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaLimit(struct OBJVASPACE *pVAS);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);    // this

// Up-thunk(s) to bridge OBJIOVASPACE methods to ancestors (if any)
NvU32 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetFlags(struct OBJIOVASPACE *pVAS);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceMap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags);    // this
void __nvoc_up_thunk_OBJVASPACE_iovaspaceUnmap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceReserveMempool(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags);    // this
OBJEHEAP * __nvoc_up_thunk_OBJVASPACE_iovaspaceGetHeap(struct OBJIOVASPACE *pVAS);    // this
NvU64 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetMapPageSize(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock);    // this
NvU64 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetBigPageSize(struct OBJIOVASPACE *pVAS);    // this
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsFaultCapable(struct OBJIOVASPACE *pVAS);    // this
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsExternallyOwned(struct OBJIOVASPACE *pVAS);    // this
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsAtsEnabled(struct OBJIOVASPACE *pVAS);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPasid(struct OBJIOVASPACE *pVAS, NvU32 *pPasid);    // this
PMEMORY_DESCRIPTOR __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageDirBase(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspacePinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu);    // this
void __nvoc_up_thunk_OBJVASPACE_iovaspaceUnpinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu);    // this
void __nvoc_up_thunk_OBJVASPACE_iovaspaceInvalidateTlb(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageTableInfo(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceSetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceFreeV2(struct OBJIOVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize);    // this

// Class-specific details for OBJIOVASPACE
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJIOVASPACE = 
{
    .classInfo.size =               sizeof(OBJIOVASPACE),
    .classInfo.classId =            classId(OBJIOVASPACE),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJIOVASPACE",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJIOVASPACE,
    .pCastInfo =          &__nvoc_castinfo__OBJIOVASPACE,
    .pExportInfo =        &__nvoc_export_info__OBJIOVASPACE
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJIOVASPACE __nvoc_metadata__OBJIOVASPACE = {
    .rtti.pClassDef = &__nvoc_class_def_OBJIOVASPACE,    // (iovaspace) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJIOVASPACE,
    .rtti.offset    = 0,
    .metadata__OBJVASPACE.rtti.pClassDef = &__nvoc_class_def_OBJVASPACE,    // (vaspace) super
    .metadata__OBJVASPACE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJVASPACE.rtti.offset    = NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE),
    .metadata__OBJVASPACE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJVASPACE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJVASPACE.metadata__Object.rtti.offset    = NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE.__nvoc_base_Object),

    .vtable.__iovaspaceConstruct___ = &iovaspaceConstruct__IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceConstruct___ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceConstruct_,    // pure virtual
    .vtable.__iovaspaceAlloc__ = &iovaspaceAlloc_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceAlloc__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceAlloc,    // pure virtual
    .vtable.__iovaspaceFree__ = &iovaspaceFree_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceFree__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceFree,    // pure virtual
    .vtable.__iovaspaceApplyDefaultAlignment__ = &iovaspaceApplyDefaultAlignment_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceApplyDefaultAlignment__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceApplyDefaultAlignment,    // pure virtual
    .vtable.__iovaspaceIncAllocRefCnt__ = &iovaspaceIncAllocRefCnt_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceIncAllocRefCnt__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceIncAllocRefCnt,    // inline virtual body
    .vtable.__iovaspaceGetVaStart__ = &iovaspaceGetVaStart_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetVaStart__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaStart,    // virtual
    .vtable.__iovaspaceGetVaLimit__ = &iovaspaceGetVaLimit_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetVaLimit__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaLimit,    // virtual
    .vtable.__iovaspaceGetVasInfo__ = &iovaspaceGetVasInfo_IMPL,    // virtual override (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceGetVasInfo__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVasInfo,    // pure virtual
    .vtable.__iovaspaceGetFlags__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetFlags,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetFlags__ = &vaspaceGetFlags_edd98b,    // inline virtual body
    .vtable.__iovaspaceMap__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceMap,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceMap__ = &vaspaceMap_14ee5e,    // inline virtual body
    .vtable.__iovaspaceUnmap__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceUnmap,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceUnmap__ = &vaspaceUnmap_af5be7,    // inline virtual body
    .vtable.__iovaspaceReserveMempool__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceReserveMempool,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceReserveMempool__ = &vaspaceReserveMempool_ac1694,    // inline virtual body
    .vtable.__iovaspaceGetHeap__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetHeap,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetHeap__ = &vaspaceGetHeap_9451a7,    // inline virtual body
    .vtable.__iovaspaceGetMapPageSize__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetMapPageSize,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetMapPageSize__ = &vaspaceGetMapPageSize_c26fae,    // inline virtual body
    .vtable.__iovaspaceGetBigPageSize__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetBigPageSize,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetBigPageSize__ = &vaspaceGetBigPageSize_c26fae,    // inline virtual body
    .vtable.__iovaspaceIsFaultCapable__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceIsFaultCapable,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceIsFaultCapable__ = &vaspaceIsFaultCapable_2fa1ff,    // inline virtual body
    .vtable.__iovaspaceIsExternallyOwned__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceIsExternallyOwned,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceIsExternallyOwned__ = &vaspaceIsExternallyOwned_2fa1ff,    // inline virtual body
    .vtable.__iovaspaceIsAtsEnabled__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceIsAtsEnabled,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceIsAtsEnabled__ = &vaspaceIsAtsEnabled_2fa1ff,    // inline virtual body
    .vtable.__iovaspaceGetPasid__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPasid,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetPasid__ = &vaspaceGetPasid_14ee5e,    // inline virtual body
    .vtable.__iovaspaceGetPageDirBase__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageDirBase,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetPageDirBase__ = &vaspaceGetPageDirBase_9451a7,    // inline virtual body
    .vtable.__iovaspacePinRootPageDir__ = &__nvoc_up_thunk_OBJVASPACE_iovaspacePinRootPageDir,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspacePinRootPageDir__ = &vaspacePinRootPageDir_14ee5e,    // inline virtual body
    .vtable.__iovaspaceUnpinRootPageDir__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceUnpinRootPageDir,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceUnpinRootPageDir__ = &vaspaceUnpinRootPageDir_af5be7,    // inline virtual body
    .vtable.__iovaspaceInvalidateTlb__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceInvalidateTlb,    // virtual inherited (vaspace) base (vaspace)
    .metadata__OBJVASPACE.vtable.__vaspaceInvalidateTlb__ = &vaspaceInvalidateTlb_IMPL,    // virtual
    .vtable.__iovaspaceGetPageTableInfo__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageTableInfo,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetPageTableInfo__ = &vaspaceGetPageTableInfo_14ee5e,    // inline virtual body
    .vtable.__iovaspaceGetPteInfo__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPteInfo,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceGetPteInfo__ = &vaspaceGetPteInfo_14ee5e,    // inline virtual body
    .vtable.__iovaspaceSetPteInfo__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceSetPteInfo,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceSetPteInfo__ = &vaspaceSetPteInfo_14ee5e,    // inline virtual body
    .vtable.__iovaspaceFreeV2__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceFreeV2,    // inline virtual inherited (vaspace) base (vaspace) body
    .metadata__OBJVASPACE.vtable.__vaspaceFreeV2__ = &vaspaceFreeV2_14ee5e,    // inline virtual body
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJIOVASPACE = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__OBJIOVASPACE.rtti,    // [0]: (iovaspace) this
        &__nvoc_metadata__OBJIOVASPACE.metadata__OBJVASPACE.rtti,    // [1]: (vaspace) super
        &__nvoc_metadata__OBJIOVASPACE.metadata__OBJVASPACE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 8 down-thunk(s) defined to bridge methods in OBJIOVASPACE from superclasses

// iovaspaceConstruct_: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceConstruct_(struct OBJVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return iovaspaceConstruct_((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

// iovaspaceAlloc: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceAlloc(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return iovaspaceAlloc((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

// iovaspaceFree: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceFree(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return iovaspaceFree((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), vAddr);
}

// iovaspaceApplyDefaultAlignment: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return iovaspaceApplyDefaultAlignment((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

// iovaspaceIncAllocRefCnt: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceIncAllocRefCnt(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return iovaspaceIncAllocRefCnt((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), vAddr);
}

// iovaspaceGetVaStart: virtual override (vaspace) base (vaspace)
NvU64 __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaStart(struct OBJVASPACE *pVAS) {
    return iovaspaceGetVaStart((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceGetVaLimit: virtual override (vaspace) base (vaspace)
NvU64 __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaLimit(struct OBJVASPACE *pVAS) {
    return iovaspaceGetVaLimit((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceGetVasInfo: virtual override (vaspace) base (vaspace)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return iovaspaceGetVasInfo((struct OBJIOVASPACE *)(((unsigned char *) pVAS) - NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pParams);
}


// 19 up-thunk(s) defined to bridge methods in OBJIOVASPACE to superclasses

// iovaspaceGetFlags: inline virtual inherited (vaspace) base (vaspace) body
NvU32 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetFlags(struct OBJIOVASPACE *pVAS) {
    return vaspaceGetFlags((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceMap: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceMap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return vaspaceMap((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu, vaLo, vaHi, pTarget, flags);
}

// iovaspaceUnmap: inline virtual inherited (vaspace) base (vaspace) body
void __nvoc_up_thunk_OBJVASPACE_iovaspaceUnmap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    vaspaceUnmap((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu, vaLo, vaHi);
}

// iovaspaceReserveMempool: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceReserveMempool(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return vaspaceReserveMempool((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pDevice, size, pageSizeLockMask, flags);
}

// iovaspaceGetHeap: inline virtual inherited (vaspace) base (vaspace) body
OBJEHEAP * __nvoc_up_thunk_OBJVASPACE_iovaspaceGetHeap(struct OBJIOVASPACE *pVAS) {
    return vaspaceGetHeap((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceGetMapPageSize: inline virtual inherited (vaspace) base (vaspace) body
NvU64 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetMapPageSize(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return vaspaceGetMapPageSize((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pMemBlock);
}

// iovaspaceGetBigPageSize: inline virtual inherited (vaspace) base (vaspace) body
NvU64 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetBigPageSize(struct OBJIOVASPACE *pVAS) {
    return vaspaceGetBigPageSize((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceIsFaultCapable: inline virtual inherited (vaspace) base (vaspace) body
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsFaultCapable(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsFaultCapable((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceIsExternallyOwned: inline virtual inherited (vaspace) base (vaspace) body
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsExternallyOwned(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsExternallyOwned((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceIsAtsEnabled: inline virtual inherited (vaspace) base (vaspace) body
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsAtsEnabled(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsAtsEnabled((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
}

// iovaspaceGetPasid: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPasid(struct OBJIOVASPACE *pVAS, NvU32 *pPasid) {
    return vaspaceGetPasid((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pPasid);
}

// iovaspaceGetPageDirBase: inline virtual inherited (vaspace) base (vaspace) body
PMEMORY_DESCRIPTOR __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageDirBase(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspaceGetPageDirBase((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
}

// iovaspacePinRootPageDir: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspacePinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspacePinRootPageDir((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
}

// iovaspaceUnpinRootPageDir: inline virtual inherited (vaspace) base (vaspace) body
void __nvoc_up_thunk_OBJVASPACE_iovaspaceUnpinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    vaspaceUnpinRootPageDir((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
}

// iovaspaceInvalidateTlb: virtual inherited (vaspace) base (vaspace)
void __nvoc_up_thunk_OBJVASPACE_iovaspaceInvalidateTlb(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    vaspaceInvalidateTlb((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu, type);
}

// iovaspaceGetPageTableInfo: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageTableInfo(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return vaspaceGetPageTableInfo((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pParams);
}

// iovaspaceGetPteInfo: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return vaspaceGetPteInfo((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pParams, pPhysAddr);
}

// iovaspaceSetPteInfo: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceSetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return vaspaceSetPteInfo((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu, pParams);
}

// iovaspaceFreeV2: inline virtual inherited (vaspace) base (vaspace) body
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceFreeV2(struct OBJIOVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return vaspaceFreeV2((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), vAddr, pSize);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJIOVASPACE = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJIOVASPACE object.
void __nvoc_iovaspaceDestruct(OBJIOVASPACE*);
void __nvoc_dtor_OBJVASPACE(OBJVASPACE*);
void __nvoc_dtor_OBJIOVASPACE(OBJIOVASPACE* pThis) {

// Call destructor.
    __nvoc_iovaspaceDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct OBJIOVASPACE object.
NV_STATUS __nvoc_ctor_OBJVASPACE(OBJVASPACE *);
NV_STATUS __nvoc_ctor_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);
    if (status != NV_OK) goto __nvoc_ctor_OBJIOVASPACE_fail_OBJVASPACE;

    // Initialize data fields.
    __nvoc_init_dataField_OBJIOVASPACE(pThis);
    goto __nvoc_ctor_OBJIOVASPACE_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJIOVASPACE_fail_OBJVASPACE:
__nvoc_ctor_OBJIOVASPACE_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJIOVASPACE_1(OBJIOVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJIOVASPACE_1


// Initialize vtable(s) for 27 virtual method(s).
void __nvoc_init_funcTable_OBJIOVASPACE(OBJIOVASPACE *pThis) {
    __nvoc_init_funcTable_OBJIOVASPACE_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJIOVASPACE(OBJIOVASPACE *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJVASPACE = &pThis->__nvoc_base_OBJVASPACE;    // (vaspace) super
    pThis->__nvoc_pbase_OBJIOVASPACE = pThis;    // (iovaspace) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJVASPACE(&pThis->__nvoc_base_OBJVASPACE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJIOVASPACE.metadata__OBJVASPACE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJIOVASPACE.metadata__OBJVASPACE;    // (vaspace) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJIOVASPACE;    // (iovaspace) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJIOVASPACE(pThis);
}

NV_STATUS __nvoc_objCreate_OBJIOVASPACE(OBJIOVASPACE **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJIOVASPACE *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJIOVASPACE));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJIOVASPACE));

    __nvoc_pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__OBJIOVASPACE(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJIOVASPACE(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJIOVASPACE_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJIOVASPACE_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJIOVASPACE));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJIOVASPACE(OBJIOVASPACE **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJIOVASPACE(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

