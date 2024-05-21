#define NVOC_TRACEABLE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_traceable_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x6305d2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

void __nvoc_init_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_funcTable_OBJTRACEABLE(OBJTRACEABLE*);
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_dataField_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJTRACEABLE;

static const struct NVOC_RTTI __nvoc_rtti_OBJTRACEABLE_OBJTRACEABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJTRACEABLE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJTRACEABLE,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJTRACEABLE = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_OBJTRACEABLE_OBJTRACEABLE,
    },
};

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJTRACEABLE),
        /*classId=*/            classId(OBJTRACEABLE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJTRACEABLE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJTRACEABLE,
    /*pExportInfo=*/        &__nvoc_export_info_OBJTRACEABLE
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJTRACEABLE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_OBJTRACEABLE(pThis);
    goto __nvoc_ctor_OBJTRACEABLE_exit; // Success

__nvoc_ctor_OBJTRACEABLE_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJTRACEABLE_1(OBJTRACEABLE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJTRACEABLE_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    __nvoc_init_funcTable_OBJTRACEABLE_1(pThis);
}

void __nvoc_init_OBJTRACEABLE(OBJTRACEABLE *pThis) {
    pThis->__nvoc_pbase_OBJTRACEABLE = pThis;
    __nvoc_init_funcTable_OBJTRACEABLE(pThis);
}

