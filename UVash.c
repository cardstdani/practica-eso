#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_ARGUMENTS 1000
#define MAX_PARALLEL_COMMANDS 1000
#define MAX_COMMANDS 1000

struct command {
  char **arg_array;
  size_t num_argumentos;
};

char error_message[30] = "An error has occurred\n";

void printError() {
  fprintf(stderr, "%s", error_message);
  fflush(stderr);
}

struct command separarComando(char *buff) {
  struct command comando =
      (struct command){(char **)calloc(MAX_ARGUMENTS + 1, sizeof(char *)), 0};
  char *ptr = buff;
  while ((ptr = strsep(&buff, " \t\n")) != NULL) {
    if (*ptr == '\0')
      continue;
    if (comando.num_argumentos < MAX_ARGUMENTS)
      comando.arg_array[comando.num_argumentos++] = ptr;
    else
      printError();
  }
  // Resizear el array de cosas
  comando.arg_array = (char **)realloc(
      comando.arg_array, (comando.num_argumentos + 1) * sizeof(char *));
  if (comando.arg_array == NULL) {
    printError();
    exit(EXIT_FAILURE);
  }
  return comando;
}

char **separarString(char *buff, char delimiter, size_t limit) {
  char delim[2] = {delimiter, '\0'};

  // separar entrada por caracter separador y limite
  char **comandos = (char **)calloc(limit + 1, sizeof(char *));
  char *ptr = buff;
  size_t num_comandos = 0;
  while ((ptr = strsep(&buff, delim)) != NULL) {
    if (*ptr == '\0')
      continue;
    if (num_comandos < limit)
      comandos[num_comandos++] = ptr;
    else
      printError();
  }
  // Resizear el array de comandos
  comandos = (char **)realloc(comandos, (num_comandos + 1) * sizeof(char *));
  if (comandos == NULL) {
    printError();
    exit(EXIT_FAILURE);
  }
  return comandos;
}

void printCommand(struct command *cmd) {
  printf("Number of arguments: %zu\n", cmd->num_argumentos);
  printf("Arguments:\n");
  for (size_t i = 0; i < cmd->num_argumentos; ++i) {
    printf("[%zu]: %s\n", i, cmd->arg_array[i]);
  }
  printf("\n");
}

int main(int argc, char **args) {
  if (argc <= 1) {
  }

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

    // Separar comandos paralelos
    struct command **comandos = (struct command **)calloc(
        MAX_PARALLEL_COMMANDS, sizeof(struct command *));
    char **comandos_paralelos = separarString(buff, '&', MAX_PARALLEL_COMMANDS);
    size_t num_parallel_commands = 0;
    while (comandos_paralelos[num_parallel_commands] != NULL) {
      // Separar comandos internos por ;
      struct command *command_array =
          (struct command *)calloc(MAX_COMMANDS, sizeof(struct command));
      char *ptr = comandos_paralelos[num_parallel_commands];
      char *comando;
      size_t num_commands = 0;
      while ((comando = strsep(&ptr, ";")) != NULL) {
        if (*comando == '\0')
          continue;
        command_array[num_commands++] = separarComando(comando);
      }
      comandos[num_parallel_commands++] = command_array;
    }
    comandos = (struct command **)realloc(
        comandos, (num_parallel_commands + 1) * sizeof(struct command *));
    free(comandos_paralelos);
    for (size_t i = 0; i < num_parallel_commands; ++i) {
      printf("Command set %zu:\n", i + 1);
      struct command *cmds = comandos[i];
      size_t j = 0;
      while (cmds[j].num_argumentos != 0) {
        printCommand(&cmds[j++]);
      }
    }

    // Comprobar si el usuario quiere salir del shell
    /*if (strcmp(arg_array[0], "exit") == 0) {
      if (num_argumentos <= 1) {
        exit(0);
      } else {
        printError();
        continue;
      }
    }

    // Comprobar si el usuario pone un cd
    if (strcmp(arg_array[0], "cd") == 0) {
      if (num_argumentos != 2 || chdir(arg_array[1]) != 0) {
        printError();
        continue;
      }
    }

    for (int i = 0; i < num_argumentos; i++) {
      printf("%s\n", arg_array[i]);
      fflush(stdout);
    }
    // Ejecutar comando/s

    // Liberar memoria de argumentos
    free(arg_array);*/
  }
  return 0;
}