/**
 * @file src/pixmap.c
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Pixmap implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 */

#include "qtree.h"

FILE *read_pgm_file(Pixmap *pix, const char *filename)
{
    FILE *fptr = NULL;
    char buffer[BUFFER_SIZE];
    int number_of_items = 0x0;
    unsigned int temp_grey_level = 0x0U;

    if (!(fptr = fopen(filename, "rb")))
    {
        fprintf(stderr, "Error: %s file not found\n", filename);
        return NULL;
    }

    if (NULL == fgets(buffer, BUFFER_SIZE, fptr))
    {
        fclose(fptr);
        return NULL;
    }

    if (sscanf(buffer, "%s\n", pix->magic_number) != 1 || strcmp(buffer, "P5\n"))
    {
        fprintf(stderr, "Magic number: %s\n", buffer);
    }

    while (fgets(buffer, BUFFER_SIZE, fptr) && *buffer == '#')
    {
        fprintf(stdout, "Comment: %s", buffer); /* comments */
    }

    number_of_items = sscanf(buffer, "%hu %hu %u", &pix->width, &pix->height, &temp_grey_level);
    if (number_of_items != 3)
    {
        /* third parameters on the next line */
        if (number_of_items == 2 && fgets(buffer, BUFFER_SIZE, fptr))
        {
            number_of_items += sscanf(buffer, "%u", &temp_grey_level);
        }
    }
    pix->grey_level = (unsigned char)temp_grey_level;

    if (number_of_items != 3)
    {
        fprintf(stderr, "width: %hu\nheight: %hu\ngrey_level: %hu\n", pix->width, pix->height, pix->grey_level);
        fprintf(stderr, "Error: incorect format when reading .pgm file\n");
        return NULL;
    }

    return fptr;
}

/* Read PGM file with the name of the file, put value on pixmap */
void free_pixmap(Pixmap *pix)
{
    if (pix && pix->data)
    {
        free(pix->data);
        pix->data = NULL;
    }
}

/* Make pixmap base on PGM file*/
void init_pixmap(Pixmap *pix, const char *filename)
{
    size_t check = 0UL;
    FILE *file = NULL;
    if (!pix)
    {
        fprintf(stderr, "Erreur: pixmap est NULL dans init_pixmap()!\n");
        return;
    }
    if (!(file = read_pgm_file(pix, filename)))
    {
        fprintf(stderr, "Erreur de lecture du fichier PGM !\n");
        return;
    }
    pix->data = NULL;
    if (!(pix->data = malloc(pix->height * pix->width * sizeof(*pix->data))))
    {
        fprintf(stderr, "Erreur d'allocation de mémoire pour les données de l'image !\n");
        free(pix);
        return;
    }
    check = fread(pix->data, sizeof(*pix->data), (pix->height * pix->width), file);

    if (check != (size_t)(pix->height * pix->width))
    {
        fprintf(stderr, "Fichier PGM tronqué!\n");
        free_pixmap(pix);
        fclose(file);
        return;
    }

    fclose(file);
}

void from_pixmap_to_pgm(Pixmap *pix, const char *filename)
{
    FILE *fptr = NULL;
    size_t i = 0UL;
    if (!pix)
    {
        fprintf(stderr, "Erreur: pixmap est NULL dans from_pixmap_to_pgm()!\n");
        return;
    }
    if (!(fptr = fopen(filename, "w")))
    {
        fprintf(stderr, "Erreur: %s fichier non trouvé dans from_pixmap_to_pgm()!\n", filename);
        return;
    }
    fprintf(fptr, "%s\n", pix->magic_number); /* magic number */
    fprintf(fptr, "%s%s\n", "# Created by ", AUTHORS);
    fprintf(fptr, "%hu %hu\n%u\n", pix->width, pix->height, (unsigned int)pix->grey_level);
    for (i = 0UL; i < pix->width * pix->height; ++i)
    {
        fputc(pix->data[i], fptr);
    }
    fclose(fptr);
}
