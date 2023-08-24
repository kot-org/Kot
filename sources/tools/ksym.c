#include <elf.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
    Elf64_Addr address;
    Elf64_Xword size;
    char name[];
}__attribute__((packed)) ksym_t;

static inline bool check_elf_signature(Elf64_Ehdr* header){
    return (header->e_ident[EI_MAG0] == ELFMAG0 && header->e_ident[EI_MAG1] == ELFMAG1 && header->e_ident[EI_MAG2] == ELFMAG2 && header->e_ident[EI_MAG3] == ELFMAG3);
}

static inline void add_symbol_to_list(FILE* list_file, const char* name, Elf64_Addr address, Elf64_Xword size){
    size_t name_len = strlen(name);
    size_t info_sym_size = sizeof(Elf64_Addr) + sizeof(Elf64_Xword) + name_len + 1;
    ksym_t* sym = malloc(info_sym_size);
    sym->address = address;
    sym->size = size;
    strncpy((char*)&sym->name, name, name_len);
    sym->name[name_len] = '\0';
    fwrite(sym, info_sym_size, 1, list_file);
}

int main(int argc, char* args[]) {
    if (argc != 3) {
        printf("Usage: %s <elfpath> <listpath>\n", args[0]);
        return EINVAL;
    }

    FILE* elf_file = fopen(args[1], "r");
    if(elf_file == NULL){
        perror("Error opening file");
        return ENOENT;
    }

    Elf64_Ehdr header;
    fread(&header, sizeof(Elf64_Ehdr), 1, elf_file);

    if(!check_elf_signature(&header)){
        printf("Invalid executable file : %s, bad header signature\n", args[1]);
        fclose(elf_file);
        return EINVAL;
    }

    if(header.e_ident[EI_CLASS] != ELFCLASS64){
        printf("Invalid executable file : %s, wrong elf class\n", args[1]);
        fclose(elf_file);
        return EINVAL;
    }

    fseek(elf_file, 0, SEEK_END);
    size_t size = ftell(elf_file);
    fseek(elf_file, 0, SEEK_SET);

    void* buffer = malloc(size);
    
    fread(buffer, size, 1, elf_file);

    FILE* list_file = fopen(args[2], "w+");

    if(list_file == NULL){
        perror("Error opening file");
        fclose(elf_file);
        return ENOENT;
    }

    for(Elf64_Half i = 0; i < header.e_shnum; i++){
        Elf64_Shdr* section_header = (Elf64_Shdr*)((uint8_t*)buffer + header.e_shoff + header.e_shentsize * i);

        if(section_header->sh_type == SHT_SYMTAB){
            Elf64_Shdr* strtab_hdr = (Elf64_Shdr*)((uint8_t*)buffer + header.e_shoff + header.e_shentsize * section_header->sh_link);
            char* sym_names = (char*)((uint8_t*)buffer + strtab_hdr->sh_offset);
            Elf64_Sym* sym_table = (Elf64_Sym*)((uint8_t*)buffer + section_header->sh_offset);

            for(uint64_t y = 0; y < section_header->sh_size / sizeof(Elf64_Sym); y++){
                add_symbol_to_list(list_file, (const char*)(sym_names + sym_table[y].st_name), sym_table[y].st_value, sym_table[y].st_size);
            }
        }
    }

    free(buffer);
    fclose(elf_file);

    return 0;
}
