#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "elf_header.h"
#include "elf.h"
#include "elf_sections.h"
#include "elf_symtab.h"
#include "my_fonctions.h"
#include "gestionErr.h"

Elf32_SymNames * createSymNames(int i,Elf32_ShdrNames *Sn){
    return my_malloc(sizeof(Elf32_SymNames)*Sn[i].S.sh_size);
}

void freeSymNames(Elf32_SymNames * symN,char *SymbNames){
    if(symN!=NULL){free(symN);}
    if(SymbNames!=NULL){free(SymbNames);}
}

void checkSymbolTableIndex0 (Elf32_SymNames *SymN) {
    if(!(
        SymN[0].symb.st_name     ==    0 &&
        SymN[0].symb.st_value    ==    0 &&
        SymN[0].symb.st_size     ==    0 &&
        SymN[0].symb.st_info     ==    0 &&
        SymN[0].symb.st_other    ==    0 &&
        SymN[0].symb.st_shndx    ==    SHN_UNDEF
    )){
		errorAct(FORMAT,"in checkSymbolTableIndex0\n");
    }
}


// Lecture de la table des symboles dans un fichier
void read_symtab(FILE *f,Elf32_ShdrNames *Sn,Elf32_Ehdr elf,Elf32_SymNames *SymN,char *SymbNames)
{
	if(errorCheck()){return;}
    int i=0;

    // On parcourt les sections à la recherche de .strtab 
    while(strcmp(Sn[i].name,".strtab") && i<elf.e_shnum){
        i++;
    }
    // Si on ne trouve pas on renvoie une erreur
	if(i==elf.e_shnum){
		errorAct(FORMAT,".strtab not found\n");
		return;
	}
	

    // On crée une structure pour les noms 
    SymbNames=my_malloc(Sn[i].S.sh_size);
    my_fseek(f,Sn[i].S.sh_offset,SEEK_SET);
    my_fread(SymbNames,1,Sn[i].S.sh_size,f);
    if(errorCheck()){return;}
	i=0;
    while(strcmp(Sn[i].name,".symtab") && i<elf.e_shnum){
        i++;
    }
    my_fseek(f,Sn[i].S.sh_offset,SEEK_SET);
	for(int j=0;j<Sn[i].S.sh_size/16;j++){
        SymN[j].name = "";
        my_fread(&SymN[j].symb,1,sizeof(SymN[j].symb),f);
		if(errorCheck()){return;}
        SymN[j].symb.st_name=reverse_4(SymN[j].symb.st_name);
        SymN[j].symb.st_value=reverse_4(SymN[j].symb.st_value);
        SymN[j].symb.st_size=reverse_4(SymN[j].symb.st_size);
        SymN[j].symb.st_shndx=reverse_2(SymN[j].symb.st_shndx);
        if (SymN[j].symb.st_name){
            SymN[j].name = SymbNames + SymN[j].symb.st_name;
        }
    }
}


// Ecriture de la table des symboles

void print_symtab(Elf32_SymNames *SymN, int nbEntry){
    printf("Symbol table '.symtab' contains %d entries: \n",nbEntry);
    int i =0;
    printf("Num:    Value   Size    Type       Bind    Vis      Ndx  Name\n");
    for(i=0;i<nbEntry;i++){
        
        //Numéro
        if(i<10){
            printf(" %d:  ",i);
        }else{
            printf("%d:  ",i);
        }

        //Value
        printf("%8.8x      ",SymN[i].symb.st_value);

        //Size
        printf("%d ",SymN[i].symb.st_size);
        printf("   ");

        //Type st_info
        switch(ELF32_ST_TYPE(SymN[i].symb.st_info)){
            case STT_NOTYPE: 
                printf("NOTYPE "); 
                break;
            case STT_SECTION:
                printf("SECTION"); 
                break;
            case STT_OBJECT: 
                printf("OBJECT"); 
                break;
            case STT_FUNC: 
                printf("FUNC"); 
                break;
            case STT_FILE: 
                printf("FILE");
                break;
            case STT_COMMON: 
                printf("COMMON"); 
                break;
            case STT_TLS: 
                printf("TLS"); 
                break;
            case STT_NUM: 
                printf("NUM"); 
                break;
            case STT_LOOS: 
                printf("LOOS");
                break;
            case STT_HIOS: 
                printf("HIOS"); 
                break;
            case STT_LOPROC: 
                printf("LOPROC"); 
                break;
            case STT_HIPROC: 
                printf("HIPROC"); 
                break;
            default:
                printf("Read Error");
                break;
        }

        // Bind st_info
        printf("    ");
        switch(ELF32_ST_BIND(SymN[i].symb.st_info)){
            case STB_LOCAL: 
                printf("LOCAL "); 
                break;
            case STB_GLOBAL: 
                printf("GLOBAL"); 
                break;
            case STB_WEAK: 
                printf("WEAK  "); 
                break;
            case STB_NUM: 
                printf("NUM   "); 
                break;
            case STB_LOOS: 
                printf("LOOS  "); 
                break;
            case STB_HIOS: 
                printf("HIOS  "); 
                break;
            case STB_LOPROC: 
                printf("LOPROC"); 
                break;
            case STB_HIPROC: 
                printf("HIPROC"); 
                break;
            default:
                printf("Read Error");
                break;
        }
        printf("  ");

        // Visibility
        switch(ELF32_ST_VISIBILITY(SymN[i].symb.st_other)){
            case STV_DEFAULT: 
                printf("DEFAULT"); 
                break;
            default:
                printf("Read Error");
                break;
        }
        printf("  ");
        
        // Index Ndx
        switch(SymN[i].symb.st_shndx){
            case 0:
                printf("UND  ");
                break;
            case 0xff00:
                printf("LOPROC  ");
                break;
            case 0xff1f:
                printf("HIPROC   ");
                break;
            case 0xfff1:
                printf("ABS ");
                break;            
            case 0xfff2:
                printf("COMMON ");
                break;
            case 0xffff :
                printf("HIRESERVE ");
                break;    
            default :
                printf("%2d ",SymN[i].symb.st_shndx);
                break;
        }
        // Nom
        printf("  %s\n",SymN[i].name);
    }
}
