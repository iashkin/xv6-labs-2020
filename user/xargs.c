#include <kernel/param.h>
#include <kernel/types.h>
#include <user/user.h>

char buf[512];
char *args[MAXARG];

static void
call_arg()
{
  int pid = fork();

  if (pid < 0) {
    printf("xargs: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    exec(args[0], args);
    printf("xargs: exec %s failed\n", "sh");
    exit(1);
  } else {
    wait(0);
  }
}

int
main(int argc, char *argv[])
{
  int argn, bytes, start;

  if (argc < 2) {
    fprintf(2, "Usage: xargs command [arguments]\n");
    exit(1);
  }

  /* copy all arguments for given command */
  for (argn = 1; argn != argc; ++argn)
    args[argn - 1] = argv[argn];

  while((bytes = read(0, &buf, sizeof(buf))) != 0) {
    start = 0;
    for (int i = 0; i < bytes; ++i) {
      if (buf[i] == '\n') {
        start = i + 1;
        buf[i] = 0;

        args[argn - 1] = buf + start;
        args[argn] = 0;

        call_arg();
      }
    }
  }

  exit(0);
}
