#define NVOC_THIRD_PARTY_P2P_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_third_party_p2p_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x3e3a6a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_P2PTokenShare;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for P2PTokenShare
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__P2PTokenShare(P2PTokenShare*);
void __nvoc_init_funcTable_P2PTokenShare(P2PTokenShare*);
NV_STATUS __nvoc_ctor_P2PTokenShare(P2PTokenShare*);
void __nvoc_init_dataField_P2PTokenShare(P2PTokenShare*);
void __nvoc_dtor_P2PTokenShare(P2PTokenShare*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__P2PTokenShare;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__P2PTokenShare;

// Down-thunk(s) to bridge P2PTokenShare methods from ancestors (if any)

// Up-thunk(s) to bridge P2PTokenShare methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_P2PTokenShare = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(P2PTokenShare),
        /*classId=*/            classId(P2PTokenShare),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "P2PTokenShare",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_P2PTokenShare,
    /*pCastInfo=*/          &__nvoc_castinfo__P2PTokenShare,
    /*pExportInfo=*/        &__nvoc_export_info__P2PTokenShare
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__P2PTokenShare __nvoc_metadata__P2PTokenShare = {
    .rtti.pClassDef = &__nvoc_class_def_P2PTokenShare,    // (shrp2p) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_P2PTokenShare,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(P2PTokenShare, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(P2PTokenShare, __nvoc_base_RsShared.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__P2PTokenShare = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__P2PTokenShare.rtti,    // [0]: (shrp2p) this
        &__nvoc_metadata__P2PTokenShare.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__P2PTokenShare.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__P2PTokenShare = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_P2PTokenShare(P2PTokenShare *pThis) {
    __nvoc_shrp2pDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_P2PTokenShare(P2PTokenShare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_P2PTokenShare(P2PTokenShare *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_P2PTokenShare_fail_RsShared;
    __nvoc_init_dataField_P2PTokenShare(pThis);

    status = __nvoc_shrp2pConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_P2PTokenShare_fail__init;
    goto __nvoc_ctor_P2PTokenShare_exit; // Success

__nvoc_ctor_P2PTokenShare_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_P2PTokenShare_fail_RsShared:
__nvoc_ctor_P2PTokenShare_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_P2PTokenShare_1(P2PTokenShare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_P2PTokenShare_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_P2PTokenShare(P2PTokenShare *pThis) {
    __nvoc_init_funcTable_P2PTokenShare_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__P2PTokenShare(P2PTokenShare *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_P2PTokenShare = pThis;    // (shrp2p) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__P2PTokenShare.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__P2PTokenShare.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__P2PTokenShare;    // (shrp2p) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_P2PTokenShare(pThis);
}

NV_STATUS __nvoc_objCreate_P2PTokenShare(P2PTokenShare **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    P2PTokenShare *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(P2PTokenShare), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(P2PTokenShare));

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__P2PTokenShare(pThis);
    status = __nvoc_ctor_P2PTokenShare(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_P2PTokenShare_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_P2PTokenShare_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(P2PTokenShare));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_P2PTokenShare(P2PTokenShare **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_P2PTokenShare(ppThis, pParent, createFlags);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x34d08b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ThirdPartyP2P;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for ThirdPartyP2P
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__ThirdPartyP2P(ThirdPartyP2P*);
void __nvoc_init_funcTable_ThirdPartyP2P(ThirdPartyP2P*);
NV_STATUS __nvoc_ctor_ThirdPartyP2P(ThirdPartyP2P*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_ThirdPartyP2P(ThirdPartyP2P*);
void __nvoc_dtor_ThirdPartyP2P(ThirdPartyP2P*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ThirdPartyP2P;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ThirdPartyP2P;

// Down-thunk(s) to bridge ThirdPartyP2P methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super

// Up-thunk(s) to bridge ThirdPartyP2P methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pControl(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pMap(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pUnmap(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_thirdpartyp2pShareCallback(struct ThirdPartyP2P *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pGetRegBaseOffsetAndSize(struct ThirdPartyP2P *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pGetMapAddrSpace(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pInternalControlForward(struct ThirdPartyP2P *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_thirdpartyp2pGetInternalObjectHandle(struct ThirdPartyP2P *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_thirdpartyp2pAccessCallback(struct ThirdPartyP2P *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pGetMemInterMapParams(struct ThirdPartyP2P *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pCheckMemInterUnmap(struct ThirdPartyP2P *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pGetMemoryMappingDescriptor(struct ThirdPartyP2P *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pControlSerialization_Prologue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_thirdpartyp2pControlSerialization_Epilogue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pControl_Prologue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_thirdpartyp2pControl_Epilogue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_thirdpartyp2pCanCopy(struct ThirdPartyP2P *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pIsDuplicate(struct ThirdPartyP2P *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_thirdpartyp2pPreDestruct(struct ThirdPartyP2P *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pControlFilter(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_thirdpartyp2pIsPartialUnmapSupported(struct ThirdPartyP2P *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pMapTo(struct ThirdPartyP2P *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pUnmapFrom(struct ThirdPartyP2P *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_thirdpartyp2pGetRefCount(struct ThirdPartyP2P *pResource);    // this
void __nvoc_up_thunk_RsResource_thirdpartyp2pAddAdditionalDependants(struct RsClient *pClient, struct ThirdPartyP2P *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_ThirdPartyP2P = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ThirdPartyP2P),
        /*classId=*/            classId(ThirdPartyP2P),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ThirdPartyP2P",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ThirdPartyP2P,
    /*pCastInfo=*/          &__nvoc_castinfo__ThirdPartyP2P,
    /*pExportInfo=*/        &__nvoc_export_info__ThirdPartyP2P
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ThirdPartyP2P[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdRegisterVaSpace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0102u,
        /*paramSize=*/  sizeof(NV503C_CTRL_REGISTER_VA_SPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdRegisterVaSpace"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdUnregisterVaSpace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0103u,
        /*paramSize=*/  sizeof(NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdUnregisterVaSpace"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdRegisterVidmem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0104u,
        /*paramSize=*/  sizeof(NV503C_CTRL_REGISTER_VIDMEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdRegisterVidmem"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdUnregisterVidmem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0105u,
        /*paramSize=*/  sizeof(NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdUnregisterVidmem"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) thirdpartyp2pCtrlCmdRegisterPid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x503c0106u,
        /*paramSize=*/  sizeof(NV503C_CTRL_REGISTER_PID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ThirdPartyP2P.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "thirdpartyp2pCtrlCmdRegisterPid"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ThirdPartyP2P __nvoc_metadata__ThirdPartyP2P = {
    .rtti.pClassDef = &__nvoc_class_def_ThirdPartyP2P,    // (thirdpartyp2p) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ThirdPartyP2P,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__thirdpartyp2pControl__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__thirdpartyp2pMap__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__thirdpartyp2pUnmap__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__thirdpartyp2pShareCallback__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__thirdpartyp2pGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__thirdpartyp2pGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__thirdpartyp2pInternalControlForward__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__thirdpartyp2pGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_thirdpartyp2pGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__thirdpartyp2pAccessCallback__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__thirdpartyp2pGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__thirdpartyp2pCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__thirdpartyp2pGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__thirdpartyp2pControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__thirdpartyp2pControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__thirdpartyp2pControl_Prologue__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__thirdpartyp2pControl_Epilogue__ = &__nvoc_up_thunk_RmResource_thirdpartyp2pControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__thirdpartyp2pCanCopy__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__thirdpartyp2pIsDuplicate__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__thirdpartyp2pPreDestruct__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__thirdpartyp2pControlFilter__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__thirdpartyp2pIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__thirdpartyp2pMapTo__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__thirdpartyp2pUnmapFrom__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__thirdpartyp2pGetRefCount__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__thirdpartyp2pAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_thirdpartyp2pAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ThirdPartyP2P = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__ThirdPartyP2P.rtti,    // [0]: (thirdpartyp2p) this
        &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 25 up-thunk(s) defined to bridge methods in ThirdPartyP2P to superclasses

// thirdpartyp2pControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pControl(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// thirdpartyp2pMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pMap(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// thirdpartyp2pUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pUnmap(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// thirdpartyp2pShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_thirdpartyp2pShareCallback(struct ThirdPartyP2P *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// thirdpartyp2pGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pGetRegBaseOffsetAndSize(struct ThirdPartyP2P *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// thirdpartyp2pGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pGetMapAddrSpace(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// thirdpartyp2pInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_thirdpartyp2pInternalControlForward(struct ThirdPartyP2P *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)), command, pParams, size);
}

// thirdpartyp2pGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_thirdpartyp2pGetInternalObjectHandle(struct ThirdPartyP2P *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource)));
}

// thirdpartyp2pAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_thirdpartyp2pAccessCallback(struct ThirdPartyP2P *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// thirdpartyp2pGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pGetMemInterMapParams(struct ThirdPartyP2P *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// thirdpartyp2pCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pCheckMemInterUnmap(struct ThirdPartyP2P *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// thirdpartyp2pGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pGetMemoryMappingDescriptor(struct ThirdPartyP2P *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// thirdpartyp2pControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pControlSerialization_Prologue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// thirdpartyp2pControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_thirdpartyp2pControlSerialization_Epilogue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// thirdpartyp2pControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_thirdpartyp2pControl_Prologue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// thirdpartyp2pControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_thirdpartyp2pControl_Epilogue(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// thirdpartyp2pCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_thirdpartyp2pCanCopy(struct ThirdPartyP2P *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// thirdpartyp2pIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pIsDuplicate(struct ThirdPartyP2P *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// thirdpartyp2pPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_thirdpartyp2pPreDestruct(struct ThirdPartyP2P *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// thirdpartyp2pControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pControlFilter(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// thirdpartyp2pIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_thirdpartyp2pIsPartialUnmapSupported(struct ThirdPartyP2P *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// thirdpartyp2pMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pMapTo(struct ThirdPartyP2P *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// thirdpartyp2pUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_thirdpartyp2pUnmapFrom(struct ThirdPartyP2P *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// thirdpartyp2pGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_thirdpartyp2pGetRefCount(struct ThirdPartyP2P *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// thirdpartyp2pAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_thirdpartyp2pAddAdditionalDependants(struct RsClient *pClient, struct ThirdPartyP2P *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ThirdPartyP2P, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__ThirdPartyP2P = 
{
    /*numEntries=*/     5,
    /*pExportEntries=*/ __nvoc_exported_method_def_ThirdPartyP2P
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_ThirdPartyP2P(ThirdPartyP2P *pThis) {
    __nvoc_thirdpartyp2pDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ThirdPartyP2P(ThirdPartyP2P *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ThirdPartyP2P(ThirdPartyP2P *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ThirdPartyP2P_fail_GpuResource;
    __nvoc_init_dataField_ThirdPartyP2P(pThis);

    status = __nvoc_thirdpartyp2pConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ThirdPartyP2P_fail__init;
    goto __nvoc_ctor_ThirdPartyP2P_exit; // Success

__nvoc_ctor_ThirdPartyP2P_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_ThirdPartyP2P_fail_GpuResource:
__nvoc_ctor_ThirdPartyP2P_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ThirdPartyP2P_1(ThirdPartyP2P *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // thirdpartyp2pCtrlCmdRegisterVaSpace -- exported (id=0x503c0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__thirdpartyp2pCtrlCmdRegisterVaSpace__ = &thirdpartyp2pCtrlCmdRegisterVaSpace_IMPL;
#endif

    // thirdpartyp2pCtrlCmdUnregisterVaSpace -- exported (id=0x503c0103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__thirdpartyp2pCtrlCmdUnregisterVaSpace__ = &thirdpartyp2pCtrlCmdUnregisterVaSpace_IMPL;
#endif

    // thirdpartyp2pCtrlCmdRegisterVidmem -- exported (id=0x503c0104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__thirdpartyp2pCtrlCmdRegisterVidmem__ = &thirdpartyp2pCtrlCmdRegisterVidmem_IMPL;
#endif

    // thirdpartyp2pCtrlCmdUnregisterVidmem -- exported (id=0x503c0105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__thirdpartyp2pCtrlCmdUnregisterVidmem__ = &thirdpartyp2pCtrlCmdUnregisterVidmem_IMPL;
#endif

    // thirdpartyp2pCtrlCmdRegisterPid -- exported (id=0x503c0106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__thirdpartyp2pCtrlCmdRegisterPid__ = &thirdpartyp2pCtrlCmdRegisterPid_IMPL;
#endif
} // End __nvoc_init_funcTable_ThirdPartyP2P_1 with approximately 5 basic block(s).


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_ThirdPartyP2P(ThirdPartyP2P *pThis) {

    // Initialize vtable(s) with 5 per-object function pointer(s).
    __nvoc_init_funcTable_ThirdPartyP2P_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ThirdPartyP2P(ThirdPartyP2P *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_ThirdPartyP2P = pThis;    // (thirdpartyp2p) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__ThirdPartyP2P.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ThirdPartyP2P;    // (thirdpartyp2p) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ThirdPartyP2P(pThis);
}

NV_STATUS __nvoc_objCreate_ThirdPartyP2P(ThirdPartyP2P **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ThirdPartyP2P *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ThirdPartyP2P), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ThirdPartyP2P));

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__ThirdPartyP2P(pThis);
    status = __nvoc_ctor_ThirdPartyP2P(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ThirdPartyP2P_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ThirdPartyP2P_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ThirdPartyP2P));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ThirdPartyP2P(ThirdPartyP2P **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ThirdPartyP2P(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

