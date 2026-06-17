# traps

## RISC-V assembly

### RISC-V assembly

Compile the user programs and generate the assembly files:
make fs.img

Question 1: Which registers contain arguments to functions? For example, which register holds 13 in main's call to printf?
- Answer: In the RISC-V calling convention, function arguments are passed in registers a0 through a7 (which map to physical registers x10 through x17).
- Explanation: In the call to printf in main:
- a0 holds the address of the format string "%d %d\n".
- a1 holds the first formatted value (which is 12).
- a2 holds the second formatted value (which is 13).
Question 2: Where is the call to function f in the assembly code for main? Where is the call to g?
- Answer: There are no calls to f or g in the assembly code for main.
- Explanation: The compiler optimizations completely inlined the functions. Since f(8) + 1 resolves to g(8) + 1 = 8 + 3 + 1 = 12, which is a constant, the compiler pre-calculated this value at compile-time and directly loaded 12 into register a1 (usually using li a1, 12).
Question 3: At what address is the function printf located?
- Answer: Look in your user/call.asm file for the label <printf>: or look at the instruction in main that jumps to printf.
- How to find it: It is usually at address 0x630 (or a similar address like 0x30, depending on the exact build). Locate the line:
jal ra, <address> <printf>
The hexadecimal value <address> is the location of printf.
Question 4: What value is in the register ra just after the jalr to printf in main?
- Answer: ra (the Return Address register) will contain the address of the instruction immediately following the jump to printf.
- How to find it: If your jump instruction jal is at address 0x30 and is 4 bytes long, the instruction right below it will be at 0x34. After the jump executes, ra will be set to 0x34 so printf knows where to return when it finishes.
Question 5: Endianness Test
unsigned int i = 0x00646c72;
printf("H%x Wo%s", 57616, (char *) &i);
- What is the output?
- He110 World
- Why?
1. 57616 in hexadecimal is 0xe110. Thus, H%x prints He110.
2. RISC-V is little-endian, meaning the least significant byte is stored at the lowest memory address. The integer 0x00646c72 is stored in memory byte-by-byte as:
- Byte 0: 0x72 (ASCII character 'r')
- Byte 1: 0x6c (ASCII character 'l')
- Byte 2: 0x64 (ASCII character 'd')
- Byte 3: 0x00 (null terminator '\0')
3. When (char *)&i is printed with %s, it reads those bytes sequentially until the null terminator, printing "rld".
- If RISC-V were Big-Endian instead, what would you set i to?
- 0x726c6400
- In a big-endian system, the most significant byte is stored at the lowest address, so we reverse the byte order.
- Would you need to change 57616?
- No. 57616 is passed as a numerical value in a register. Register values are independent of memory endianness.
Question 6: Missing Arguments
printf("x=%d y=%d", 3);
- What is printed after y=?
- A garbage value (whatever leftover value happens to be in register a2 at that moment).
- Why does this happen?
- printf is a variadic function. It doesn't know how many arguments were passed to it; it simply parses the format string. When it sees the second %d, it attempts to read the next argument register, which is a2. Since we only passed 3 (placed in a1), register a2 was never set by our code, so printf prints whatever arbitrary value was left in a2 by previous, unrelated instructions.
