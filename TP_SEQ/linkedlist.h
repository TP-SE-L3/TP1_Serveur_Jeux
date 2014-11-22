#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef enum Type_e{
	INT,
	FLOAT,
	STRING,
	OTHER // N'importe quel type : il faut que le programmeur sache ce qu'il y a dedans
} Type_e;



typedef struct element element;
struct element{
	void** val; // Double, car il faut pouvoir changer l'adresse du pointer (pour les float, double...)
	Type_e type;
	element* next;
};

typedef element* linkedlist_t;

/* Ajoute un élément à la tête de la liste
 * Retourne la tête de la liste actuel
 * @param list: la liste
 * @param val: la valeur à ajouter
 * @param typeVal: Le type de la valeur ajoutée */
linkedlist_t addHeadL(linkedlist_t list, void* val, Type_e typeVal);

/* Ajoute à la fin de la liste 
 * Retourne la tête de la liste (Elle peut changer si la liste est null)
 * @param list: la liste
 * @param val: la valeur à ajouter
 * @param typeVal: Le type de la valeur ajoutée */
linkedlist_t addQueueL(linkedlist_t list, void* val, Type_e typeVal);

/* Ajoute un élément à la tête de liste (Attention, c'est direcetment un élément qui est demandé)
 * @param list: la liste
 * @param elem: l'élément que l'on va ajouter dans la liste*/
linkedlist_t addElemHeadL(linkedlist_t list, element* elem);

/* Retourne le dernier élément de la list (Attention la fonction demande un pointer sur liste)
   L'élément sera retiré de la liste, mais il faudra libérer la mémoire après utilisation
   Rerourne NULL si aucun élément n'est trouvé
 * @param: pointeur sur la liste */
element* popL(linkedlist_t *list);

/* Permet de savoir si la liste est vide
 * Retourne 1 si la liste est vide et 0 sinon
 * @param list: la liste*/
int isEmptyL(linkedlist_t list);

/* Retourne le nombre d'élément dans la liste
 * @param list: la liste*/
int sizeL(linkedlist_t list);

/* Vide la liste 
 * Retourne la nouvelle liste (NULL)
 * @param list: la liste à vider
 * @param freeStr: si != 0, libère la mémoire de valeurs string des éléments*/
linkedlist_t cleanL(linkedlist_t list, int freeStr);

/* Ajoute la list2 à la suite de la list1
 * Retourne la tête de la liste
 * @param list1: liste principale
 * @param list2: liste que l'on va concaténer à la list1
 * */
linkedlist_t mergeL(linkedlist_t list1, linkedlist_t list2);

/* Retourne le nombre d'occurence des éléments ayant le type passé en paramètre
 * @param list: la liste dans laquelle on cherche
 * @param type: le type d'élément que l'on cherche
 * */
int nbOccTypeL(linkedlist_t list, Type_e type);

/* Retourne la liste des éléments sous forme d'une chaine de caractères
 * @param list: La liste à transformer en chaine*/
char* listToStringL(linkedlist_t list);
#endif
