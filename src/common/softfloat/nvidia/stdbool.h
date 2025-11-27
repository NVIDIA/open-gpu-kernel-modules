#ifndef _NV_STDBOOL_H_
#define _NV_STDBOOL_H_

#if defined(NV_KERNEL_INTERFACE_LAYER) && defined(NV_LINUX)
  #include <linux/types.h>
#else
  #include_next <stdbool.h>
#endif

#endif // _NV_STDBOOL_H_
