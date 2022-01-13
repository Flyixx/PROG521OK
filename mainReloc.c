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

int main (int argc,char *argv[]){
    errorIni();
    if(argc < 3){ 
        errorAct(ARGUMENTS,"use : ./mainReloc <Source> <Destination> <section1> <adrSection1> ... <sectionN> <adrSectionN>\n");
        errorInfo("Sections acceptees : .text .data .bss\n");
        errorExit();
    }

    //Ouverture fichiers
    FILE *src = NULL;
    FILE *target=NULL;
    src=my_fopen(argv[1],"r+");
    if(errorCheck()){errorExit();}
    target=my_fopen(argv[2],"w+");
    if(errorCheck()){
        fclose(src);
        errorExit();
    }

    

    //Lecture des sections et addresses associees
    int tabAdr[3] = {0,0,0};
    for(int i=3;i<argc-1;i+=2){
        if(!strcmp(argv[i],".text")){
            sscanf(argv[i+1],"%d",&tabAdr[0]);//Adrresse = int ?
        }else if (!strcmp(argv[i],".bss")) {
            sscanf(argv[i+1],"%d",&tabAdr[1]);//Adrresse = int ?
        }else if (!strcmp(argv[i],".data")) {
            sscanf(argv[i+1],"%d",&tabAdr[2]);//Adrresse = int ?
        }else{
            errorAct(ARGUMENTS,"Sections acceptees : .text .data .bss\n");
            fclose(src);
            fclose(target);
            errorExit();
        }
    }

    //printf("tabAdr : %d %d %d\n",tabAdr[0],tabAdr[1],tabAdr[2]);

    //Initialisation header
    Elf32_Ehdr elf,Header;
    elf=read_header(src);
    if(errorCheck()){
        fclose(src);
        fclose(target);
        errorExit();
    }
    //Initialisation section table
    Elf32_ShdrNames *Sn = createSectionTable(elf);
    char * SectNames=NULL;
    read_sections(src, Sn, elf,SectNames);
	if(errorCheck()){
		fclose(src);
        fclose(target);
		freeSectionTable(Sn,SectNames);
		errorExit();
	}
    //Initialisation de l'index .Symtab
    int indexSymtab = 0;
    while(strcmp(Sn[indexSymtab].name,".symtab") && indexSymtab<elf.e_shnum){
        indexSymtab++;
    }
    if(indexSymtab==elf.e_shnum){
    	errorAct(FORMAT,".symtab not found\n");//???
		fclose(src);
        fclose(target);
		freeSectionTable(Sn,SectNames);
		errorExit();
    }

    //Initialisation de la structure elf symtab
    Elf32_SymNames *symN = createSymNames(indexSymtab,Sn);
    char *SymNames=NULL;
    read_symtab(src,Sn,elf,symN,SymNames);
	if(errorCheck()){
		fclose(src);
        fclose(target);
		freeSymNames(symN,SymNames);
		freeSectionTable(Sn,SectNames);
		errorExit();
    }
    //--------------------------------------------------------------------------------
    
    //correction des structures
    Header=changeHeader(elf,Sn);
    Elf32_ShdrNames *SnNew=changeSections(elf,Header,Sn);
    if(errorCheck()) {
		fclose(src);
        fclose(target);
		freeSymNames(symN,SymNames);
		freeSectionTable(Sn,SectNames);
		errorExit();
    }
    indexSymtab = 0;
    int tabindice[3] = {0,0,0};
    for(int i=0;i<Header.e_shnum;i++){
        if(!strcmp(SnNew[i].name,".symtab"))
            indexSymtab=i;
        if(!strcmp(SnNew[i].name,".text"))
            tabindice[0]=i;
        if(!strcmp(SnNew[i].name,".bss"))
            tabindice[1]=i;
        if(!strcmp(SnNew[i].name,".data"))
            tabindice[2]=i;
    }
    if(indexSymtab==0){ 
        errorAct(FORMAT,".symtab not found\n");
        errorInfo("Normalement impossible de voir ce message\n");
    }
    Elf32_SymNames *SymNNew =changeSymTable (Sn,SnNew,indexSymtab,symN,tabAdr,tabindice);
    if(errorCheck()){
		fclose(src);
        fclose(target);
		freeSymNames(symN,SymNames);
		freeSectionTable(Sn,SectNames);
        freeSectionTable(SnNew,NULL);
		errorExit();
    }
    Elf32_Rel **R=NULL;
    R=read_reloc(src,Sn,symN,elf);  
	if(errorCheck()){
		fclose(src);
        fclose(target);
		free_reloc(R,elf,Sn);
		freeSymNames(symN,SymNames);
		freeSectionTable(Sn,SectNames);
        freeSymNames(SymNNew,NULL);
        freeSectionTable(SnNew,NULL);
		errorExit();
    }
    //ecriture du nouveau fichier ELF
    writeFileHeader(target,Header);
    writeFileSectionHeader(target,Header,SnNew);
    writeFileSection(src,target,elf,Header,Sn,SnNew);
    writeFileSymTable (target,SymNNew,SnNew,indexSymtab);
    writeReimp (target,R,elf,Sn,SnNew,SymNNew);

    //Liberation memoire
    fclose(src);
    fclose(target);
    free_reloc(R,elf,Sn);
	freeSymNames(symN,SymNames);
	freeSectionTable(Sn,SectNames);
    freeSymNames(SymNNew,NULL);
    freeSectionTable(SnNew,NULL);
    if(errorCheck()){errorExit();}
    return 0;
}