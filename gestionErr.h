#ifndef __GESTIONERR__H__
#define __GESTIONERR__H__

/*
mode d'emploi :
	le principe est de faire remonter les erreurs vers le main pour pouvoir free les structures.
	Pour cela il faux faire des fonctions robustes qui ne plantent pas s'il y a eu une erreur plus tôt,
	en pratique il suffit de mettre if(errorCheck()){return <valeur retour defaut>;} apres qu'une erreur
	aie été levée (debut de la fonction, apres my_XXXX, etc..).
	On peut enchainer les fonctions my_XXXX et ne le faire qu'une fois a la fin.
	
	Pour ajouter une erreur :
	si le type n'est pas dans enum errorType, l'ajouter ainsi que dans errorAff_perType avec un message
	appropriée (de preference laisser NONE en premier dans la def du type).
	Utiliser errorAct(<type d'erreur>) pour "activer l'erreur" puis 
	errorInfo pour ajouter des details (errorInfo fonctionne comme printf, % compris)

*/

enum errorType {
	NONE,
	ARGUMENTS,
	FILE_OPEN,
	FILE_READING,//souvent EOF
	FILE_OTHER,
	ALLOC,
	FORMAT
};

//used to initialise the error system
void errorIni(void);
//used when an error is detected
//first argument is the error type, rest are the same as printf
void errorAct(const enum errorType type,const char * str,...);
//used after a call of errorAct
//adds informations about the error
//same syntax as printf
void errorInfo(char* str,...);
//used to check if there is an error
int errorCheck(void);
//used to exit the program
//WARNING : you need to free your strucures before calling this function
void errorExit(void);

#endif