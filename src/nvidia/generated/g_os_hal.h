// This file is automatically generated by rmconfig - DO NOT EDIT!
//
// Provides access to OS Hal interfaces.
//
// Profile:  shipping-gpus-openrm
// Haldef:   os.def
// Template: templates/gt_eng_hal.h
//

#ifndef _G_OSHAL_H_
#define _G_OSHAL_H_

//
// Typedefs for OS public object interfaces.
//

//
// "struct" to list OS's public interfaces, eg:  pOs->osInit(pGpu, pOs)
//

//
// macro defines to directly access OS's OBJ interfaces,
// eg: #define osReadFoo(_pGpu, _pOs) _pOs->obj._osReadFoo(_pGpu, _pOs)
//

//
// macro defines to access OS's function pointers,
// eg: #define osReadFoo_FNPTR(_pOs) _pOs->obj.__osReadFoo__
// or  #define osReadFoo_FNPTR(_pOs) _pOs->__osReadFoo__
//

//
// Typedefs for OS HAL interfaces.
//

//
// struct to access OS's hal interfaces, eg:  pOs->hal.osReadFoo(pGpu, pOs)
//

//
// macro defines to directly access OS's hal interfaces,
// eg: #define osReadFoo_HAL(_pGpu, _pOs) _pOs->hal.osReadFoo(_pGpu, _pOs)
//

//
// Inline stub function definitions.
//

//
// OS PDB properties
//

typedef struct PDB_PROP_OS {

    NvBool PDB_PROP_OS_PAT_UNSUPPORTED;
    NvBool PDB_PROP_OS_SLI_ALLOWED;
    NvBool PDB_PROP_OS_SYSTEM_EVENTS_SUPPORTED;
    NvBool PDB_PROP_OS_ONDEMAND_VBLANK_CONTROL_ENABLE_DEFAULT;
    NvBool PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM;
    NvBool PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED;
    NvBool PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE;
    NvBool PDB_PROP_OS_LIMIT_GPU_RESET;
    NvBool PDB_PROP_OS_SUPPORTS_TDR;
    NvBool PDB_PROP_OS_GET_ACPI_TABLE_FROM_UEFI;
    NvBool PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER;      // Set if this OS supports the display remapper (otherwise force DNISO to vidmem if display can't access all of sysmem).
    NvBool PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS; // The OS does not allow the driver to map the PCIE config space.
    NvBool PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS;        // Accessing paged segment might cause problem at some code path. Set the flag up on the code path to make it fail osPagedSegmentAccessCheck()

} PDB_PROP_OS;


#endif  // _G_OSHAL_H_
