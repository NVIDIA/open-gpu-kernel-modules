// nvVer.h - Versions of NV drivers

#define NV_COMPANY_NAME_STRING_SHORT    "NVIDIA"
#define NV_COMPANY_NAME_STRING_FULL     "NVIDIA Corporation"
#define NV_COMPANY_NAME_STRING          NV_COMPANY_NAME_STRING_FULL
#define NV_COPYRIGHT_YEAR               "2022"
#define NV_COPYRIGHT                    "(C) " NV_COPYRIGHT_YEAR " NVIDIA Corporation. All rights reserved."  // Please do not use the non-ascii copyright symbol for (C).

#if defined(NV_LINUX) || defined(NV_BSD) || defined(NV_SUNOS) || defined(NV_VMWARE) || defined(NV_QNX) || defined(NV_INTEGRITY) || \
    (defined(RMCFG_FEATURE_PLATFORM_GSP) && RMCFG_FEATURE_PLATFORM_GSP == 1)

// All Version numbering for Unix builds has moved.  (Source should be re-directed to directly include that header.)
#include "nvUnixVersion.h"

#else

#endif
