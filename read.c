#include <stdio.h>
#include <stdlib.h>
#include "ELF.h"
#include <stdint.h>
// const char *ET[4] = {"", "ET_RELOC", "ET_EXEC", "ET_DYN"};
// typedef struct
// {
//     unsigned char e_ident[16]; /* ELF Identification */
// 
//     uint16_t e_type;      /* Object file type */
//     uint16_t e_machine;   /* Architecture */
//     uint32_t e_version;   /* ELF format version */
// 
//     uint64_t e_entry;     /* Entry point virtual address */
//     uint64_t e_phoff;     /* Program header table file offset */
//     uint64_t e_shoff;     /* Section header table file offset */
// 
//     uint32_t e_flags;     /* Processor-specific flags */
//     uint16_t e_ehsize;    /* ELF header size */
//     uint16_t e_phentsize; /* Size of one program header */
//     uint16_t e_phnum;     /* Number of program headers */
//     uint16_t e_shentsize; /* Size of one section header */
//     uint16_t e_shnum;     /* Number of section headers */
//     uint16_t e_shstrndx;  /* Index of section name string table */
// } Elf64_Ehdr_;
// 
// typedef struct
// {
//     uint32_t p_type;   /* Segment type */
//     uint32_t p_flags;  /* Segment flags */
//     uint64_t p_offset; /* Offset of segment in file */
//     uint64_t p_vaddr;  /* Virtual address in memory */
//     uint64_t p_paddr;  /* Physical address (ignored on most systems) */
//     uint64_t p_filesz; /* Size of segment in file */
//     uint64_t p_memsz;  /* Size of segment in memory */
//     uint64_t p_align;  /* Alignment of segment */
// } Elf64_Phdr_;
// 
// typedef struct
// {
//     uint32_t sh_name;      /* Offset into section header string table */
//     uint32_t sh_type;      /* Section type */
//     uint64_t sh_flags;     /* Section flags */
//     uint64_t sh_addr;      /* Virtual address in memory */
//     uint64_t sh_offset;    /* Offset in file */
//     uint64_t sh_size;      /* Size in bytes */
//     uint32_t sh_link;      /* Linked section index */
//     uint32_t sh_info;      /* Extra information */
//     uint64_t sh_addralign; /* Section alignment */
//     uint64_t sh_entsize;   /* Size of each entry (if table) */
// } Elf64_Shdr_;


#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof(ARR[0]))
int main(int argc, char *argv[]){
  if (argc < 2) return 1;
  FILE *fptr = fopen(argv[1], "r");
  /*unsigned char e_int[4];
  size_t ret = fread(e_int, sizeof(*e_int), ARRAY_SIZE(e_int), fptr);
  if (ret != ARRAY_SIZE(e_int)){
    fprintf(stderr, "error\n");
    fclose(fptr);
    exit(EXIT_FAILURE);
  }
  printf("ELF MAGIC: %#04x%02x%02x%02x\n", e_int[0], e_int[1], e_int[2], e_int[3]);
  printf("ELF MAGIC: %#04x%c%c%c\n", e_int[0], e_int[1], e_int[2], e_int[3]);
  unsigned char et[1];
  fseek(fptr, 16, SEEK_SET);
  ret = fread(et, 1, 1, fptr);
  if (ret != ARRAY_SIZE(et))
  {
    fprintf(stderr, "error\n");
    fclose(fptr);
    exit(EXIT_FAILURE);
  }
  printf("%#02x: %s\n", et[0], ET[et[0]]);
  fclose(fptr);*/
  Elf64_Ehdr_ ehdr;
  fread(&ehdr, sizeof(ehdr), 1, fptr);

  fseek(fptr, ehdr.e_phoff, SEEK_SET);
  printf("%zu\n", ehdr.e_phoff);
  printf("%d\n", ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++) {
    Elf64_Phdr_ phdr;
    fread(&phdr, sizeof(phdr), 1, fptr);

    printf("PHDR %d:\n", i);
    printf("  Type:   0x%x\n", phdr.p_type);
    printf("  Flags:  0x%x\n", phdr.p_flags);
    printf("  Offset: 0x%lx\n", phdr.p_offset);
    printf("  VAddr:  0x%lx\n", phdr.p_vaddr);
    printf("  PAddr:  0x%lx\n", phdr.p_paddr);
    printf("  FileSz: 0x%lx\n", phdr.p_filesz);
    printf("  MemSz:  0x%lx\n", phdr.p_memsz);
    printf("  Align:  0x%lx\n\n", phdr.p_align);
  }
	int off = fseek(fptr, ehdr.e_shoff, SEEK_SET);
  printf("%d\n", off);
  printf("Section Headers\n");
	Elf64_Shdr_ *shdr = malloc(sizeof(Elf64_Shdr_) * ehdr.e_shnum);
  fread(shdr, sizeof(Elf64_Shdr_), ehdr.e_shnum, fptr);

  Elf64_Shdr_ shstr = shdr[ehdr.e_shstrndx];
  char *shstrtab = malloc(shstr.sh_size);

  fseek(fptr, shstr.sh_offset, SEEK_SET);
  fread(shstrtab, shstr.sh_size, 1, fptr);
  for (int i = 0; i < ehdr.e_shnum; i++){
    const char* name = shstrtab + shdr[i].sh_name;
    printf("[%2d] %-20s off: 0x%06lx size: 0x%06lx\n", i, name, shdr[i].sh_offset, shdr[i].sh_size);
  }
  free(shdr);
  free(shstrtab); 
  return 0;
}
