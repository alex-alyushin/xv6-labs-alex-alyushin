#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char* MSG_PING = "ping";
char* MSG_PONG = "pong";

int main(int argc, char *argv[]) {
  int p[2];
  int q[2];

  pipe(p);
  pipe(q);

  int child_pid = fork();

  if (child_pid > 0) {

    // 1: Send ping to child: p[1]
    close(p[0]);
    write(p[1], MSG_PING, 4);
    close(p[1]);

    // 2: Receive pong from child: q[0]
    char buf[4];
    read(q[0], buf, 4);
    fprintf(2, "%d: received %s\n", getpid(), buf);

    close(q[0]);
    close(q[1]);
  }
  else if (child_pid == 0) {

    // 1: Receive ping from parent: p[0]
    char buf[4];
    read(p[0], buf, 4);
    fprintf(2, "%d: received %s\n", getpid(), buf);

    // 2: Send pong to parent: q[1]
    close(q[0]);
    write(q[1], MSG_PONG, 4);
    close(q[1]);

    close(p[0]);
    close(p[1]);
  }
  else {
    exit(1);
  }

  exit(0);
}
