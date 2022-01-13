#include <stdio.h>
#include <stdlib.h>
#include "my_fonctions.h"
#include "elf.h"
#include <stdarg.h>
#include "string.h"
#include "gestionErr.h"

/*=================================================================================================================================================
 															Fonctions utiles 
 =================================================================================================================================================*/

// fread robuste
void my_fread (void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!errorCheck() && nmemb != fread(ptr,size,nmemb,stream)) { //erreur de lecture
		errorAct(FILE_READING,"");
		if(feof(stream)){
			errorInfo("my_fread : EOF\n");
		}else{
			errorInfo("my_fread : not EOF\n");
		}
    }
}

// scanf robuste
void my_scanf(char* str,...){
  //on suppose que nb truc a lire = nb % dans str
  va_list ap;
  va_start(ap,str);
  int nbP=0;
  char* c = str;
  while(*c!='\0'){//compte le nb de % dans nbP
    if(*c=='%'){
      nbP++;
    }
    c++;
  }
  if(!errorCheck() && vfscanf(stdout,str,ap)!=nbP){
	errorAct(FILE_READING,"???");//????
  }
  va_end(ap);
}


// malloc robuste
void* my_malloc(size_t size){
	void* ptr = malloc(size);
	if(!errorCheck() && ptr==NULL){
		errorAct(ALLOC,"ask size : %lu\n",size);
	}
	return ptr;
}

// fopen robuste
FILE * my_fopen(char* name,const char* opt){
	FILE * f = fopen(name,opt);
	if(!errorCheck() && f==NULL){
		errorAct(FILE_OPEN,"file : %s\n",name);
	}
	return f;
}

// fseek robuste
void my_fseek(FILE * stream, long int offset, int origin){
	if(fseek(stream,offset,origin)!=0){
		errorAct(FILE_OTHER,"fail to seek \n offset : %ld \n origin : %d\n",offset,origin);
	}
}
