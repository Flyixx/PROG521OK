#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "elf_header.h"
#include "elf.h"
#include "my_fonctions.h"
#include "gestionErr.h"

/*
ATTENTION : Il manque la verification de format
*/

// Fonction de contrôle de la valeur du magic (valeur fixée)
void checkMagic (Elf32_Ehdr elf) {
	if(!errorCheck() && !(
	elf.e_ident[EI_MAG0] == 0x7f &&
	elf.e_ident[EI_MAG1] == 'E' &&
	elf.e_ident[EI_MAG2] == 'L'&&
	elf.e_ident[EI_MAG3] == 'F' )){
		errorAct(FORMAT,"in checkMagic : not an ELF file\n");
	}
}

/*============================================================================================================================================
Fonction de lecture de l'en-tête
==============================================================================================================================================*/

Elf32_Ehdr read_header(FILE *f){  //lecture de l'en-tête d'un fichier ELF
    Elf32_Ehdr elf;
    

    
    my_fseek(f,0,SEEK_SET);
	

    // On récupère le contenu du fichier et on le place dans notre structure elf (en reversant pour Big Endian <=> Little Endian)
    my_fread(&elf,1,sizeof(elf),f);
    elf.e_type = reverse_2(elf.e_type);
    elf.e_machine = reverse_2(elf.e_machine);
    elf.e_version = reverse_4(elf.e_version);
    elf.e_entry = reverse_4(elf.e_entry);
    elf.e_phoff = reverse_4(elf.e_phoff);
    elf.e_shoff = reverse_4(elf.e_shoff);
    elf.e_flags = reverse_4(elf.e_flags);
    elf.e_ehsize = reverse_2(elf.e_ehsize);
    elf.e_phentsize = reverse_2(elf.e_phentsize);
    elf.e_phnum = reverse_2(elf.e_phnum); 
    elf.e_shentsize = reverse_2(elf.e_shentsize);
    elf.e_shnum = reverse_2(elf.e_shnum);
    elf.e_shstrndx = reverse_2(elf.e_shstrndx);

    return elf;
}

/*============================================================================================================================================
Fonctions d'affichage des différents éléments de l'en-tête
==============================================================================================================================================*/

//Affichage du "magic"
void print_magic(Elf32_Ehdr elf){
    printf(" Magic:     ");
    for(int i=0;i<EI_NIDENT; i++){
        printf("%2.2x ", elf.e_ident[i]);
    }
    printf("\n");
}

//Classe du fichier
void print_class(Elf32_Ehdr elf){
    printf(" Class :                             ");

    switch(elf.e_ident[EI_CLASS]){
        case ELFCLASSNONE:
            printf("None");
            break;
        case ELFCLASS32:
            printf("ELF32");
            break;
        case ELFCLASS64:
            printf("ELF64");
            break;
        default:
            printf("Read error");
            break;
    }

    printf("\n");
}

//Endianess 
void print_data(Elf32_Ehdr elf){
    printf(" Data :                              ");

    switch(elf.e_ident[EI_DATA]){
        case ELFDATANONE:
            errorAct(FORMAT,"Invalid data encoding\n");
            break;
        case ELFDATA2LSB:
            printf("2's complement, little endian");
            break;
        case ELFDATA2MSB:
            printf("2's complement, big endian");
            break;
        default:
            printf("Read error");
            break;
    }

    printf("\n");
}

//ELF header version number
void print_version_magic(Elf32_Ehdr elf){
    printf(" Version :                           %d (current)\n",elf.e_ident[EI_VERSION]);
}

//OS/ABI
void print_OS_ABI(Elf32_Ehdr elf){
    printf(" OS/ABI :                            ");

    switch (elf.e_ident[EI_OSABI])
    {
        case ELFOSABI_NONE : 
            printf("UNIX - System V\n"); 
            break;
        case ELFOSABI_HPUX : 
            printf("HP-UX\n"); 
            break;
        case ELFOSABI_NETBSD : 
            printf("NetBSD\n"); 
            break;
        case ELFOSABI_GNU : 
            printf("object uses GNU ELF extensions\n"); 
            break;
        case ELFOSABI_SOLARIS : 
            printf("Sun Solaris\n"); 
            break;
        case ELFOSABI_AIX : 
            printf("IBM AIX\n"); 
            break;
        case ELFOSABI_IRIX : 
            printf("SGI Irix\n"); 
            break;
        case ELFOSABI_FREEBSD : 
            printf("FreeBSD\n"); 
            break;
        case ELFOSABI_TRU64 : 
            printf("Compaq TRU64 UNIX\n"); 
            break;
        case ELFOSABI_MODESTO : 
            printf("Novell Modesto\n"); 
            break;
        case ELFOSABI_OPENBSD : 
            printf("OpenBSD\n"); 
            break;
        case ELFOSABI_ARM_AEABI : 
            printf("ARM EABI\n"); 
            break;
        case ELFOSABI_ARM : 
            printf("ARM\n"); 
            break;
        case ELFOSABI_STANDALONE : 
            printf("Standalone (embedded) application\n"); 
            break;
        default : 
            printf("Read error\n");
            break;
    }
}

//ABI Version
void print_ABI_version(Elf32_Ehdr elf){
    printf(" ABI Version :                       %d\n",elf.e_ident[EI_ABIVERSION]);
}

//Type
void print_type(Elf32_Ehdr elf){
    printf(" Type :                              ");
    
    switch(elf.e_type){
        case ET_NONE:
            printf("NONE (No file type)");
            break;
        case ET_REL:
            printf("REL (Relocatable file)");
            break;
        case ET_EXEC:
            printf("EXEC (Executable file)");
            break;
        case ET_DYN:
            printf("DYN (Shared object file)");
            break;
        case ET_CORE:
            printf("CORE (Core file)");
            break;
        case ET_LOPROC:
            printf("Processor_specific");
            break;
        case ET_HIPROC:
            printf("Processor specific");
            break;
        default:
            printf("Read error");
            break;
    }

    printf("\n");
}

//Machine
void print_machine(Elf32_Ehdr elf){
    printf(" Machine :                           ");

switch(elf.e_machine){
        case ET_NONE:
            printf("No machine");
            break;
        case EM_M32:
            printf("AT&T WE 32100");
            break;
        case EM_SPARC:
            printf("SPARC");
            break;
        case EM_386:
            printf("Intel Architecture");
            break;
         case EM_68K:
            printf("Motorola 68000");
            break;
         case EM_88K:
            printf("Motorola 88000");
            break;
         case EM_860:
            printf("Intel 80860");
            break;
         case EM_MIPS:
            printf("MIPS RS3000 Big-Endian");
            break;
         case EM_ARM:
            printf("ARM");
            break;
        default:
            printf("Erreur de valeur");
            break;
        
    }

    printf("\n");
}

//Object file version
void print_version(Elf32_Ehdr elf){
    printf(" Version:                            ");
    printf("0x%x\n",elf.e_version);
}

//Entry point address
void print_EntryAdr(Elf32_Ehdr elf){
    printf(" Entry Point Address:                 0x%x\n", elf.e_entry);
}

//Start Prog Header
void print_start_prog_header(Elf32_Ehdr elf){
    printf(" Start of program headers:           ");

    switch(elf.e_phoff){
        case 0:
            printf("%d (bytes into file)\n",elf.e_phoff);
            break;
        default:
            printf("%d (bytes into file)\n",elf.e_phoff);
            break;
    }
}

//Start Sect Header
void print_start_sect_header(Elf32_Ehdr elf){
    printf(" Start of section headers:           ");
    switch(elf.e_shoff){
        case 0:
            printf("No section headers table\n");
            break;
        default:
            printf("%d (bytes into file)\n",elf.e_shoff);
            break;
    }
}

//Flags
void print_flags(Elf32_Ehdr elf){
    printf(" Flags:                              0x%x, VERSION5 EABI\n",elf.e_flags);
}
//Header size
void print_header_size(Elf32_Ehdr elf){
    printf(" Size of this header:                %d (bytes)\n",elf.e_ehsize);
}
//Prog header size
void print_prog_header_size(Elf32_Ehdr elf){
    printf(" Size of program headers:            %d (bytes)\n",elf.e_phentsize);
}
//Nb de prog headers
void print_prog_header_nb(Elf32_Ehdr elf){
    printf(" Number of program headers:          %d\n",elf.e_phnum);
}
//Size section headers
void print_sect_header_size(Elf32_Ehdr elf){
    printf(" Size of section headers:            %d (bytes)\n",elf.e_shentsize);
}
//NB section headers
void print_sect_header_nb(Elf32_Ehdr elf)
{
    printf(" Number of section headers:          %hd\n", elf.e_shnum) ;
}
//Table index
void print_table_index(Elf32_Ehdr elf)
{
    printf(" Section header string table index:  %d\n", elf.e_shstrndx) ;
}


//Affichage du header complet
void print_elf_header_complete(Elf32_Ehdr elf){
    printf("ELF Header:\n") ;
    print_magic(elf) ;
    print_class(elf) ;
    print_data(elf) ;
    print_version_magic(elf) ;
    print_OS_ABI(elf) ;
    print_ABI_version(elf) ;
    print_type(elf) ;
    print_machine(elf) ;
    print_version(elf) ;
    print_EntryAdr(elf) ;
    print_start_prog_header(elf) ;
    print_start_sect_header(elf) ;
    print_flags(elf) ;
    print_header_size(elf) ;
    print_prog_header_size(elf) ;
    print_prog_header_nb(elf) ;
    print_sect_header_size(elf) ;
    print_sect_header_nb(elf) ;
    print_table_index(elf) ;
}
