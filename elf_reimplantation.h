#ifndef __ELF__RENUMSEC__H__
#define __ELF__RENUMSEC__H__
#include "elf.h"
#include "elf_header.h"

/*=================================================== changeHeader ===============================================================================
creer le header du nouveau .o
Le meme sans les sections SHT_REL
=================================================================================================================================================*/
Elf32_Ehdr changeHeader (Elf32_Ehdr elf,Elf32_ShdrNames *Sn);


/*================================================== changeSections ===============================================================================
creer la table des sections du nouveau .o
La meme sans les sections SHT_REL
=================================================================================================================================================*/
Elf32_ShdrNames * changeSections (Elf32_Ehdr elf,Elf32_Ehdr Header,Elf32_ShdrNames *SnOld);

/*================================================== changeSymTable ===============================================================================
creer la table des symboles du nouveau .o
change les values d'offset a valeur absolue
supporte .data .text .bss
=================================================================================================================================================*/
Elf32_SymNames * changeSymTable (Elf32_ShdrNames *SnOld,Elf32_ShdrNames *SnNew,int indice,Elf32_SymNames *SymNOld,int* tabAdr,int* tabIndice); 

/*================================================== writeFileHeader ==============================================================================
ecris le nouveau header dans fNew
=================================================================================================================================================*/
void writeFileHeader (FILE* fNew, Elf32_Ehdr elfNew);

/*================================================= writeFileSectionHeader ========================================================================
ecris la nouvelle table des sections dans fNew
=================================================================================================================================================*/
void writeFileSectionHeader(FILE* fNew,Elf32_Ehdr elfNew,Elf32_ShdrNames *SnNew);

/*================================================== writeFileSection =============================================================================
ecris les anciennes sections au bonnes endroi dans fNew
=================================================================================================================================================*/
void writeFileSection(FILE* fOld,FILE* fNew,Elf32_Ehdr elfOld,Elf32_Ehdr elfNew,Elf32_ShdrNames *SnOld,Elf32_ShdrNames *SnNew);

/*================================================== writeFileSymTable ============================================================================
ecris la nouvelle table des symboles dans fNew
=================================================================================================================================================*/
void writeFileSymTable (FILE* fNew, Elf32_SymNames * SymNNew, Elf32_ShdrNames * SnNew, int indice);

/*==================================================== writeReimp =================================================================================
effectue les modifications indiquee dans la table de reimplentation dans FNew
pour R_ARM_ABS8 R_ARM_ABS16 R_ARM_ABS32
R_ARM_JUMP24 R_ARM_CALL ne fonctionnent pas
=================================================================================================================================================*/
void writeReimp (FILE *FNew,Elf32_Rel **R,Elf32_Ehdr elfOld,Elf32_ShdrNames *SnOld,Elf32_ShdrNames *SnNew,Elf32_SymNames *symN) ;
#endif