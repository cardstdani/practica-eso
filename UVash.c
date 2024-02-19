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
  size_t num_argumentos;
  char **arg_array;
  char *path;
};

char error_message[30] = "An error has occurred\n";

void printError() {
  fprintf(stderr, "%s", error_message);
  fflush(stderr);
}

struct command separarComando(char *buff) {
  struct command comando = (struct command){0, (char **)calloc(MAX_ARGUMENTS, sizeof(char *)), NULL};
  char *ptr = buff;
  char *path_ptr = buff;
  while ((path_ptr = strsep(&buff, ">\n")) != NULL) { // Separar redireccion
    if (*path_ptr == '\0')
      continue;
    if (comando.num_argumentos++ == 1)
      comando.path = path_ptr;
    else if (comando.num_argumentos > 1)
      printError();
  }
  comando.num_argumentos = 0;
  buff = ptr;
  while ((ptr = strsep(&buff, " \t\n")) != NULL) { // Separar argumentos
    if (*ptr == '\0')
      continue;
    if (comando.num_argumentos < MAX_ARGUMENTS)
      comando.arg_array[comando.num_argumentos++] = ptr;
    else
      printError();
  }
  // Resizear el array de argumentos
  comando.arg_array = (char **)realloc(comando.arg_array, (comando.num_argumentos) * sizeof(char *));
  if (comando.arg_array == NULL) {
    printError();
    exit(1);
  }
  return comando;
}

char **separarString(char *buff, char *delimiter, size_t *size, size_t limit) {
  // separar entrada por caracter separador y limite
  char **comandos = (char **)calloc(limit, sizeof(char *));
  char *ptr = buff;
  size_t num_comandos = 0;
  while ((ptr = strsep(&buff, delimiter)) != NULL) {
    if (*ptr == '\0')
      continue;
    if (num_comandos < limit)
      comandos[num_comandos++] = ptr;
    else
      printError();
  }
  // Resizear el array de comandos
  comandos = (char **)realloc(comandos, num_comandos * sizeof(char *));
  if (comandos == NULL) {
    printError();
    exit(1);
  }
  *size = num_comandos;
  return comandos;
}

void printCommand(struct command *cmd) {
  printf("Number of arguments: %zu\nPath:%s\n", cmd->num_argumentos, cmd->path);
  for (size_t i = 0; i < cmd->num_argumentos; i++)
    printf("[%zu]: %s\n", i, cmd->arg_array[i]);
}

int main(int argc, char **args) {

  while (1) {
    fprintf(stdout, "UVash> ");
    // Lectura de comando de entrada
    char *buff;
    size_t input_size = 0;
    if (getline(&buff, &input_size, stdin) == -1) {
      printError();
      continue;
    }
    if (buff[0] == '\n') // Continuar si no hay nada escrito
      continue;

    // Separar comandos paralelos
    size_t num_parallel_commands = 0;
    char **comandos_paralelos = separarString(buff, "&\n", &num_parallel_commands, MAX_PARALLEL_COMMANDS);
    struct command *comandos[num_parallel_commands];

    // Construir tensor de comandos
    for (size_t i = 0; i < num_parallel_commands; i++) {
      size_t num_comandos_internos = 0;
      char **comandos_internos = separarString(comandos_paralelos[i], ";\n", &num_comandos_internos, MAX_COMMANDS);
      comandos[i] = (struct command *)calloc(num_comandos_internos + 1, sizeof(struct command));
      for (size_t j = 0; j < num_comandos_internos; j++)
        comandos[i][j] = separarComando(comandos_internos[j]);
      free(comandos_internos);
    }
    free(comandos_paralelos);
    // DEBUG
    for (size_t i = 0; i < num_parallel_commands; i++) {
      printf("Commando paralelo %zu:\n", i + 1);
      for (struct command *cmd = comandos[i]; cmd->num_argumentos != 0;)
        printCommand(cmd++);
    }
    free(buff);

    // Ejecutar comando/s

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
    */
  }
  return 0;
}