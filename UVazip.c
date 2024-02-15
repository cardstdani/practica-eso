#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **args) {
  if (argc <= 1) {
    printf("UVazip: file1 [file2 ...]\n");
    fflush(stdout);
    exit(1);
  }

  FILE *fp;
  char character, lastCharacter = '\0';
  int occurences = 0;
  for (int i = 1; i < argc; i++) {
    fp = fopen(args[i], "r");
    if (fp == NULL) {
      printf("UVazip: cannot open file\n");
      fflush(stdout);
      exit(1);
    }

    do {
      character = fgetc(fp);
      if (character != EOF || i == argc - 1) {
        if (lastCharacter != character) {
          if (lastCharacter != '\0') {
            fwrite(&occurences, sizeof(int), 1, stdout);
            fwrite(&lastCharacter, sizeof(char), 1, stdout);
          }
          occurences = 0;
        }
        occurences++;
        lastCharacter = character;
      }
    } while (character != EOF);
  }
  fflush(stdout);
  fclose(fp);
  return 0;
}