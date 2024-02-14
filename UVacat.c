#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **args)
{
    if (argc <= 1)
    {
        exit(0);
    }

    FILE *fp;
    char buffer[1000];
    for (int i = 1; i < argc; i++)
    {
        fp = fopen(args[i], "r");
        if (fp == NULL)
        {
            printf("UVacat: no puedo abrir fichero\n");
            exit(1);
        }

        while (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            fprintf(stdout, "%s", buffer);
        }
        fflush(stdout);
        fclose(fp);
    }
    return 0;
}