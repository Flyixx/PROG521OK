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
#include "elf_reimplantation.h"

int main(int argc, char *argv[]){
	errorIni();
    if(argc<2){
		errorAct(ARGUMENTS,"you need at least one option\n");
		errorExit();
	}
    char *cmd = argv[1];
    
    //Lecture du mode
    FILE *src = NULL;
    char *name=NULL;
    

    //Definition de variables pour savoir ce qu'on va afficher (suivant le paramètre -? de la commande)
    int doAffHeader = 0;
    int doAffSectionsTable = 0;
    int doAffSectionDetail = 0;
    int doAffSymbolsTable = 0;
    int doAffRelTable = 0;
    if(!strcmp(cmd,"-a")){
    	doAffHeader=1;
    	doAffSectionsTable=1;
    	doAffSymbolsTable=1;
    	doAffRelTable=1;
    }else if(!strcmp(cmd,"-x")) {
    	doAffSectionDetail=1;
    }else if(!strcmp(cmd,"-h")) {
    	doAffHeader=1;
    }else if(!strcmp(cmd,"-S")) {
    	doAffSectionsTable=1;
    }else if(!strcmp(cmd,"-s")) {
    	doAffSymbolsTable=1;
    }else if(!strcmp(cmd,"-r")) {
    	doAffRelTable=1;
    }else if(!strcmp(cmd,"-H") || !strcmp(cmd,"-help") || !strcmp(cmd,"--help")) {  //aide pour utilisation de mainPrint
    	fprintf(stderr,"utilisation : ./main <options> <fichier>\n");
    	fprintf(stderr,"-a : affiche toutes les informations\n");
        fprintf(stderr,"-h : affiche l'en tete du fichier ELF\n");
    	fprintf(stderr,"-x <section> : affichage de <section> en hexa\n");
    	fprintf(stderr,"-S : affichage de la table des sections\n");
    	fprintf(stderr,"-s : affichage de la table des symboles\n");
    	fprintf(stderr,"-r : affichage de la table de la table de reimplentation\n");
    	fprintf(stderr,"-H :\n-help :\n--help : affichage de l'aide\n");
    	return 0;
    }else{
		errorAct(ARGUMENTS,"unknown option : %s\n",cmd);
    	errorExit();
    }
    
    //Contrôle du nb d'arguments
    if(argc<3){
		errorAct(ARGUMENTS,"you need at least a file name\n");
		errorExit();
	}
    
    // Récupération du/des fichier(s) en argument
    if(!strcmp(cmd,"-x")){
    	if(argc<4){
    		errorAct(ARGUMENTS,"you need a section name and file name\n");
			errorExit();
    	}
        name=argv[2];
        src = my_fopen(argv[3], "r");
    } else {
        src = my_fopen(argv[2], "r");
    }
	
    if(errorCheck()){errorExit();}//probleme d'ouverture

    // Initialisation de la structure elf header avec contrôles
    Elf32_Ehdr elf;
    elf = read_header(src);
	checkMagic(elf);
	if(errorCheck()){
		fclose(src);
		errorExit();
	}
    uint32_t nbSection = elf.e_shnum;
    //dprintf(3,"nbSection : %d",nbSection);
    //uint32_t SectionOffset = elf.e_shoff;
    
    // Initialisation de la structure elf section
    Elf32_ShdrNames *Sn = createSectionTable(elf);
    char * SectNames=NULL;
    read_sections(src, Sn, elf,SectNames);
    checkSectionTableIndex0(Sn);
	if(errorCheck()){
		fclose(src);
		freeSectionTable(Sn,SectNames);
		errorExit();
	}
    ;
    //Initialisation de l'index .Symtab
    int indexSymtab = 0;
    while(strcmp(Sn[indexSymtab].name,".symtab") && indexSymtab<elf.e_shnum){
        indexSymtab++;
    }
    if(indexSymtab==elf.e_shnum){
    	errorAct(FORMAT,".symtab not found\n");//???
		fclose(src);
		freeSectionTable(Sn,SectNames);
		errorExit();
    }

    //Initialisation de la structure elf symtab
    Elf32_SymNames *symN = createSymNames(indexSymtab,Sn);
    char *SymNames=NULL;
    read_symtab(src,Sn,elf,symN,SymNames);
	checkSymbolTableIndex0(symN);
	if(errorCheck()){
		fclose(src);
		freeSymNames(symN,SymNames);
		freeSectionTable(Sn,SectNames);
		errorExit();
    }

    //Initialisation de la table des reimplémentation
    Elf32_Rel **R=NULL;
    R=read_reloc(src,Sn,symN,elf);  
	if(errorCheck()){
		fclose(src);
		free_reloc(R,elf,Sn);
		freeSymNames(symN,SymNames);
		freeSectionTable(Sn,SectNames);
		errorExit();
    }

    //Affichage
    if(doAffHeader){
        print_elf_header_complete(elf);
    }
    if(doAffSectionsTable){
        print_sections_header(Sn,nbSection);
    }
    if(doAffSectionDetail){
        print_sections_hexdump(src,Sn,elf,name);
    }
    if(doAffSymbolsTable){
        print_symtab(symN,Sn[indexSymtab].S.sh_size/16);
    }
    if(doAffRelTable){
        print_reloc(src,Sn,symN,elf,R);
    }
    
    //liberation memoire
    free_reloc(R,elf,Sn);
    freeSymNames(symN,SymNames);
    freeSectionTable(Sn,SectNames);
    fclose(src);
    return 0;
}
