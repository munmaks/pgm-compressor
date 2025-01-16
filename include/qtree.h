/**
 * @file include/qtree.h
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Quadtree implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */

#ifndef QTREE_H
#define QTREE_H

#include "pixmap.h"

/*
    formula for sum of 4^k, for k in [0, n-1], in this case `k` is `qtree->niveau`,
    source: https://www.cuemath.com/geometric-sum-formula/
    4^level is equivalent to (2^level) * (2^level)
    sum of geometric series : (4 ^ level) / (4 - 1)
*/
#define DETERMINE_QTREE_SIZE(level) ((1UL << (2UL * (level + 1UL))) / 3UL)

#define MAX_CHILD 4

/**
 * `float variance;`
 *
 * `unsigned char color;`
 *
 * `unsigned char e;`
 *
 * `unsigned char u;`
 * }
 */
typedef struct node
{
    /*unsigned int e : 2;*/ /* 2 bits, value in [0, 3] */
    /*unsigned int u : 1;*/ /* true if uniform, false otherwise */
    float variance;         /* variance of the node */
    unsigned char e;        /* without -ansi flag, 2 bytes of memory */
    unsigned char u;        /* without -ansi flag, 2 bytes of memory */
    unsigned char color;    /* 8 bits from 0 to 255 */
} Node;

/**
 * Quadtree structure with nodes, grey_level and niveau
 *
 * `Node *nodes;`
 *
 * `int grey_level;`
 *
 * `unsigned char niveau;`
 *
 */
typedef struct qtree
{
    Node *nodes;          /* nodes of the quadtree */
    int grey_level;       /* 4 bytes */
    unsigned char niveau; /* 8 bits from 0 to 255 */
} QTree;

/**
 * @brief Determine the level of the quadtree
 *
 * @param pix the pixmap
 * @return unsigned char the level of the quadtree
 */
extern unsigned char determine_qtree_level(Pixmap *pix);

/**
 * @brief Make a quadtree with the given size, grey level and niveau
 *
 * @param tree the quadtree
 */
extern size_t make_qtree(QTree *tree, unsigned char grey_level, unsigned char niveau);

/**
 * @brief Initialize the quadtree with the pixmap's data
 *
 * @param tree the quadtree
 * @param pix the pixmap
 * @return void
 */
extern void init_quadtree(QTree *tree, Pixmap *pix);

/**
 * @brief Free the quadtree
 *
 * @param tree the quadtree
 * @return void
 */
extern void free_qtree(QTree *tree);

/**
 * @brief Create a .qtc file from the quadtree
 *
 * @param qtree the quadtree
 * @param width the width of the image
 * @param file_name the name of the file
 * @return void
 */
extern void create_qtc_file(QTree *qtree, unsigned short width, const char *file_name);

/**
 * @brief Initialize the quadtree from a file
 *
 * @param tree the quadtree
 * @param file_name the name of the file
 * @return void
 */
extern void init_quadtree_from_file(QTree *tree, const char *file_name);

/**
 * @brief Create a pixmap from the quadtree
 *
 * @param qtree the quadtree
 * @param pix the pixmap
 * @return void
 */
extern void pixmap_from_quadtree(QTree *qtree, Pixmap *pix);

/**
 * @brief Filtering the quadtree on the root
 *
 * @param qtree the quadtree
 * @param medvar the median variance
 * @param maxvar the maximum variance
 * @param alpha the alpha value
 */
extern void filter_quadtree(QTree *qtree, double medvar, double maxvar, double alpha);

/**
 * @brief Filtering the quadtree recursively on the root
 *
 * @param qtree the quadtree
 * @param alpha the alpha value (based on user input)
 * @param do_filter the boolean value to determine if the filtering should be done
 */
extern void must_filter_qtree(QTree *qtree, double alpha, bool flag);

#endif
