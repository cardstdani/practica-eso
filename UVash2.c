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
};
enum errorType { ERROR, NO_ERROR };
enum redireccion {NOTHING, LEFT, RIGHT};
const struct command nullCommand = {0, NULL, NULL};
const char error_message[30] = "An error has occurred\n";

void printError() {
  fprintf(stderr, "%s", error_message);
  fflush(stderr);
}

void cleanCommand(struct command *cmd) {
  free(cmd->arg_array);
  cmd->num_argumentos = 0;
  cmd->path = NULL;
}

void freeMemory(char **comandos_paralelos, struct command comandos[], size_t num_parallel_comands) {
  free(comandos_paralelos);
  for (size_t i = 0; i < num_parallel_comands; i++)
    cleanCommand(&comandos[i]);
}

struct command separarComando(char *buff) {
  struct command comando = (struct command){0, (char **)calloc(MAX_ARGUMENTS + 1, sizeof(char *)), NULL};
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
  // Comprobar que el archivo de redireccion es unico
  if (comando.path != NULL) {
    enum errorType valid = ERROR;
    comando.num_argumentos = 0;
    path_ptr = buff = comando.path;
    while ((path_ptr = strsep(&buff, " \t\n")) != NULL) {
      if (*path_ptr == '\0')
        continue;
      if (comando.num_argumentos++ >= 1) {
        cleanCommand(&comando);
        return nullCommand;
      } else {
        comando.path = path_ptr;
        
        valid = NO_ERROR;
      }
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

enum errorType ejecutarComando(struct command *cmd, enum redireccion r) {
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

  FILE *fp = NULL;    

  // Ejecutar comando
  pid_t pid = fork();
  if (pid == 0) {
    // ***************************** EXAMEN
    if(r==LEFT){
      //En realidad tendria que poner un nombre lo mas unico posible, por si acaso
      if ((fp = fopen("fich", "w")) == NULL)
        return ERROR;      
      if(dup2(fileno(fp), 1) == -1)
        return ERROR;
    } else if(r==RIGHT) {
      if ((fp = fopen("fich", "r")) == NULL)
        return ERROR;      
      if(dup2(fileno(fp), 0) == -1)
        return ERROR;
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
  // ******************** EXAMEN
  // Separar tuberias, aprovecho las variables que se llaman comandos paralelos, aunque realmente sean tuberias
  size_t num_parallel_commands = 0;
  char **comandos_paralelos = separarString(buff, "|\n", &num_parallel_commands, MAX_PARALLEL_COMMANDS);
  if (comandos_paralelos == NULL || num_parallel_commands > 2) //examen
    return ERROR;
  // ******************** EXAMEN

  // Construir tensor de comandos
  struct command comandos[num_parallel_commands];
  for (size_t i = 0; i < num_parallel_commands; i++) {
    comandos[i] = separarComando(comandos_paralelos[i]);
    if (comandos[i].num_argumentos == 0) { // Error al separar comando
      freeMemory(comandos_paralelos, comandos, i + 1);
      return ERROR;
    }
  }

  // ******************** EXAMEN
  if(num_parallel_commands == 1) {
    if (ejecutarComando(&comandos[0], NOTHING) == ERROR) {
      freeMemory(comandos_paralelos, comandos, num_parallel_commands);
      return ERROR;
    }
  } else {            
    if (ejecutarComando(&comandos[0], LEFT) == ERROR) {
      freeMemory(comandos_paralelos, comandos, num_parallel_commands);      
      return ERROR;
    }
    wait(NULL);
    
    if (ejecutarComando(&comandos[1], RIGHT) == ERROR) {
      freeMemory(comandos_paralelos, comandos, num_parallel_commands);
      return ERROR;
    }
  }
  wait(NULL);
  // ******************** EXAMEN
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