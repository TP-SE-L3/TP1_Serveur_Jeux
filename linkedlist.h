#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef enum Type_e{
	INT,
	STRING
} Type_e;



typedef struct element element;
struct element{
	void* val;
	Type_e type;
	element* next;
};

typedef element* linkedlist_t;

/* Ajoute un élément à la tête */
linkedlist_t addHeadL(linkedlist_t list, void* val, Type_e typeVal);
/* Ajoute à la fin de la liste */
linkedlist_t addQueueL(linkedlist_t list, void* val, Type_e typeVal);
/* Retourne le dernier élément de la list (Attention la fonction demande un pointer sur liste)
   L'élément sera retiré de la liste, mais il faudra libérer la mémoire après utilisation
   Rerourne NULL si aucun élément n'est trouvé
*/
element* popL(linkedlist_t *list);
/* Retourne 1 si la liste est vide et 0 sinon*/
int isEmptyL(linkedlist_t list);
/* Retourne le nombre d'élément dans la liste*/
int sizeL(linkedlist_t list);
/* Vide la liste */
linkedlist_t cleanL(linkedlist_t list);

#endif
