#ifndef __NV_UNIX_VERSION_H__
#define __NV_UNIX_VERSION_H__

#if defined(NV_LINUX) || defined(NV_BSD) || defined(NV_SUNOS) || defined(NV_VMWARE) || defined(NV_QNX) || defined(NV_INTEGRITY) || \
    (defined(RMCFG_FEATURE_PLATFORM_GSP) && RMCFG_FEATURE_PLATFORM_GSP == 1)

#define NV_VERSION_STRING               "560.35.03"

#else

#error "nvUnixVersion.h should only be included in UNIX builds"

#endif

#endif /* __NV_UNIX_VERSION_H__ */
