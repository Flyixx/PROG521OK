#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "elf.h"
#include "elf_header.h"
#include "elf_sections.h"
#include "my_fonctions.h"


//===============Fonction pour afficher le contenu d'une section en Hexa=======================

void print_sections_hexdump(FILE *f,Elf32_ShdrNames *Sn,Elf32_Ehdr H, char *name){
	int i;
	// ça trouve l'indice si un nom a été donnée apres le -x
	if(name[0]=='.'){
		for(i=0;i<H.e_shnum&& strcmp(Sn[i].name,name);i++);
	}
	//ça convertie le string du chiffre passez apres -x en int
	else{
		i = atoi(name);
	}	
	if(i<H.e_shnum && Sn[i].S.sh_size !=0){
		printf("Hex dump of section '%s': \n",Sn[i].name);

		char * Sec;
		Sec=my_malloc(Sn[i].S.sh_size);

		fseek(f,Sn[i].S.sh_offset,SEEK_SET);
		my_fread(Sec,1,Sn[i].S.sh_size,f);

		int j=0,taille=16,t=0;
		printf(" 0x%#.8x",0);

		//Print du hexdump
		for(j=0;j<Sn[i].S.sh_size;j++){
			//espace chaque 4 octect
			if(j%4==0){
				printf(" ");
				t++;
			}	
			//ecriture des charactere en ascii correspendant a la ligne ecrite et saut a la ligne
			if(j!=0&&j%16==0)
			{
				for(int k=j-16;k<j;k++){		
					if(Sec[k]<=31 || Sec[k]>=128) printf(".");
					else printf("%c",Sec[k]);
				}
				printf("\n");
				printf(" %#.8x ",taille);
				taille=taille+16;
				t=0;
			}
			printf("%02hhx",Sec[j]);    
		}
		//ecriture des charactere en ascii correspendant a derniere ligne ecrite 
		int l,s;
		if(j==16){ s=31; } 
		else if(j%16==0){ s=32;}
		else{ s=(j%16)*2; }

		for(int k=0;k<(36-s-t);k++){ printf(" "); }

		if(j==16){ l=0; }
		else if(j!=0&&j%16==0){ l=j-16; }
		else{ l=j-(j%16); }

		for(int k=l;k<j;k++){		
			if(Sec[k]<=31 || Sec[k]>=128) printf(".");
			else printf("%c",Sec[k]);
		}
		printf("\n");
		free(Sec);
	}
	else if(i>=H.e_shnum){printf("Ce nom ou cet index de section n'existe pas\n");}
	else{printf("Section '%s' has no data to dump.",Sn[i].name);}
	printf("\n");
}
