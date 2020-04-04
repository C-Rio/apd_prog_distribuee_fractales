#ifndef __list_H__
#define __list_H__

/********BIBLIOTHEQUES********/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

/********STRUCTURES********/

typedef struct maillon_s
{
	char value;
	struct maillon_s *next;
} maillon;

typedef struct list
{
	maillon *head;
	maillon *tail;
	unsigned int size;
} list;

/********DECLARATION DES FONCTIONS********/

list *new_list();

void list_free(list *);
void list_push(list *, char);
void list_pushBack(list *, char);
char list_pop(list *);
char list_popBack(list *);
void list_print(list *);
_Bool list_is_empty(list *);

void maillon_replace(maillon *, maillon *);
list *array_to_list(char *, unsigned int);
char *list_to_array(list *, unsigned int);
void write_symbol(list *, unsigned int);

#endif
