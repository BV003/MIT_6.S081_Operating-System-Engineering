# Copy-on-Write

There is a saying in computer systems that any systems problem can be solved with a level of indirection.

To implement Copy-On-Write (COW) Fork in xv6, you need to modify the kernel to defer physical page allocation until a write actually occurs. When a process forks, parent and child will initially share all physical memory pages, marked as read-only. When either process attempts to write to a shared page, the CPU triggers a page fault, at which point the kernel copies the page, updates the page table to point to the new private copy with write permissions, and continues execution.

Laziness is Efficiency: Why copy $10\text{MB}$ of memory during fork() if the child process might call exec() or exit immediately without writing to it? COW saves time and RAM by waiting until a write is actually attempted.
Virtual Memory is an Illusion: Both processes think they have their own independent copies of memory, but behind the scenes, their virtual addresses point to the exact same physical RAM.
Hardware & Software Teamwork: 
- The OS marks the page "Read-Only" in the page tables.
- The Hardware (CPU/MMU) blocks any write attempt and alerts the OS with a "page fault."
- The OS intercepts the alert, copies the page, and upgrades the permissions on-the-fly.
Reference Counting: When resources are shared, you cannot just free memory when one process finishes. You must track how many owners remain and only reclaim the memory when the reference count drops to zero.