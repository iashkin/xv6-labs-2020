#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

static int find(const char *path, const char *filename)
{
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return 0;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  if (st.type != T_DIR) {
    fprintf(2, "find: %s isn't directory\n", path);
    close(fd);
    return 0;
  }

  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';
  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    switch(st.type) {
    case T_DIR:
      if ((!strcmp(de.name, ".")) || (!strcmp(de.name, "..")))
          continue;
      find(buf, filename);
      break;
    case T_FILE:
      if (!strcmp(de.name, filename))
        printf("%s\n", buf);
      break;
    }
  }

  close(fd);

  return 0;
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "Usage: %s <dir> <file>\n", argv[0]);
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}
