#ifndef __ELF_SEC__H__
#define __ELF_SEC__H__

#include <stdint.h>
#include "elf.h"

Elf32_ShdrNames * createSectionTable(Elf32_Ehdr elf);
void freeSectionTable(Elf32_ShdrNames* Sn,char* SectNames);
void read_sections(FILE *f, Elf32_ShdrNames *SN,Elf32_Ehdr H,char* SectNames) ;
void print_section_hexa(FILE *f, Elf32_Shdr sectionTable[], uint32_t numSection, char*stringtab);
void print_sections_header(Elf32_ShdrNames *sectionTable,uint16_t nbSection);
void checkSectionTableIndex0 (Elf32_ShdrNames *sectionTable);

int show_Name(char stringtab[], int name_index);


//void read_string_table(FILE *f,Elf32_Ehdr head,Elf32_Shdr section_tab[],char strtab[],int j);


#endif
