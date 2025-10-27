#ifndef ELFF_H
#define ELFF_H
#include <stdint.h>
#define __offsetof(s, m) ((size_t)&(((s*)0)->m))

const char *ET[4] = {"", "ET_RELOC", "ET_EXEC", "ET_DYN"};
typedef struct
{
    unsigned char e_ident[16]; /* ELF Identification */
    uint16_t e_type;      /* Object file type */
    uint16_t e_machine;   /* Architecture */
    uint32_t e_version;   /* ELF format version */

    uint64_t e_entry;     /* Entry point virtual address */
    uint64_t e_phoff;     /* Program header table file offset */
    uint64_t e_shoff;     /* Section header table file offset */

    uint32_t e_flags;     /* Processor-specific flags */
    uint16_t e_ehsize;    /* ELF header size */
    uint16_t e_phentsize; /* Size of one program header */
    uint16_t e_phnum;     /* Number of program headers */
    uint16_t e_shentsize; /* Size of one section header */
    uint16_t e_shnum;     /* Number of section headers */
    uint16_t e_shstrndx;  /* Index of section name string table */
} Elf64_Ehdr_;

typedef struct
{
    uint32_t p_type;   /* Segment type */
    uint32_t p_flags;  /* Segment flags */
    uint64_t p_offset; /* Offset of segment in file */
    uint64_t p_vaddr;  /* Virtual address in memory */
    uint64_t p_paddr;  /* Physical address (ignored on most systems) */
    uint64_t p_filesz; /* Size of segment in file */
    uint64_t p_memsz;  /* Size of segment in memory */
    uint64_t p_align;  /* Alignment of segment */
} Elf64_Phdr_;

typedef struct
{
    uint32_t sh_name;      /* Offset into section header string table */
    uint32_t sh_type;      /* Section type */
    uint64_t sh_flags;     /* Section flags */
    uint64_t sh_addr;      /* Virtual address in memory */
    uint64_t sh_offset;    /* Offset in file */
    uint64_t sh_size;      /* Size in bytes */
    uint32_t sh_link;      /* Linked section index */
    uint32_t sh_info;      /* Extra information */
    uint64_t sh_addralign; /* Section alignment */
    uint64_t sh_entsize;   /* Size of each entry (if table) */
} Elf64_Shdr_;
#endif // ELFF_H

