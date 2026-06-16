#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
solve(int fd)
{
  char buf[128];
  int len = 0;
  int is_number = 1;
  char c;

  while (read(fd, &c, 1) > 0) {
    if (strchr(" -\r\t\n./,", c) != 0) {
      if (len > 0) {
        if (is_number) {
          buf[len] = '\0';
          int val = atoi(buf);
          if (val % 5 == 0 || val % 6 == 0) {
            printf("%d\n", val);
          }
        }
      }
      len = 0;
      is_number = 1;
    } else {
      if (c >= '0' && c <= '9') {
        if (len < sizeof(buf) - 1) {
          buf[len++] = c;
        }
      } else {
        is_number = 0;
        if (len < sizeof(buf) - 1) {
          buf[len++] = c;
        }
      }
    }
  }

  if (len > 0) {
    if (is_number) {
      buf[len] = '\0';
      int val = atoi(buf);
      if (val % 5 == 0 || val % 6 == 0) {
        printf("%d\n", val);
      }
    }
  }
}

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: sixfive <file1> <file2> ...\n");
    exit(1);
  }

  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], 0);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      continue;
    }
    solve(fd);
    close(fd);
  }

  exit(0);
}
