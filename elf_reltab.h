#ifndef __ELF__RELTAB__H__
#define __ELF__RELTAB__H__

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "elf_header.h"
#include "elf_sections.h"
#include "elf_hexdump.h"
#include "elf.h"
#include "elf_symtab.h"
#include "util.h"

Elf32_Rel ** read_reloc(FILE *f,Elf32_ShdrNames* Sn,Elf32_SymNames* SymN,Elf32_Ehdr H);
void print_reloc(FILE *f,Elf32_ShdrNames* Sn,Elf32_SymNames* SymN,Elf32_Ehdr H,Elf32_Rel **R);
void free_reloc(Elf32_Rel **R,Elf32_Ehdr H,Elf32_ShdrNames *Sn);
#endif