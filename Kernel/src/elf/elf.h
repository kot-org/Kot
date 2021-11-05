#pragma once
#include "../main/kernelInit.h"

namespace ELF{
    /* Function */
    int loadElf(void* buffer);

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
    Elf64_Xword	p_memsz;		/* Segment size in memory */
    Elf64_Xword	p_align;		/* Segment alignment */
    };


    #define	PT_NULL		0		/* Program header table entry unused */
    #define PT_LOAD		1		/* Loadable program segment */
    #define PT_DYNAMIC	2		/* Dynamic linking information */
    #define PT_INTERP	3		/* Program interpreter */
    #define PT_NOTE		4		/* Auxiliary information */
    #define PT_SHLIB	5		/* Reserved */
    #define PT_PHDR		6		/* Entry for header table itself */
    #define PT_TLS		7		/* Thread-local storage segment */
    #define	PT_NUM		8		/* Number of defined types */
    #define PT_LOOS		    0x60000000	/* Start of OS-specific */
    #define PT_GNU_EH_FRAME	0x6474e550	/* GCC .eh_frame_hdr segment */
    #define PT_GNU_STACK	0x6474e551	/* Indicates stack executability */
    #define PT_GNU_RELRO	0x6474e552	/* Read-only after relocation */
    #define PT_LOSUNW	    0x6ffffffa
    #define PT_SUNWBSS	    0x6ffffffa	/* Sun Specific segment */
    #define PT_SUNWSTACK	0x6ffffffb	/* Stack segment */
    #define PT_HISUNW	    0x6fffffff
    #define PT_HIOS		    0x6fffffff	/* End of OS-specific */
    #define PT_LOPROC	    0x70000000	/* Start of processor-specific */
    #define PT_HIPROC	    0x7fffffff	/* End of processor-specific */

    #define EI_MAG0 0x7f
    #define EI_MAG1 'E'
    #define EI_MAG2 'L'
    #define EI_MAG3 'F'
}