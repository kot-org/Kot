#include <global/pmm.h>

#ifndef _AMD64_IMPL_VMM_H
#define _AMD64_IMPL_VMM_H

typedef void* vmm_space_t;

#define VMM_USERSPACE_TOP_ADDRESS hhdm_address
#define VMM_USERSPACE_BOTTOM_ADDRESS PAGE_SIZE

#endif // _AMD64_IMPL_VMM_H
