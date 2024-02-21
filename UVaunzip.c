#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **args) {
  if (argc <= 1) {
    printf("UVaunzip: file1 [file2 ...]\n");
    fflush(stdout);
    exit(1);
  }

  FILE *fp;
  char character;
  int occurences, ret;
  for (int i = 1; i < argc; i++) {
    fp = fopen(args[i], "r");
    if (fp == NULL) {
      printf("UVaunzip: cannot open file\n");
      fflush(stdout);
      exit(1);
    }

    while (!feof(fp)) {
      ret = fread(&occurences, sizeof(int), 1, fp);
      if (ret != 1)
        break;
      ret = fread(&character, sizeof(char), 1, fp);
      if (ret != 1)
        break;

      for (; occurences > 0; occurences--)
        fprintf(stdout, "%c", character);
    }
  }
  fflush(stdout);
  fclose(fp);
  return 0;
}