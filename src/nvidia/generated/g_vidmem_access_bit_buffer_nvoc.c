#define NVOC_VIDMEM_ACCESS_BIT_BUFFER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_vidmem_access_bit_buffer_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ebb6da = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VidmemAccessBitBuffer;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for VidmemAccessBitBuffer
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_VidmemAccessBitBuffer(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__VidmemAccessBitBuffer;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__VidmemAccessBitBuffer;

// Down-thunk(s) to bridge VidmemAccessBitBuffer methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super

// Up-thunk(s) to bridge VidmemAccessBitBuffer methods to ancestors (if any)

// Class-specific details for VidmemAccessBitBuffer
const struct NVOC_CLASS_DEF __nvoc_class_def_VidmemAccessBitBuffer = 
{
    .classInfo.size =               sizeof(VidmemAccessBitBuffer),
    .classInfo.classId =            classId(VidmemAccessBitBuffer),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "VidmemAccessBitBuffer",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_VidmemAccessBitBuffer,
    .pCastInfo =          &__nvoc_castinfo__VidmemAccessBitBuffer,
    .pExportInfo =        &__nvoc_export_info__VidmemAccessBitBuffer
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)
static NV_STATUS vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__EXPORT(void *pVidmemAccessBitBuffer, void *pParams) {
    return vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_IMPL(pVidmemAccessBitBuffer, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010048u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VidmemAccessBitBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10048u)
        /*flags=*/      0x10048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc7630103u,
        /*paramSize=*/  sizeof(NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VidmemAccessBitBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vidmemAccessBitBufCtrlCmdVidmemAccessBitDump"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__VidmemAccessBitBuffer __nvoc_metadata__VidmemAccessBitBuffer = {
    .rtti.pClassDef = &__nvoc_class_def_VidmemAccessBitBuffer,    // (vidmemAccessBitBuf) this
    .rtti.dtor      = &__nvoc_dtor_VidmemAccessBitBuffer,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__VidmemAccessBitBuffer = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__VidmemAccessBitBuffer.rtti,    // [0]: (vidmemAccessBitBuf) this
        &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__VidmemAccessBitBuffer.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__VidmemAccessBitBuffer.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__VidmemAccessBitBuffer = 
{
    .numEntries=     1,
    .pExportEntries= __nvoc_exported_method_def_VidmemAccessBitBuffer
};


// Destruct VidmemAccessBitBuffer object.
void __nvoc_vidmemAccessBitBufDestruct(VidmemAccessBitBuffer*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_VidmemAccessBitBuffer(Dynamic* pThis) {

    VidmemAccessBitBuffer *__nvoc_this = (VidmemAccessBitBuffer *) pThis;

// Call destructor.
    __nvoc_vidmemAccessBitBufDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct VidmemAccessBitBuffer object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pVidmemAccessBitBuffer, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline body
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pVidmemAccessBitBuffer->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VidmemAccessBitBuffer_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pVidmemAccessBitBuffer->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_VidmemAccessBitBuffer_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_VidmemAccessBitBuffer(pVidmemAccessBitBuffer, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_vidmemAccessBitBufConstruct(pVidmemAccessBitBuffer, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VidmemAccessBitBuffer_fail__init;
    goto __nvoc_ctor_VidmemAccessBitBuffer_exit; // Success

    // Unwind on error.
__nvoc_ctor_VidmemAccessBitBuffer_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pVidmemAccessBitBuffer->__nvoc_base_Notifier);
__nvoc_ctor_VidmemAccessBitBuffer_fail_Notifier:
    __nvoc_dtor_GpuResource((Dynamic *)&pVidmemAccessBitBuffer->__nvoc_base_GpuResource);
__nvoc_ctor_VidmemAccessBitBuffer_fail_GpuResource:
__nvoc_ctor_VidmemAccessBitBuffer_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VidmemAccessBitBuffer_1(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
} // End __nvoc_init_funcTable_VidmemAccessBitBuffer_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_VidmemAccessBitBuffer_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_VidmemAccessBitBuffer = pThis;    // (vidmemAccessBitBuf) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__VidmemAccessBitBuffer;    // (vidmemAccessBitBuf) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_VidmemAccessBitBuffer(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_VidmemAccessBitBuffer(VidmemAccessBitBuffer **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    VidmemAccessBitBuffer *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(VidmemAccessBitBuffer));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(VidmemAccessBitBuffer));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_VidmemAccessBitBuffer_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_VidmemAccessBitBuffer_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__VidmemAccessBitBuffer(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_VidmemAccessBitBuffer(__nvoc_pThis, pRmhalspecowner, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_VidmemAccessBitBuffer_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_VidmemAccessBitBuffer_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(VidmemAccessBitBuffer));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_VidmemAccessBitBuffer(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_VidmemAccessBitBuffer((VidmemAccessBitBuffer **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

