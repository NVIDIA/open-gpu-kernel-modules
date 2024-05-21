#define NVOC_HOST_ENG_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_host_eng_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb356e7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHOSTENG;

void __nvoc_init_OBJHOSTENG(OBJHOSTENG*);
void __nvoc_init_funcTable_OBJHOSTENG(OBJHOSTENG*);
NV_STATUS __nvoc_ctor_OBJHOSTENG(OBJHOSTENG*);
void __nvoc_init_dataField_OBJHOSTENG(OBJHOSTENG*);
void __nvoc_dtor_OBJHOSTENG(OBJHOSTENG*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJHOSTENG;

static const struct NVOC_RTTI __nvoc_rtti_OBJHOSTENG_OBJHOSTENG = {
    /*pClassDef=*/          &__nvoc_class_def_OBJHOSTENG,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJHOSTENG,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJHOSTENG = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_OBJHOSTENG_OBJHOSTENG,
    },
};

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHOSTENG = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJHOSTENG),
        /*classId=*/            classId(OBJHOSTENG),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJHOSTENG",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJHOSTENG,
    /*pExportInfo=*/        &__nvoc_export_info_OBJHOSTENG
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJHOSTENG = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJHOSTENG(OBJHOSTENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJHOSTENG(OBJHOSTENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJHOSTENG(OBJHOSTENG *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_OBJHOSTENG(pThis);
    goto __nvoc_ctor_OBJHOSTENG_exit; // Success

__nvoc_ctor_OBJHOSTENG_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJHOSTENG_1(OBJHOSTENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // hostengHaltAndReset -- virtual
    pThis->__hostengHaltAndReset__ = &hostengHaltAndReset_IMPL;
} // End __nvoc_init_funcTable_OBJHOSTENG_1 with approximately 1 basic block(s).


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_OBJHOSTENG(OBJHOSTENG *pThis) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_OBJHOSTENG_1(pThis);
}

void __nvoc_init_OBJHOSTENG(OBJHOSTENG *pThis) {
    pThis->__nvoc_pbase_OBJHOSTENG = pThis;
    __nvoc_init_funcTable_OBJHOSTENG(pThis);
}

