#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "elf_header.h"
#include "elf.h"
#include "elf_sections.h"
#include "my_fonctions.h"
#include "gestionErr.h"

//
int show_Name(char stringtab[], int name_index){
    printf("%s\n", &stringtab[name_index]);
    return 1;
}


//Fonction de contrôle 
static inline void debugPrintSectionTableIndex (int niveau, Elf32_ShdrNames sectionTable[], int index){
    if(niveau<=DEBUG){
        fprintf(stderr,"index %d :\n",index);
        //fprintf(stderr,"sh_type %"PRIu32"\nsh_flags %"PRIu32"\nsh_addr %"PRIu32"\nsh_offset %"PRIu32"\nsh_size %"PRIu32"\nsh_link %"PRIu32"\nsh_info %"PRIu32"\nsh_addralign %"PRIu32"\nsh_entsize %"PRIu32"\n",
        fprintf(stderr,"sh_type %d \nsh_flags %d\nsh_addr %d\nsh_offset %d\nsh_size %d\nsh_link %d\nsh_info %d\nsh_addralign %d\nsh_entsize %d\n",
        sectionTable[0].S.sh_type,     
        sectionTable[0].S.sh_flags,    
        sectionTable[0].S.sh_addr,     
        sectionTable[0].S.sh_offset,   
        sectionTable[0].S.sh_size,     
        sectionTable[0].S.sh_link,     
        sectionTable[0].S.sh_info,     
        sectionTable[0].S.sh_addralign,
        sectionTable[0].S.sh_entsize  
        );
    }
}

// Création et libération de la table des sections
Elf32_ShdrNames * createSectionTable(Elf32_Ehdr elf){
	return my_malloc(elf.e_shnum*sizeof(Elf32_ShdrNames));
}

void freeSectionTable(Elf32_ShdrNames* Sn,char* SectNames){
    if(SectNames!=NULL){free(SectNames);}
	if(Sn!=NULL){free(Sn);}
}

// Controle sur l'index 0 de la table des sections (valeurs fixées, voir doc)
void checkSectionTableIndex0 (Elf32_ShdrNames *sectionTable){
    if(!errorCheck() && !(
		sectionTable[0].S.sh_name		  ==  0
	&&	sectionTable[0].S.sh_type     	  ==  SHT_NULL
	&&	sectionTable[0].S.sh_flags        ==  0
	&&	sectionTable[0].S.sh_addr         ==  0
	&&	sectionTable[0].S.sh_offset       ==  0
	&&	sectionTable[0].S.sh_size         ==  0
	&&	sectionTable[0].S.sh_link         ==  SHN_UNDEF
	&&	sectionTable[0].S.sh_info         ==  0
	&&	sectionTable[0].S.sh_addralign    ==  0
	&&	sectionTable[0].S.sh_entsize      ==  0
	))
    {
	errorAct(FORMAT,"in checkSectionTableIndex0\n");
    //debugPrintSectionTableIndex(3,sectionTable,0);
    }
    //PRIu32
}
void read_sections(FILE *f,Elf32_ShdrNames *Sn,Elf32_Ehdr H ,char* SectNames )
{
	if(errorCheck()){return;}
	Elf32_Shdr Sec;
  	my_fseek(f,H.e_shoff+(H.e_shentsize*H.e_shstrndx),SEEK_SET);
	my_fread(&Sec, 1, sizeof(Elf32_Shdr), f);
	SectNames = my_malloc(reverse_4(Sec.sh_size));

	my_fseek(f,reverse_4(Sec.sh_offset),SEEK_SET);//my_my_fseek ?
	my_fread(SectNames, 1, reverse_4(Sec.sh_size), f);
	
  	for (int i = 0; i < H.e_shnum; i++){
        char* name = "";
        my_fseek(f, H.e_shoff + i * sizeof(Sec), SEEK_SET);
        my_fread(&Sn[i].S, 1, sizeof(Sec), f);
        if(errorCheck()){return;}
		if (reverse_4(Sn[i].S.sh_name));
        name=SectNames + reverse_4(Sn[i].S.sh_name);
	    Sn[i].name=name;
        Sn[i].S.sh_name=reverse_4(Sn[i].S.sh_name);
        Sn[i].S.sh_type=reverse_4(Sn[i].S.sh_type);
        Sn[i].S.sh_flags=reverse_4(Sn[i].S.sh_flags);
        Sn[i].S.sh_addr=reverse_4(Sn[i].S.sh_addr);
        Sn[i].S.sh_offset=reverse_4(Sn[i].S.sh_offset);
        Sn[i].S.sh_size=reverse_4(Sn[i].S.sh_size);
        Sn[i].S.sh_link=reverse_4(Sn[i].S.sh_link);
        Sn[i].S.sh_info=reverse_4(Sn[i].S.sh_info);
        Sn[i].S.sh_addralign=reverse_4(Sn[i].S.sh_addralign);
        Sn[i].S.sh_entsize=reverse_4(Sn[i].S.sh_entsize);
    }
    
}

// =========================================================AFFICHAGE SECTION HEADER ============================================================================
void print_sections_header(Elf32_ShdrNames *sectionTable,uint16_t nbSection){
    printf(" Section Headers:\n");
    printf(" [Nr] Name                   Type                 Addr     Off    Size   ES    Flg Lk Inf Al\n");
    for(int i=0;i<nbSection;i++){
    //Affichage Numero
    if(i<10){
        printf("[ %d] ",i);
    }else{
        printf("[%d] ",i);
    }
    //Affichage Name
    printf("%s",sectionTable[i].name);
    //Affichage Type
    int h=strlen(sectionTable[i].name);
    for(int i=0;i<(23-h);i++){printf(" ");}
    char *name;
   switch(sectionTable[i].S.sh_type){
        case 0  :
            name="NULL";
            printf("NULL");
            break;
        case 1:
            name="PROGBITS";
            printf("PROGBITS");
            break;
        case SHT_SYMTAB:
            name="SYMTAB";
            printf("SYMTAB");
            break;
        case SHT_STRTAB:
            name="STRTAB";
            printf("STRTAB");
            break;
        case SHT_RELA:
            name="RELA";
            printf("RELA");
            break;
        case SHT_HASH:
        name="HASH";
            printf("HASH");
            break;
        case SHT_DYNAMIC:
            name="DYNAMIC";
            printf("DYNAMIC");
            break;
        case SHT_NOTE:
            name="NOTE";
            printf("NOTE");
            break;
        case SHT_NOBITS:
            name="NOBITS";
            printf("NOBITS");
            break;
        case SHT_REL:
            name="REL";
            printf("REL");
            break;
        case SHT_SHLIB:
            name="SHLIB";
            printf("SHLIB");
            break;
        case SHT_DYNSYM:
            name="DYNSYM";
            printf("DYNSYM");
            break;
        case SHT_LOPROC:
            name="LOPROC";
            printf("LOPROC");
            break;
        case SHT_HIPROC:
            name="HIPROC";
            printf("HIPROC");
            break;
        case SHT_LOUSER:
            name="LOUSER";
            printf("LOUSER");
            break;
        case SHT_HIUSER:
            name="HIUSER";
            printf("HIUSER");
            break;
        case 0x70000003:
			name="ARM_ATTRIBUTES";
			printf("%s",name);
			break;
        default:
            name="READ ERROR";
            printf("READ ERROR");
            break; 
    }
    for(int i=0;i<21-strlen(name);i++){printf(" ");}
    //Affichage adresse
    printf("%8.8x ",sectionTable[i].S.sh_addr);

    //Offset
    printf("%6.6x ",sectionTable[i].S.sh_offset);

    //Size
    printf("%6.6x ",sectionTable[i].S.sh_size);

    //EntSize
    printf("%6.6x ",sectionTable[i].S.sh_entsize);

    //Flags
    int t=0;
    if(sectionTable[i].S.sh_flags & 1<<0){
			printf("W");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<1){
			printf("A");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<2){
			printf("X");
			t++;
		}
		if(sectionTable[i].S.sh_flags & 1<<4){
			printf("M");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<5){
			printf("S");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<6){
			printf("I");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<7){
			printf("L");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<8){
			printf("o");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<9){
			printf("G");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<10){
			printf("T");
			t++;
			}
		if(sectionTable[i].S.sh_flags & 1<<31){
			printf("E");
			t++;
			}
        for(int i=0;i<3-t;i++)printf(" ");

    //Link
    if(sectionTable[i].S.sh_link<10)printf(" ");
		printf(" %d ",sectionTable[i].S.sh_link);
    //info
		if(sectionTable[i].S.sh_info<10)printf(" ");
		printf(" %d ",sectionTable[i].S.sh_info);
		if(sectionTable[i].S.sh_addralign<10)printf(" ");
    //addralign
		printf("%d ",sectionTable[i].S.sh_addralign);
		printf("\n");
    }
    printf("Key to Flags:\n W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n L (link order), O (extra OS processing required), G (group), T (TLS),\n E (exclude)\n");
}
