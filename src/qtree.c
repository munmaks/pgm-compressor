/**
 * @file src/qtree.c
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Quadtree implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 */

#include "qtree.h"
#include <math.h>

static void fill_quadtree_recursive(QTree *qtree, Pixmap *pix, unsigned int child, unsigned char niveau, unsigned int line, unsigned int col);

static bool is_uniform(QTree *tree, unsigned int child);

static void qtc_from_quadtree(QTree *qtree, FileBit *filebit, unsigned int *encoded_size, bool need_to_write);

static unsigned int filtrage(QTree *qtree, unsigned int index, int niveau, double sigma, double alpha);

static unsigned short calculate_child_sum(QTree *tree, unsigned int child);

static float calculate_variance(QTree *qtree, unsigned int index, unsigned int child_index);

/**
 * @brief Normalize the value of the pixel
 *
 * @param value the value of the pixel
 * @param depth the depth of the pixel
 * @return `unsigned char` the normalized value
 */
static __inline__ unsigned char normalize_value(unsigned char value, unsigned char depth)
{
    return (unsigned char)(value * QTC_GREY_LEVEL / depth);
}

/**
 * @brief Determine the size of the quadtree
 *
 * @param pix
 * @return unsigned char
 */
extern unsigned char determine_qtree_level(Pixmap *pix)
{
    unsigned char n = 0; /* tree level */
    size_t power = 1UL;
    if (!pix)
    {
        fprintf(stderr, "Error: pixmap is NULL!\n");
        return 0UL;
    }
    while (power < pix->width) /* width == height, so we can use only one */
    {
        power <<= 1UL;
        ++n;
    }
    return n;
}

/**
 * @brief Make a quadtree with the given size, grey level and niveau
 *
 * @param tree
 * @param grey_level
 * @param niveau
 * @return size_t the size of the quadtree
 */
extern size_t make_qtree(QTree *tree, unsigned char grey_level, unsigned char niveau)
{
    size_t i = 0UL;
    size_t size = 0UL;
    if (!tree)
    {
        fprintf(stderr, "Error: tree is NULL!\n");
        return 0;
    }
    tree->niveau = niveau;
    size = DETERMINE_QTREE_SIZE(tree->niveau);
    tree->grey_level = grey_level;
    if (!(tree->nodes = malloc(size * sizeof(*tree->nodes))))
    {
        fprintf(stderr, "Memory allocation error for the quad tree !\n");
        return 0;
    }
    for (i = 0UL; i < size; ++i)
    {
        tree->nodes[i].color = 0x0U;
        tree->nodes[i].e = 0x0U;
        tree->nodes[i].u = 0x0U;
    }
    return size;
}

extern void free_qtree(QTree *tree)
{
    if (tree && tree->nodes)
    {
        free(tree->nodes);
        tree->nodes = NULL;
    }
}

extern void init_quadtree(QTree *tree, Pixmap *pix)
{
    if (!tree || !pix || !tree->nodes || !pix->data)
    {
        fprintf(stderr, "Error: tree / pixmap is NULL in init_quadtree()!\n");
    }
    /* fprintf(stderr, "sizeof Node: %lu\n", sizeof(Node)); */
    fill_quadtree_recursive(tree, pix, 0U, tree->niveau, 0U, 0U);
}

/**
 * @brief Check if the children of a node are uniform
 *
 * @param tree the quadtree
 * @param child the index of the first child
 * @return true if the children are uniform
 * @return false otherwise
 */
static bool is_uniform(QTree *tree, unsigned int child)
{
    /* even the fact that the children satisfy (m1 = m2 = m3 = m4) is not enough */
    return (tree->nodes[child].color == tree->nodes[child + 0x1U].color) &&
           (tree->nodes[child].color == tree->nodes[child + 0x2U].color) &&
           (tree->nodes[child].color == tree->nodes[child + 0x3U].color) &&
           /* it's also necessary that (u1 = u2 = u3 = u4 = 1) */
           (tree->nodes[child].u == 0x1U) &&
           (tree->nodes[child].u == tree->nodes[child + 0x1U].u) &&
           (tree->nodes[child].u == tree->nodes[child + 0x2U].u) &&
           (tree->nodes[child].u == tree->nodes[child + 0x3U].u);
}

/**
 * @brief Calculate the sum of the children of a node
 *
 * @param tree the quadtree
 * @param child the index of the first child
 * @return unsigned short the sum of the children
 */
static unsigned short calculate_child_sum(QTree *tree, unsigned int child)
{
    return (unsigned short)(tree->nodes[child].color +
                            tree->nodes[child + 0x1U].color +
                            tree->nodes[child + 0x2U].color +
                            tree->nodes[child + 0x3U].color);
}

/**
 * @brief helper function to fill the output array recursively
 *
 * @param qtree the quadtree
 * @param pix the pixmap
 * @param index the index of the node
 * @param niveau the level of the quadtree
 * @param line the line of the pixmap
 * @param col the column of the pixmap
 * @param child the index of the first child
 * @return void
 */
static void fill_quadtree_recursive(
    QTree *qtree, Pixmap *pix,
    unsigned int index, unsigned char niveau,
    unsigned int line, unsigned int col)
{
    unsigned int child_index = 0x0U;
    unsigned short child_sum = 0x0U;

    if (!niveau)
    {
        qtree->nodes[index].u = 0x1U;
        qtree->nodes[index].e = 0x0U;
        qtree->nodes[index].variance = 0.0F; /* we need to calculate the variance for `filtrage` if it's present */
        qtree->nodes[index].color = normalize_value(pix->data[line * pix->width + col], pix->grey_level);
        return;
    }

    /* going down to leaf node first */
    --niveau;

    /* divide into quadrants and recurse */
    child_index = index * MAX_CHILD + 0x1U;
    fill_quadtree_recursive(qtree, pix, child_index, niveau, line, col);
    fill_quadtree_recursive(qtree, pix, child_index + 1, niveau, line, col + (0x1U << niveau));
    fill_quadtree_recursive(qtree, pix, child_index + 2, niveau, line + (0x1U << niveau), col + (0x1U << niveau));
    fill_quadtree_recursive(qtree, pix, child_index + 3, niveau, line + (0x1U << niveau), col);

    /* for internal nodes, we calculate the color as the average of the children */
    child_sum = calculate_child_sum(qtree, child_index);
    qtree->nodes[index].e = child_sum & 0x3U;                      /* we check if there any error on first two bits */
    qtree->nodes[index].u = is_uniform(qtree, child_index);        /* not a leaf node, so we need to determine `u` from childs */
    qtree->nodes[index].color = (unsigned char)(child_sum / 4.0F); /* average of the children */

    /* we need to calculate the variance for `filtrage` if it's present */
    qtree->nodes[index].variance = calculate_variance(qtree, index, child_index);
}

extern void create_qtc_file(QTree *qtree, unsigned short width, const char *file_name)
{
    FileBit out = {0};
    FILE *fptr = NULL;
    time_t current_time = 0L;
    char *c_time_string = NULL;
    unsigned int encoded_size = 0U;
    if (!qtree || !file_name)
    {
        fprintf(stderr, "Error: qtree / file_name is NULL in create_qtc_file()!\n");
        return;
    }
    if (!(fptr = fopen(file_name, "w")))
    {
        fprintf(stderr, "Error: %s file not found in create_qtc_file()!\n", file_name);
        return;
    }
    fBitinit(&out, fptr);

    current_time = time(NULL);
    c_time_string = ctime(&current_time);

    fprintf(fptr, "Q1\n# %s", c_time_string);
    fprintf(fptr, "# compression rate ");

    qtc_from_quadtree(qtree, &out, &encoded_size, false);

    /* writing the compression rate */
    fprintf(fptr, "%.2f%%\n", ((float)encoded_size * 100.0F) / (width * width * 8.0F));

    fEcritCharbin(&out, qtree->niveau);
    qtc_from_quadtree(qtree, &out, &encoded_size, true);

    fBitclose(&out);
}

/**
 * @brief Create a .qtc file from the quadtree
 *
 * @param qtree the quadtree
 * @param filebit the filebit structure
 * @param encoded_size the size of the encoded data
 * @param need_to_write true if we need to write the data
 * @return void
 */
static void qtc_from_quadtree(
    QTree *qtree, FileBit *filebit,
    unsigned int *encoded_size, bool need_to_write)
{
    unsigned int error = 0U;
    size_t i = 0UL;
    size_t child_index = 0x0UL;
    size_t parent_index = 0x0UL;
    size_t qtree_size = 0UL;

    if (!qtree || !filebit)
    {
        fprintf(stderr, "Error: qtree / filebit is NULL in qtc_from_quadtree()!\n");
        return;
    }
    qtree_size = DETERMINE_QTREE_SIZE(qtree->niveau);

    for (i = 0; i < qtree_size; ++i)
    {
        error = qtree->nodes[i].e;
        child_index = i * MAX_CHILD + 0x1U;
        parent_index = (i) ? ((i - 1) / MAX_CHILD) : (0x0UL); /* parent index */

        /* parent is uniform */
        if (qtree->nodes[parent_index].u)
        {
            continue;
        }

        /* that's a leaf */
        if (child_index >= qtree_size)
        {
            /* not 4-th child */
            if (i % MAX_CHILD > 0UL)
            {
                if (need_to_write)
                {
                    fEcritCharbin(filebit, qtree->nodes[i].color);
                }
                else
                {
                    *encoded_size += 0x8U;
                }
            }
            continue;
        }
        /* internal nodes and root */
        if (!i || (i % MAX_CHILD))
        {

            if (need_to_write)
            {
                fEcritCharbin(filebit, qtree->nodes[i].color);
            }
            else
            {
                (*encoded_size) += 0x8U;
            }
        }

        /* writing error everytime */
        if (need_to_write)
        {
            fEcrireBit(filebit, (int)(error >> 0x1U));
            fEcrireBit(filebit, (int)(error & 0x1U));
        }
        else
        {
            (*encoded_size) += 0x2U;
        }

        /* error is 0, so we write uniform */
        if (!error)
        {
            if (need_to_write)
            {
                fEcrireBit(filebit, qtree->nodes[i].u);
            }
            else
            {
                ++(*encoded_size);
            }
        }
    }

    if (!need_to_write)
    {
        /* padding at the end */
        *encoded_size += (0x8U - *encoded_size % 0x8U) % 0x8U;
    }
}

extern void init_quadtree_from_file(QTree *tree, const char *file_name)
{
    FileBit in = {0};
    size_t i = 0UL, qtree_size = 0UL, child_index = 0x0UL, parent_index = 0x0UL;
    unsigned char character = 0U, niveau = 0U, color_fourth_child = 0U;

    if (!tree || !file_name)
    {
        fprintf(stderr, "Error: tree / file_name is NULL in init_quadtree_from_file()!\n");
        return;
    }
    if (!(fBitopen(&in, file_name, "r")))
    {
        fprintf(stderr, "Error: %s file not found in init_quadtree_from_file()!\n", file_name);
        return;
    }

    if ('Q' != fLireCharbin(&in) || '1' != fLireCharbin(&in))
    {
        fprintf(stderr, "Error: %s is not a QTC file!\n", file_name);
        return;
    }
    (void)fLireCharbin(&in); /* read '\n' */

    /* read all comments */
    do
    {
        character = fLireCharbin(&in);
        if ('#' == character)
        {
            fprintf(stderr, "comment: ");
            while ('\n' != character)
            {
                fprintf(stderr, "%c", character);
                character = fLireCharbin(&in);
            }
            fprintf(stderr, "\n");
        }
        else
        {
            niveau = character;
            break;
        }
    } while ('\n' == character);

    qtree_size = make_qtree(tree, QTC_GREY_LEVEL, niveau);

    for (i = 0UL; i < qtree_size; ++i)
    {
        parent_index = (i) ? (i - 1) / MAX_CHILD : (0UL);
        child_index = i * MAX_CHILD + 0x1U;

        /* that's a leaf */
        if (child_index >= qtree_size)
        {
            if (tree->nodes[parent_index].u)
            {
                tree->nodes[i].color = tree->nodes[parent_index].color;
            }
            else if (!(i % MAX_CHILD))
            {
                /* m_4 = (4m + e) - (m_1 + m_2 + m_3) */
                color_fourth_child = (unsigned char)((tree->nodes[parent_index].color * MAX_CHILD + /* 4m */
                                                      tree->nodes[parent_index].e) -                /* + e */
                                                     (tree->nodes[i - 3].color +                    /* - m_1 */
                                                      tree->nodes[i - 2].color +                    /* - m_2 */
                                                      tree->nodes[i - 1].color));                   /* - m_3 */
                tree->nodes[i].color = color_fourth_child;
            }
            else
            {
                tree->nodes[i].color = fLireCharbin(&in);
            }
            tree->nodes[i].e = 0x0U;
            tree->nodes[i].u = 0x1U;
            continue;
        }

        /* internal every 4th nodes, without root */
        if (i && !(i % MAX_CHILD))
        {
            /* m_4 = (4m + e) - (m_1 + m_2 + m_3) */
            color_fourth_child = (unsigned char)((tree->nodes[parent_index].color * MAX_CHILD + /* 4m */
                                                  tree->nodes[parent_index].e) -                /* + e */
                                                 (tree->nodes[i - 3].color +                    /* - m_1 */
                                                  tree->nodes[i - 2].color +                    /* - m_2 */
                                                  tree->nodes[i - 1].color));                   /* - m_3 */
            tree->nodes[i].color = color_fourth_child;

            tree->nodes[i].e = (unsigned char)(fLireBit(&in) << 0x1U);
            tree->nodes[i].e |= (unsigned char)(fLireBit(&in));
            tree->nodes[i].u = (!tree->nodes[i].e) ? (unsigned char)(fLireBit(&in)) : (0x0U);
            continue;
        }

        tree->nodes[i].color = fLireCharbin(&in);                                         /* m */
        tree->nodes[i].e = (unsigned char)(fLireBit(&in) << 0x1U);                        /* e */
        tree->nodes[i].e |= (unsigned char)(fLireBit(&in));                               /* e */
        tree->nodes[i].u = (!tree->nodes[i].e) ? (unsigned char)(fLireBit(&in)) : (0x0U); /* u */
    }

    fBitclose(&in);
}

static void fill_pixmap_recursive(QTree *qtree, Pixmap *pix,
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
                pix->data[(line + i) * pix->width + (col + j)] = qtree->nodes[index].color;
            }
        }
        return;
    }

    if (!niveau)
    {
        pix->data[line * pix->width + col] = qtree->nodes[index].color;
        return;
    }

    /* going down to leaf node first */
    --niveau;

    /* divide into quadrants and recurse */
    child_index = index * MAX_CHILD + 0x1U;

    fill_pixmap_recursive(qtree, pix, child_index, niveau, line, col);
    fill_pixmap_recursive(qtree, pix, child_index + 1, niveau, line, col + (0x1U << niveau));
    fill_pixmap_recursive(qtree, pix, child_index + 2, niveau, line + (0x1U << niveau), col + (0x1U << niveau));
    fill_pixmap_recursive(qtree, pix, child_index + 3, niveau, line + (0x1U << niveau), col);
}

extern void pixmap_from_quadtree(QTree *qtree, Pixmap *pix)
{
    (void)strncpy(pix->magic_number, "P5", 2UL);
    pix->magic_number[2] = '\0';

    pix->height = pix->width = (unsigned short)(1UL << (qtree->niveau));
    pix->data = malloc(pix->width * pix->height * sizeof(*pix->data));
    if (!pix->data)
    {
        fprintf(stderr, "Error: memory allocation error in pixmap_from_quadtree()!\n");
        return;
    }
    pix->grey_level = QTC_GREY_LEVEL;
    if (!pix->data)
    {
        fprintf(stderr, "Error: memory allocation error in pixmap_from_quadtree()!\n");
        return;
    }
    memset(pix->data, 0, pix->width * pix->height * sizeof(*pix->data));

    fill_pixmap_recursive(qtree, pix, 0U, qtree->niveau, 0U, 0U);
}

/*******************************************************************************/
/*******************************************************************************/
/********************************   FILTERING   ********************************/
/*******************************************************************************/
/*******************************************************************************/

/**
 * @brief Compute the average (medium) variance among all nodes in the quadtree.
 *
 * @param qtree The quadtree to inspect.
 * @return double The average variance.
 */
static double compute_average_variance(QTree *qtree)
{
    size_t qtree_size = 0UL, number_of_nodes = 0UL, i = 0UL;
    double sum = 0.0;
    if (!qtree || !qtree->nodes)
    {
        return 0.0;
    }
    qtree_size = DETERMINE_QTREE_SIZE(qtree->niveau);
    number_of_nodes = qtree_size - (1UL << (2UL * qtree->niveau));
    for (i = 0; i < qtree_size; ++i)
    {
        sum += qtree->nodes[i].variance;
    }

    return (qtree_size > 0) ? ((sum / (double)number_of_nodes)) : (0.0);
}

/**
 * @brief Compute the maximum variance among all nodes in the quadtree.
 *
 * @param qtree The quadtree to inspect.
 * @return double The maximum variance.
 */
static double compute_max_variance(QTree *qtree)
{
    size_t qtree_size = 0UL, i = 0UL;
    double max_var = 0.0;
    if (!qtree || !qtree->nodes)
    {
        return 0.0;
    }
    qtree_size = DETERMINE_QTREE_SIZE(qtree->niveau);
    for (i = 0; i < qtree_size; ++i)
    {
        if (qtree->nodes[i].variance > max_var)
        {
            max_var = qtree->nodes[i].variance;
        }
    }
    return max_var;
}

extern void must_filter_qtree(QTree *qtree, double alpha, bool flag)
{
    double medvar = 0.0, maxvar = 0.0;
    if (!qtree || !qtree->nodes)
    {
        fprintf(stderr, "Error: empty quadtree!\n");
        return;
    }
    if (!flag)
    {
        fprintf(stderr, "Filtering is disabled. Skipping.\n");
        return;
    }

    medvar = compute_average_variance(qtree);
    maxvar = compute_max_variance(qtree);
    filter_quadtree(qtree, medvar, maxvar, alpha);
}

/**
 * @brief Calculate the variance of the node
 *
 * @param qtree the quadtree
 * @param index the index of the node
 * @param child_index the index of the first child
 * @return float the variance of the node
 */
static float calculate_variance(QTree *qtree, unsigned int index, unsigned int child_index)
{
    unsigned int k = 0U;
    float mu = 0.0F, mk = 0.0F, vk = 0.0F, m = qtree->nodes[index].color;
    for (k = 0; k < MAX_CHILD; ++k)
    {
        mk = qtree->nodes[child_index + k].color;    /* color of child */
        vk = qtree->nodes[child_index + k].variance; /* variance of child */
        mu += (vk * vk) + ((m - mk) * (m - mk));     /* given formula */
    }

    return (float)sqrt(mu) / 4.0F;
}

/**
 * @brief Filtering the quadtree recursively on the root
 * \mu = \sum from 0 to k < 4, (v_k^2) + (m - m_k)^2 => v = \sqrt{\mu / 4}
 *
 * @param qtree the quadtree
 * @param index the index of the node
 * @param niveau the level of the quadtree
 * @param sigma the threshold
 * @param alpha the alpha value
 * @return unsigned int 1 if the node is uniform, 0 otherwise
 */
static unsigned int filtrage(QTree *qtree,
                             unsigned int index, int niveau,
                             double sigma, double alpha)
{
    unsigned int s = 0U, child_index = 0U;

    /* if the node is already uniform or if the node is a leaf, return 1 */
    if (qtree->nodes[index].u || !niveau)
    {
        return 1U;
    }

    --niveau;
    child_index = index * MAX_CHILD + 1U;

    /* descend into the lower levels: all children must be processed */
    s += filtrage(qtree, child_index, niveau, sigma * alpha, alpha);
    s += filtrage(qtree, child_index + 1U, niveau, sigma * alpha, alpha);
    s += filtrage(qtree, child_index + 2U, niveau, sigma * alpha, alpha);
    s += filtrage(qtree, child_index + 3U, niveau, sigma * alpha, alpha);

    /* the current node is 'uniformized' only if:       *
     * - all 4 children have already been 'uniformized' *
     * - it is variance meets the filter conditions     */
    if (s < MAX_CHILD || qtree->nodes[index].variance > sigma)
    {
        return 0U;
    }

    qtree->nodes[index].e = 0U;
    qtree->nodes[index].u = 1U;
    return 1U;
}

extern void filter_quadtree(QTree *qtree, double medvar, double maxvar, double alpha)
{
    double sigma = medvar / maxvar; /* initial threshold */
    (void)filtrage(qtree, 0U, qtree->niveau, sigma, alpha);
}
