#ifndef _GLOBAL_MODULES_H
#define _GLOBAL_MODULES_H 1

#include <stdint.h>
#include <stdbool.h>
#include <lib/modules/vfs.h>
#include <lib/modules/pci.h>
#include <lib/modules/time.h>
#include <lib/modules/storage.h>

#define MODULE_TYPE_UNDEFINE        (0)
#define MODULE_TYPE_VFS             (1)
#define MODULE_TYPE_PCI             (2)
#define MODULE_TYPE_STORAGE         (3)

#define MODULE_TYPE_COUNT       (4)

#define MODULE_FLAGS_LOADED     (1 << 0)

typedef uint8_t module_type_t;
typedef uint8_t module_flags_t;

typedef struct{
    int (*init)(int argc, char* argv[]);
	int (*fini)(void);
    module_type_t type;
    char* name;
} module_metadata_t;

extern volatile vfs_handler_t* vfs_handler;
extern volatile pci_handler_t* pci_handler;
extern volatile time_handler_t* time_handler;
extern volatile storage_handler_t* storage_handler;

void modules_init(void);
int modules_request_dependency(module_type_t type);
int modules_set_load_state(module_type_t type, bool value);

#endif // _GLOBAL_MODULES_H