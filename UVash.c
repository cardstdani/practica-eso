#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_ARGUMENTS 10000

char error_message[30] = "An error has occurred\n";

void printError() {
  fprintf(stderr, "%s", error_message);
  fflush(stderr);
}

struct command {
  char **arg_array;
  size_t num_argumentos;
};

int main(int argc, char **args) {
  while (1) {
    fprintf(stdout, "UVash%s> ", "\xE2\x98\xBA");
    fflush(stdout);
    // Lectura de comando de entrada
    char *buff;
    size_t size = 0;
    if (getline(&buff, &size, stdin) == -1) {
      printf("Error reading input\n");
      exit(1);
    }

    //  Separar por espacios
    char **arg_array = (char **)calloc(MAX_ARGUMENTS, sizeof(char *));
    char *ptr = buff;
    size_t num_argumentos = 0;
    while ((ptr = strsep(&buff, " \n\t")) != NULL) {
      if (*ptr == '\0')
        continue;
      arg_array[num_argumentos++] = ptr;
    }
    arg_array[num_argumentos] = NULL; // Poner ultimo elemento NULL

    for (int a = 0; a < num_argumentos; a++) {
      printf("%s\n", arg_array[a]);
      fflush(stdout);
    }

    // Comprobar si el usuario quiere salir del shell
    if (strcmp(arg_array[0], "exit") == 0) {
      printf("Cerrando shell...");
      exit(0);
    }

    // Comprobar si el usuario ha introducido el comando correctamente
    /*
    int estado;
    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
      estado = execvp(arg_array[0], arg_array);
      if (estado == -1) {
        exit(1);
      }
    } else if (pid > 0) { // Proceso padre
      wait(NULL);         // espera a que termine el hijo
    } else {              // Error en el proceso
      perror("fork");
      return 1;
    }*/

    // Liberar memoria de argumentos
    for (int i = 0; i < num_argumentos; i++) {
      // free(arg_array[i]);
    }
    free(arg_array);
  }
  return 0;
}