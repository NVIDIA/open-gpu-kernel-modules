#define NVOC_SYSTEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_system_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x40e2c8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSYS;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

void __nvoc_init_OBJSYS(OBJSYS*);
void __nvoc_init_funcTable_OBJSYS(OBJSYS*);
NV_STATUS __nvoc_ctor_OBJSYS(OBJSYS*);
void __nvoc_init_dataField_OBJSYS(OBJSYS*);
void __nvoc_dtor_OBJSYS(OBJSYS*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJSYS;

static const struct NVOC_RTTI __nvoc_rtti_OBJSYS_OBJSYS = {
    /*pClassDef=*/          &__nvoc_class_def_OBJSYS,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJSYS,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJSYS_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJSYS, __nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJSYS_OBJTRACEABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJTRACEABLE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJSYS, __nvoc_base_OBJTRACEABLE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJSYS = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_OBJSYS_OBJSYS,
        &__nvoc_rtti_OBJSYS_OBJTRACEABLE,
        &__nvoc_rtti_OBJSYS_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSYS = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJSYS),
        /*classId=*/            classId(OBJSYS),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJSYS",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJSYS,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJSYS,
    /*pExportInfo=*/        &__nvoc_export_info_OBJSYS
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJSYS = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJSYS(OBJSYS *pThis) {
    __nvoc_sysDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJSYS(OBJSYS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
    pThis->setProperty(pThis, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE, ((0) || (1) || (0)));
    pThis->setProperty(pThis, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT, ((1) && !0));
    pThis->setProperty(pThis, PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS, (0));
    pThis->setProperty(pThis, PDB_PROP_SYS_INTERNAL_EVENT_BUFFER_ALLOC_ALLOWED, ((0) || (0)));
    pThis->setProperty(pThis, PDB_PROP_SYS_IS_AGGRESSIVE_GC6_ENABLED, (0));
    pThis->setProperty(pThis, PDB_PROP_SYS_PRIORITY_BOOST, (0));
    pThis->setProperty(pThis, PDB_PROP_SYS_PRIORITY_THROTTLE_DELAY_US, 16 * 1000);
    pThis->setProperty(pThis, PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS, ((NvBool)(0 == 0)));
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE* );
NV_STATUS __nvoc_ctor_OBJSYS(OBJSYS *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail_Object;
    status = __nvoc_ctor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail_OBJTRACEABLE;
    __nvoc_init_dataField_OBJSYS(pThis);

    status = __nvoc_sysConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail__init;
    goto __nvoc_ctor_OBJSYS_exit; // Success

__nvoc_ctor_OBJSYS_fail__init:
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
__nvoc_ctor_OBJSYS_fail_OBJTRACEABLE:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJSYS_fail_Object:
__nvoc_ctor_OBJSYS_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJSYS_1(OBJSYS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__sysCaptureState__ = &sysCaptureState_IMPL;
}

void __nvoc_init_funcTable_OBJSYS(OBJSYS *pThis) {
    __nvoc_init_funcTable_OBJSYS_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_OBJSYS(OBJSYS *pThis) {
    pThis->__nvoc_pbase_OBJSYS = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJTRACEABLE = &pThis->__nvoc_base_OBJTRACEABLE;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    __nvoc_init_funcTable_OBJSYS(pThis);
}

NV_STATUS __nvoc_objCreate_OBJSYS(OBJSYS **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJSYS *pThis;

    pThis = portMemAllocNonPaged(sizeof(OBJSYS));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(OBJSYS));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJSYS);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJSYS(pThis);
    status = __nvoc_ctor_OBJSYS(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJSYS_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_OBJSYS_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJSYS(OBJSYS **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJSYS(ppThis, pParent, createFlags);

    return status;
}

