#ifndef __ELF__HEXDUMP__H__
#define __ELF__HEXDUMP__H__
#include <stdint.h>
#include "elf.h"
#include "elf_header.h"
#include "elf_sections.h"
void print_sections_hexdump(FILE *f,Elf32_ShdrNames *Sn,Elf32_Ehdr H,char *name);
#endif