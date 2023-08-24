#ifndef _GLOBAL_ELF_H
#define _GLOBAL_ELF_H 1

#include <stdint.h>

/* data */
/* type for a 16-bit quantity.  */
typedef uint16_t elf32_half;
typedef uint16_t elf64_half;
/* types for signed and unsigned 32-bit quantities.  */
typedef uint32_t elf32_word;
typedef	int32_t  elf32_sword;
typedef uint32_t elf64_word;
typedef	int32_t  elf64_sword;
/* types for signed and unsigned 64-bit quantities.  */
typedef uint64_t elf32_xword;
typedef	int64_t  elf32_sxword;
typedef uint64_t elf64_xword;
typedef	int64_t  elf64_sxword;
/* type of addresses.  */
typedef uint32_t elf32_addr;
typedef uint64_t elf64_addr;
/* type of file offsets.  */
typedef uint32_t elf32_off;
typedef uint64_t elf64_off;
/* type for section indices, which are 16-bit quantities.  */
typedef uint16_t elf32_section;
typedef uint16_t elf64_section;
/* type for version symbol information.  */
typedef elf32_half elf32_versym;
typedef elf64_half elf64_versym;


#define ELFMAG0         0x7f
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'

#define ELFCLASS32      1
#define ELFCLASS64      2

#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4

#define EI_MAG0         0
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_OSABI        7
#define EI_ABIVERSION   8
#define EI_PAD          9
#define EI_NIDENT       16


#define SHN_UNDEF    0
#define SHN_LOPROC   0xFF00
#define SHN_HIPROC   0xFF1F
#define SHN_LOOS     0xFF20
#define SHN_HIOS     0xFF3F
#define SHN_ABS      0xFFF1
#define SHN_COMMON   0xFFF2

#define R_X86_64_NONE		0
#define R_X86_64_64		    1
#define R_X86_64_PC32		2	
#define R_X86_64_GOT32		3	
#define R_X86_64_PLT32		4	
#define R_X86_64_COPY		5	
#define R_X86_64_GLOB_DAT	6	
#define R_X86_64_JUMP_SLOT	7
#define R_X86_64_RELATIVE	8	
#define R_X86_64_GOTPCREL	9
#define R_X86_64_32		    10
#define R_X86_64_32S		11
#define R_X86_64_16		    12
#define R_X86_64_PC16		13
#define R_X86_64_8		    14	
#define R_X86_64_PC8		15
#define R_X86_64_PC64		24
#define R_X86_64_GOTOFF64   25
#define R_X86_64_GOTPC32    26
#define R_X86_64_GOT64      27
#define R_X86_64_GOTPCREL64 28
#define R_X86_64_GOTPC64    29
#define R_X86_64_PLTOFF64   30

#define ELF64_R_SYM(i)	        ((i) >> 32)
#define ELF64_R_TYPE(i)			((i) & 0xffffffff)

struct elf64_ehdr{
    unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
    elf64_half	e_type;			/* object file type */
    elf64_half	e_machine;		/* architecture */
    elf64_word	e_version;		/* object file version */
    elf64_addr	e_entry;		/* entry point virtual address */
    elf64_off	e_phoff;		/* program header table file offset */
    elf64_off	e_shoff;		/* section header table file offset */
    elf64_word	e_flags;		/* processor-specific flags */
    elf64_half	e_ehsize;		/* elf header size in bytes */
    elf64_half	e_phentsize;		/* program header table entry size */
    elf64_half	e_phnum;		/* program header table entry count */
    elf64_half	e_shentsize;		/* section header table entry size */
    elf64_half	e_shnum;		/* section header table entry count */
    elf64_half	e_shstrndx;		/* section header string table index */
};

struct elf64_phdr{
    elf64_word	p_type;			/* segment type */
    elf64_word	p_flags;		/* segment flags */
    elf64_off	p_offset;		/* segment file offset */
    elf64_addr	p_vaddr;		/* segment virtual address */
    elf64_addr	p_paddr;		/* segment physical address */
    elf64_xword	p_filesz;		/* segment size in file */
    elf64_xword	p_memsz;		/* segment  size in memory */
    elf64_xword	p_align;		/* segmentalignment */
};

struct elf64_shdr{
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint64_t   sh_flags;
    elf64_addr sh_addr;
    elf64_off  sh_offset;
    uint64_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint64_t   sh_addralign;
    uint64_t   sh_entsize;
};

struct Elf64_Dyn{
  elf64_sxword d_tag;		/* entry tag value */
  union{
    elf64_xword d_val;
    elf64_addr d_ptr;
  }d_un;
};

struct elf64_sym{
    uint32_t      st_name;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t      st_shndx;
    elf64_addr    st_value;
    uint64_t      st_size;
};

struct elf64_rela{
    elf64_addr r_offset;
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


#endif // _GLOBAL_ELF_H