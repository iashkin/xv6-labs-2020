#include "kernel/types.h"
#include "user/user.h"

#define STDIN  0
#define STDOUT 1
#define STDERR 2

int
main(int argc, char *argv[])
{
  int pchild[2];
  int pparent[2];

  char c;

  if ((pipe(pchild) < 0) || (pipe(pparent) < 0)) {
    fprintf(STDERR, "ERROR: pipe\n");
    exit(1);
  }

  if (fork() == 0) {
    read(pchild[STDIN], &c, sizeof(c));
    printf("%d: received ping\n", getpid());
    write(pparent[STDOUT], &c, sizeof(c));
    exit(0);
  } else {
    c = 42;
    write(pchild[STDOUT], &c, sizeof(c));
    read(pparent[STDIN], &c, sizeof(c));
    printf("%d: received pong\n", getpid());
  }

  wait(0);

  exit(0);
}
