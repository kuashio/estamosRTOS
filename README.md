# estamosRTOS v0.2
##### a quick and dirty Real Time Operating System for ARM Cortex M microcontrollers

This Real Time Operating System started as a "why not?" project in an RTOS graduate class. It is intended for educational use, and is supposed to keep evolving over time.

## Purpose
estamosRTOS is not intended to compete with professional RTOSes like freeRTOS, or RTX. Instead, it's intended for education, so it was designed with a simple implementation, putting readability over efficiency.
The purpose is to allow students to read the code and make modifications to it in order to understand the inner workings of the many elements of an RTOS.
    

## Components
Here's what estamosRTOS currently consists of: 

 - A multitasking mechanism with a simple Round Robin scheduler.
 - Mutex support.
 - A fast-acting yield function.
 - Interrupt support.
 - A System Call mechanism

## To Do 
 - Semaphore support.
 - Inter-Task Communication mechanisms such as message queues, signals, pipes, sockets, etc.
 - Services
 - Implement System Calls


## Files 
 - **`estamosRTOS.c`** - The C code for estamosRTOS.
 - **`estamosRTOS.h`** - Type definitions used by estamosRTOS.
 - **`estamosRTOS_MCU.h`** - Symbol definitions used by estamosRTOS.
 - **`estamosRTOS_asm.S`** - The assembly code for estamosRTOS.
 - **`estamosRTOS.uvoptx`**	- Keil uVision 5 project options file.
 - **`estamosRTOS.uvprojx`** - Keil uVision 5 project file.
 - **`main.c`** - A demo applications with 4 threads and a mutex.
 - **`estamosRTOS_signals.uvl`** - Logic analyzer signals used in `main.c`.
 

## Adaptation Notes
This project was developed in Keil uVision MDK v5.23, and tested on a [NUCLEO-F303K8](https://os.mbed.com/platforms/ST-Nucleo-F303K8/) platform, which runs on an STMicro ARM Cortex-M4 MCU.

There are several ways to customize estamosRTOS to meet your needs, such as Target MCU, Heap Size, Private Stack Size, etc.

Here's a list of recommended customizations prior to attempting to implement estamosRTOS in your MCU:

 - To adapt the code for your own MCU (not the STM32), there are some lines of code you'll want to edit:
    - `#define MCU_STM32F303X8` at the beginning of `estamosRTOS_MCU.h`. This line has to change to define a symbol for your MCU's exclusive pieces of code with `#ifdef` directives. Alternatively, you can define this symbol in the compiler settings dialog box.
    - Look for MCU_STM32F303X8 throughout `estamosRTOS.c`, `estamosRTOS.h`, and `estamosRTOS_MCU.h`, and add the appropriate pieces of code for your MCU's symbol.
    - These pieces of code include:
		- Your MCU's .h file in the `#include` line
		- Your MCU's SystemCoreClockUpdate() function, which initializes the system clock.
		- Your MCU's SysTick_Config(uint32_t ticks) function, which starts the SysTick timer.


 - If you're not planning to use the floating point unit (and also if you are), you should make your toolchain aware of it. This is important because when using the FPU, the context switch performed by the NVIC module includes single precision floating point registers S0 to S15 along with the FPSCR register in the context pushed into the stack, as shown [here](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Babefdjc.html) and  [here](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0298a/BCGHEEFD.html). This is not good for two reasons: 1) If you're not using the FPU, you're wasting 18 4-byte register slots in the stack for each context switch, and 2) If you are using the FPU, there are 16 floating point registers you may want to push into the stack each time, and that has to be done explicitly by your code (just like r4-r11 are pushed in the provided code). In KEIL uVision 5, you may select to use or not use the FPU in the **Options** dialog box (launched with the magic wand tool), in the **Target** tab, under **Code Generation**, in the ***Floating Point Hardware*** field.

 - The size of the private stack for your tasks is configurable with the **`ESTAMOSRTOS_STACK_SIZE`** symbol. You may define this in your own code, as it has an **`#ifndef`** guard. It is expressed in 4-byte entries, not in bytes, so the default value of 64 in that symbol means 256 bytes.

 - define **`ESTAMOSRTOS_DEBUG`** in your code to add debug information in your code. This includes pushing notable values to the registers in the initial stack frame (0x33333333 for R3 for example). You don't want this for production, as it means more code, but it's sometimes useful for debugging. Alternatively, you can define this symbol in the compiler settings dialog box. Look for **`ESTAMOSRTOS_DEBUG`** in the code to learn more. 

 - define **`ESTAMOSRTOS_LOGIC_ANALYZER`** in your code to add debug information in your code for the logic analyzer in Keil uVision's debugger to display. This mode is included in the Debug mode by default. Alternatively, you can define this symbol in the compiler settings dialog box. Look for **`ESTAMOSRTOS_LOGIC_ANALYZER`** in the code to learn more. 


## Why the silly name?
In spanish, RTOS sounds like "fed up", so the author thought it would be appropriate to call his ungodly project "We're Fed Up".

## Bug report and contact
kuashio@gmail.com

## Known Bugs 
The current version does not use the PendSV handler for context switching, and so it has the potential of handling other interrupts while context-switching, resulting in a usage fault

## License 
estamosRTOS is distributed under the ***DWYWADC*** license (Do Whatever You Want And Don't Complain).

By using estamosRTOS, you agree to the following terms:

- Do whatever you want and don't complain.

enjoy!
