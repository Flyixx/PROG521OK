#ifndef __ELF_HEAD_H__
#define __ELF_HEAD_H__

#include "elf.h"
typedef struct {
	Elf32_Shdr S;
	char *name;
} Elf32_ShdrNames;
void checkMagic (Elf32_Ehdr elf);
Elf32_Ehdr read_header(FILE *f);
void print_header(Elf32_Ehdr elf);
void print_elf_header_complete(Elf32_Ehdr elf);

#endif