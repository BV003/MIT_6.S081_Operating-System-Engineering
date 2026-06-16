# System Calls

## Use GDB to get all anwsers

The RISC-V CPU has different Privilege Modes (security levels) to keep the system safe:
User Mode (U-mode): Where normal programs (like init, sh, sleep) run. They have low privileges.   
Supervisor Mode (S-mode): Where the operating system kernel runs. It has high privileges.
In operating systems like xv6, Assembly (.S or .asm) files bridge the gap between the bare hardware (CPU) and high-level C code.

CPU Bootstrap (Booting up)
Accessing Privileged Registers
Saving/Restoring CPU State (Context Switching)

asm just for debugging

### Write down the assembly instruction the kernel is panicing at. Which register corresponds to the variable num?
```
80001d08:	00002683          	lw	a3,0(zero) # 0 <_entry-0x80000000>
 a3 
```



### What is the name of the process that was running when the kernel paniced? What is its process id (pid)?
```
(gdb) p p->name
$1 = "init", '\000' <repeats 11 times>
(gdb) p p->pid     
$2 = 1
```

## Sandbox a command
build a system call called interpose.
### 1. How System Calls Work
### 2. Process State and the proc Structure
### 3. Process Inheritance (fork) 
### 4. Basic Security (Sandboxing)

## Sandbox with allowed pathnames

### Step 1: Adding the allowed_path field to the process checklist (proc.h) 
### Step 2: Clearing and copying the allowed path on process creation (proc.c) 
### Step 3: Saving the path argument inside sys_interpose (sysproc.c)
### Step 4: Enforcing the Pathname check inside syscall() (syscall.c) 

## Attack xv6
To get a sense for how bugs can be exploited, we have introduced a bug into xv6 and your goal is to exploit that bug to steal a secret from another process.

虚拟内存与物理内存的映射（Virtual & Physical Memory）
- 每个进程都有自己的虚拟地址空间，看起来像是在独占整个内存。                                                                                 
- 但实际上，虚拟地址是通过页表（Page Table）映射到物理内存（Physical Pages）上的。                                                           
- 不同的进程可能会在不同的时间，共享并复用相同的物理内存页。 