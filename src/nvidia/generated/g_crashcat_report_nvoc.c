#define NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_crashcat_report_nvoc.h"

void __nvoc_init_halspec_CrashCatReportHal(CrashCatReportHal *pCrashCatReportHal, NV_CRASHCAT_PACKET_FORMAT_VERSION version, CrashCatImplementer implementer)
{
    // V1_GENERIC
    if(version == 0x1 && implementer == 0x0)
    {
        pCrashCatReportHal->__nvoc_HalVarIdx = 0;
    }
    // V1_LIBOS2
    else if(version == 0x1 && implementer == 0x4C49424F53322E30)
    {
        pCrashCatReportHal->__nvoc_HalVarIdx = 1;
    }
    // V1_LIBOS3
    else if(version == 0x1 && implementer == 0x4C49424F53332E31)
    {
        pCrashCatReportHal->__nvoc_HalVarIdx = 2;
    }
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__de4777 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatReport;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for CrashCatReport
void __nvoc_init__Object(Object*);
void __nvoc_init__CrashCatReport(CrashCatReport*,
        NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version, CrashCatImplementer CrashCatReportHal_implementer);
void __nvoc_init_funcTable_CrashCatReport(CrashCatReport*);
NV_STATUS __nvoc_ctor_CrashCatReport(CrashCatReport*, void **ppReportBytes, NvLength bytesRemaining);
void __nvoc_init_dataField_CrashCatReport(CrashCatReport*);
void __nvoc_dtor_CrashCatReport(CrashCatReport*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatReport;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatReport;

// Down-thunk(s) to bridge CrashCatReport methods from ancestors (if any)

// Up-thunk(s) to bridge CrashCatReport methods to ancestors (if any)

// Class-specific details for CrashCatReport
const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatReport = 
{
    .classInfo.size =               sizeof(CrashCatReport),
    .classInfo.classId =            classId(CrashCatReport),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CrashCatReport",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CrashCatReport,
    .pCastInfo =          &__nvoc_castinfo__CrashCatReport,
    .pExportInfo =        &__nvoc_export_info__CrashCatReport
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CrashCatReport __nvoc_metadata__CrashCatReport = {
    .rtti.pClassDef = &__nvoc_class_def_CrashCatReport,    // (crashcatReport) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CrashCatReport,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(CrashCatReport, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatReport = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__CrashCatReport.rtti,    // [0]: (crashcatReport) this
        &__nvoc_metadata__CrashCatReport.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatReport = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct CrashCatReport object.
void __nvoc_crashcatReportDestruct(CrashCatReport*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_CrashCatReport(CrashCatReport* pThis) {

// Call destructor.
    __nvoc_crashcatReportDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_CrashCatReport(CrashCatReport *pThis) {
    CrashCatReportHal *reportHal = &pThis->reportHal;
    const unsigned long reportHal_HalVarIdx = (unsigned long)reportHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(reportHal);
    PORT_UNREFERENCED_VARIABLE(reportHal_HalVarIdx);
}


// Construct CrashCatReport object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_CrashCatReport(CrashCatReport *arg_this, void **ppReportBytes, NvLength bytesRemaining) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&arg_this->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatReport_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_CrashCatReport(arg_this);

    // Call the constructor for this class.
    status = __nvoc_crashcatReportConstruct(arg_this, ppReportBytes, bytesRemaining);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatReport_fail__init;
    goto __nvoc_ctor_CrashCatReport_exit; // Success

    // Unwind on error.
__nvoc_ctor_CrashCatReport_fail__init:
    __nvoc_dtor_Object(&arg_this->__nvoc_base_Object);
__nvoc_ctor_CrashCatReport_fail_Object:
__nvoc_ctor_CrashCatReport_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CrashCatReport_1(CrashCatReport *pThis) {
    CrashCatReportHal *reportHal = &pThis->reportHal;
    const unsigned long reportHal_HalVarIdx = (unsigned long)reportHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(reportHal);
    PORT_UNREFERENCED_VARIABLE(reportHal_HalVarIdx);

    // crashcatReportSourceContainment -- halified (2 hals) body
    if (( ((reportHal_HalVarIdx >> 5) == 0UL) && ((1UL << (reportHal_HalVarIdx & 0x1f)) & 0x00000004UL) )) /* CrashCatReportHal: V1_LIBOS3 */ 
    {
        pThis->__crashcatReportSourceContainment__ = &crashcatReportSourceContainment_V1_LIBOS3;
    }
    else
    {
        pThis->__crashcatReportSourceContainment__ = &crashcatReportSourceContainment_3e9f29;
    }

    // crashcatReportLogReporter -- halified (3 hals)
    if (( ((reportHal_HalVarIdx >> 5) == 0UL) && ((1UL << (reportHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* CrashCatReportHal: V1_LIBOS2 */ 
    {
        pThis->__crashcatReportLogReporter__ = &crashcatReportLogReporter_V1_LIBOS2;
    }
    else if (( ((reportHal_HalVarIdx >> 5) == 0UL) && ((1UL << (reportHal_HalVarIdx & 0x1f)) & 0x00000004UL) )) /* CrashCatReportHal: V1_LIBOS3 */ 
    {
        pThis->__crashcatReportLogReporter__ = &crashcatReportLogReporter_V1_LIBOS3;
    }
    else
    {
        pThis->__crashcatReportLogReporter__ = &crashcatReportLogReporter_V1_GENERIC;
    }

    // crashcatReportLogSource -- halified (3 hals)
    if (( ((reportHal_HalVarIdx >> 5) == 0UL) && ((1UL << (reportHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* CrashCatReportHal: V1_LIBOS2 */ 
    {
        pThis->__crashcatReportLogSource__ = &crashcatReportLogSource_V1_LIBOS2;
    }
    else if (( ((reportHal_HalVarIdx >> 5) == 0UL) && ((1UL << (reportHal_HalVarIdx & 0x1f)) & 0x00000004UL) )) /* CrashCatReportHal: V1_LIBOS3 */ 
    {
        pThis->__crashcatReportLogSource__ = &crashcatReportLogSource_V1_LIBOS3;
    }
    else
    {
        pThis->__crashcatReportLogSource__ = &crashcatReportLogSource_V1_GENERIC;
    }

    // crashcatReportLogVersionProtobuf -- halified (3 hals)
    if (( ((reportHal_HalVarIdx >> 5) == 0UL) && ((1UL << (reportHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* CrashCatReportHal: V1_LIBOS2 */ 
    {
        pThis->__crashcatReportLogVersionProtobuf__ = &crashcatReportLogVersionProtobuf_V1_LIBOS2;
    }
    else if (( ((reportHal_HalVarIdx >> 5) == 0UL) && ((1UL << (reportHal_HalVarIdx & 0x1f)) & 0x00000004UL) )) /* CrashCatReportHal: V1_LIBOS3 */ 
    {
        pThis->__crashcatReportLogVersionProtobuf__ = &crashcatReportLogVersionProtobuf_V1_LIBOS3;
    }
    else
    {
        pThis->__crashcatReportLogVersionProtobuf__ = &crashcatReportLogVersionProtobuf_V1_GENERIC;
    }
} // End __nvoc_init_funcTable_CrashCatReport_1 with approximately 11 basic block(s).


// Initialize vtable(s) for 4 virtual method(s).
void __nvoc_init_funcTable_CrashCatReport(CrashCatReport *pThis) {

    // Initialize vtable(s) with 4 per-object function pointer(s).
    __nvoc_init_funcTable_CrashCatReport_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CrashCatReport(CrashCatReport *pThis,
        NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version, CrashCatImplementer CrashCatReportHal_implementer) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_CrashCatReport = pThis;    // (crashcatReport) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CrashCatReport.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CrashCatReport;    // (crashcatReport) this

    // Initialize halspec data.
    __nvoc_init_halspec_CrashCatReportHal(&pThis->reportHal, CrashCatReportHal_version, CrashCatReportHal_implementer);

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CrashCatReport(pThis);
}

NV_STATUS __nvoc_objCreate_CrashCatReport(CrashCatReport **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags,
        NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version, CrashCatImplementer CrashCatReportHal_implementer, void **ppReportBytes, NvLength bytesRemaining)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CrashCatReport *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CrashCatReport));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CrashCatReport));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__CrashCatReport(__nvoc_pThis, CrashCatReportHal_version, CrashCatReportHal_implementer);
    __nvoc_status = __nvoc_ctor_CrashCatReport(__nvoc_pThis, ppReportBytes, bytesRemaining);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CrashCatReport_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CrashCatReport_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CrashCatReport));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CrashCatReport(CrashCatReport **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version = va_arg(__nvoc_args, NV_CRASHCAT_PACKET_FORMAT_VERSION);
    CrashCatImplementer CrashCatReportHal_implementer = va_arg(__nvoc_args, CrashCatImplementer);
    void **ppReportBytes = va_arg(__nvoc_args, void **);
    NvLength bytesRemaining = va_arg(__nvoc_args, NvLength);

    __nvoc_status = __nvoc_objCreate_CrashCatReport(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, CrashCatReportHal_version, CrashCatReportHal_implementer, ppReportBytes, bytesRemaining);

    return __nvoc_status;
}

