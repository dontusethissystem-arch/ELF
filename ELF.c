#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ELF.h"
#include <stdlib.h>
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <ELF file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    Elf64_Ehdr_ eh;
    fread(&eh, 1, sizeof(eh), f);

    // Jump to section header table
    fseek(f, eh.e_shoff, SEEK_SET);

    // Load all section headers
    Elf64_Shdr_ sh[eh.e_shnum];
    fread(sh, eh.e_shnum, sizeof(Elf64_Shdr_), f);

    // Load section string table (.shstrtab)
    Elf64_Shdr_ shstr = sh[eh.e_shstrndx];
    char *shstrtab = malloc(shstr.sh_size);
    fseek(f, shstr.sh_offset, SEEK_SET);
    fread(shstrtab, 1, shstr.sh_size, f);

    // Find .fini
    int index = -1;
    for (int i = 0; i < eh.e_shnum; i++) {
        char *name = shstrtab + sh[i].sh_name;
        if (strcmp(name, ".fini") == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf(".fini not found\n");
        return 1;
    }

    Elf64_Shdr_ *fini = &sh[index];

    printf(".fini: offset=0x%lx size=0x%lx addr=0x%lx\n",
           fini->sh_offset, fini->sh_size, fini->sh_addr);

    // Read code bytes
    uint8_t *code = malloc(fini->sh_size);
    fseek(f, fini->sh_offset, SEEK_SET);
    fread(code, 1, fini->sh_size, f);

    printf("Raw bytes: ");
    for (size_t i = 0; i < fini->sh_size; i++)
        printf("%02x ", code[i]);
    printf("\n\nDisasm:\n");

    // Very tiny decoder for common .fini
    size_t i = 0;
    while (i < fini->sh_size) {
        if (code[i] == 0x48 && code[i+1] == 0x83) {
            if (code[i+2] == 0xEC) {
                printf("%lx: sub rsp, %d\n", fini->sh_addr + i, code[i+3]);
                i += 4;
                continue;
            }
            if (code[i+2] == 0xC4) {
                printf("%lx: add rsp, %d\n", fini->sh_addr + i, code[i+3]);
                i += 4;
                continue;
            }
        }
        if (code[i] == 0xC3) {
            printf("%lx: ret\n", fini->sh_addr + i);
            i++;
            continue;
        }
        printf("%lx: db 0x%02x\n", fini->sh_addr + i, code[i]);
        i++;
    }

    free(code);
    free(shstrtab);
    fclose(f);
    return 0;
}
