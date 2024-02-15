#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **args) {
  if (argc <= 1) {
    exit(0);
  }

  FILE *fp;
  unsigned MAX_LINE_LENGTH = 1000;
  char buffer[MAX_LINE_LENGTH];
  for (int i = 1; i < argc; i++) {
    fp = fopen(args[i], "r");
    if (fp == NULL) {
      printf("UVacat: no puedo abrir fichero\n");
      fflush(stdout);
      exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
      fprintf(stdout, "%s", buffer);
    }
    fflush(stdout);
    fclose(fp);
  }
  return 0;
}