#include <sys/defs.h>

enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };

struct file {
    struct file_ops *f_op; /* file ops table */
    unsigned long f_count; /* file object's usage count */
    int f_flags;           /* flags specified on open */
    uint64_t f_pos;           /* file offset (file pointer) */
    unsigned long f_size;  /* file size */
    int f_error;           /* error code */
    void *private_data;    /* tty driver hook, for TARFS it points to file's ustar header */
};

typedef struct file *FILE;



