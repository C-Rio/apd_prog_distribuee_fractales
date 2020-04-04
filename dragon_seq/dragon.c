#include "dragon.h"

int main(int argc, char **argv)
{
    cairo_t *cr;
    cairo_surface_t *surface_dragon;

    char *tab_list;

    double length;
    double width;
    double height;

    srand(time(NULL));
    int nb_it = ask_it();

    width = 500 + nb_it * 200;
    height = width;
    length = width / 2;

    unsigned int nb_symbol = symbol_number(nb_it);

    length = pow(1 / sqrt(2), nb_it) * length;

    clock_t begin = clock();

    surface_dragon = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cr = set_cr(surface_dragon, 2.0);
    cairo_move_to(cr, width / 4, height / 3);

    list *l = new_list();

    list_pushBack(l, 'F');

    write_symbol(l, nb_it);

    // On converti la liste en tableau pour pouvoir la transmettre
    tab_list = list_to_array(l, nb_symbol);

    draw_dragon(tab_list, nb_symbol, cr, length, surface_dragon);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Temps d'execution : %lf secondes\n", time_spent);

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

void save_image(cairo_t *cr, cairo_surface_t *surface)
{
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_paint(cr);

    cairo_surface_write_to_png(surface, "dragon.png");
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}