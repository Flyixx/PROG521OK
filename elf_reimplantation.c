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

Elf32_Ehdr changeHeader (Elf32_Ehdr elf,Elf32_ShdrNames *Sn){
	int k=0;
    Elf32_Ehdr Header= elf;
    if(errorCheck()){return Header;}
    for(int i=0;i<elf.e_shnum;i++){
        if(Sn[i].S.sh_type == 9){//SHT_REL
            k++;
            Header.e_shoff = Header.e_shoff - Sn[i].S.sh_size;
        }
    }
    Header.e_shnum=Header.e_shnum-k;
    Header.e_shstrndx=Header.e_shstrndx-k;
    return Header;
}

Elf32_ShdrNames * changeSections (Elf32_Ehdr elf,Elf32_Ehdr Header,Elf32_ShdrNames *SnOld) {
    if(errorCheck()){return NULL;}
    Elf32_ShdrNames * SnNew = createSectionTable(Header);
    int l=0;
    uint32_t tailleRel=0;
    for (int i=0;i<elf.e_shnum;i++)
    {
        if(SnOld[i].S.sh_type!=9){//SHT_REL
            SnNew[l].S=SnOld[i].S;
            SnNew[l].name=SnOld[i].name;
            l++;
        }
        else{
            tailleRel+=SnOld[i].S.sh_size;
        }
    }
    int st=0;
    int strt=0;
    for(int i=0;i<Header.e_shnum;i++)
    {
       if (!strcmp(SnNew[i].name,".symtab"))
       {
           st=i;
       }
       if (!strcmp(SnNew[i].name,".strtab"))
       {
           strt=i;
       }
    }
    if(st==0 || strt==0){
        errorAct(FORMAT,"section(s) not found :%s%s\n",((st==0)?(" .symtab"):("")),((strt==0)?(" .strtab"):("")));
        return NULL;
    }
    SnNew[st].S.sh_link=strt;
    SnNew[l-1].S.sh_offset-=tailleRel;
    return SnNew;
}

Elf32_SymNames * changeSymTable (Elf32_ShdrNames *SnOld,Elf32_ShdrNames *SnNew,int indice,Elf32_SymNames *SymNOld,int* tabAdr,int* tabIndice) {
	if(errorCheck()){return NULL;}
    Elf32_SymNames * SymNNew = createSymNames(indice,SnNew);
	for(int i=0;i<SnNew[indice].S.sh_size/16;i++){
		SymNNew[i].symb=SymNOld[i].symb;
        int indexSymtab = 0;
        while(strcmp(SnNew[indexSymtab].name,SnOld[SymNOld[i].symb.st_shndx].name)){
            indexSymtab++;
        }
        SymNNew[i].symb.st_shndx=indexSymtab;
        SymNNew[i].name=SymNOld[i].name;
		if(i!=0){//indice a ne pas modifier
            if(SymNNew[i].symb.st_shndx==tabIndice[0]){
                SymNNew[i].symb.st_value += tabAdr[0];
            }else if(SymNNew[i].symb.st_shndx==tabIndice[1]){
                SymNNew[i].symb.st_value += tabAdr[1];
            }else if(SymNNew[i].symb.st_shndx==tabIndice[2]){
                SymNNew[i].symb.st_value += tabAdr[2];
            }	
		}
	}
	return SymNNew;
}

void writeReimp (FILE* fNew,Elf32_Rel **R,Elf32_Ehdr elfOld,Elf32_ShdrNames *SnOld,Elf32_ShdrNames *SnNew,Elf32_SymNames *symN) {
    int DebugInt = 0;
    if(errorCheck()){return;}
    int l=0,K=0;
    int S,adr;
    int32_t A,tmp;
    for(int i=0;i<elfOld.e_shnum;i++){
        if(SnOld[i].S.sh_type==9)
        {
           for(int j=0;j<SnOld[i].S.sh_size/8;j++)
           {
                adr = SnOld[SnOld[i].S.sh_info].S.sh_offset+R[l][j].r_offset;
                my_fseek(fNew,adr,SEEK_SET);
                fflush(fNew);
                my_fread(&A,1,sizeof(int32_t),fNew);
                A=reverse_4(A);
                fflush(fNew);
                my_fseek(fNew,adr,SEEK_SET);
                S = symN[ELF32_R_SYM(R[l][j].r_info)].symb.st_value;
                switch(ELF32_R_TYPE(R[l][j].r_info)){ 
                    case R_ARM_ABS8 :
                        K=(int8_t) S+A;
                        fwrite(&K,sizeof(int8_t),1,fNew);
                        break;
                    case R_ARM_ABS16 :
                        K=(int16_t) S+A;
                        K=reverse_2(K);
                        fwrite(&K,sizeof(int16_t),1,fNew);
                        break;
                    case R_ARM_ABS32:
                        K=(int32_t) S+A;
                        K=reverse_4(K);
                        fwrite(&K,sizeof(int32_t),1,fNew);
                        break;
                    //R_ARM_JUMP24 & R_ARM_CALL ne fonctionne pas ...
                    case R_ARM_JUMP24:
                    case R_ARM_CALL:
                        if(DebugInt==0){fprintf(stderr,"PARTIE NON TERMINEE, RESULTAT INCORRECT (mais vraiment pas loin)\n");}
                        DebugInt++;
                        tmp=A;
                        A = A & 0x00ffffff;
                        K = (S+ (A << 2) - adr ) & 0x03FFFFFE ;
                        K = K >> 2;
                        tmp = (tmp & 0xff000000) | (K & 0x00ffffff);
                        tmp=reverse_4(tmp);
                        my_fseek(fNew,adr,SEEK_SET);
                        fwrite(&tmp,sizeof(int32_t),1,fNew);
                        break;
                    default :
                        errorAct(FORMAT,"instruction not supported : %d\n",ELF32_R_TYPE(R[l][j].r_info));
                        return;
                }
            }
            l++;
        }
    }
}

void writeFileSymTable (FILE* fNew, Elf32_SymNames * SymNNew, Elf32_ShdrNames *SnNew, int indice){
    my_fseek(fNew,SnNew[indice].S.sh_offset,SEEK_SET);
    if(errorCheck()){return;}
	for(int i=0;i<SnNew[indice].S.sh_size/16;i++){
		SymNNew[i].symb.st_name =	reverse_4(SymNNew[i].symb.st_name );
		SymNNew[i].symb.st_value =	reverse_4(SymNNew[i].symb.st_value);
		SymNNew[i].symb.st_size =	reverse_4(SymNNew[i].symb.st_size );
		SymNNew[i].symb.st_shndx =	reverse_2(SymNNew[i].symb.st_shndx);
		fwrite(&SymNNew[i].symb, sizeof(Elf32_Sym),1,fNew);
        SymNNew[i].symb.st_name =	reverse_4(SymNNew[i].symb.st_name );
		SymNNew[i].symb.st_value =	reverse_4(SymNNew[i].symb.st_value);
		SymNNew[i].symb.st_size =	reverse_4(SymNNew[i].symb.st_size );
		SymNNew[i].symb.st_shndx =	reverse_2(SymNNew[i].symb.st_shndx);
	}
}

void writeFileHeader (FILE* fNew, Elf32_Ehdr elfNew){
    if(errorCheck()){return;}
    elfNew.e_type = reverse_2(elfNew.e_type);
    elfNew.e_machine = reverse_2(elfNew.e_machine);
    elfNew.e_version = reverse_4(elfNew.e_version);
    elfNew.e_entry = reverse_4(elfNew.e_entry);
    elfNew.e_phoff = reverse_4(elfNew.e_phoff);
    elfNew.e_shoff = reverse_4(elfNew.e_shoff);
    elfNew.e_flags = reverse_4(elfNew.e_flags);
    elfNew.e_ehsize = reverse_2(elfNew.e_ehsize);
    elfNew.e_phentsize = reverse_2(elfNew.e_phentsize);
    elfNew.e_phnum = reverse_2(elfNew.e_phnum); 
    elfNew.e_shentsize = reverse_2(elfNew.e_shentsize);
    elfNew.e_shnum = reverse_2(elfNew.e_shnum);
    elfNew.e_shstrndx = reverse_2(elfNew.e_shstrndx);

    fwrite(&elfNew, sizeof(Elf32_Ehdr),1,fNew);

    elfNew.e_type = reverse_2(elfNew.e_type);
    elfNew.e_machine = reverse_2(elfNew.e_machine);
    elfNew.e_version = reverse_4(elfNew.e_version);
    elfNew.e_entry = reverse_4(elfNew.e_entry);
    elfNew.e_phoff = reverse_4(elfNew.e_phoff);
    elfNew.e_shoff = reverse_4(elfNew.e_shoff);
    elfNew.e_flags = reverse_4(elfNew.e_flags);
    elfNew.e_ehsize = reverse_2(elfNew.e_ehsize);
    elfNew.e_phentsize = reverse_2(elfNew.e_phentsize);
    elfNew.e_phnum = reverse_2(elfNew.e_phnum); 
    elfNew.e_shentsize = reverse_2(elfNew.e_shentsize);
    elfNew.e_shnum = reverse_2(elfNew.e_shnum);
    elfNew.e_shstrndx = reverse_2(elfNew.e_shstrndx);
}

void writeFileSectionHeader(FILE* fNew,Elf32_Ehdr elfNew,Elf32_ShdrNames *SnNew){
    my_fseek(fNew,elfNew.e_shoff,SEEK_SET);
    if(errorCheck()){return;}
    for(int i=0;i<elfNew.e_shnum;i++){
        SnNew[i].S.sh_name=reverse_4(SnNew[i].S.sh_name);
        SnNew[i].S.sh_type=reverse_4(SnNew[i].S.sh_type);
        SnNew[i].S.sh_flags=reverse_4(SnNew[i].S.sh_flags);
        SnNew[i].S.sh_addr=reverse_4(SnNew[i].S.sh_addr);
        SnNew[i].S.sh_offset=reverse_4(SnNew[i].S.sh_offset);
        SnNew[i].S.sh_size=reverse_4(SnNew[i].S.sh_size);
        SnNew[i].S.sh_link=reverse_4(SnNew[i].S.sh_link);
        SnNew[i].S.sh_info=reverse_4(SnNew[i].S.sh_info);
        SnNew[i].S.sh_addralign=reverse_4(SnNew[i].S.sh_addralign);
        SnNew[i].S.sh_entsize=reverse_4(SnNew[i].S.sh_entsize);

        fwrite(&SnNew[i].S,sizeof(Elf32_Shdr),1,fNew);

        SnNew[i].S.sh_name=reverse_4(SnNew[i].S.sh_name);
        SnNew[i].S.sh_type=reverse_4(SnNew[i].S.sh_type);
        SnNew[i].S.sh_flags=reverse_4(SnNew[i].S.sh_flags);
        SnNew[i].S.sh_addr=reverse_4(SnNew[i].S.sh_addr);
        SnNew[i].S.sh_offset=reverse_4(SnNew[i].S.sh_offset);
        SnNew[i].S.sh_size=reverse_4(SnNew[i].S.sh_size);
        SnNew[i].S.sh_link=reverse_4(SnNew[i].S.sh_link);
        SnNew[i].S.sh_info=reverse_4(SnNew[i].S.sh_info);
        SnNew[i].S.sh_addralign=reverse_4(SnNew[i].S.sh_addralign);
        SnNew[i].S.sh_entsize=reverse_4(SnNew[i].S.sh_entsize);
    }
}

void writeFileSection(FILE* fOld,FILE* fNew,Elf32_Ehdr elfOld,Elf32_Ehdr elfNew,Elf32_ShdrNames *SnOld,Elf32_ShdrNames *SnNew){
    if(errorCheck()){return;}    
    char * sectionData;
    int l=0;
    for (int i=0;i<elfOld.e_shnum;i++)
    {
        if(SnOld[i].S.sh_type!=9){
            sectionData=my_malloc(SnOld[i].S.sh_size);
            my_fseek(fOld,SnOld[i].S.sh_offset,SEEK_SET);
            //bug ici
            my_fread(sectionData,1,SnOld[i].S.sh_size,fOld);
            my_fseek(fNew,SnNew[l].S.sh_offset,SEEK_SET);
            if(errorCheck()){return;}
            fwrite(sectionData,SnNew[l].S.sh_size,1,fNew);
            l++;
            free(sectionData);
        }
    }
}