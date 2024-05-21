#define NVOC_CHIPSET_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_chipset_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x547dbb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJCL;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJCL(OBJCL*);
void __nvoc_init_funcTable_OBJCL(OBJCL*);
NV_STATUS __nvoc_ctor_OBJCL(OBJCL*);
void __nvoc_init_dataField_OBJCL(OBJCL*);
void __nvoc_dtor_OBJCL(OBJCL*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJCL;

static const struct NVOC_RTTI __nvoc_rtti_OBJCL_OBJCL = {
    /*pClassDef=*/          &__nvoc_class_def_OBJCL,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJCL,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJCL_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJCL, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJCL = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJCL_OBJCL,
        &__nvoc_rtti_OBJCL_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJCL = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJCL),
        /*classId=*/            classId(OBJCL),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJCL",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJCL,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJCL,
    /*pExportInfo=*/        &__nvoc_export_info_OBJCL
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJCL = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJCL(OBJCL *pThis) {
    __nvoc_clDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJCL(OBJCL *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
    pThis->setProperty(pThis, PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR, ((NvBool)(0 != 0)));
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJCL(OBJCL *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJCL_fail_Object;
    __nvoc_init_dataField_OBJCL(pThis);

    status = __nvoc_clConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJCL_fail__init;
    goto __nvoc_ctor_OBJCL_exit; // Success

__nvoc_ctor_OBJCL_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJCL_fail_Object:
__nvoc_ctor_OBJCL_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJCL_1(OBJCL *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJCL_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJCL(OBJCL *pThis) {
    __nvoc_init_funcTable_OBJCL_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJCL(OBJCL *pThis) {
    pThis->__nvoc_pbase_OBJCL = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJCL(pThis);
}

NV_STATUS __nvoc_objCreate_OBJCL(OBJCL **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJCL *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJCL), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJCL));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJCL);

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

    __nvoc_init_OBJCL(pThis);
    status = __nvoc_ctor_OBJCL(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJCL_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJCL_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJCL));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJCL(OBJCL **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJCL(ppThis, pParent, createFlags);

    return status;
}

