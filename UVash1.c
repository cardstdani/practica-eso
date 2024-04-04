#define _GNU_SOURCE
#define MAX_ARGUMENTS 1000
#define MAX_PARALLEL_COMMANDS 1000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

struct command {
  ssize_t num_argumentos;
  char **arg_array;
  char *path;
  char *error_path; //EXAMEN
};
enum errorType { ERROR, NO_ERROR };
const struct command nullCommand = {0, NULL, NULL, NULL};
const char error_message[30] = "An error has occurred\n";

void printError() {
  fprintf(stderr, "%s", error_message);
  fflush(stderr);
}

void cleanCommand(struct command *cmd) {  
  free(cmd->arg_array);
  cmd->num_argumentos = 0;
  cmd->path = NULL;
  cmd->error_path = NULL; //EXAMEN
}

void freeMemory(char **comandos_paralelos, struct command comandos[], size_t num_parallel_comands) {
  free(comandos_paralelos);
  for (size_t i = 0; i < num_parallel_comands; i++)
    cleanCommand(&comandos[i]);
}

struct command separarComando(char *buff) {
  struct command comando = (struct command){0, (char **)calloc(MAX_ARGUMENTS + 1, sizeof(char *)), NULL, NULL};
  char *ptr = buff;
  char *path_ptr = buff;
  while ((path_ptr = strsep(&buff, ">")) != NULL) { // Separar redireccion
    if (comando.num_argumentos > 1 || *path_ptr == '\0') {
      cleanCommand(&comando);
      return nullCommand;
    }
    if (comando.num_argumentos++ == 1)
      comando.path = path_ptr;
  }
  // Comprobar que el archivo de redireccion es unico, o hay 2
  if (comando.path != NULL) {
    enum errorType valid = ERROR;
    comando.num_argumentos = 0;
    path_ptr = buff = comando.path;
    while ((path_ptr = strsep(&buff, " \t\n")) != NULL) {
      if (*path_ptr == '\0')
        continue;
      if (comando.num_argumentos >= 2) { //EXAMEN
        cleanCommand(&comando);
        return nullCommand;
      } else {
        // ***************EXAMEN
        if(comando.num_argumentos== 0) {
          comando.path = path_ptr;
        } else {
          comando.error_path = path_ptr;
        }
        // ***************EXAMEN
        
        valid = NO_ERROR;
      }
      comando.num_argumentos++; //EXAMEN
    }
    // Comprobar que el path no este vacio
    if (valid == ERROR) {
      cleanCommand(&comando);
      return nullCommand;
    }
  }  

  // Separar argumentos
  comando.num_argumentos = 0;
  buff = ptr;
  while ((ptr = strsep(&buff, " \t\n")) != NULL) {
    if (*ptr == '\0')
      continue;
    if (comando.num_argumentos < MAX_ARGUMENTS)
      comando.arg_array[comando.num_argumentos++] = ptr;
    else {
      cleanCommand(&comando);
      return nullCommand;
    }
  }

  // Comprobar si el comando separado es valido y Resizear el array de argumentos
  if ((comando.arg_array = (char **)realloc(comando.arg_array, (comando.num_argumentos + 1) * sizeof(char *))) == NULL) {
    cleanCommand(&comando);
    return nullCommand;
  }
  if (comando.num_argumentos == 0)
    comando.num_argumentos = -1;
  return comando;
}

// separar entrada por caracter separador y limite
char **separarString(char *buff, char *delimiter, size_t *size, size_t limit) {
  char **comandos = (char **)calloc(limit, sizeof(char *));
  if (comandos == NULL) {
    free(comandos);
    return NULL;
  }
  char *ptr = buff;
  *size = 0; // Contador de comandos
  while ((ptr = strsep(&buff, delimiter)) != NULL) {
    if (*ptr == '\0')
      continue;
    if (*size < limit)
      comandos[(*size)++] = ptr;
    else {
      free(comandos);
      return NULL;
    }
  }
  // Resizear el array de comandos
  comandos = (char **)realloc(comandos, (*size) * sizeof(char *));
  if (comandos == NULL) {
    free(comandos);
    return NULL;
  }
  return comandos;
}

enum errorType ejecutarComando(struct command *cmd) {
  // Comprobar si el usuario quiere salir del shell
  if (strcmp(cmd->arg_array[0], "exit") == 0) {
    if (cmd->num_argumentos <= 1)
      exit(0);
    else
      return ERROR;
  }

  // Comprobar si el usuario pone un cd
  if (strcmp(cmd->arg_array[0], "cd") == 0)
    return (cmd->num_argumentos != 2 || chdir(cmd->arg_array[1]) != 0) ? ERROR : NO_ERROR;

  // ***************************** EXAMEN
  FILE *fp_out = NULL;
  FILE *fp_err = NULL;
  if (cmd->path != NULL)
    fp_out = fopen(cmd->path, "w");
  if (cmd->error_path != NULL)
    fp_err = fopen(cmd->error_path, "w");
  // ***************************** EXAMEN
  
  // Ejecutar comando
  pid_t pid = fork();
  if (pid == 0) {        
    // ***************************** EXAMEN      
    if(cmd->path!=NULL) {
      if(cmd->error_path!=NULL){
        if ((fp_out == NULL || fp_err == NULL || dup2(fileno(fp_out), 1) == -1 || dup2(fileno(fp_err), 2) == -1))
          return ERROR;
      }else {
        if ((fp_out == NULL || dup2(fileno(fp_out), 1) == -1 || dup2(fileno(fp_out), 2) == -1))
          return ERROR;
      }
    }
    // ***************************** EXAMEN
    
    if (execvp(cmd->arg_array[0], cmd->arg_array) == -1) {
      printError();
      exit(0);
    }
  } else if (pid < 0)
    return ERROR;
  return NO_ERROR;
}

enum errorType procesarEntrada(char *buff) {
  // Separar comandos paralelos
  size_t num_parallel_commands = 0;
  char **comandos_paralelos = separarString(buff, "&\n", &num_parallel_commands, MAX_PARALLEL_COMMANDS);
  if (comandos_paralelos == NULL)
    return ERROR;

  // Construir tensor de comandos
  struct command comandos[num_parallel_commands];
  for (size_t i = 0; i < num_parallel_commands; i++) {
    comandos[i] = separarComando(comandos_paralelos[i]);
    if (comandos[i].num_argumentos == 0) { // Error al separar comando
      freeMemory(comandos_paralelos, comandos, i + 1);
      return ERROR;
    }
  }

  // Ejecutar comandos
  for (size_t i = 0; i < num_parallel_commands; i++)
    if (comandos[i].num_argumentos != -1)
      if (ejecutarComando(&comandos[i]) == ERROR) {
        freeMemory(comandos_paralelos, comandos, num_parallel_commands);
        return ERROR;
      }
  for (size_t i = 0; i < num_parallel_commands; i++)
    wait(NULL);
  freeMemory(comandos_paralelos, comandos, num_parallel_commands);
  return NO_ERROR;
}

int main(int argc, char **args) {
  char *buff = NULL;
  size_t input_size = 0;
  if (argc == 2) { // Modo bash
    FILE *fp = fopen(args[1], "r");
    if (fp == NULL) {
      printError();
      exit(1);
    }

    while (getline(&buff, &input_size, fp) != -1) {
      if (buff[0] == '\n') // Continuar si no hay nada escrito
        continue;
      switch (procesarEntrada(buff)) {
      case ERROR:
        printError();
        break;
      case NO_ERROR:
        break;
      }
    }
    free(buff);
  } else if (argc == 1) { // Modo interactivo
    while (1) {
      fprintf(stdout, "UVash>");
      fflush(stdout);
      // Lectura de comando de entrada
      if (getline(&buff, &input_size, stdin) == -1) {
        printError();
        free(buff);
        continue;
      }
      if (buff[0] == '\n') { // Continuar si no hay nada escrito
        free(buff);
        continue;
      }

      switch (procesarEntrada(buff)) {
      case ERROR:
        printError();
        break;
      case NO_ERROR:
        break;
      }
    }
    free(buff);
  } else {
    printError();
    exit(1);
  }
  return 0;
}