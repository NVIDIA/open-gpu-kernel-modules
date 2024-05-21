#define NVOC_GPU_ACCESS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_access_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x40549c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IoAperture;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterAperture;

void __nvoc_init_IoAperture(IoAperture*);
void __nvoc_init_funcTable_IoAperture(IoAperture*);
NV_STATUS __nvoc_ctor_IoAperture(IoAperture*, struct IoAperture * arg_pParentAperture, OBJGPU * arg_pGpu, NvU32 arg_deviceIndex, NvU32 arg_deviceInstance, DEVICE_MAPPING * arg_pMapping, NvU32 arg_mappingStartAddr, NvU32 arg_offset, NvU32 arg_length);
void __nvoc_init_dataField_IoAperture(IoAperture*);
void __nvoc_dtor_IoAperture(IoAperture*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_IoAperture;

static const struct NVOC_RTTI __nvoc_rtti_IoAperture_IoAperture = {
    /*pClassDef=*/          &__nvoc_class_def_IoAperture,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_IoAperture,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_IoAperture_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(IoAperture, __nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_IoAperture_RegisterAperture = {
    /*pClassDef=*/          &__nvoc_class_def_RegisterAperture,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_IoAperture = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_IoAperture_IoAperture,
        &__nvoc_rtti_IoAperture_RegisterAperture,
        &__nvoc_rtti_IoAperture_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_IoAperture = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(IoAperture),
        /*classId=*/            classId(IoAperture),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "IoAperture",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_IoAperture,
    /*pCastInfo=*/          &__nvoc_castinfo_IoAperture,
    /*pExportInfo=*/        &__nvoc_export_info_IoAperture
};

// 8 down-thunk(s) defined to bridge methods in IoAperture from superclasses

// ioaprtReadReg08: virtual override (regaprt) base (regaprt)
static NvU8 __nvoc_down_thunk_IoAperture_regaprtReadReg08(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtReadReg08((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr);
}

// ioaprtReadReg16: virtual override (regaprt) base (regaprt)
static NvU16 __nvoc_down_thunk_IoAperture_regaprtReadReg16(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtReadReg16((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr);
}

// ioaprtReadReg32: virtual override (regaprt) base (regaprt)
static NvU32 __nvoc_down_thunk_IoAperture_regaprtReadReg32(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtReadReg32((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr);
}

// ioaprtWriteReg08: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_IoAperture_regaprtWriteReg08(struct RegisterAperture *pAperture, NvU32 addr, NvV8 value) {
    ioaprtWriteReg08((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr, value);
}

// ioaprtWriteReg16: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_IoAperture_regaprtWriteReg16(struct RegisterAperture *pAperture, NvU32 addr, NvV16 value) {
    ioaprtWriteReg16((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr, value);
}

// ioaprtWriteReg32: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_IoAperture_regaprtWriteReg32(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    ioaprtWriteReg32((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr, value);
}

// ioaprtWriteReg32Uc: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_IoAperture_regaprtWriteReg32Uc(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    ioaprtWriteReg32Uc((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr, value);
}

// ioaprtIsRegValid: virtual override (regaprt) base (regaprt)
static NvBool __nvoc_down_thunk_IoAperture_regaprtIsRegValid(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtIsRegValid((struct IoAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_IoAperture_RegisterAperture.offset), addr);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_IoAperture = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RegisterAperture(RegisterAperture*);
void __nvoc_dtor_IoAperture(IoAperture *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_IoAperture(IoAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RegisterAperture(RegisterAperture* );
NV_STATUS __nvoc_ctor_IoAperture(IoAperture *pThis, struct IoAperture * arg_pParentAperture, OBJGPU * arg_pGpu, NvU32 arg_deviceIndex, NvU32 arg_deviceInstance, DEVICE_MAPPING * arg_pMapping, NvU32 arg_mappingStartAddr, NvU32 arg_offset, NvU32 arg_length) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_IoAperture_fail_Object;
    status = __nvoc_ctor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
    if (status != NV_OK) goto __nvoc_ctor_IoAperture_fail_RegisterAperture;
    __nvoc_init_dataField_IoAperture(pThis);

    status = __nvoc_ioaprtConstruct(pThis, arg_pParentAperture, arg_pGpu, arg_deviceIndex, arg_deviceInstance, arg_pMapping, arg_mappingStartAddr, arg_offset, arg_length);
    if (status != NV_OK) goto __nvoc_ctor_IoAperture_fail__init;
    goto __nvoc_ctor_IoAperture_exit; // Success

__nvoc_ctor_IoAperture_fail__init:
    __nvoc_dtor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
__nvoc_ctor_IoAperture_fail_RegisterAperture:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_IoAperture_fail_Object:
__nvoc_ctor_IoAperture_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_IoAperture_1(IoAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // ioaprtReadReg08 -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtReadReg08__ = &ioaprtReadReg08_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtReadReg08__ = &__nvoc_down_thunk_IoAperture_regaprtReadReg08;

    // ioaprtReadReg16 -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtReadReg16__ = &ioaprtReadReg16_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtReadReg16__ = &__nvoc_down_thunk_IoAperture_regaprtReadReg16;

    // ioaprtReadReg32 -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtReadReg32__ = &ioaprtReadReg32_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtReadReg32__ = &__nvoc_down_thunk_IoAperture_regaprtReadReg32;

    // ioaprtWriteReg08 -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtWriteReg08__ = &ioaprtWriteReg08_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg08__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg08;

    // ioaprtWriteReg16 -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtWriteReg16__ = &ioaprtWriteReg16_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg16__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg16;

    // ioaprtWriteReg32 -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtWriteReg32__ = &ioaprtWriteReg32_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg32__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg32;

    // ioaprtWriteReg32Uc -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtWriteReg32Uc__ = &ioaprtWriteReg32Uc_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg32Uc__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg32Uc;

    // ioaprtIsRegValid -- virtual override (regaprt) base (regaprt)
    pThis->__ioaprtIsRegValid__ = &ioaprtIsRegValid_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtIsRegValid__ = &__nvoc_down_thunk_IoAperture_regaprtIsRegValid;
} // End __nvoc_init_funcTable_IoAperture_1 with approximately 16 basic block(s).


// Initialize vtable(s) for 8 virtual method(s).
void __nvoc_init_funcTable_IoAperture(IoAperture *pThis) {

    // Initialize vtable(s) with 8 per-object function pointer(s).
    __nvoc_init_funcTable_IoAperture_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RegisterAperture(RegisterAperture*);
void __nvoc_init_IoAperture(IoAperture *pThis) {
    pThis->__nvoc_pbase_IoAperture = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    pThis->__nvoc_pbase_RegisterAperture = &pThis->__nvoc_base_RegisterAperture;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
    __nvoc_init_funcTable_IoAperture(pThis);
}

NV_STATUS __nvoc_objCreate_IoAperture(IoAperture **ppThis, Dynamic *pParent, NvU32 createFlags, struct IoAperture * arg_pParentAperture, OBJGPU * arg_pGpu, NvU32 arg_deviceIndex, NvU32 arg_deviceInstance, DEVICE_MAPPING * arg_pMapping, NvU32 arg_mappingStartAddr, NvU32 arg_offset, NvU32 arg_length)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    IoAperture *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(IoAperture), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(IoAperture));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_IoAperture);

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

    __nvoc_init_IoAperture(pThis);
    status = __nvoc_ctor_IoAperture(pThis, arg_pParentAperture, arg_pGpu, arg_deviceIndex, arg_deviceInstance, arg_pMapping, arg_mappingStartAddr, arg_offset, arg_length);
    if (status != NV_OK) goto __nvoc_objCreate_IoAperture_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_IoAperture_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(IoAperture));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_IoAperture(IoAperture **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct IoAperture * arg_pParentAperture = va_arg(args, struct IoAperture *);
    OBJGPU * arg_pGpu = va_arg(args, OBJGPU *);
    NvU32 arg_deviceIndex = va_arg(args, NvU32);
    NvU32 arg_deviceInstance = va_arg(args, NvU32);
    DEVICE_MAPPING * arg_pMapping = va_arg(args, DEVICE_MAPPING *);
    NvU32 arg_mappingStartAddr = va_arg(args, NvU32);
    NvU32 arg_offset = va_arg(args, NvU32);
    NvU32 arg_length = va_arg(args, NvU32);

    status = __nvoc_objCreate_IoAperture(ppThis, pParent, createFlags, arg_pParentAperture, arg_pGpu, arg_deviceIndex, arg_deviceInstance, arg_pMapping, arg_mappingStartAddr, arg_offset, arg_length);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x6d0f88 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwBcAperture;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterAperture;

void __nvoc_init_SwBcAperture(SwBcAperture*);
void __nvoc_init_funcTable_SwBcAperture(SwBcAperture*);
NV_STATUS __nvoc_ctor_SwBcAperture(SwBcAperture*, struct IoAperture * arg_pApertures, NvU32 arg_numApertures);
void __nvoc_init_dataField_SwBcAperture(SwBcAperture*);
void __nvoc_dtor_SwBcAperture(SwBcAperture*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SwBcAperture;

static const struct NVOC_RTTI __nvoc_rtti_SwBcAperture_SwBcAperture = {
    /*pClassDef=*/          &__nvoc_class_def_SwBcAperture,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SwBcAperture,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SwBcAperture_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwBcAperture, __nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SwBcAperture_RegisterAperture = {
    /*pClassDef=*/          &__nvoc_class_def_RegisterAperture,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SwBcAperture = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_SwBcAperture_SwBcAperture,
        &__nvoc_rtti_SwBcAperture_RegisterAperture,
        &__nvoc_rtti_SwBcAperture_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SwBcAperture = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SwBcAperture),
        /*classId=*/            classId(SwBcAperture),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SwBcAperture",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SwBcAperture,
    /*pCastInfo=*/          &__nvoc_castinfo_SwBcAperture,
    /*pExportInfo=*/        &__nvoc_export_info_SwBcAperture
};

// 8 down-thunk(s) defined to bridge methods in SwBcAperture from superclasses

// swbcaprtReadReg08: virtual override (regaprt) base (regaprt)
static NvU8 __nvoc_down_thunk_SwBcAperture_regaprtReadReg08(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtReadReg08((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr);
}

// swbcaprtReadReg16: virtual override (regaprt) base (regaprt)
static NvU16 __nvoc_down_thunk_SwBcAperture_regaprtReadReg16(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtReadReg16((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr);
}

// swbcaprtReadReg32: virtual override (regaprt) base (regaprt)
static NvU32 __nvoc_down_thunk_SwBcAperture_regaprtReadReg32(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtReadReg32((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr);
}

// swbcaprtWriteReg08: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg08(struct RegisterAperture *pAperture, NvU32 addr, NvV8 value) {
    swbcaprtWriteReg08((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr, value);
}

// swbcaprtWriteReg16: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg16(struct RegisterAperture *pAperture, NvU32 addr, NvV16 value) {
    swbcaprtWriteReg16((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr, value);
}

// swbcaprtWriteReg32: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg32(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    swbcaprtWriteReg32((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr, value);
}

// swbcaprtWriteReg32Uc: virtual override (regaprt) base (regaprt)
static void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg32Uc(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    swbcaprtWriteReg32Uc((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr, value);
}

// swbcaprtIsRegValid: virtual override (regaprt) base (regaprt)
static NvBool __nvoc_down_thunk_SwBcAperture_regaprtIsRegValid(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtIsRegValid((struct SwBcAperture *)(((unsigned char *) pAperture) - __nvoc_rtti_SwBcAperture_RegisterAperture.offset), addr);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_SwBcAperture = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RegisterAperture(RegisterAperture*);
void __nvoc_dtor_SwBcAperture(SwBcAperture *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SwBcAperture(SwBcAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RegisterAperture(RegisterAperture* );
NV_STATUS __nvoc_ctor_SwBcAperture(SwBcAperture *pThis, struct IoAperture * arg_pApertures, NvU32 arg_numApertures) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_SwBcAperture_fail_Object;
    status = __nvoc_ctor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
    if (status != NV_OK) goto __nvoc_ctor_SwBcAperture_fail_RegisterAperture;
    __nvoc_init_dataField_SwBcAperture(pThis);

    status = __nvoc_swbcaprtConstruct(pThis, arg_pApertures, arg_numApertures);
    if (status != NV_OK) goto __nvoc_ctor_SwBcAperture_fail__init;
    goto __nvoc_ctor_SwBcAperture_exit; // Success

__nvoc_ctor_SwBcAperture_fail__init:
    __nvoc_dtor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
__nvoc_ctor_SwBcAperture_fail_RegisterAperture:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_SwBcAperture_fail_Object:
__nvoc_ctor_SwBcAperture_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SwBcAperture_1(SwBcAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // swbcaprtReadReg08 -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtReadReg08__ = &swbcaprtReadReg08_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtReadReg08__ = &__nvoc_down_thunk_SwBcAperture_regaprtReadReg08;

    // swbcaprtReadReg16 -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtReadReg16__ = &swbcaprtReadReg16_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtReadReg16__ = &__nvoc_down_thunk_SwBcAperture_regaprtReadReg16;

    // swbcaprtReadReg32 -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtReadReg32__ = &swbcaprtReadReg32_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtReadReg32__ = &__nvoc_down_thunk_SwBcAperture_regaprtReadReg32;

    // swbcaprtWriteReg08 -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtWriteReg08__ = &swbcaprtWriteReg08_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg08__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg08;

    // swbcaprtWriteReg16 -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtWriteReg16__ = &swbcaprtWriteReg16_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg16__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg16;

    // swbcaprtWriteReg32 -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtWriteReg32__ = &swbcaprtWriteReg32_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg32__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg32;

    // swbcaprtWriteReg32Uc -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtWriteReg32Uc__ = &swbcaprtWriteReg32Uc_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtWriteReg32Uc__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg32Uc;

    // swbcaprtIsRegValid -- virtual override (regaprt) base (regaprt)
    pThis->__swbcaprtIsRegValid__ = &swbcaprtIsRegValid_IMPL;
    pThis->__nvoc_base_RegisterAperture.__regaprtIsRegValid__ = &__nvoc_down_thunk_SwBcAperture_regaprtIsRegValid;
} // End __nvoc_init_funcTable_SwBcAperture_1 with approximately 16 basic block(s).


// Initialize vtable(s) for 8 virtual method(s).
void __nvoc_init_funcTable_SwBcAperture(SwBcAperture *pThis) {

    // Initialize vtable(s) with 8 per-object function pointer(s).
    __nvoc_init_funcTable_SwBcAperture_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RegisterAperture(RegisterAperture*);
void __nvoc_init_SwBcAperture(SwBcAperture *pThis) {
    pThis->__nvoc_pbase_SwBcAperture = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    pThis->__nvoc_pbase_RegisterAperture = &pThis->__nvoc_base_RegisterAperture;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);
    __nvoc_init_funcTable_SwBcAperture(pThis);
}

NV_STATUS __nvoc_objCreate_SwBcAperture(SwBcAperture **ppThis, Dynamic *pParent, NvU32 createFlags, struct IoAperture * arg_pApertures, NvU32 arg_numApertures)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    SwBcAperture *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SwBcAperture), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(SwBcAperture));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SwBcAperture);

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

    __nvoc_init_SwBcAperture(pThis);
    status = __nvoc_ctor_SwBcAperture(pThis, arg_pApertures, arg_numApertures);
    if (status != NV_OK) goto __nvoc_objCreate_SwBcAperture_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SwBcAperture_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SwBcAperture));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SwBcAperture(SwBcAperture **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct IoAperture * arg_pApertures = va_arg(args, struct IoAperture *);
    NvU32 arg_numApertures = va_arg(args, NvU32);

    status = __nvoc_objCreate_SwBcAperture(ppThis, pParent, createFlags, arg_pApertures, arg_numApertures);

    return status;
}

