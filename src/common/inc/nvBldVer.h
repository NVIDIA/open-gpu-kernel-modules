#ifndef _NVBLDVER_H_
#define _NVBLDVER_H_

#ifndef NVBLDVER_STRINGIZE
#define NVBLDVER_STRINGIZE(t) #t
#endif
#ifndef STRINGIZE
#define STRINGIZE(t) NVBLDVER_STRINGIZE(t)
#endif

// These variables can be overridden using ENV vars, see nvCommon.nvmk. 
// If no env vars are set, then the defaults seen here will be used.
// In DVS builds, the ENV vars are used to control these values. 
// Note- the value of NV_BUILD_CL and NV_BUILD_TYPE_NON_BM is only used in
// non-buildmeister builds, see override section below.
// DVS_SW_CHANGELIST has been added to ENV vars in bug 1486673
#ifndef DVS_SW_CHANGELIST
    #define DVS_SW_CHANGELIST           0
#endif
#ifndef NV_BUILD_CL
   #define NV_BUILD_CL                (DVS_SW_CHANGELIST)
#endif
#if NV_BUILD_CL == 0
   #define NV_BUILD_CL                (DVS_SW_CHANGELIST)
#endif
#ifndef NV_BUILD_TYPE_NON_BM
    #define NV_BUILD_TYPE_NON_BM        Private
#endif
#ifndef NV_BUILD_AUTHOR
    #define NV_BUILD_AUTHOR             unknown
#endif
// End ENV var section


// The values of the following strings are set via a buildmeister python script,
// and then checked back in. You cannot make changes to these sections without
// corresponding changes to the buildmeister script
#ifndef NV_BUILD_BRANCH
    #define NV_BUILD_BRANCH             r560_78
#endif
#ifndef NV_PUBLIC_BRANCH
    #define NV_PUBLIC_BRANCH             r560_78
#endif

#if defined(NV_LINUX) || defined(NV_BSD) || defined(NV_SUNOS)
#define NV_BUILD_BRANCH_VERSION         "rel/gpu_drv/r560/r560_78-120"
#define NV_BUILD_CHANGELIST_NUM         (34643855)
#define NV_BUILD_TYPE                   "Official"
#define NV_BUILD_NAME                   "rel/gpu_drv/r560/r560_78-120"
#define NV_LAST_OFFICIAL_CHANGELIST_NUM (34643855)

#else     /* Windows builds */
#define NV_BUILD_BRANCH_VERSION         "r560_78-2"
#define NV_BUILD_CHANGELIST_NUM         (34643068)
#define NV_BUILD_TYPE                   "Official"
#define NV_BUILD_NAME                   "560.81"
#define NV_LAST_OFFICIAL_CHANGELIST_NUM (34643068)
#define NV_BUILD_BRANCH_BASE_VERSION    R560
#endif
// End buildmeister python edited section

// A few of the values are defined differently for non-buildmeister builds, 
// this section redefines those defines
#ifndef NV_BUILDMEISTER_BLD
    #undef NV_BUILD_TYPE
    #define NV_BUILD_TYPE               STRINGIZE(NV_BUILD_TYPE_NON_BM)
    #undef NV_BUILD_CHANGELIST_NUM     
    #define NV_BUILD_CHANGELIST_NUM     NV_BUILD_CL
#endif

#define NV_DISPLAY_DRIVER_TITLE         NV_BUILD_TYPE " " STRINGIZE(NV_BUILD_BRANCH) " " NV_BUILD_NAME " " STRINGIZE(NV_BUILD_AUTHOR)

#endif
