#define NVOC_GPU_HALSPEC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_halspec_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x34a6d6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmHalspecOwner;

void __nvoc_init_RmHalspecOwner(RmHalspecOwner*,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        NvU32 DispIpHal_ipver);
void __nvoc_init_funcTable_RmHalspecOwner(RmHalspecOwner*);
NV_STATUS __nvoc_ctor_RmHalspecOwner(RmHalspecOwner*);
void __nvoc_init_dataField_RmHalspecOwner(RmHalspecOwner*);
void __nvoc_dtor_RmHalspecOwner(RmHalspecOwner*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RmHalspecOwner;

static const struct NVOC_RTTI __nvoc_rtti_RmHalspecOwner_RmHalspecOwner = {
    /*pClassDef=*/          &__nvoc_class_def_RmHalspecOwner,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmHalspecOwner,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RmHalspecOwner = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_RmHalspecOwner_RmHalspecOwner,
    },
};

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_RmHalspecOwner = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmHalspecOwner),
        /*classId=*/            classId(RmHalspecOwner),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmHalspecOwner",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_RmHalspecOwner,
    /*pExportInfo=*/        &__nvoc_export_info_RmHalspecOwner
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_RmHalspecOwner = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmHalspecOwner(RmHalspecOwner *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmHalspecOwner(RmHalspecOwner *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmHalspecOwner(RmHalspecOwner *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_RmHalspecOwner(pThis);
    goto __nvoc_ctor_RmHalspecOwner_exit; // Success

__nvoc_ctor_RmHalspecOwner_exit:

    return status;
}

static void __nvoc_init_funcTable_RmHalspecOwner_1(RmHalspecOwner *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_RmHalspecOwner(RmHalspecOwner *pThis) {
    __nvoc_init_funcTable_RmHalspecOwner_1(pThis);
}

void __nvoc_init_RmHalspecOwner(RmHalspecOwner *pThis,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        NvU32 DispIpHal_ipver) {
    pThis->__nvoc_pbase_RmHalspecOwner = pThis;
    __nvoc_init_halspec_ChipHal(&pThis->chipHal, ChipHal_arch, ChipHal_impl, ChipHal_hidrev);
    __nvoc_init_halspec_RmVariantHal(&pThis->rmVariantHal, RmVariantHal_rmVariant);
    __nvoc_init_halspec_DispIpHal(&pThis->dispIpHal, DispIpHal_ipver);
    __nvoc_init_funcTable_RmHalspecOwner(pThis);
}

