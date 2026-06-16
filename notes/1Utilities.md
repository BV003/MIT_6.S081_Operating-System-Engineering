# Utilities

## sleep

User Mode (User-level)  
#### Step 1: Create and write user/sleep.c               
#### Step 2: Add sleep to UPROGS in the Makefile                 
#### Step 3: Compile and Test Manually    
#### Step 4: Run the Automated Grading Test                   

#### test
== Test sleep, no arguments == sleep, no arguments: OK (0.9s) 
== Test sleep, returns == sleep, returns: OK (0.9s) 
== Test sleep, makes syscall == sleep, makes syscall: OK (1.0s) 

## sixfive

### Learn
#### 1. 系统调用：open() 和 read()

- open(path, flags)：打开一个文件。在用户态，我们使用 open(argv[i], 0) 来以只读方式打开文件（0 代表只读，定义在 kernel/fcntl.h 中）。
- read(fd, buf, n)：从文件描述符 fd 中读取最多 n 个字节。

#### 2. 字符串匹配：strchr()   
- 在 user/user.h 中声明了 char* strchr(const char *s, int c)。
- 分隔符集合：" -\r\t\n./," 

#### 3. 解析状态机的设计

我们需要对读入的每个字符建立状态逻辑。对于非分隔符，我们将它们拼成一个“单词 (token)”：                                                           
- 如果这个单词里的所有字符都是 0-9 的数字，那它就是一个合法的数字。                                                                              
- 如果单词里混入了非数字字符（比如字母 x 或 v，如 xv6），那这个单词就不是合法数字。                                                              
- 当遇到分隔符或文件结束（EOF）时，代表一个单词结束。此时：                                                                                      
- 如果它是合法数字，用 atoi() 转换成数。                                                                                                   
- 判断 val % 5 == 0 || val % 6 == 0，如果是，用 printf("%d\n", val) 输出。

### test
michael@MacBook-Pro xv6-labs-2025 % ./grade-lab-util sixfive    
make: `kernel/kernel' is up to date.
== Test sixfive_test == sixfive_test: OK (0.8s) 
== Test sixfive_readme == sixfive_readme: OK (1.4s) 
== Test sixfive_all == sixfive_all: OK (1.0s) 

## memdump
A pointer is just a memory address. In C, the type of the pointer tells the compiler how many bytes to read from that address
In xv6 (running on RISC-V), multi-byte numbers are stored in "little-endian" order. 

## find

Directories are just special files  
Recursion (Functions calling themselves) 
The Infinite Loop Trap (. and ..)      
C String Comparison 