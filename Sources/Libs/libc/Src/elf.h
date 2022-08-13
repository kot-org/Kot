#ifndef _ELF_H
#define _ELF_H 1

#include <kot/types.h>

/* Data */
/* Type for a 16-bit quantity.  */
typedef uint16_t Elf32_Half;
typedef uint16_t Elf64_Half;
/* Types for signed and unsigned 32-bit quantities.  */
typedef uint32_t Elf32_Word;
typedef	int32_t  Elf32_Sword;
typedef uint32_t Elf64_Word;
typedef	int32_t  Elf64_Sword;
/* Types for signed and unsigned 64-bit quantities.  */
typedef uint64_t Elf32_Xword;
typedef	int64_t  Elf32_Sxword;
typedef uint64_t Elf64_Xword;
typedef	int64_t  Elf64_Sxword;
/* Type of addresses.  */
typedef uint32_t Elf32_Addr;
typedef uint64_t Elf64_Addr;
/* Type of file offsets.  */
typedef uint32_t Elf32_Off;
typedef uint64_t Elf64_Off;
/* Type for section indices, which are 16-bit quantities.  */
typedef uint16_t Elf32_Section;
typedef uint16_t Elf64_Section;
/* Type for version symbol information.  */
typedef Elf32_Half Elf32_Versym;
typedef Elf64_Half Elf64_Versym;
#define EI_NIDENT (16)

#define ELF64_R_SYM(i)			((i) >> 32)
#define ELF64_R_TYPE(i)			((i) & 0xffffffff)

struct Elf64_Ehdr{
    unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
    Elf64_Half	e_type;			/* Object file type */
    Elf64_Half	e_machine;		/* Architecture */
    Elf64_Word	e_version;		/* Object file version */
    Elf64_Addr	e_entry;		/* Entry point virtual address */
    Elf64_Off	e_phoff;		/* Program header table file offset */
    Elf64_Off	e_shoff;		/* Section header table file offset */
    Elf64_Word	e_flags;		/* Processor-specific flags */
    Elf64_Half	e_ehsize;		/* ELF header size in bytes */
    Elf64_Half	e_phentsize;		/* Program header table entry size */
    Elf64_Half	e_phnum;		/* Program header table entry count */
    Elf64_Half	e_shentsize;		/* Section header table entry size */
    Elf64_Half	e_shnum;		/* Section header table entry count */
    Elf64_Half	e_shstrndx;		/* Section header string table index */
};

struct Elf64_Phdr{
    Elf64_Word	p_type;			/* Segment type */
    Elf64_Word	p_flags;		/* Segment flags */
    Elf64_Off	p_offset;		/* Segment file offset */
    Elf64_Addr	p_vaddr;		/* Segment virtual address */
    Elf64_Addr	p_paddr;		/* Segment physical address */
    Elf64_Xword	p_filesz;		/* Segment size in file */
    Elf64_Xword	p_memsz;		/* Segment  size in memory */
    Elf64_Xword	p_align;		/* Segmentalignment */
};

struct Elf64_Shdr{
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint64_t   sh_flags;
    Elf64_Addr sh_addr;
    Elf64_Off  sh_offset;
    uint64_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint64_t   sh_addralign;
    uint64_t   sh_entsize;
};

struct Elf64_Dyn{
  Elf64_Sxword d_tag;		/* entry tag value */
  union{
    Elf64_Xword d_val;
    Elf64_Addr d_ptr;
  }d_un;
};

typedef struct Elf64_Sym{
    uint32_t      st_name;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t      st_shndx;
    Elf64_Addr    st_value;
    uint64_t      st_size;
};

typedef struct Elf64_Rela{
    Elf64_Addr r_offset;
    uint64_t   r_info;
    int64_t    r_addend;
};


#define	PT_NULL		        0		/* Program header table entry unused */
#define PT_LOAD		        1		/* Loadable program segment */
#define PT_DYNAMIC	        2		/* Dynamic linking information */
#define PT_INTERP	        3		/* Program interpreter */
#define PT_NOTE		        4		/* Auxiliary information */
#define PT_SHLIB	        5		/* Reserved */
#define PT_PHDR		        6		/* Entry for header table itself */
#define PT_TLS		        7		/* thread-local storage segment */
#define	PT_NUM		        8		/* Number of defined types */
#define PT_LOOS		        0x60000000	/* Start of OS-specific */
#define PT_GNU_EH_FRAME	    0x6474e550	/* GCC .eh_frame_hdr segment */
#define PT_GNU_STACK	    0x6474e551	/* Indicates stack executability */
#define PT_GNU_RELRO	    0x6474e552	/* Read-only after relocation */
#define PT_LOSUNW	        0x6ffffffa
#define PT_SUNWBSS	        0x6ffffffa	/* Sun Specific segment */
#define PT_SUNWSTACK	    0x6ffffffb	/* Stack segment */
#define PT_HISUNW	        0x6fffffff
#define PT_HIOS		        0x6fffffff	/* End of OS-specific */
#define PT_LOPROC	        0x70000000	/* Start of processor-specific */
#define PT_HIPROC	        0x7fffffff	/* End of processor-specific */
#define EI_MAG0             0x7f
#define EI_MAG1             'E'
#define EI_MAG2             'L'
#define EI_MAG3             'F'

#define DT_NULL		        0
#define DT_NEEDED	        1
#define DT_PLTRELSZ	        2
#define DT_PLTGOT	        3
#define DT_HASH		        4
#define DT_STRTAB	        5
#define DT_SYMTAB	        6
#define DT_RELA		        7
#define DT_RELASZ	        8
#define DT_RELAENT	        9
#define DT_STRSZ	        10
#define DT_SYMENT	        11
#define DT_INIT		        12
#define DT_FINI		        13
#define DT_SONAME	        14
#define DT_RPATH 	        15
#define DT_SYMBOLIC	        16
#define DT_REL	            17
#define DT_RELSZ	        18
#define DT_RELENT	        19
#define DT_PLTREL	        20
#define DT_DEBUG	        21
#define DT_TEXTREL	        22
#define DT_JMPREL	        23
#define DT_ENCODING	        32
#define OLD_DT_LOOS	        0x60000000
#define DT_LOOS		        0x6000000d
#define DT_HIOS		        0x6ffff000
#define DT_VALRNGLO	        0x6ffffd00
#define DT_VALRNGHI	        0x6ffffdff
#define DT_ADDRRNGLO	    0x6ffffe00
#define DT_ADDRRNGHI	    0x6ffffeff
#define DT_VERSYM	        0x6ffffff0
#define DT_RELACOUNT	    0x6ffffff9
#define DT_RELCOUNT	        0x6ffffffa
#define DT_FLAGS_1	        0x6ffffffb
#define DT_VERDEF	        0x6ffffffc
#define	DT_VERDEFNUM	    0x6ffffffd
#define DT_VERNEED	        0x6ffffffe
#define	DT_VERNEEDNUM	    0x6fffffff
#define OLD_DT_HIOS         0x6fffffff
#define DT_LOPROC	        0x70000000
#define DT_HIPROC	        0x7fffffff

#define SHT_NULL	        0
#define SHT_PROGBITS	    1
#define SHT_SYMTAB	        2
#define SHT_STRTAB	        3
#define SHT_RELA	        4
#define SHT_HASH	        5
#define SHT_DYNAMIC	        6
#define SHT_NOTE	        7
#define SHT_NOBITS	        8
#define SHT_REL		        9
#define SHT_SHLIB	        10
#define SHT_DYNSYM	        11
#define SHT_NUM		        12

#endif