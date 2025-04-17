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
char __nvoc_class_id_uniqueness_check__0xde4777 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatReport;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for CrashCatReport
void __nvoc_init__Object(Object*);
void __nvoc_init__CrashCatReport(CrashCatReport*,
        NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version, CrashCatImplementer CrashCatReportHal_implementer);
void __nvoc_init_funcTable_CrashCatReport(CrashCatReport*);
NV_STATUS __nvoc_ctor_CrashCatReport(CrashCatReport*, void **arg_ppReportBytes, NvLength arg_bytesRemaining);
void __nvoc_init_dataField_CrashCatReport(CrashCatReport*);
void __nvoc_dtor_CrashCatReport(CrashCatReport*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatReport;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatReport;

// Down-thunk(s) to bridge CrashCatReport methods from ancestors (if any)

// Up-thunk(s) to bridge CrashCatReport methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatReport = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(CrashCatReport),
        /*classId=*/            classId(CrashCatReport),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "CrashCatReport",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CrashCatReport,
    /*pCastInfo=*/          &__nvoc_castinfo__CrashCatReport,
    /*pExportInfo=*/        &__nvoc_export_info__CrashCatReport
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_CrashCatReport(CrashCatReport *pThis) {
    __nvoc_crashcatReportDestruct(pThis);
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

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_CrashCatReport(CrashCatReport *pThis, void ** arg_ppReportBytes, NvLength arg_bytesRemaining) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatReport_fail_Object;
    __nvoc_init_dataField_CrashCatReport(pThis);

    status = __nvoc_crashcatReportConstruct(pThis, arg_ppReportBytes, arg_bytesRemaining);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatReport_fail__init;
    goto __nvoc_ctor_CrashCatReport_exit; // Success

__nvoc_ctor_CrashCatReport_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
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

    // crashcatReportSourceContainment -- halified (2 hals)
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

NV_STATUS __nvoc_objCreate_CrashCatReport(CrashCatReport **ppThis, Dynamic *pParent, NvU32 createFlags,
        NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version, CrashCatImplementer CrashCatReportHal_implementer, void ** arg_ppReportBytes, NvLength arg_bytesRemaining)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    CrashCatReport *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(CrashCatReport), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(CrashCatReport));

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__CrashCatReport(pThis, CrashCatReportHal_version, CrashCatReportHal_implementer);
    status = __nvoc_ctor_CrashCatReport(pThis, arg_ppReportBytes, arg_bytesRemaining);
    if (status != NV_OK) goto __nvoc_objCreate_CrashCatReport_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_CrashCatReport_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(CrashCatReport));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_CrashCatReport(CrashCatReport **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version = va_arg(args, NV_CRASHCAT_PACKET_FORMAT_VERSION);
    CrashCatImplementer CrashCatReportHal_implementer = va_arg(args, CrashCatImplementer);
    void ** arg_ppReportBytes = va_arg(args, void **);
    NvLength arg_bytesRemaining = va_arg(args, NvLength);

    status = __nvoc_objCreate_CrashCatReport(ppThis, pParent, createFlags, CrashCatReportHal_version, CrashCatReportHal_implementer, arg_ppReportBytes, arg_bytesRemaining);

    return status;
}

