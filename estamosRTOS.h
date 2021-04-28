/*
///////////////////////////////////////////////////////////////////////////////////////////////////
// estamosRTOS is distributed under the DWYWADC license (Do Whatever You Want And Don't Complain).
// 
// By using estamosRTOS, you agree to the following terms:
// 
//                       - Do whatever you want and don't complain.
//
// enjoy!
//
// Copyright © 2017-2018, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////
*/
#ifndef ESTAMOSRTOS_H
#define ESTAMOSRTOS_H
#include <stdint.h>
#include "estamosRTOS_MCU.h"

/*
// State size in 32 bit words, NOT in bytes
#define STATE_SIZE 16U

// Stack size in 32 bit words, NOT in bytes
#ifndef ESTAMOSRTOS_STACK_SIZE
  #define ESTAMOSRTOS_STACK_SIZE (STATE_SIZE * 4U)
#endif  // ESTAMOSRTOS_STACK_SIZE 

#define STACK_SIZE ESTAMOSRTOS_STACK_SIZE 
*/

typedef uint32_t * pointer;
typedef void (*t_funcPtr)(void);

///////////////////////////////////////////////////////////////////////////////////////////
// Thread Control Block
// Contains:
//           - A Stack pointer, which HAS to be the first field (asm code assumes so).
//           - A buffer pointer, used to be able to free the allocated stack.
//           - A function pointer, which points to the thread function.
///////////////////////////////////////////////////////////////////////////////////////////

struct TCB_NODE{
	pointer SP;
	pointer buffer;
	t_funcPtr func;
	struct TCB_NODE * next;
}; // Thread Control Block

typedef struct TCB_NODE TCB;
typedef TCB task;

typedef uint32_t estamosRTOS_mutex;  // TODO: Consider uint8_t

/*
// These two symbols need to be defined in estamosRTOS_asm.s with the same values!!!!
#define ESTAMOSRTOS_MUTEX_LOCKED    1
#define ESTAMOSRTOS_MUTEX_UNLOCKED  0
// These two symbols need to be defined in estamosRTOS_asm.s with the same values!!!!

//#define ESTAMOSRTOS_TICKS_TO_SCHEDULER (400)  
#define ESTAMOSRTOS_TICKS_TO_SCHEDULER (10000)  

// SVC code definitions for system calls, including yield operation.
#define SVC_KILL  0x11
#define SVC_YIELD 0x22
#define SVC_MISC  0x33
*/

///////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////
void estamosRTOS_init(void); // Initializes the helper variables.

void estamosRTOS_add_task(task *, t_funcPtr);  // Adds a task to the runqueue
                                              // Also initializes the task, allocating 
                                             // a stack buffer, and ISR return data.

void estamosRTOS_start(void);  // Launches the scheduler by: 
                              //      - Wrapping the runqueue around 
                             //       - Launching the SysTick interrupt 
                            //        - Moving the Main Stack Pointer to the first task's
                           //           allocated stack buffer
                          //          - Calling the first task's function

void estamosRTOS_asm_launch(void);  // Assembly function that Moves the Main Stack 
                                   // Pointer to the first task's allocated stack buffer.
																	// This function is implemented in estamosRTOS_asm.s
																	 
void estamosRTOS_scheduler(void);  // C function that Performs the task switch. 
                                  // The SysTick_Handler switches stack pointer spaces
                                 // by moving the MSP to the next task's allocated buffer.
                                // The SysTick_Handler is implemented in estamosRTOS_asm.s																 

uint32_t estamosRTOS_mutex_lock(estamosRTOS_mutex *);  // Assembly function that locks a mutex. 

void estamosRTOS_mutex_unlock(estamosRTOS_mutex *);  // Assembly function that unlocks a mutex. 

void estamosRTOS_yield(void); // C function that yields the CPU to the scheduler


// SVC polymorphic functions for system calls, including yield operation.
void __svc(SVC_KILL)  svc_kill(const task *the_waling_dead);
void __svc(SVC_YIELD) svc_yield(void);
void __svc(SVC_MISC)  svc_misc(const char *string);

#endif // ESTAMOSRTOS_H

////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright © 2017-2018, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////
