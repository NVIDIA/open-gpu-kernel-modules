#define NVOC_OS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_os_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xaa1d70 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJOS;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJOS(OBJOS*);
void __nvoc_init_funcTable_OBJOS(OBJOS*);
NV_STATUS __nvoc_ctor_OBJOS(OBJOS*);
void __nvoc_init_dataField_OBJOS(OBJOS*);
void __nvoc_dtor_OBJOS(OBJOS*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJOS;

static const struct NVOC_RTTI __nvoc_rtti_OBJOS_OBJOS = {
    /*pClassDef=*/          &__nvoc_class_def_OBJOS,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJOS,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJOS_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJOS, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJOS = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJOS_OBJOS,
        &__nvoc_rtti_OBJOS_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJOS = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJOS),
        /*classId=*/            classId(OBJOS),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJOS",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJOS,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJOS,
    /*pExportInfo=*/        &__nvoc_export_info_OBJOS
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJOS = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJOS(OBJOS *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJOS(OBJOS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
    pThis->setProperty(pThis, PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER, !(1));
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJOS(OBJOS *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJOS_fail_Object;
    __nvoc_init_dataField_OBJOS(pThis);
    goto __nvoc_ctor_OBJOS_exit; // Success

__nvoc_ctor_OBJOS_fail_Object:
__nvoc_ctor_OBJOS_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJOS_1(OBJOS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_OBJOS(OBJOS *pThis) {
    __nvoc_init_funcTable_OBJOS_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJOS(OBJOS *pThis) {
    pThis->__nvoc_pbase_OBJOS = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJOS(pThis);
}

NV_STATUS __nvoc_objCreate_OBJOS(OBJOS **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJOS *pThis;

    pThis = portMemAllocNonPaged(sizeof(OBJOS));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(OBJOS));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJOS);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJOS(pThis);
    status = __nvoc_ctor_OBJOS(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJOS_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_OBJOS_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJOS(OBJOS **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJOS(ppThis, pParent, createFlags);

    return status;
}

