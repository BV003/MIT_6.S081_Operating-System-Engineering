# Page Tables

## Inspect a user-process page table
result
```
print_pgtbl starting
va 0x0 pte 0x21FC885B pa 0x87F22000 perm 0x5B
va 0x1000 pte 0x21FC7C5B pa 0x87F1F000 perm 0x5B
va 0x2000 pte 0x21FC7817 pa 0x87F1E000 perm 0x17
va 0x3000 pte 0x21FC7407 pa 0x87F1D000 perm 0x7
va 0x4000 pte 0x21FC70D7 pa 0x87F1C000 perm 0xD7
va 0x5000 pte 0x0 pa 0x0 perm 0x0
va 0x6000 pte 0x0 pa 0x0 perm 0x0
va 0x7000 pte 0x0 pa 0x0 perm 0x0
va 0x8000 pte 0x0 pa 0x0 perm 0x0
va 0x9000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF6000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF7000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF8000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF9000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFA000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFB000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFC000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFD000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFE000 pte 0x21FD08C7 pa 0x87F42000 perm 0xC7
va 0x3FFFFFF000 pte 0x2000184B pa 0x80006000 perm 0x4B
print_pgtbl: OK
ugetpid_test starting
usertrap(): unexpected scause 0xd pid=3
            sepc=0x7d0 stval=0x3fffffd000
```
we use perm to show different page permission.

1. va 0x0 & va 0x1000
- PTE Contents: 0x21FC885B & 0x21FC7C5B
- Physical Addresses: 0x87F22000 & 0x87F1F000
- Permission Bits: 0x5B (PTE_A | PTE_U | PTE_X | PTE_R | PTE_V)
- Logical Content: User Text & Read-Only Data Segments
- Explanation: These pages hold the compiled instructions (code) and constants of the user program. The permissions are set to Valid, Readable, Executable, and User-accessible, allowing the CPU to execute user-mode instructions from these addresses. The Accessed (PTE_A) bit is set because the CPU has fetched and executed code from these pages.
2. va 0x2000
- PTE Contents: 0x21FC7817
- Physical Address: 0x87F1E000
- Permission Bits: 0x17 (PTE_U | PTE_W | PTE_R | PTE_V)
- Logical Content: User Data Segment & Heap
- Explanation: This page stores global variables, static variables, and dynamically allocated heap space. It is Valid, Readable, Writable, and User-accessible, but not executable (since it contains data, not code).
3. va 0x3000
- PTE Contents: 0x21FC7407
- Physical Address: 0x87F1D000
- Permission Bits: 0x07 (PTE_W | PTE_R | PTE_V)
- Logical Content: User Stack Guard Page
- Explanation: This is a guard page placed directly below the user stack. It is not user-accessible (the PTE_U bit is 0). If the user stack grows too large and overflows downward, any access to va 0x3000 triggers a page fault because user-mode code is forbidden from accessing pages without PTE_U.
4. va 0x4000
- PTE Contents: 0x21FC70D7
- Physical Address: 0x87F1C000
- Permission Bits: 0xD7 (PTE_D | PTE_A | PTE_U | PTE_W | PTE_R | PTE_V)
- Logical Content: User Stack Page
- Explanation: This page is the active stack used for local variables and function calls. It is Valid, Readable, Writable, and User-accessible. Both Accessed (PTE_A) and Dirty (PTE_D) are set because the process has read from and written to the stack.
5. va 0x5000 through va 0x9000 (and up to va 0x3FFFFFC000)
- PTE Contents: 0x0
- Physical Address: 0x0
- Permission Bits: 0x0
- Logical Content: Unmapped / Free Address Space
- Explanation: These virtual pages are unallocated and marked invalid. Any access to these addresses from user space will trigger a page fault.
6. va 0x3FFFFFD000
- PTE Contents: 0x0
- Physical Address: 0x0
- Permission Bits: 0x0
- Logical Content: USYSCALL (Unmapped / Missing)
- Explanation: This page resides at TRAPFRAME - PGSIZE (which is 0x3FFFFFD000). It is currently unmapped (pte 0x0), which is why your ugetpid_test crashed with a page fault (scause 0xd). In subsequent parts of the lab, this page should be mapped with PTE_U | PTE_R to allow user programs to directly read the process PID without entering the kernel via a system call.
7. va 0x3FFFFFE000
- PTE Contents: 0x21FD08C7
- Physical Address: 0x87F42000
- Permission Bits: 0xC7 (PTE_D | PTE_A | PTE_W | PTE_R | PTE_V)
- Logical Content: Kernel Trapframe Page
- Explanation: This page contains the process's trapframe, which saves the user registers when a system call or interrupt occurs. It is Valid, Readable, and Writable (with Accessed and Dirty set). The User (PTE_U) bit is 0 to prevent user processes from tampering with saved register values and compromising kernel security.
8. va 0x3FFFFFF000
- PTE Contents: 0x2000184B
- Physical Address: 0x80006000
- Permission Bits: 0x4B (PTE_A | PTE_X | PTE_R | PTE_V)
- Logical Content: Kernel Trampoline Page
- Explanation: This is the highest virtual page in the address space. It maps the entry and exit code for system traps (trampoline). It is Valid, Readable, and Executable, but the User (PTE_U) bit is 0 so the user program cannot directly read or execute code on this page. Execution transitions here only when hardware traps into supervisor mode.

## Speed up system calls
Normally, when a user program runs getpid(), it has to:
1. Trigger a software interrupt (trap).
2. Save user registers.
3. Switch from User Mode to Kernel Mode.
4. Execute kernel code to lookup the PID.
5. Restore registers and switch back to User Mode.
This transition (called a kernel crossing) is slow. 

The Optimization: 
During process creation, the kernel allocates a physical page of memory, writes the process's ID (pid) into it, and maps it in the process's page table at a specific virtual address (USYSCALL). 
We configure the permission bits to be User-Accessible and Read-Only (PTE_U | PTE_R). This allows user-mode code to read the PID directly from that memory location using a regular pointer read—bypassing the kernel crossing completely

Some xv6 system calls could be made faster
1. uptime():
- How: The kernel updates a global tick counter in an interrupt handler. If this counter is placed in a shared read-only page, user programs can read the current uptime ticks directly without making a system call.
2. gettimeofday() / time():
- How: In production OSs (like Linux via vDSO), the current real-world time is written to a shared memory page by the kernel. User space can read the time instantly.
3. sysinfo() / statistics:
- How: System-wide metrics like the total number of free physical memory pages or the number of running processes can be updated periodically by the kernel and read instantly by user-space performance tools.

## Print a page table
Macros: 一般指把一串操作/指令打包成一个按钮或快捷键自动执行的功能
PTE: 页表中的一个项
多级页表
PA&VA
skip the hard superpages