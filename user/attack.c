#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
is_valid_secret(char *s)
{
  int len = strlen(s);
  if (len == 0)
    return 0;
  for (int i = 0; i < len; i++) {
    char c = s[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
      return 0;
    }
  }
  return 1;
}

int
main(int argc, char *argv[])
{
  int num_pages = 50;
  int size = num_pages * 4096;
  char *mem = sbrk(size);
  
  if (mem == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  char *target = "This may help.";
  int target_len = strlen(target);

  for (int i = 0; i < size - 32; i++) {
    if (memcmp(mem + i, target, target_len) == 0) {
      char *secret = mem + i + 16;
      if (is_valid_secret(secret)) {
        printf("%s\n", secret);
        exit(0);
      }
    }
  }

  exit(1);
}
