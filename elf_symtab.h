#ifndef __ELF__SYMTAB__H__
#define __ELF__SYMTAB__H__
#include "elf.h"
typedef struct {
	Elf32_Sym symb;
	char* name;
} Elf32_SymNames;

void checkSymbolTableIndex0 (Elf32_SymNames *SymN);
Elf32_SymNames * createSymNames(int i,Elf32_ShdrNames *Sn);
void freeSymNames(Elf32_SymNames * symN,char *SymbNames);
void read_symtab(FILE *f,Elf32_ShdrNames *Sn,Elf32_Ehdr elf,Elf32_SymNames *SymN,char *SymbNames);
void print_symtab(Elf32_SymNames *SymN, int nbEntry);

#endif
