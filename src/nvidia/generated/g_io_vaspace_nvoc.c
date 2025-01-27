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

// Down-thunk(s) to bridge OBJIOVASPACE methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceConstruct_(struct OBJVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceAlloc(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceFree(struct OBJVASPACE *pVAS, NvU64 vAddr);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceApplyDefaultAlignment(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceIncAllocRefCnt(struct OBJVASPACE *pVAS, NvU64 vAddr);    // this
NvU64 __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaStart(struct OBJVASPACE *pVAS);    // this
NvU64 __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaLimit(struct OBJVASPACE *pVAS);    // this
NV_STATUS __nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVasInfo(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);    // this

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


// Up-thunk(s) to bridge OBJIOVASPACE methods to ancestors (if any)
NvU32 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetFlags(struct OBJIOVASPACE *pVAS);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceMap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags);    // this
void __nvoc_up_thunk_OBJVASPACE_iovaspaceUnmap(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceReserveMempool(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags);    // this
OBJEHEAP * __nvoc_up_thunk_OBJVASPACE_iovaspaceGetHeap(struct OBJIOVASPACE *pVAS);    // this
NvU64 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetMapPageSize(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock);    // this
NvU64 __nvoc_up_thunk_OBJVASPACE_iovaspaceGetBigPageSize(struct OBJIOVASPACE *pVAS);    // this
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsMirrored(struct OBJIOVASPACE *pVAS);    // this
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsFaultCapable(struct OBJIOVASPACE *pVAS);    // this
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsExternallyOwned(struct OBJIOVASPACE *pVAS);    // this
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsAtsEnabled(struct OBJIOVASPACE *pVAS);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPasid(struct OBJIOVASPACE *pVAS, NvU32 *pPasid);    // this
PMEMORY_DESCRIPTOR __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageDirBase(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu);    // this
PMEMORY_DESCRIPTOR __nvoc_up_thunk_OBJVASPACE_iovaspaceGetKernelPageDirBase(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspacePinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu);    // this
void __nvoc_up_thunk_OBJVASPACE_iovaspaceUnpinRootPageDir(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu);    // this
void __nvoc_up_thunk_OBJVASPACE_iovaspaceInvalidateTlb(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageTableInfo(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceGetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceSetPteInfo(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_OBJVASPACE_iovaspaceFreeV2(struct OBJIOVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize);    // this

// 21 up-thunk(s) defined to bridge methods in OBJIOVASPACE to superclasses

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

// iovaspaceIsMirrored: inline virtual inherited (vaspace) base (vaspace) body
NvBool __nvoc_up_thunk_OBJVASPACE_iovaspaceIsMirrored(struct OBJIOVASPACE *pVAS) {
    return vaspaceIsMirrored((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)));
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

// iovaspaceGetKernelPageDirBase: inline virtual inherited (vaspace) base (vaspace) body
PMEMORY_DESCRIPTOR __nvoc_up_thunk_OBJVASPACE_iovaspaceGetKernelPageDirBase(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return vaspaceGetKernelPageDirBase((struct OBJVASPACE *)(((unsigned char *) pVAS) + NV_OFFSETOF(OBJIOVASPACE, __nvoc_base_OBJVASPACE)), pGpu);
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

// Vtable initialization
static void __nvoc_init_funcTable_OBJIOVASPACE_1(OBJIOVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJIOVASPACE_1


// Initialize vtable(s) for 29 virtual method(s).
void __nvoc_init_funcTable_OBJIOVASPACE(OBJIOVASPACE *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__OBJIOVASPACE vtable = {
        .__iovaspaceConstruct___ = &iovaspaceConstruct__IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceConstruct___ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceConstruct_,    // pure virtual
        .__iovaspaceAlloc__ = &iovaspaceAlloc_IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceAlloc__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceAlloc,    // pure virtual
        .__iovaspaceFree__ = &iovaspaceFree_IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceFree__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceFree,    // pure virtual
        .__iovaspaceApplyDefaultAlignment__ = &iovaspaceApplyDefaultAlignment_IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceApplyDefaultAlignment__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceApplyDefaultAlignment,    // pure virtual
        .__iovaspaceIncAllocRefCnt__ = &iovaspaceIncAllocRefCnt_IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceIncAllocRefCnt__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceIncAllocRefCnt,    // inline virtual body
        .__iovaspaceGetVaStart__ = &iovaspaceGetVaStart_IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceGetVaStart__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaStart,    // virtual
        .__iovaspaceGetVaLimit__ = &iovaspaceGetVaLimit_IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceGetVaLimit__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVaLimit,    // virtual
        .__iovaspaceGetVasInfo__ = &iovaspaceGetVasInfo_IMPL,    // virtual override (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceGetVasInfo__ = &__nvoc_down_thunk_OBJIOVASPACE_vaspaceGetVasInfo,    // pure virtual
        .__iovaspaceGetFlags__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetFlags,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetFlags__ = &vaspaceGetFlags_edd98b,    // inline virtual body
        .__iovaspaceMap__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceMap,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceMap__ = &vaspaceMap_14ee5e,    // inline virtual body
        .__iovaspaceUnmap__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceUnmap,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceUnmap__ = &vaspaceUnmap_af5be7,    // inline virtual body
        .__iovaspaceReserveMempool__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceReserveMempool,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceReserveMempool__ = &vaspaceReserveMempool_ac1694,    // inline virtual body
        .__iovaspaceGetHeap__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetHeap,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetHeap__ = &vaspaceGetHeap_9451a7,    // inline virtual body
        .__iovaspaceGetMapPageSize__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetMapPageSize,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetMapPageSize__ = &vaspaceGetMapPageSize_c26fae,    // inline virtual body
        .__iovaspaceGetBigPageSize__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetBigPageSize,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetBigPageSize__ = &vaspaceGetBigPageSize_c26fae,    // inline virtual body
        .__iovaspaceIsMirrored__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceIsMirrored,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceIsMirrored__ = &vaspaceIsMirrored_2fa1ff,    // inline virtual body
        .__iovaspaceIsFaultCapable__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceIsFaultCapable,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceIsFaultCapable__ = &vaspaceIsFaultCapable_2fa1ff,    // inline virtual body
        .__iovaspaceIsExternallyOwned__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceIsExternallyOwned,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceIsExternallyOwned__ = &vaspaceIsExternallyOwned_2fa1ff,    // inline virtual body
        .__iovaspaceIsAtsEnabled__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceIsAtsEnabled,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceIsAtsEnabled__ = &vaspaceIsAtsEnabled_2fa1ff,    // inline virtual body
        .__iovaspaceGetPasid__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPasid,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetPasid__ = &vaspaceGetPasid_14ee5e,    // inline virtual body
        .__iovaspaceGetPageDirBase__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageDirBase,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetPageDirBase__ = &vaspaceGetPageDirBase_9451a7,    // inline virtual body
        .__iovaspaceGetKernelPageDirBase__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetKernelPageDirBase,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetKernelPageDirBase__ = &vaspaceGetKernelPageDirBase_9451a7,    // inline virtual body
        .__iovaspacePinRootPageDir__ = &__nvoc_up_thunk_OBJVASPACE_iovaspacePinRootPageDir,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspacePinRootPageDir__ = &vaspacePinRootPageDir_14ee5e,    // inline virtual body
        .__iovaspaceUnpinRootPageDir__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceUnpinRootPageDir,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceUnpinRootPageDir__ = &vaspaceUnpinRootPageDir_af5be7,    // inline virtual body
        .__iovaspaceInvalidateTlb__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceInvalidateTlb,    // virtual inherited (vaspace) base (vaspace)
        .OBJVASPACE.__vaspaceInvalidateTlb__ = &vaspaceInvalidateTlb_IMPL,    // virtual
        .__iovaspaceGetPageTableInfo__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPageTableInfo,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetPageTableInfo__ = &vaspaceGetPageTableInfo_14ee5e,    // inline virtual body
        .__iovaspaceGetPteInfo__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceGetPteInfo,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceGetPteInfo__ = &vaspaceGetPteInfo_14ee5e,    // inline virtual body
        .__iovaspaceSetPteInfo__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceSetPteInfo,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceSetPteInfo__ = &vaspaceSetPteInfo_14ee5e,    // inline virtual body
        .__iovaspaceFreeV2__ = &__nvoc_up_thunk_OBJVASPACE_iovaspaceFreeV2,    // inline virtual inherited (vaspace) base (vaspace) body
        .OBJVASPACE.__vaspaceFreeV2__ = &vaspaceFreeV2_14ee5e,    // inline virtual body
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJVASPACE.__nvoc_vtable = &vtable.OBJVASPACE;    // (vaspace) super
    pThis->__nvoc_vtable = &vtable;    // (iovaspace) this
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

NV_STATUS __nvoc_objCreate_OBJIOVASPACE(OBJIOVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJIOVASPACE *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJIOVASPACE), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJIOVASPACE));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJIOVASPACE);

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

    __nvoc_init_OBJIOVASPACE(pThis);
    status = __nvoc_ctor_OBJIOVASPACE(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJIOVASPACE_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJIOVASPACE_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJVASPACE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJIOVASPACE));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJIOVASPACE(OBJIOVASPACE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJIOVASPACE(ppThis, pParent, createFlags);

    return status;
}

