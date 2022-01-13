#ifndef __FONCTIONS_BIS_H__
#define __FONCTIONS_BIS_H__

#include <stdio.h>

/*
Les fonctions my_XXXX incluent la getion d'erreur si les fonctions XXXX echouent
*/
void my_fread (void *ptr, size_t size, size_t nmemb, FILE *stream);
void my_scanf(char* str,...);
void* my_malloc(size_t size);
FILE * my_fopen(char* name,const char* opt);
void my_fseek(FILE * stream, long int offset, int origin);

#endif
