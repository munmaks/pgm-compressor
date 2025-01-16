/**
 * @file include/bits_operations.h
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Bit operations implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */

#ifndef __BITS_OPERATIONS_H__
#define __BITS_OPERATIONS_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>

/* #define NBLETTRES 256 */
#define MAX_SIZE 1024

typedef struct
{
  FILE *fich;          /* descripteur du fichier                  */
  unsigned char nbBit; /* nombre de bits écrits dans le buffer    */
  unsigned char stock; /* buffer de 8 bits                        */
} FileBit;

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

#endif
