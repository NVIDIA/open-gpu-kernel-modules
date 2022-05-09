#define NVOC_GPU_GROUP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_group_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xe40531 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUGRP;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJGPUGRP(OBJGPUGRP*);
void __nvoc_init_funcTable_OBJGPUGRP(OBJGPUGRP*);
NV_STATUS __nvoc_ctor_OBJGPUGRP(OBJGPUGRP*);
void __nvoc_init_dataField_OBJGPUGRP(OBJGPUGRP*);
void __nvoc_dtor_OBJGPUGRP(OBJGPUGRP*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUGRP;

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUGRP_OBJGPUGRP = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGPUGRP,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPUGRP,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUGRP_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPUGRP, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGPUGRP = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGPUGRP_OBJGPUGRP,
        &__nvoc_rtti_OBJGPUGRP_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUGRP = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGPUGRP),
        /*classId=*/            classId(OBJGPUGRP),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGPUGRP",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPUGRP,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGPUGRP,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGPUGRP
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUGRP = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJGPUGRP(OBJGPUGRP *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGPUGRP(OBJGPUGRP *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJGPUGRP(OBJGPUGRP *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUGRP_fail_Object;
    __nvoc_init_dataField_OBJGPUGRP(pThis);
    goto __nvoc_ctor_OBJGPUGRP_exit; // Success

__nvoc_ctor_OBJGPUGRP_fail_Object:
__nvoc_ctor_OBJGPUGRP_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJGPUGRP_1(OBJGPUGRP *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_OBJGPUGRP(OBJGPUGRP *pThis) {
    __nvoc_init_funcTable_OBJGPUGRP_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJGPUGRP(OBJGPUGRP *pThis) {
    pThis->__nvoc_pbase_OBJGPUGRP = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJGPUGRP(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGPUGRP(OBJGPUGRP **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJGPUGRP *pThis;

    pThis = portMemAllocNonPaged(sizeof(OBJGPUGRP));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(OBJGPUGRP));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGPUGRP);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJGPUGRP(pThis);
    status = __nvoc_ctor_OBJGPUGRP(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGPUGRP_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_OBJGPUGRP_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPUGRP(OBJGPUGRP **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGPUGRP(ppThis, pParent, createFlags);

    return status;
}

