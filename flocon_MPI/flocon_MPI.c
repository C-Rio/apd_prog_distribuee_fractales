/**** BIBLIOTHEQUES ****/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo.h>
#include <math.h>
#include <mpi.h>
#include <time.h>


/**** CONSTANTES ****/

#define MAX_PROCS 6
#define ROOT 0


/**** MACROS ****/

#define tracer_segments(indice_debut, indice_fin) ({\
    for(unsigned long i=indice_debut; i < indice_fin; i++)\
    {\
        angle = read_angle(tab, find_case(i));\
        tracer(cr, angle, longueur);\
    }\
})

#define tracer_segments_reverse(indice_debut, indice_fin) ({\
    for(unsigned long i=indice_debut; i < indice_fin; i++)\
    {\
        angle = read_angle_reverse(tab, find_case(i));\
        tracer(cr, angle, longueur);\
    }\
})

/**** DECLARATION DES FONCTIONS ****/

cairo_t* set_cr(cairo_surface_t*, double);
int ask_it();
int* create_tab(int);
void init_tab(int*, int);
int read_angle(int*, int);
int read_angle_reverse(int*, int);
double log4(int);
int find_case(unsigned long);
double deg_to_rad(int);
void tracer(cairo_t*, int, double);
int* surf_to_tab_opti(cairo_surface_t*, int, int, unsigned int*);
void tab_to_surf_opti(int*, cairo_surface_t*, int, int, unsigned int, unsigned int, unsigned int);
void tracer_flocon(cairo_t* cr, int* tab, double longueur, int angle_initial, int part, int iteration);
void tracer_flocon_reverse(cairo_t* cr, int* tab, double longueur, int angle_initial, int part, int iteration);
void send_surface(cairo_surface_t* surface, int dest, int width, int height);
void recv_surface(int sender, cairo_surface_t* surface, int width, int height, unsigned int r, unsigned int g, unsigned int b);


/**** MAIN ****/

int main(int argc, char **argv)
{
    int rang, nb_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);
    if(nb_procs > MAX_PROCS)
        nb_procs = MAX_PROCS;

    int n = 0;
    double start;
    if(rang == ROOT)
    {
        n = ask_it();
        start = MPI_Wtime();
    }
    MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    int width = 1000 * n;
    int height = 4*(width/3)*sin(deg_to_rad(60));
    int origine_y = height/4;
    double longueur = width/pow(3,n);

    int* tab = create_tab(n);

    cairo_surface_t* surface_koch;
    if(rang == ROOT)
        surface_koch = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
    else
        surface_koch = cairo_image_surface_create(CAIRO_FORMAT_A8, width, height);
    cairo_t* cr  = set_cr(surface_koch, 2.0);

    assert(nb_procs > 0);
    switch(rang)
    {
        case ROOT :
            cairo_move_to(cr, 0, origine_y);
            if(nb_procs <= MAX_PROCS/2)
                tracer_flocon(cr, tab, longueur, 0, nb_procs, n);
            else
                tracer_flocon(cr, tab, longueur, 0, MAX_PROCS, n);
            break;

        case 1 :
            cairo_move_to(cr, 0, origine_y);
            if(nb_procs == 4 || nb_procs == 5)
                tracer_flocon_reverse(cr, tab, longueur, 60, MAX_PROCS/2, n);
            else
                tracer_flocon_reverse(cr, tab, longueur, 60, nb_procs, n);
            send_surface(surface_koch, ROOT, width, height);
            break;

        case 2 :
            cairo_move_to(cr, width, origine_y);
            if(nb_procs < MAX_PROCS-1)
                tracer_flocon(cr, tab, longueur, 120, MAX_PROCS/2, n);
            else
                tracer_flocon(cr, tab, longueur, 120, MAX_PROCS, n);
            send_surface(surface_koch, ROOT, width, height);
            break;

        case 3 :
                cairo_move_to(cr, width, origine_y);
                tracer_flocon_reverse(cr, tab, longueur, 180, MAX_PROCS, n);
                send_surface(surface_koch, ROOT, width, height);
            break;

        case 4 :
            cairo_move_to(cr, width/2, height);
            tracer_flocon_reverse(cr, tab, longueur, -60, MAX_PROCS, n);
            send_surface(surface_koch, ROOT, width, height);
            break;

        case 5 :
            cairo_move_to(cr, width/2, height);
            tracer_flocon(cr, tab, longueur, -120, MAX_PROCS, n);
            send_surface(surface_koch, ROOT, width, height);
            break;

        default :
            printf("Proc %d pas utilisé\n", rang);
    }

    // reception et fusion
    if (rang == ROOT)
    {
        //                           rouge       vert       rose          jaune       cyan
        unsigned int couleur[15] = {255,0,0,   0,255,0,  255,0,255,    255,255,0,  0,255,255};
        unsigned int r, g, b;
        for(int i=nb_procs; i>1; i--)
        {
            r = couleur[(i-1)*3 - 3];
            g = couleur[(i-1)*3 - 2];
            b = couleur[(i-1)*3 - 1];
            recv_surface(i-1, surface_koch, width, height, r, g, b);
        }

        printf("durée d'execution : %lfs\n", MPI_Wtime()-start);
        cairo_surface_write_to_png(surface_koch, "Koch.png");
    }

    free(tab);
    cairo_destroy(cr);
    cairo_surface_destroy(surface_koch);
    MPI_Finalize();
    return EXIT_SUCCESS;
}


/**** DEFINITION DES FONCTIONS ****/

cairo_t* set_cr(cairo_surface_t* surface, double cr_size)
{
    cairo_t* cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_line_width(cr, cr_size);
    return cr;
}

int ask_it()
{
    int n;
    printf("Choisissez le nombre d'itérations (entier positif) :\n");
    scanf("%d", &n);
    while(n < 0)
    {
        printf("Merci d'entrer un entier positif :\n");
        scanf("%d", &n);
    }
    if(n > 10)
    {
        printf("Le nombre d'iterations est trop élevé.\n");
        assert(0);
    }
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

int* surf_to_tab_opti(cairo_surface_t *surface, int width, int height, unsigned int* size)
{
    // vraie taille théorique = width*height*2 (2 coord par pixel)
    // on alloue moins car peu de pixels blancs
    unsigned int taille =  (width*height/100);

    int* tab = (int*)calloc(taille, sizeof(int));
    unsigned int i=0;

    cairo_surface_flush(surface);
    unsigned char* current_row = cairo_image_surface_get_data(surface);
    int stride = cairo_image_surface_get_stride(surface);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if(current_row[x] > 50) //seuil
            {
                tab[i] = x;
                tab[i+1] = y;
                i += 2;
                if(i > (taille-2))
                {
                    tab = (int*)realloc(tab, 2*taille*sizeof(int));
                    taille+=taille;
                }
            }
        }
        current_row += stride;
    }
    // on récupère la taille réelle des données utiles
    *size = i;
    return tab;
}

void tab_to_surf_opti(int* tab, cairo_surface_t* surface, int width, int height, unsigned int r, unsigned int g, unsigned int b)
{
    unsigned char* current_row = cairo_image_surface_get_data(surface);
    int stride = cairo_image_surface_get_stride(surface);
    unsigned int i=0;

    // parcourt de l'image
    for (int y = 0; y < height; y++)
    {
        unsigned int *row = (void *) current_row;
        for (int x = 0; x < width; x++)
        {
            // si les coord sont dans le tableau, on colore le pixel
            if(tab[i] == x && tab[i+1] == y)
            {
                row[x] = (r << 16) | (g << 8) | b;
                i += 2;
            }
        }
        current_row += stride;
    }

    cairo_surface_mark_dirty(surface);
}

void tracer_flocon(cairo_t* cr, int* tab, double longueur, int angle_initial, int part, int n)
{
    int angle = 0;
    tracer(cr, angle_initial, longueur);

    int limite = (3*(pow(4,n))/part);
    if(limite < pow(4,n))
        tracer_segments(1, limite);
    else
    {
        tracer_segments(1, pow(4,n));
        init_tab(tab, n);
        tracer(cr, 120, longueur);

        if(limite < 2*pow(4, n))
            tracer_segments(pow(4,n)+1, limite);
        else
        {
            tracer_segments(pow(4,n)+1, 2*pow(4,n));
            init_tab(tab, n);
            tracer(cr, 120, longueur);
            tracer_segments(2*pow(4,n)+1, limite);
        }
    }
    cairo_stroke(cr);
}

void tracer_flocon_reverse(cairo_t* cr, int* tab, double longueur, int angle_initial, int part, int n)
{
    int angle = 0;
    tracer(cr, angle_initial, longueur);

    int limite = (3*(pow(4,n))/part);
    if(limite < pow(4,n))
        tracer_segments_reverse(1, limite);
    else
    {
        tracer_segments_reverse(1, pow(4,n));
        init_tab(tab, n);
        tracer(cr, -120, longueur);

        if(limite < 2*pow(4,n))
            tracer_segments_reverse(pow(4,n)+1, limite);
        else
        {
            tracer_segments_reverse(pow(4,n)+1, 2*pow(4,n));
            init_tab(tab, n);
            tracer(cr, -120, longueur);
            tracer_segments_reverse(2*pow(4,n)+1, limite);
        }
    }
    cairo_stroke(cr);
}

void send_surface(cairo_surface_t* surface, int dest, int width, int height)
{
    unsigned int size = 0;
    int* data = surf_to_tab_opti(surface, width, height, &size);
    MPI_Send(&size, 1, MPI_UNSIGNED, dest, 0, MPI_COMM_WORLD);
    MPI_Send(data, size, MPI_INT, dest, 0, MPI_COMM_WORLD);
}

void recv_surface(int sender, cairo_surface_t* surface, int width, int height, unsigned int r, unsigned int g, unsigned int b)
{
    unsigned int size;
    MPI_Recv(&size, 1, MPI_UNSIGNED, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int* data = (int*)malloc(sizeof(int) * size);
    MPI_Recv(data, size, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    tab_to_surf_opti(data, surface, width, height, r, g, b);
}
