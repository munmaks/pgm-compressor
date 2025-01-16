/**
 * @file include/qtc.h
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Codec and Decodec for Quadtree Compression header file
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */

#ifndef __QTC_H__
#define __QTC_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>

#define MAX_SIZE 1024

typedef struct
{
    FILE *fich;          /* file descriptor */
    unsigned char nbBit; /* number of bites written in buffer */
    unsigned char stock; /* buffer of 8 bits */
} FileBit;

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

typedef struct args
{
    char *file_name_input;  /* `-i` + input.{pgm | qtc} */
    char *file_name_output; /* `-o` + output.{pgm | qtc}, by default: {QTC | PGM}/out.{qtc | pgm} */
    double alpha;           /* `-a`: quadtree filtering */
    bool mode;              /* `-c`: false - (encodeur),`-u`: true - (decodeur) */
    bool seg_grid;          /* `-g`: la grille de segmentation is  */
    bool verbose;           /* `-v`: verbose */
    bool help;              /* `-h`: help  */
    bool err;               /* `error`: unknown option */
} Args;

typedef struct pixmap /* struct occupies 16 bits */
{
    unsigned char *data;      /* data of the image */
    unsigned short width;     /* width of the image, 0 - 65535 */
    unsigned short height;    /* height of the image, 0 - 65535 */
    unsigned char grey_level; /* grey level of the image, 0 - 255 */
    char magic_number[3];     /* for example `P5` or `P2` */
} Pixmap;

/****************************************************************/
/****************************************************************/
/***************    FUNCTIONS FOR BIT OPERATIONS    *************/
/****************************************************************/
/****************************************************************/

/**
 * @brief Ouvre un fichier en lecture/Ecriture bit a bit
 * @return unsigned char: 1 si OK et 0 sinon
 */
extern unsigned char fBitopen(FileBit *__restrict__ f, const char *__restrict__ path, const char *__restrict__ mode);

/**
 * @brief Initialise une structure FileBit
 * avec un descripteur de fichier deja ouvert
 *
 * @param f
 * @param fich
 */
extern void fBitinit(FileBit *__restrict__ f, FILE *__restrict__ fich);

/**
 * @brief Ferme le fichier en argument, et ecrit les bits en attente
 *
 * @param f
 * @return int
 */
extern int fBitclose(FileBit *__restrict__ f);

/**
 * @brief Ecrit un bit dans le fichier
   Note: l'ecriture prend effet par paquet de 8 bits
 *
 * @param f
 * @param bit
 * @return int
 */
extern int fEcrireBit(FileBit *__restrict__ f, int bit);

/**
 * @brief Renvoie un bit lu depuis le fichier
 *
 * @param f
 * @return int
 */
extern int fLireBit(FileBit *__restrict__ f);

/**
 * @brief Ecrit les 8 bits d'un char en argument
 *
 * @param f
 * @param n
 */
extern void fEcritCharbin(FileBit *__restrict__ f, unsigned char n);

/**
 * @brief Renvoie le char contruit par les 8 bits suivant lus dans le fichier
 *
 * @param f
 * @return unsigned char
 */
extern unsigned char fLireCharbin(FileBit *__restrict__ f);

/****************************************************************/
/****************************************************************/
/**************    FUNCTIONS FOR GRID OPERATIONS    *************/
/****************************************************************/
/****************************************************************/

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

/****************************************************************/
/****************************************************************/
/************   FUNCTIONS FOR ARGUMENTS OPERATIONS   ************/
/****************************************************************/
/****************************************************************/

/**
 * @brief Fonction to use arguments passed in command line
 *
 * @param argc
 * @param argv
 * @param args
 * @return int
 */
extern int option_handle_args(int argc, char *argv[], Args *__restrict__ args);

/**
 * @brief Display help message
 *
 * @param void
 * @return void
 */
extern void option_print_help(void);

/**
 * @brief Display help message in verbose mode
 *
 * @param void
 * @return void
 */
extern void option_print_help_verbose(void);

/****************************************************************/
/****************************************************************/
/**************   FUNCTIONS FOR PIXMAP OPERATIONS   *************/
/****************************************************************/
/****************************************************************/

#define BUFFER_SIZE 1 << 13 /* 8192 */
#define QTC_GREY_LEVEL 255

#define AUTHORS "MUNAITPASOV M. & BENVENISTE A."

/**
 * @brief Initialize a pixmap from a file (P5 PGM format)
 *
 * @param pix Pixmap to initialize
 * @param filename Filename of the image
 * @return void
 */
void init_pixmap(Pixmap *pix, const char *filename);

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

/****************************************************************/
/****************************************************************/
/*************   FUNCTIONS FOR QUADTREE OPERATIONS   ************/
/****************************************************************/
/****************************************************************/

/*
    formula for sum of 4^k, for k in [0, n-1], in this case `k` is `qtree->niveau`,
    source: https://www.cuemath.com/geometric-sum-formula/
    4^level is equivalent to (2^level) * (2^level)
    sum of geometric series : (4 ^ level) / (4 - 1)
*/
#define DETERMINE_QTREE_SIZE(level) ((1UL << (2UL * (level + 1UL))) / 3UL)

#define MAX_CHILD 4

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

#endif /* __QTC_H__ */
