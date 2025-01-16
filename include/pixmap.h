/**
 * @file include/pixmap.h
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Pixmap implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */

#ifndef PIXMAP_H
#define PIXMAP_H

#include "option.h"

#define BUFFER_SIZE 1 << 13 /* 8192 */
#define QTC_GREY_LEVEL 255

#define AUTHORS "MUNAITPASOV M. & BENVENISTE A."
typedef struct pixmap /* struct occupies 16 bits */
{
    unsigned char *data;      /* data of the image */
    unsigned short width;     /* width of the image, 0 - 65535 */
    unsigned short height;    /* height of the image, 0 - 65535 */
    unsigned char grey_level; /* grey level of the image, 0 - 255 */
    char magic_number[3];     /* for example `P5` or `P2` */
} Pixmap;

/**
 * @brief Initialize a pixmap from a file (P5 PGM format)
 *
 * @param pix Pixmap to initialize
 * @param filename Filename of the image
 * @return void
 */
void init_pixmap(Pixmap *pix, const char *filename);

/**
 * @brief Print a pixmap to the standard output
 *
 * @param pix Pixmap to print
 * @return void
 */
void print_pix(Pixmap *pix);

/**
 * @brief Free a pixmap
 *
 * @param pix Pixmap to free
 * @return void
 */
void free_pixmap(Pixmap *pix);

/**
 * @brief Save a pixmap to a file
 *
 * @param pix Pixmap to save
 * @param filename Filename of the image
 * @return void
 */
void from_pixmap_to_pgm(Pixmap *pix, const char *filename);

#endif
