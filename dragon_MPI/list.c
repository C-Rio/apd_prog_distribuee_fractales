#include "list.h"

/********DEFINITION DES FONCTIONS********/

list *new_list()
{
	list *l = (list *)malloc(sizeof(list));
	if (l == NULL)
	{
		fprintf(stderr, "Echec malloc\n");
		assert(0);
	}
	l->head = NULL;
	l->tail = NULL;
	l->size = 0;
	return l;
}

void list_free(list *l)
{
	maillon *m;
	while (l->head != NULL)
	{
		m = l->head;
		l->head = l->head->next;
		free(m);
	}
	free(l);
}

void list_push(list *l, char v)
{
	maillon *m = (maillon *)malloc(sizeof(maillon));
	if (m == NULL)
	{
		fprintf(stderr, "Echec malloc\n");
		assert(0);
	}
	m->value = v;
	m->next = l->head;
	l->head = m;
	l->size++;
	if (l->tail == NULL)
		l->tail = l->head;
}

void list_pushBack(list *l, char v)
{
	maillon *m = (maillon *)malloc(sizeof(maillon));
	if (m == NULL)
	{
		fprintf(stderr, "Echec malloc\n");
		assert(0);
	}
	m->value = v;

	if (l->head == NULL)
		l->head = m;
	else
		l->tail->next = m;

	l->tail = m;
	l->tail->next = NULL;
	l->size++;
}

char list_pop(list *l)
{
	char r;
	maillon *m;

	assert(!list_is_empty(l));
	m = l->head;
	l->head = l->head->next;
	l->size--;
	if (list_is_empty(l))
		l->tail = NULL;
	r = m->value;
	free(m);
	return r;
}

char list_popBack(list *l)
{
	char r;
	maillon *m, *t;

	assert(!list_is_empty(l));

	if (l->head->next == NULL)
	{
		m = l->head;
		l->head = NULL;
		l->tail = NULL;
	}
	else
	{
		t = l->head;
		while (t->next != l->tail)
			t = t->next;
		m = l->tail;
		t->next = NULL;
		l->tail = t;
	}

	r = m->value;
	free(m);
	l->size--;
	return r;
}

void list_print(list *l)
{
	maillon *i = l->head;
	while (i != NULL)
	{
		printf("%c", i->value);
		i = i->next;
	}
	printf("\n");
}

_Bool list_is_empty(list *l)
{
	return (l->head == NULL);
}

void maillon_replace(maillon *m, maillon *fin)
{
	m->value = '+';

	maillon **m_tab = malloc(sizeof(maillon *) * 5);
	for (size_t i = 0; i <= 4; i++)
	{
		m_tab[i] = (maillon *)malloc(sizeof(maillon));

		if (m_tab[i] == NULL)
		{
			fprintf(stderr, "Echec malloc\n");
			assert(0);
		}
	}

	m->next = m_tab[0];

	m_tab[0]->value = 'F';
	m_tab[0]->next = m_tab[1];

	m_tab[1]->value = '-';
	m_tab[1]->next = m_tab[2];

	m_tab[2]->value = '-';
	m_tab[2]->next = m_tab[3];

	m_tab[3]->value = 'F';
	m_tab[3]->next = m_tab[4];

	m_tab[4]->value = '+';
	m_tab[4]->next = fin;
}

list *array_to_list(char *tab, unsigned int size)
{
	list *l = new_list();

	for (size_t i = 0; i < size; i++)
		list_pushBack(l, tab[i]);

	return l;
}

char *list_to_array(list *l, unsigned int list_size)
{
	char *array = malloc(sizeof(char) * list_size);
	maillon *m = l->head;
	unsigned int i = 0;

	while (m != NULL)
	{
		array[i] = m->value;
		m = m->next;
		i++;
	}

	return array;
}

void write_symbol(list *l, unsigned int it_number)
{
	for (unsigned int i = 0; i < it_number; i++)
	{
		maillon *m = l->head;
		while (m != NULL)
		{
			if (m->value == 'F')
			{
				maillon *next = m->next;
				maillon_replace(m, next);
				l->size += 5;
				m = next;
			}
			else
				m = m->next;
		}
	}
}