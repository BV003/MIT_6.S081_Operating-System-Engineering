#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int has_exec = 0;
char *exec_args[64];
int exec_argc = 0;

void
run_exec(char *path)
{
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    exit(1);
  }
  if (pid == 0) {
    char *args[128];
    for (int i = 0; i < exec_argc; i++) {
      args[i] = exec_args[i];
    }
    args[exec_argc] = path;
    args[exec_argc + 1] = 0;
    exec(args[0], args);
    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  } else {
    wait(0);
  }
}

void
find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE: {
    char *name = path + strlen(path);
    while (name >= path && *name != '/') {
      name--;
    }
    name++;
    if (strcmp(name, target) == 0) {
      if (has_exec) {
        run_exec(path);
      } else {
        printf("%s\n", path);
      }
    }
    break;
  }

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      fprintf(2, "find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if(stat(buf, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", buf);
        continue;
      }

      if(strcmp(de.name, target) == 0){
        if (has_exec) {
          run_exec(buf);
        } else {
          printf("%s\n", buf);
        }
      }

      if(st.type == T_DIR){
        find(buf, target);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc >= 4 && strcmp(argv[3], "-exec") == 0) {
    has_exec = 1;
    for (int i = 4; i < argc; i++) {
      exec_args[exec_argc++] = argv[i];
    }
  } else if (argc != 3) {
    fprintf(2, "Usage: find <directory> <filename> [-exec <cmd> <args>...]\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}
