/**** BIBLIOTHEQUES ****/

#include <stdlib.h>
#include <stdio.h>
#include <cairo.h>
#include <math.h>
#include <assert.h>


/**** DECLARATION DES FONCTIONS ****/

cairo_t* set_cr(cairo_surface_t*, double);
// void background(cairo_t*, double, double);
unsigned int ask_it();
int* create_tab(int);
void init_tab(int*, int);
int read_angle(int*, int);
int read_angle_reverse(int*, int);
double log4(int);
int find_case(unsigned long);
double deg_to_rad(int);
void tracer(cairo_t*, int, double);


/**** MAIN ****/

int main(int argc, char const *argv[])
{
    unsigned int n = ask_it();

    double width = 1000;
    double height = 4*(width/3)*sin(deg_to_rad(60));
    double origine_y = height/4;

    double longueur = width/pow(3,n);
    int angle = 0;

    int* tab = create_tab(n);

    cairo_surface_t* surface_koch = cairo_image_surface_create(CAIRO_FORMAT_A8, width, height);
    cairo_t* cr = set_cr(surface_koch, 1.0);
    cairo_move_to(cr, 0, origine_y);

    tracer(cr, 0, longueur);
    for(unsigned long i=1; i < (3*(pow(4,n))); i++)
    {
        if(i == pow(4,n) || i == 2*pow(4,n))
        {
            init_tab(tab, n);
            angle = 120;
        }
        else
        {
            angle = read_angle(tab, find_case(i));
        }
        tracer(cr, angle, longueur);
    }

    free(tab);

    cairo_stroke(cr);
    cairo_surface_write_to_png(surface_koch, "Koch.png");

    cairo_destroy(cr);
    cairo_surface_destroy(surface_koch);

    return EXIT_SUCCESS;
}


/**** DEFINITION DES FONCTIONS ****/

cairo_t* set_cr(cairo_surface_t* surface, double cr_size)
{
    cairo_t* cr = cairo_create(surface);
    cairo_set_line_width(cr, cr_size);
    //cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    return cr;
}

// void background(cairo_t* cr, double width, double height)
// {
//     cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
//     cairo_rectangle(cr, 0, 0, width, height);
//     cairo_fill(cr);
//     cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
// }

unsigned int ask_it()
{
    unsigned int n;
    printf("Choisissez le nombre d'itérations (entier positif) :\n");
    scanf("%d", &n);
    while(n < 0)
    {
        printf("Merci d'entrer un entier positif :\n");
        scanf("%d", &n);
    }
    assert(n < 13);
    return n;
}

int* create_tab(int n)
{
    int* tab = (int*)calloc(n, sizeof(int));
    return tab;
}

void init_tab(int* tab, int n)
{
    for(int i=0; i<n; i++)
        tab[i]=0;
}

int read_angle(int* tab, int i)
{
    int angle=0;
    if(tab[i] == 1)
        angle = 120;
    else
        angle = -60;
    tab[i] = (tab[i]+1)%3;
    return angle;
}

int read_angle_reverse(int* tab, int i)
{
    int angle=0;
    if(tab[i] == 1)
        angle = -120;
    else
        angle = 60;
    tab[i] = (tab[i]+1)%3;
    return angle;
}

double log4(int x)
{
    return log10(x)/log10(4);
}

int find_case(unsigned long i)
{
    for(int j=floor(log4(i)); j>0; j--)
    {
        if(i%(unsigned long)(pow(4,j))==0)
            return j;
    }
    return 0;
}

double deg_to_rad(int angle)
{
    return (double)angle*M_PI/180;
}

void tracer(cairo_t* cr, int angle, double longueur)
{
    cairo_rotate(cr, deg_to_rad(angle));
    cairo_rel_line_to(cr, longueur, 0);
}
