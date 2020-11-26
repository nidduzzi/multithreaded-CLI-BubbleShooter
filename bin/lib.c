#include "lib.h"

void textLoader(char address[], char ***dest, int *maxrow, int *maxline)
{
    FILE *fp = fopen(address, "r");
    int eofcount = 0, linenum = 0, rownum = 0;
    char c;
    if (fp == NULL)
    {
        printf("Failed to open file: %s\n", address);
        exit(1);
    }
    else
    {
        *maxrow = 0;
        (*dest) = (char **)calloc(1, sizeof(**dest));
        while (fscanf(fp, "%c", &c) != EOF)
        {
            if(*dest == NULL)
            {
                printf("Failed to allocate memory!\n");
                exit(1);
            }
            (*dest)[linenum] = (char *)realloc((*dest)[linenum], (rownum + 1) * sizeof(char));
            (*dest)[linenum][rownum] = c;
            ++rownum;
            if (c == '\n')
            {
                ++linenum;
                if((*maxrow) < (rownum - 1)) *maxrow = rownum;
                rownum = 0;
                (*dest) = (char **)realloc((*dest), (linenum + 1) * sizeof(*(*dest)));
            }
        }
        *maxline = linenum;
        for(linenum = 0; linenum < *maxline; ++linenum)
        {
            (*dest)[linenum] = realloc((*dest)[linenum], (*maxrow) * sizeof(char));
        }
    }
}
