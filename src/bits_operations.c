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

/****************************************************************/
/****************************************************************/
/************    LES FONCTIONS POUR BIT OPERATIONS    ***********/
/****************************************************************/
/****************************************************************/

extern unsigned char fBitopen(
    FileBit *__restrict__ f,
    const char *__restrict__ path,
    const char *__restrict__ mode)
{
    if (!(f->fich = fopen(path, mode)))
    {
        return 0;
    }
    f->stock = 0U;
    f->nbBit = 0U;
    return 1;
}

extern void fBitinit(
    FileBit *__restrict__ f,
    FILE *__restrict__ fich)
{
    f->fich = fich;
    f->stock = 0U;
    f->nbBit = 0U;
}

extern int fBitclose(FileBit *__restrict__ f)
{
    if (f->nbBit != 0)
    {
        /* decaler a gauche les derniers bits si necessaire*/
        if (f->nbBit != 8)
            f->stock <<= 8 - f->nbBit;
        fputc(f->stock, f->fich);
    }
    return fclose(f->fich);
}

extern int fEcrireBit(FileBit *__restrict__ f, int bit)
{
    int coderetour = 1;
    if (f->nbBit == 8)
    {
        f->nbBit = 0;
        coderetour = fputc((int)f->stock, f->fich);
    }
    ++f->nbBit;
    f->stock = (unsigned char)((f->stock << 1) | bit);
    return coderetour;
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
        f->stock = (unsigned char)n;
        f->nbBit = 8;
    }
    --f->nbBit;
    return (f->stock >> f->nbBit) & 1;
}

extern void fEcritCharbin(FileBit *__restrict__ f, unsigned char n)
{
    int i = 0;
    unsigned int mask = 0U; /* to test, if mask should be 1 or 0 at the beginning */
    for (i = 7; i >= 0; --i)
    {
        mask = 1U << i;

        /* code before:  fEcrireBit(f, (n & mask) != 0); */
        (void)fEcrireBit(f, (n & mask) != 0); /* ignore return value; we don't need it */
    }
}

extern unsigned char fLireCharbin(FileBit *__restrict__ f)
{
    char i = 0;
    unsigned char c = 0;
    for (i = 0; i < 8; ++i)
    {
        c = (unsigned char)((c << 1U) | fLireBit(f));
    }
    return c;
}
