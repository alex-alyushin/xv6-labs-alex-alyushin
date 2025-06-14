#include <stdbool.h>
#include <stdint.h>

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void process(int read_fd) {
  int32_t prime = 0;
  if (read(read_fd, &prime, sizeof(int32_t)) == 0) {
    return;
  }

  fprintf(2, "prime %d\n", prime);

  int32_t counter = 0;
  int32_t *buffer = malloc(300 * sizeof(int32_t));

  if (buffer == 0) {
      fprintf(2, "memory allocation failed\n");
      exit(1);
  }

  for (int i = 0;; i += 1) {
    if (read(read_fd, &buffer[i], sizeof(int32_t)) == 0) {
      break;
    }

    counter += 1;
  }

  close(read_fd);

  int p[2];
  if (pipe(p) == -1) {
    fprintf(2, "pipe creation failed\n");
    exit(1);
  }

  if (fork() > 0) {
    for (int i = 0; i < counter; i += 1) {
      if (buffer[i] % prime != 0) {
        write(p[1], &buffer[i], sizeof(int32_t));
      }
    }

    free(buffer);

    close(p[0]);
    close(p[1]);

    wait(0);
  } else {
    close(p[1]);
    process(p[0]);

    exit(0);
  }
}

int main(int argc, char *argv[]) {
  int p[2];
  pipe(p);

  if (fork() > 0) {
    for (int32_t num = 2; num <= 280; num += 1) {
      write(p[1], &num, sizeof(int32_t));
    }

    close(p[0]);
    close(p[1]);

    wait(0);
  } else {
    close(p[1]);
    process(p[0]);

    exit(0);
  }

  exit(0);
}
