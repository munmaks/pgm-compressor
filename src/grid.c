#include "grid.h"

/**
 * @brief Generate a grid from a quadtree
 * Create new string based on the source string
 * For exemple: "PGM/input.pgm" -> "PGM/input_g.pgm"
 *
 * @param src the source string
 * @return char * the new string
 */
extern char *change_filename_to_seg_grid(const char *src)
{
    size_t len = 0UL;
    char *new_str = NULL;
    if (!src)
    {
        fprintf(stderr, "Error: src is NULL in change_filename_to_seg_grid()!\n");
        return NULL;
    }
    /* we know ".pgm" is always present, so let's insert "_g" right before ".pgm". */
    len = strlen(src);
    if (len < 4)
    {
        fprintf(stderr, "Error: source length is too short!\n");
        return NULL;
    }
    /* allocate enough room: length + 2 (for "_g") + 1 (for '\0'). */
    new_str = malloc((len + 3) * sizeof(*new_str));
    if (!new_str)
    {
        fprintf(stderr, "Error: memory allocation failed!\n");
        return NULL;
    }

    /* copy everything except the last 4 characters (".pgm"), then append "_g", then ".pgm". */
    strncpy(new_str, src, len - 4);
    new_str[len - 4] = '\0';
    strcat(new_str, "_g");
    strcat(new_str, ".pgm");

    return new_str;
}

static void generate_grid_from_qtree_recursive(QTree *qtree, Pixmap *pix,
                                               unsigned int index, unsigned char niveau,
                                               unsigned int line, unsigned int col)
{
    unsigned int i = 0U, j = 0U, child_index = 0x0U, size = 1U << niveau;

    if (niveau > 0 && qtree->nodes[index].u)
    {
        for (i = 0; i < size; ++i)
        {
            for (j = 0; j < size; ++j)
            {
                if (!i || !j)
                {
                    pix->data[(line + i) * pix->width + (col + j)] = 0U;
                }
                else
                {
                    pix->data[(line + i) * pix->width + (col + j)] = 255U;
                }
            }
        }
        return;
    }

    if (!niveau)
    {
        pix->data[line * pix->width + col] = !(index % 2) ? (255U) : (0U);
        return;
    }

    --niveau;
    child_index = index * MAX_CHILD + 1U;

    generate_grid_from_qtree_recursive(qtree, pix, child_index, niveau, line, col);
    generate_grid_from_qtree_recursive(qtree, pix, child_index + 1U, niveau, line, col + (1U << niveau));
    generate_grid_from_qtree_recursive(qtree, pix, child_index + 2U, niveau, line + (1U << niveau), col + (1U << niveau));
    generate_grid_from_qtree_recursive(qtree, pix, child_index + 3U, niveau, line + (1U << niveau), col);
}

extern void generate_grid_from_quadtree(QTree *qtree, Pixmap *pix)
{
    if (!qtree || !pix)
    {
        fprintf(stderr, "Error: invalid arguments in generate_grid_from_quadtree()!\n");
        return;
    }
    (void)strcpy(pix->magic_number, "P5");
    pix->height = pix->width = (unsigned short)(1UL << (qtree->niveau));
    pix->data = malloc(pix->width * pix->height * sizeof(*pix->data));
    if (!pix->data)
    {
        fprintf(stderr, "Error: memory allocation error in pixmap_from_quadtree()!\n");
        return;
    }
    pix->grey_level = QTC_GREY_LEVEL;
    (void)memset(pix->data, 0, pix->width * pix->height * sizeof(*pix->data));
    generate_grid_from_qtree_recursive(qtree, pix, 0U, qtree->niveau, 0U, 0U);
}
