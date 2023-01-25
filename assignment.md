# Lab 3: ARM Binary Translator (110 pt)

In this homework, you will implement a basic [binary translator] to understand the basic principles of software virtualization and learn a code
instrumentation technique to instrument [inline monitors] on-the-fly.

The homework provides [skeleton codes] to implement a minimal [binary
translator] for the ARM architecture. In summary, your implementation will (1) dynamically decode instruction, (2) patch the branch (or control) instruction to switch into callout context, and (3) profile program execution at runtime.

Although the assignment is tested and verified for GCC compilers (version 8.3), the other environmental factors may affect compilers to generate different machine instruction sequences, resulting in runtime failures. Please feel free to consult the instructor or TA regarding such issues.

## Preliminaries

You will start the project by building and running the provided skeleton
code. Then, you will incrementally extend it in the course of completing each part. Two recursive functions with no [side effects] ([Fibonacci] and
[Factorial]), written in ARM assembly, are provided to test your binary
translator. Factorial functions come with three versions ([fact.S],
[fact2.S], and [fact3.S]) with varying optimization levels.

Both functions have the same function signatures that take a single integer argument and return an integer. Inside the main driver code ([lab3.c]), all functions are alias as `int user_prog(int)`. You will build different binary outputs to invoke different function implementations.

First, download the template code located using the following command.

```bash
kjee@kjee-rpi0:/home/kjee $ wget https://files.syssec.org/lab3.zip
```
Then run the following commands from your Raspberry Pi device to confirm the build and execution. 

```bash
kjee@kjee-rpi0:/home/kjee/repos/cs6332.001-f21-lab3 git:(master*) $ make all
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include -c -o fact.o fact.S
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include -D__FACT__  lab3.c arm_callout.S fact.o -static -Wl,-N -o lab3_fact
objdump -d lab3_fact > lab3_fact.dis
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include -c -o fact2.o fact2.S
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include  -D__FACT2__ lab3.c arm_callout.S fact2.o -static -Wl,-N -o lab3_fact2
objdump -d lab3_fact2 > lab3_fact2.dis
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include -c -o fact3.o fact3.S
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include  -D__FACT3__ lab3.c arm_callout.S fact3.o -static -Wl,-N -o lab3_fact3
objdump -d lab3_fact3 > lab3_fact3.dis
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include -c -o fib.o fib.S
cc -g -fno-stack-protector -no-pie -fno-pic -I ./include -D__FIB__  lab3.c arm_callout.S fib.o -static -Wl,-N -o lab3_fib
objdump -d lab3_fib > lab3_fib.dis
kjee@kjee-rpi0:/home/kjee/repos/cs6332.001-f21-lab3 git:(master*) $ ./lab3_fib
running fib()
input number: 20
10946
Time elapsed: 0.000436
kjee@kjee-rpi0:/home/kjee
```

### Submission guideline

This assignment will be due on *11:59 PM*, Nov 7. To submit the assignment, you will extend the provided skeleton codes to implement solution for each part.
Please do not forget to write `README.md` document to explain your code and solution.

```
<netid>-lab03/
├── Lab3-1/
├── Lab3-2/
├── Lab3-3/
├── Lab3-4/
├── Lab3-5/
└── README.md
```

### Academic honesty

The submission and code should be your own. Although strongly encourage
discussions among students and with instructor or TA, the class will not tolerate students' failure to comply with [Student code of conduct] or any unethical behaviors. The submitted code should be soley of your own. We will run [code] [similarity] [measurement] [tools] against all student submissions.

## Lab3-1: Patching ARM binary to return (10 pt)

The first step will get you warmed up and comfortable with code patching. Look at the bottom of `main()`. Just before main calls `user_prog()`, it calls `StartProfiling()` which is your hook. It allows you to inspect and/or modify the target function, `user_prog()` in this case before it starts executing. Your objective is to use `StartProfiling()` to binary patch `user_prog()` to return immediately.

Although this may sound pointless, this technique is instrumental in the real world. Often you have debugging code that needs to be removed after some time. On-the-fly binary patching allows you to remove functionality without recompiling your code. If `user_prog()` was some debugging code in the kernel, binary patching `user_prog()` to return immediately would allow you to speed up the kernel without rebooting the machine.

## Lab3-2: Callout and return (15 pt)

In this part, you should accomplish the same thing as Lab3-1, using a callout that emulates a RET (in ARM `bx lr` to be acccuate). The trickiness about callouts is that they need to save all the registers and [CPSR][CPSR register] because the code was not expecting a callout. The standard ARM calling conventions don't work. Find a proper glue code in [arm_callout.S]. You should patch the first instruction on `user_prog()` to jump to the callout. The callout should emulate the behavior of the RET by returning not to the calling site of the callout (which is the normal behavior) but directly to the return PC stored in `lr` register.

Reference: [Procedure Call Standard for the Arm Architecture](https://files.syssec.org/IHI0042J_2020Q2_aapcs32.pdf)

## Lab3-3: Simple instruction decode (15 pt)

It is simpler to disassemble ARM instructions due to the fixed-width alignment scheme of ARM architecture; four-bytes and two-bytes alignments for ARM and Thumb modes. You will add ARM decoder code to `StartProfiling()` to print the address, opcode, and length of instructions for `user_prog()` until it meets the last instruction (`bx lr (0xe12fff1e)`).

Your decoder should work for `lab3_fib, lab3_fact, lab3_fact2`. 

* Example output

```asm
input number: 10
addr 0x109b8, opcode: 92, len: 4, isCFlow: false
addr 0x109bc, opcode: 28, len: 4, isCFlow: false
addr 0x109c0, opcode: 24, len: 4, isCFlow: false
addr 0x109c4, opcode: 50, len: 4, isCFlow: false
addr 0x109c8, opcode: 51, len: 4, isCFlow: false
addr 0x109cc, opcode: 35, len: 4, isCFlow: false
addr 0x109d0, opcode: a0, len: 4, isCFlow: true
addr 0x109d4, opcode: 3a, len: 4, isCFlow: false
addr 0x109d8, opcode: a0, len: 4, isCFlow: true
addr 0x109dc, opcode: 51, len: 4, isCFlow: false
addr 0x109e0, opcode: 24, len: 4, isCFlow: false
addr 0x109e4, opcode: 1a, len: 4, isCFlow: false
addr 0x109e8, opcode: bf, len: 4, isCFlow: true
addr 0x109ec, opcode: 1a, len: 4, isCFlow: false
addr 0x109f0, opcode: 51, len: 4, isCFlow: false
addr 0x109f4, opcode: 24, len: 4, isCFlow: false
addr 0x109f8, opcode: 1a, len: 4, isCFlow: false
addr 0x109fc, opcode: bf, len: 4, isCFlow: true
addr 0x10a00, opcode: 1a, len: 4, isCFlow: false
addr 0x10a04, opcode: 8, len: 4, isCFlow: false
addr 0x10a08, opcode: 1a, len: 4, isCFlow: false
addr 0x10a0c, opcode: 24, len: 4, isCFlow: false
addr 0x10a10, opcode: 8b, len: 4, isCFlow: false
addr 0x10a14, opcode: 12, len: 4, isCFlow: true
```

## Lab3-4: Control flow following and program profiling (40 pt)

In this part, you will implement the binary translator that instrument control flow instructions (say `bl`, `bx`, `bgt`, `b`) with callout routines that can introspect the code at runtime. 

Before running the `user_prog()`, you will first run the `StartProfiling()` function with the code object as the first argument. The function will decode and follow the instruction stream until it hits the control flow instruction.  Upon an encounter with control flow instruction, the `StartProfiling()` will, 

(1) Patch the control instruction to be executed with glue code that leads to the callout routine where the saveReg `argument` provides access to the callsite context. 
(2)  Inside the callout routine, you can emulate the branch operation to determine the next branch choice. You will again follow the instruction stream to find and patch the next control instruction to be executed.
(3) Unpatch to restore the original control instruction, and return the original callsite to resume the execution.

As you implement the above algorithm, the binary translator will grant you access to the program's runtime context at each branch point regardless of your input. Make sure to have clean-up routine inside `stopProflie()` to unpatch the last instrumentation and stop the instrumentation.

As for the output, run `lab3_fib` and `lab3_fact` programs with different inputs and check the number of instructions (and basic blocks) executions. For each basic block, your binary translator will dump the instructions in that block in the same format as in Lab3-3. You should stop this process when you hit the `StartProfiling()` function. Create a data structure to capture the start address of each basic block executed and instruction count.

## Lab3-5: Conditional Instructions (30 pt)

In this part, you are to implement necessary components to instrument [fact2.S], an [optimized] version of Factorial function using ARM [conditional instruction]. To extend your binary translator and run the code, you have three places to modify and implement. First, you need to extend the ARM decoder routine ([armDecode()]) to decode the ARM instruction's conditional prefix. Second, you need to fill in the handler routine ([handleCondCallout()]) to check the condition flag ([CPSR register]) status and emulate the ARM's branch operation. Lastly, you need to implement the glue code ([condCallout] from [arm_callout.S]) to which patched instruction will jump. This time we do not provide the glue code that connects callout routine, it is your job to implement. 
    
## Resources

* [Chapter 5 from ARM® Architecture Reference Manual (ARMv7-A and ARMv7-R edition)](https://files.syssec.org/ARMv7-M_ARM.pdf)
* [Using as assembler -- ARM dependent features](https://sourceware.org/binutils/docs/as/ARM_002dDependent.html#ARM_002dDependent)

----
[handleCondCallout()]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/lab3.c#L57
[condCallout]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/arm_callout.S#L74
[armDecode()]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/lab3.c#L24
[CPSR register]:https://www.keil.com/pack/doc/CMSIS/Core_A/html/group__CMSIS__CPSR.html

[binary translator]:https://dl.acm.org/doi/10.1145/3321705.3329819
[skeleton codes]:https://files.syssec.org/lab3.zip
[side effects]:https://en.wikipedia.org/wiki/Side_effect_(computer_science)
[Fibonacci]:https://en.wikipedia.org/wiki/Fibonacci_number
[Factorial]:https://en.wikipedia.org/wiki/Factorial
[fact.S]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/fact1.S
[fact2.S]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/fact2.S
[fact3.S]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/fact3.S
[arm_callout.S]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/arm_callout.S
[lab3.c]:https://gitlab.syssec.org/utd-classes/CS6332.001-f21-Lab3/-/blob/main/lab3.c
[conditional instruction]:https://developer.arm.com/documentation/dui0068/b/ARM-Instruction-Reference/Conditional-execution
[Student code of conduct]:https://policy.utdallas.edu/utdsp5003

[code]:https://theory.stanford.edu/~aiken/moss/
[similarity]:https://en.wikipedia.org/wiki/Content_similarity_detection
[measurement]:https://github.com/genchang1234/How-to-cheat-in-computer-science-101
[tools]:https://ieeexplore.ieee.org/document/5286623
[optimized]:https://www.sciencedirect.com/topics/computer-science/conditional-execution
[inline monitors]:https://files.syssec.org/0907-mm.pdf
