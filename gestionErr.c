#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "gestionErr.h"

/*=================================================================================================================================================
 															Fonctions de contrôle des erreurs
 =================================================================================================================================================*/


enum errorType errorState;

void errorIni(void){
	errorState=NONE;
}


// Fonction pour gérer les différents types d'erreurs qu'on peut obtenir 
void errorAff_perType(){
	switch(errorState){
		case NONE :
			fprintf(stderr,"WARNING : errorAff used with NONE errorType\n");
			break;
		case ARGUMENTS :
			fprintf(stderr,"ERROR : invalid arguments\n");
			break;
		case FILE_OPEN :
			fprintf(stderr,"ERROR : fail to open a file, check existance and acces right\n");
			break;
		case FILE_READING :
			fprintf(stderr,"ERROR : fail to read a file\n");
			break;
		case FILE_OTHER :
			fprintf(stderr,"ERROR : file related\n");
			break;
		case ALLOC :
			fprintf(stderr,"ERROR : not enoth memory space\n");
			break;
		case FORMAT :
			fprintf(stderr,"ERROR : wrong format\n");
			break;
		default :
			fprintf(stderr,"visiblement on a oubliee un cas...\n");
			break;
	}
}

// Fonction appelée dans les différents programmes pour tester les erreurs, englobe errorAff_perType
void errorAct(const enum errorType type,const char* str,...){
	if(errorState==NONE){
		va_list ap;
		errorState=type;
		errorAff_perType();
		va_start(ap,str);
		vfprintf(stderr,str,ap);
		va_end(ap);
	}
}

void errorInfo(char* str,...){
	va_list ap;
	va_start(ap,str);
	vfprintf(stderr,str,ap);
	va_end(ap);
}

int errorCheck(void){
	return errorState!=NONE;
}

void errorExit(void){
	exit(errorState);
}

