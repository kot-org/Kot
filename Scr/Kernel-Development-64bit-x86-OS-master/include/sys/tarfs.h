#ifndef _TARFS_H
#define _TARFS_H
#include <sys/defs.h>
#include <sys/task.h>	
extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};


void init_tarfs();

void elfParsing();
//open, read, close, opendir, readdir, closedir 


typedef struct {
    char name[100];
    int size;
    int typeflag;
    uint64_t addr_hdr;
    int par_ind;
} tarfs_entry;

extern tarfs_entry tarfs_fs[100];

uint64_t open_dir(char * dir);
uint64_t read_dir(uint64_t addr);

typedef struct File{
} *file;

//uint64_t opendirect(char* dir);
//uint64_t readdirect(char* dir);
//void closedirect(uint64_t dir);

uint64_t opendir(char* dir);
uint64_t readdir(char* dir);
void closedir(uint64_t dir);

struct file * tarfs_open(struct posix_header_ustar *tarfheader, int flags, uint64_t mode, int *err);
uint64_t tarfs_read(struct file *fp, char *buf, size_t count, off_t *offset);

 uint64_t openfile(char * file);
//iint openfile(uint64_t fd);
int readfile(uint64_t fd, int size, uint64_t buf);
//int closefile(uint64_t fd);
 void closefile(uint64_t file_addr);

int find_elf_header(char *);

int get_per_ind(char* dir);
void tarfs_init();
uint64_t tarfs_open_file(char * filename);
int tarfs_read_file(uint64_t fd, int size, char *buf);

uint64_t sy_chdir(char *path, struct task_struct *proc );
//struct vma_struct * allocateVMA(Elf64_Ehdr ehdr, uint64_t start,uint64_t end,uint64_t flags,uint64_t pheader_offset,uint64_t type, uint64_t p_filesz);
uint64_t getElfHeader(char * fileName);
#define DIRECTORY 5
#define FILE_TYPE 0
#define TARFS_NORMAL_FILE_1   '0'
#define TARFS_NORMAL_FILE_2   '\0'
#define TARFS_HARD_LINK       '1'
#define TARFS_SYMBOLIC_LINK   '2'
#define TARFS_CHARACTER       '3'
#define TARFS_BLOCK           '4'
#define TARFS_DIRECTORY '5'

#endif
