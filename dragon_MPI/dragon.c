#include "dragon.h"

int main(int argc, char **argv)
{
    // Initialisation de MPI
    int rang, nb_proc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

    cairo_t *cr;
    cairo_surface_t *surface_dragon;

    int it_left, nb_it;

    double length;
    double width;
    double height;
    double start = 0;

    int it_procs = ceil(log2(nb_proc));
    unsigned int nb_symbol = symbol_number(it_procs);
    unsigned int symbol_per_proc = nb_symbol / nb_proc;
    unsigned int symbol_proc0 = nb_symbol - symbol_per_proc * (nb_proc - 1);

    // Variables permettant l'utilisation de scatterv et gatherv (répartition non homogène des données entre les différents processus)
    unsigned int sum, recvcount = 0;
    int *sendcounts, *displs;

    int *list_size_array;

    char *array_final, *tab_list;

    if (rang == 0)
    {
        srand(time(NULL));

        // Demande à l'utilisateur le nombre d'itérations à effectuer
        nb_it = ask_it();

        // On détermine la taille de l'image à calculer
        width = 500 + nb_it * 200;
        height = width;
        length = width / 2;

        start = MPI_Wtime();

        sendcounts = malloc(sizeof(int) * nb_proc);
        displs = malloc(sizeof(int) * nb_proc);

        // Nombre d'itérations à effectuer avant de paralléliser la réalisation du dragon
        unsigned int iteration_before_scatter = it_procs;

        // Si le nombre d'itérations demandées est plus faible que le nombre nécessaire à la parallélisation
        if (nb_it <= it_procs)
        {
            iteration_before_scatter = nb_it;
            nb_symbol = symbol_number(nb_it);
        }

        // Taille des segments
        length = pow(1 / sqrt(2), nb_it) * length;

        surface_dragon = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
        cr = set_cr(surface_dragon, 2.0);
        cairo_move_to(cr, width / 4, height / 3);

        // On initialise notre liste
        list *l = new_list();

        // Initialisation du L-système
        list_pushBack(l, 'F');

        write_symbol(l, iteration_before_scatter);

        // On converti la liste en tableau pour pouvoir la transmettre
        tab_list = list_to_array(l, nb_symbol);

        // On calcul les tailles des segments à envoyer et le déplacement
        sum = 0;
        for (unsigned int i = 0; i < nb_proc; i++)
        {
            sendcounts[i] = (i == 0) ? (symbol_proc0) : (symbol_per_proc);
            displs[i] = sum;
            sum = sum + sendcounts[i];
        }
        it_left = nb_it - iteration_before_scatter;
    }

    // On indique à chaque processus le nombre d'itération restantes
    MPI_Bcast(&it_left, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (it_left > 0)
    {
        // On récupère la taille du segment à recevoir et on alloue un tableau de la taille correspondante
        recvcount = (rang == 0) ? (symbol_proc0) : (symbol_per_proc);
        char *recv_array = malloc(sizeof(char) * recvcount);

        // On transmet les parties correspondantes du tableau à chaque processus
        MPI_Scatterv(tab_list, sendcounts, displs, MPI_CHAR, recv_array, recvcount, MPI_CHAR, 0, MPI_COMM_WORLD);

        // On transforme notre buffer en liste pour pouvoir travailler dessus
        list *nl = array_to_list(recv_array, recvcount);

        // On calcule les itérations restantes
        write_symbol(nl, it_left);

        // On calcule la taille du tableau final et on alloue la mémoire
        if (rang == 0)
        {
            list_size_array = malloc(sizeof(int) * nb_proc);
            array_final = malloc(sizeof(char) * symbol_number(nb_it));
        }

        // On transmet la taille de chaque liste/tableau au processus 0.
        MPI_Gather(&(nl->size), 1, MPI_INT, list_size_array, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rang == 0)
        {
            sum = 0;
            for (unsigned int i = 0; i < nb_proc; i++)
            {
                displs[i] = sum;
                sum = sum + list_size_array[i];
            }
        }

        // On converti notre liste en tableau pour pouvoir la transmettre
        MPI_Gatherv(list_to_array(nl, nl->size), nl->size, MPI_CHAR, array_final, list_size_array, displs, MPI_CHAR, 0, MPI_COMM_WORLD);

        if (rang == 0)
            draw_dragon_color(array_final, symbol_number(nb_it), cr, length, surface_dragon, list_size_array);
    }
    else
    {
        if (rang == 0)
            draw_dragon(tab_list, nb_symbol, cr, length, surface_dragon);
    }

    if (rang == 0)
        printf("Temps d'execution : %lf secondes\n", MPI_Wtime() - start);

    MPI_Finalize();

    return EXIT_SUCCESS;
}

/**** DEFINITION DES FONCTIONS ****/

cairo_t *set_cr(cairo_surface_t *surface, double cr_size)
{
    cairo_t *cr = cairo_create(surface);
    cairo_set_line_width(cr, cr_size);
    return cr;
}

int ask_it()
{
    int n;
    printf("Choisissez le nombre d'itérations (entier positif) :\n");
    scanf("%d", &n);
    while (n < 0)
    {
        printf("Merci d'entrer un entier positif :\n");
        scanf("%d", &n);
    }
    return n;
}

unsigned int symbol_number(int iterations)
{
    return (pow(2, iterations) + pow(2, iterations + 2) - 4);
}

double deg_to_rad(int angle)
{
    return (double)angle * M_PI / 180;
}

void draw_symbol(cairo_t *cr, char c, double size)
{

    switch (c)
    {
    case '+':
        cairo_rotate(cr, deg_to_rad(45));
        break;

    case '-':
        cairo_rotate(cr, deg_to_rad(-45));
        break;

    case 'F':
        cairo_rel_line_to(cr, size, 0);
        break;

    default:
        fprintf(stderr, "Symbole inconnu\n");
        assert(0);
        break;
    }
}

void draw_dragon(char *array, unsigned int array_size, cairo_t *cr, double size, cairo_surface_t *surface)
{
    cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OVER);
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    for (unsigned int i = 0; i < array_size; i++)
        draw_symbol(cr, array[i], size);

    cairo_stroke_preserve(cr);
    save_image(cr, surface);
}

void draw_dragon_color(char *array, unsigned int array_size, cairo_t *cr, double size, cairo_surface_t *surface, int *list_size_array)
{
    int proc_color = 0;
    change_color(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OVER);

    for (unsigned int i = 0; i < array_size; i++)
    {
        while (list_size_array[proc_color] > 0)
        {
            draw_symbol(cr, array[i], size);
            i++;
            list_size_array[proc_color]--;
        }

        cairo_stroke_preserve(cr);
        change_color(cr);
        proc_color++;
        i--;
    }

    save_image(cr, surface);
}

void change_color(cairo_t *cr)
{
    double r = (double)rand() / (double)RAND_MAX;
    double g = (double)rand() / (double)RAND_MAX;
    double b = (double)rand() / (double)RAND_MAX;
    cairo_set_source_rgba(cr, r, g, b, 1);
}

void save_image(cairo_t *cr, cairo_surface_t *surface)
{
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_paint(cr);

    cairo_surface_write_to_png(surface, "dragon.png");
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}