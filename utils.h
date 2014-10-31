#ifndef UTILS_H
#define UTILS_H

/*	Retourne numéro de la promière occurence str2 trouvée dans str1, si elle n'est pas trouvée, on retourn -1
	@param str1: chaine conteneur
	@param str2: chaine que l'on cherche 
*/
int stroc(char* str1, char* str2);

/*	Retourne une chaine de caractère entre 2 caractère
	!!! Il faut free la chaine de retour !!!
	@param str: la chaine de caractre dans laquelle on lit
	@param start: caractère de début
	@param end: caractère de fin(\0 si on veut toute la fin de chaine)
*/
char* strbtw(char* str, char start, char end);

/* Coupe la chaine str dès que un caractère de "end" est trouvé(renplace par : \0)
   Renvoie un pointeur sur le caractère après le "end" trouvé, s'il n'y en a pas ou que end n'est pas trouvé, elle renvoie NULL
   @param str: pointeur de début de chaine
   @param end: chaine de caractèe contenant les caractère de fin
*/
char* substrpbrk(char* str, char* end);

/* Retourne le nombre de fois qu'un mot a été trouvé dans une chaine
 * @param str: chaine dans laquelle on cherche
 * @param word: le mot recherché
 * */
int nbstr(char* str, const char* word);
#endif
