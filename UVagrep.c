#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void processFile(FILE *fp, char *buffer, char *word, size_t MAX_LINE_LENGTH) {
  while (getline(&buffer, &MAX_LINE_LENGTH, fp) != -1) {
    if (strstr(buffer, word) != NULL) {
      fprintf(stdout, "%s", buffer);
    }
  }
  fflush(stdout);
}

int main(int argc, char **args) {
  if (argc <= 1) {
    printf("UVagrep: searchterm [file ...]\n");
    fflush(stdout);
    exit(1);
  }

  FILE *fp;
  size_t MAX_LINE_LENGTH = 10000000;
  char *buffer = NULL;
  for (int i = 2; i < argc; i++) {
    fp = fopen(args[i], "r");
    if (fp == NULL) {
      printf("UVagrep: cannot open file\n");
      fflush(stdout);
      exit(1);
    }

    processFile(fp, buffer, args[1], MAX_LINE_LENGTH);
    fclose(fp);
  }
  if (argc == 2)
    processFile(stdin, buffer, args[1], MAX_LINE_LENGTH);
  return 0;
}