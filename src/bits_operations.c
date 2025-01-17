/**
 * @file src/bits_operations.c
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Bit operations implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */
#include "bits_operations.h"

#ifndef _UCHAR_
#define _UCHAR_
#include <limits.h>
typedef unsigned char uchar;
#endif

/****************************************************************/
/****************************************************************/
/************    LES FONCTIONS POUR BIT OPERATIONS    ***********/
/****************************************************************/
/****************************************************************/

extern uchar fBitopen(FileBit *__restrict__ f, const char *__restrict__ path, const char *__restrict__ mode)
{
    if (!(f->fich = fopen(path, mode)))
    {
        return 0;
    }
    f->stock = f->nbBit = 0U;
    return 1;
}

extern void fBitinit(FileBit *__restrict__ f, FILE *__restrict__ fich)
{
    f->fich = fich;
    f->stock = f->nbBit = 0U;
}

extern int fBitclose(FileBit *__restrict__ f)
{
    if (f->nbBit)
    {
        /* shift left the remaining bits if necessary */
        if (f->nbBit != CHAR_BIT)
            f->stock <<= CHAR_BIT - f->nbBit;
        fputc(f->stock, f->fich);
    }
    return fclose(f->fich);
}

extern int fEcrireBit(FileBit *__restrict__ f, int bit)
{
    int status = 0;
    if (f->nbBit == CHAR_BIT)
    {
        f->nbBit = 0;
        status = fputc(f->stock, f->fich);
    }
    ++(f->nbBit);
    f->stock = (uchar)((f->stock << 1U) | bit);
    return status;
}

extern int fLireBit(FileBit *__restrict__ f)
{
    int n = 0;
    if (!f->nbBit)
    {
        if (EOF == (n = fgetc(f->fich)))
        {
            return EOF;
        }
        f->stock = (uchar)n;
        f->nbBit = CHAR_BIT;
    }
    --(f->nbBit);
    return (f->stock >> f->nbBit) & 1;
}

extern void fEcritCharbin(FileBit *__restrict__ f, uchar n)
{
    int i = 0;
    unsigned int mask = 0U; /* to test, if mask should be 1 or 0 at the beginning */
    for (i = CHAR_BIT - 1; i >= 0; --i)
    {
        mask = 1U << i;
        /* ignore return value; we don't need it */
        (void)fEcrireBit(f, (n & mask) != 0U);
    }
}

extern uchar fLireCharbin(FileBit *__restrict__ f)
{
    int bit = 0;
    char i = 0;
    uchar c = 0;
    for (i = 0; i < CHAR_BIT; ++i)
    {
        bit = fLireBit(f); /* handle EOF */
        if (bit == EOF)
        {
            fprintf(stderr, "Error: EOF in fLireCharbin()!\n");
            return 0;
        }
        c = (uchar)((c << 1U) | (uchar)bit);
    }
    return c;
}
