/**
 * @file include/grid.h
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Grid implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */

#ifndef GRID_H
#define GRID_H

#include "qtree.h"

/**
 * @brief Generate segment grid from quadtree and save it to a file
 *
 * @param qtree Quadtree to generate grid from
 * @param pix Pixmap to save grid to
 * @return void
 */
extern void generate_grid_from_quadtree(QTree *qtree, Pixmap *pix);

/**
 * @brief Change filename to the same and adding `_g` before extension.
 * For segment grid files.
 *
 * @param src Source filename
 * @return `char*` new filename
 * @note The returned string must be freed after use.
 */
extern char *change_filename_to_seg_grid(const char *src);

#endif
