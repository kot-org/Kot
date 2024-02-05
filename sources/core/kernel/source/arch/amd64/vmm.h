#ifndef _AMD64_VMM_H
#define _AMD64_VMM_H

#include <impl/vmm.h>

int vmm_clear_lower_half_entries(vmm_space_t space);
int vmm_preload_higher_half_entries(vmm_space_t space);

#endif // _AMD64_VMM_H
