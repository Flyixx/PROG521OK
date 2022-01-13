#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "elf_header.h"
#include "elf_sections.h"
#include "elf_hexdump.h"
#include "elf.h"
#include "elf_symtab.h"
#include "string.h"
#include "my_fonctions.h"
#include "elf_reltab.h"
#include "util.h"
#include "gestionErr.h"

 Elf32_Rel ** read_reloc(FILE *f,Elf32_ShdrNames* Sn,Elf32_SymNames* SymN,Elf32_Ehdr H)
{
	if(errorCheck()){return NULL;}
	Elf32_Rel **R=NULL;
	int k=0;
	for(int i=0;i<H.e_shnum;i++)
	{
		if(Sn[i].S.sh_type==9)
		{
			k++;
		}
	}
	int l=0;
	R=my_malloc(sizeof(Elf32_Rel *)*k);
	if(errorCheck()){return NULL;}
	for(int i=0;i<H.e_shnum;i++)
	{
		if(Sn[i].S.sh_type==9)
		{
			R[l]=my_malloc(sizeof(Elf32_Rel)*Sn[i].S.sh_size/8);
			my_fseek(f,Sn[i].S.sh_offset,SEEK_SET);
			my_fread(R[l],1,Sn[i].S.sh_size,f);
			if(errorCheck()){return NULL;}
			for(int j=0;j<Sn[i].S.sh_size/8;j++)
			{
				R[l][j].r_offset=reverse_4(R[l][j].r_offset);
				R[l][j].r_info=reverse_4(R[l][j].r_info);
			}
			l++;
			}
	}
	return R;
}

void free_reloc(Elf32_Rel **R,Elf32_Ehdr H,Elf32_ShdrNames *Sn) 
	{if(Sn!=NULL && R!=NULL){
		int k=0;
		for(int i=0;i<H.e_shnum;i++)
		{
			if(Sn[i].S.sh_type==9)
			{
				if(R[k]!=NULL){free(R[k]);}
				k++;
			}
		}
		if(R!=NULL){free(R);}
	}}


// Fonction d'affichage
void print_reloc(FILE *f,Elf32_ShdrNames* Sn,Elf32_SymNames* SymN,Elf32_Ehdr H,Elf32_Rel **R){
	int l=0,k=0;
	for(int i=0;i<H.e_shnum;i++)
	{
		if(Sn[i].S.sh_type==9)
		{
			k++;
		}
	}
	if(k!=0){
	for(int i=0;i<H.e_shnum;i++)
	{
		if(Sn[i].S.sh_type==9)
		{
		printf("Relocation section '%s' at offset 0x%x contains %d entry:\n",Sn[i].name,Sn[i].S.sh_offset,Sn[i].S.sh_size/8);
		printf(" Offset        Info       Type               Sym.Value  Sym. Name\n");
		for(int j=0;j<(Sn[i].S.sh_size/8);j++){
    		printf("%.8x     %.8x    ",R[l][j].r_offset,R[l][j].r_info);
			switch(ELF32_R_TYPE(R[l][j].r_info)){
			case 2:
			printf("R_ARM_ABS32        %.8x  ",SymN[ELF32_R_SYM(R[l][j].r_info)].symb.st_value);
			break;		
			case 5:
			printf("R_ARM_ABS16        %.8x  ",SymN[ELF32_R_SYM(R[l][j].r_info)].symb.st_value);
			break;
			case 8:
			printf("R_ARM_ABS8         %.8x  ",SymN[ELF32_R_SYM(R[l][j].r_info)].symb.st_value);
			break;
			case 28:
			printf("R_ARM_CALL         %.8x  ",SymN[ELF32_R_SYM(R[l][j].r_info)].symb.st_value);
			break;
			case 29:
			printf("R_ARM_JUMP24       %.8x  ",SymN[ELF32_R_SYM(R[l][j].r_info)].symb.st_value);
			break;
		}
			if(ELF32_ST_TYPE(SymN[ELF32_R_SYM(R[l][j].r_info)].symb.st_info)==3){
			char *name=Sn[SymN[ELF32_R_SYM(R[l][j].r_info)].symb.st_shndx].name;
    		printf("  %s\n",name);

			}
			else {
				char *name=SymN[ELF32_R_SYM(R[l][j].r_info)].name;
    			printf("  %s\n",name);
			}
		}
		l++;
		}
	}
	}
	else printf("There are no relocations in this file.\n");
}