#ifndef __Dragon_H__
#define __Dragon_H__

/********BIBLIOTHEQUES********/

#include "list.h"
#include <math.h>
#include <cairo.h>
#include <time.h>

/********DECLARATION DES FONCTIONS********/

cairo_t *set_cr(cairo_surface_t *, double);
int ask_it();
unsigned int symbol_number(int);
double deg_to_rad(int);
void draw_symbol(cairo_t *, char, double);
void draw_dragon(char *, unsigned int, cairo_t *, double, cairo_surface_t *);
void save_image(cairo_t *, cairo_surface_t *);

#endif
