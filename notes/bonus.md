# Bonus

## basic
The 5 Core Jobs of an OS
1. Managing the CPU (Process Management)
The CPU is the computer's brain. If you have 20 apps open, they all want to use the brain at the same time. The OS acts like a traffic cop—it switches the CPU between different tasks so quickly that it feels like everything is running at the same time.
2. Managing Memory (RAM)
When you open an app, it needs space to work. The OS finds a free spot in the RAM and lends it to that app. When you close the app, the OS takes that memory back so other apps can use it.
3. Protection (Virtual Memory)
To keep your computer stable, the OS keeps apps isolated from each other. 
For example: If your web browser crashes, it won't crash your whole computer because the OS keeps its memory completely separate from the memory of other programs.
4. The File System (Storage)
Your hard drive is just a massive list of 1s and 0s. The OS translates this mess into a neat, human-friendly system of folders and files so you can easily organize your photos, documents, and programs.
5. Talking to the Outside World (Input/Output)
When you plug in a mouse, click a button, or print a document, the OS handles the complex electronic signals. It translates your physical actions into actions the software understands, and vice-versa.

## xv6 project structrue

```
xv6-labs-2025/               <-- The root folder of the project
├── Makefile                 <-- The build instructions (how to compile the OS)
│
├── kernel/                  <-- THE KERNEL (Runs in Supervisor Mode)
│   ├── main.c               <-- The starting point (boots the CPU, sets up devices)
│   ├── kalloc.c             <-- Physical Memory (allocates/frees 4096-byte RAM pages)
│   ├── vm.c                 <-- Virtual Memory (sets up page tables for isolation)
│   ├── proc.c               <-- Process Manager (creates, schedules, and runs processes)
│   ├── syscall.c            <-- System Call Dispatcher (reads system call numbers)
│   ├── sysproc.c            <-- System Calls for Processes (like fork, exit, sleep)
│   ├── sysfile.c            <-- System Calls for Files (like open, read, write)
│   ├── trap.c               <-- Trap Handler (manages interrupts, faults, and syscalls)
│   ├── trampoline.S         <-- Assembly transition code between User and Kernel modes
│   ├── fs.c                 <-- File System (manages files, directories, and inodes)
│   ├── bio.c                <-- Buffer Cache (caches disk blocks in RAM)
│   ├── spinlock.c           <-- Locks (stops CPUs from corrupting shared data)
│   └── uart.c / console.c   <-- Drivers (interacts with the screen and keyboard)
│
├── user/                    <-- USER SPACE (Programs running in User Mode)
│   ├── sh.c                 <-- The Shell (the command prompt where you type)
│   ├── ls.c                 <-- Lists files in a directory
│   ├── cat.c                <-- Prints file contents to the screen
│   ├── grep.c               <-- Searches for patterns in files
│   ├── sleep.c              <-- Pauses execution for a short time
│   └── user.h               <-- Header file containing system call declarations
│
└── mkfs/
    └── mkfs.c               <-- A tool that builds the file system disk image (fs.img)
```

- Booting up (kernel/main.c & kernel/entry.S): When the simulated computer turns on, assembly code in entry.S prepares the CPU, and jumps to main.c to initialize memory, processes, disks, and interrupts.
- The Bridge (kernel/trampoline.S & kernel/trap.c): When a user program in user/ needs the kernel's help (e.g., calling sleep), it triggers a trap. The assembly in trampoline.S switches the CPU to Kernel mode safely, and trap.c decides how to handle the request.
- System Call Enforcers (kernel/syscall.c): Once a trap occurs, the kernel looks at the system call number, checks the arguments, and runs the corresponding function in sysproc.c or sysfile.c.

## Classes

```
                  +-----------------------------------+
                  |            struct cpu             |
                  |  - Represents a physical CPU core |
                  |  - Pointer to current running proc|
                  +-----------------+-----------------+
                                    |
                                    | manages
                                    v
+-----------------------------------------------------------------------+
|                              struct proc                              |
|  - Represents a running program (process)                             |
|  - Has a State: RUNNING, RUNNABLE, SLEEPING, ZOMBIE, etc.             |
|  - PID (Process ID) & Parent process pointer                          |
|                                                                       |
|  +--------------------+  +--------------------+  +------------------+ |
|  |  struct trapframe  |  |   struct context   |  |   pagetable_t    | |
|  | - Saves CPU state  |  | - Saves registers  |  | - Virtual memory | |
|  |   during Syscalls  |  |   for switching    |  |   mappings       | |
|  |   or Interrupts    |  |   between procs    |  |                  | |
|  +--------------------+  +--------------------+  +------------------+ |
|                                                                       |
|  +--------------------+                                               |
|  |   struct file*     | <--- Index matches file descriptor (FD) number|
|  |   ofile[NOFILE]    |      e.g., 0 = stdin, 1 = stdout, 2 = stderr  |
|  +---------+----------+                                               |
+------------|----------------------------------------------------------+
             |
             | points to
             v
+--------------------------------------------+
|                struct file                 |
|  - Keeps track of an open file             |
|  - Knows if it is readable/writable        |
|  - File offset (where to read/write next)  |
|  - Type: PIPE, INODE (regular file), DEVICE|
+------+-------------------+-----------------+
       |                   |                 |
       | If type == PIPE   | If type == INODE| If type == DEVICE
       v                   v                 v
+--------------+   +---------------+   +---------------+
| struct pipe  |   | struct inode  |   | struct devsw  |
| - Temp RAM   |   | - File on disk|   | - Hardware    |
|   buffer for |   | - Size, type  |   |   read/write  |
|   piping data|   | - Block addrs |   |   handlers    |
+--------------+   +-------+-------+   +---------------+
                           |
                           | cached in
                           v
                   +---------------+
                   |  struct buf   |
                   | - RAM cache of|
                   |   disk blocks |
                   +---------------+
```