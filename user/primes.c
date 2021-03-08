#include "kernel/types.h"
#include "user/user.h"

#define OUT 0
#define IN  1

#define START_N	2
#define END_N 10000

int
main(int argc, char *argv[])
{
  int p[2], pp[2];
  int child = 0;
  int n, pr;

  if (pipe(p) < 0) {
    fprintf(2, "Error: pipe\n");
    exit(1);
  }

  if (fork() != 0) {
    /* Main process with generator */
    close(p[OUT]);
    for (int i = START_N; i <= END_N; ++i) {
      if (write(p[IN], &i, sizeof(i)) <= 0) {
        fprintf(2, "Error: writing to pipe\n");
        exit(1);
      }
    }

    close(p[IN]);
    wait(0);
  } else {
    /* The second and next processes */
    close(p[IN]);
    pr = START_N;

    while(read(p[OUT], &n, sizeof(n))) {
      if (n == pr) {
        printf("prime %d\n", n);
        continue;
      }

      if (n % pr != 0) {
        if (!child) {
          if (pipe(pp) < 0) {
            fprintf(2, "Error: pipe\n");
            exit(1);
          }

          child = fork();
          if (!child) {
            close(p[OUT]);
            close(pp[IN]);

            p[OUT] = pp[OUT];
            pr = n;

            continue;
          }
        }
        write(pp[IN], &n, sizeof(n));
      }
    }

    close(p[OUT]);
    close(pp[IN]);
    wait(0);
  }

  exit(0);
}
