#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void hijo(void) {
  printf("PID %ld: Proceso hijo ejecutándose...\n", (long)getpid());
  sleep(1);
  printf("PID %ld: Proceso hijo finalizando.\n", (long)getpid());
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Uso: %s numHijos\n", argv[0]);
    return EXIT_FAILURE;
  }

  unsigned int numHijos = atoi(argv[1]);
  pid_t pid;
  for (int i = 0; i < numHijos; i++) {
    pid = fork();
    if (pid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      hijo();
    }
  }
  for (int i = 0; i < numHijos; i++)
    wait(NULL);

  printf("Proceso padre: Todos los hijos han finalizado.\n");
  return EXIT_SUCCESS;
}
