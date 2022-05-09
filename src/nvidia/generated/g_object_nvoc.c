#define NVOC_OBJECT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_object_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x497031 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_Object(Object*);
void __nvoc_init_funcTable_Object(Object*);
NV_STATUS __nvoc_ctor_Object(Object*);
void __nvoc_init_dataField_Object(Object*);
void __nvoc_dtor_Object(Object*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Object;

static const struct NVOC_RTTI __nvoc_rtti_Object_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Object,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Object = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_Object_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Object = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Object),
        /*classId=*/            classId(Object),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Object",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Object,
    /*pCastInfo=*/          &__nvoc_castinfo_Object,
    /*pExportInfo=*/        &__nvoc_export_info_Object
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_Object = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Object(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_Object(pThis);
    goto __nvoc_ctor_Object_exit; // Success

__nvoc_ctor_Object_exit:

    return status;
}

static void __nvoc_init_funcTable_Object_1(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_Object(Object *pThis) {
    __nvoc_init_funcTable_Object_1(pThis);
}

void __nvoc_init_Object(Object *pThis) {
    pThis->__nvoc_pbase_Object = pThis;
    __nvoc_init_funcTable_Object(pThis);
}

NV_STATUS __nvoc_objCreate_Object(Object **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    Object *pThis;

    pThis = portMemAllocNonPaged(sizeof(Object));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(Object));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Object);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, pThis);
    }
    else
    {
        pThis->pParent = NULL;
    }

    __nvoc_init_Object(pThis);
    status = __nvoc_ctor_Object(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_Object_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_Object_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Object(Object **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_Object(ppThis, pParent, createFlags);

    return status;
}

