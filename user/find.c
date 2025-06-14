#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void traverse_tree(char* path, char* name) {
  int fd;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if ((fstat(fd, &st)) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    exit(1);
  }

  char *p;
  char buf[512];
  strcpy(buf, path);
  p = buf + strlen(buf);
  *(p++) = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0) {
      continue;
    }

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(buf, &st) < 0) {
      continue;
    }

    switch (st.type) {
      case T_DEVICE:
      case T_FILE:

        // result
        if (strcmp(name, de.name) == 0) {
          fprintf(2, "%s\n", buf);
        }

        break;

      case T_DIR:
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
          continue;
        }

        traverse_tree(buf, name);

        break;
    }
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(2, "Usage: find dir name\n");
    exit(1);
  }

  char* path = argv[1];
  char* name = argv[2];
  traverse_tree(path, name);

  exit(0);
}
