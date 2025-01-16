/**
 * @file include/option.h
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Option implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include "bits_operations.h"

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

#endif
