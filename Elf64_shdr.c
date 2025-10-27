#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <elf.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <ELF file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    /* Read ELF header */
    Elf64_Ehdr eh;
    fread(&eh, 1, sizeof(eh), f);

    if (eh.e_ident[0] != 0x7f ||
        eh.e_ident[1] != 'E' ||
        eh.e_ident[2] != 'L' ||
        eh.e_ident[3] != 'F')
    {
        fprintf(stderr, "Not an ELF file\n");
        fclose(f);
        return 1;
    }

    printf("ELF Header:\n");
    printf("  Class: %s\n", (eh.e_ident[EI_CLASS] == ELFCLASS64) ? "ELF64" : "Other");
    printf("  Program Headers: %u\n", eh.e_phnum);
    printf("  Section Headers: %u\n", eh.e_shnum);
    printf("\n");

    /* -------- Program Headers -------- */
    printf("Program Headers:\n");

    fseek(f, eh.e_phoff, SEEK_SET);

    for (int i = 0; i < eh.e_phnum; i++) {
        Elf64_Phdr ph;
        fread(&ph, sizeof(ph), 1, f);

        printf("PHDR %d:\n", i);
        printf("  Type:   0x%x\n", ph.p_type);
        printf("  Flags:  0x%x\n", ph.p_flags);
        printf("  Offset: 0x%lx\n", ph.p_offset);
        printf("  VAddr:  0x%lx\n", ph.p_vaddr);
        printf("  FileSz: 0x%lx\n", ph.p_filesz);
        printf("  MemSz:  0x%lx\n", ph.p_memsz);
        printf("  Align:  0x%lx\n\n", ph.p_align);
    }

    /* -------- Section Headers -------- */
    printf("\nSection Headers:\n");

    fseek(f, eh.e_shoff, SEEK_SET);

    /* Read all section headers */
    Elf64_Shdr *shdr = malloc(sizeof(Elf64_Shdr) * eh.e_shnum);
    fread(shdr, sizeof(Elf64_Shdr), eh.e_shnum, f);

    /* Load section header string table */
    Elf64_Shdr shstr = shdr[eh.e_shstrndx];

    char *shstrtab = malloc(shstr.sh_size);
    fseek(f, shstr.sh_offset, SEEK_SET);
    fread(shstrtab, shstr.sh_size, 1, f);

    /* Print names */
    for (int i = 0; i < eh.e_shnum; i++) {
        const char *name = shstrtab + shdr[i].sh_name;
        printf("[%2d] %-20s Off: 0x%06lx Size: 0x%06lx\n",
               i, name, shdr[i].sh_offset, shdr[i].sh_size);
    }

    /* Cleanup */
    free(shstrtab);
    free(shdr);
    fclose(f);
    return 0;
}
