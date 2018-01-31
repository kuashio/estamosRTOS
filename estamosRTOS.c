/*
////////////////////////////////////////////////////////////////////////////////////////////////////
// estamosRTOS is distributed under the DWYWADC license (Do Whatever You Want And Don't Complain).
// 
// By using estamosRTOS, you agree to the following terms:
// 
//                       - Do whatever you want and don't complain.
//
// enjoy!
//
// Copyright © 2017, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////
*/
#include "estamosRTOS.h"
#include <stdlib.h>

// Change this file for the specific microcontroller
// Used for SysTick configuration (SystemCoreClock functions)
#include "stm32f303x8.h"

// runqueue is the head pointer of the circular TCB linked list used by the scheduler.
static task *runqueue;

// running is a pointer to the currently running task's TCB in the runqueue
task *running;

// task-count stores the number of active tasks
static uint16_t task_count;

// yielding serves as a blocking variable while yield is going on
static uint8_t yielding;

////////////////////////////////////////////////////////////////////////////////
// void estamosRTOS_init()
//
// Initializes the helper variables
////////////////////////////////////////////////////////////////////////////////
void estamosRTOS_init(){
	runqueue=NULL;
	running=NULL;
	task_count=0;
	yielding=0;
}


////////////////////////////////////////////////////////////////////////////////
// void estamosRTOS_add_task(task *t, t_funcPtr func)
//
// Adds a task to the runqueue
// Also initializes the task, allocating a stack buffer, and ISR return data.
// Receives:
//     - A task pointer t. The task is initialized by the function
//     - A function pointer that's expected to point to the task's main function
////////////////////////////////////////////////////////////////////////////////
void estamosRTOS_add_task(task *t, t_funcPtr func){

	task **ptr=&runqueue;
	while(*ptr!=NULL)
		ptr=&((*ptr)->next);  // ptr finally points to the last pointer in the list
	
	*ptr=t;   // The last pointer is linked to the new task node to be added
  	
	t->buffer = (pointer)malloc(STACK_SIZE*sizeof(uint32_t)); // allocate stack buffer
  t->SP = t->buffer+STACK_SIZE;  // innitialize local task stack pointer 
	                               // to the end of buffer
	t->func=func;  // Task function pointer assignment
	t->next=NULL;  // This is the newly appended node, so it now points to NULL
	
	t->SP -= STATE_SIZE;  // This simulates pushing the state of the cpu for the 
	                      // first time the task will be executed
	
	t->SP[15] = 0x01000000;        // Make sure the Thumb bit is set (Program Status Register) 
	t->SP[14] = (uint32_t)t->func; // The address that will be popped into the PC is the 
	                               // task function's first instruction
	task_count++;     // Update the task counter
	
	#ifdef ESTAMOSRTOS_DEBUG
	  // The following are initial values for all registers except PSR and PC
	  // These values are spurious and will not affect execution, but are included to 
		// be seen in the debugger (at memory or watch windows)
		
		t->SP[13] = 0xCCCCCCCC;       //        
		t->SP[12] = 0x44444444;       //        
		t->SP[11] = 0x33333333;       //        
		t->SP[10] = 0x22222222;       //    R0-R4, R12    
		t->SP[9]  = 0x11111111;       //    done by interrupt process
		t->SP[8]  = 0x00000000;       //        
		
		t->SP[7]  = 0xBBBBBBBB;       //        
		t->SP[6]  = 0xAAAAAAAA;       //    
		t->SP[5]  = 0x99999999;       //        
		t->SP[4]  = 0x88888888;       //   the remaining registers	
		t->SP[3]  = 0x77777777;       //        R4-R11
		t->SP[2]  = 0x66666666;       //        
		t->SP[1]  = 0x55555555;       //        
		t->SP[0]  = 0x44444444;       //        
	#endif
}


////////////////////////////////////////////////////////////////////////////////
//void estamosRTOS_start()
//
// Launches the scheduler by: 
//    - Wrapping the runqueue around 
//    - Launching the SysTick interupt 
//    - Moving the Main Stack Pointer to the first task's
//      allocated stack buffer
//    - Calling the first task's function
////////////////////////////////////////////////////////////////////////////////
void estamosRTOS_start(){
	task **ptr=&runqueue;
	while(*ptr!=NULL)
		ptr=&((*ptr)->next); // ptr finally points to the last pointer in the list
	*ptr=runqueue;         // The list is now circular
	
	running=runqueue; // Initializing the running pointer
	
  SystemCoreClockUpdate();  // These two lines are implementation specific
  //SysTick_Config(SystemCoreClock/1000);   // Generate interrupt evey 1 ms 
  SysTick_Config(ESTAMOSRTOS_TICKS_TO_SCHEDULER);   // Generate interrupt at selected frequency
  
	
	
	running->SP = running->buffer+STACK_SIZE; // Flush the firts task's stack
	estamosRTOS_asm_launch();                 // Use the stack of the first task
  running->func();                          // Call the first task's function
	
  // This point should never be reached!!!!
  // Write your error notifications/assertions here.
	//
	// Execution may reach this point if the first task added returns.
	
	while(1){
	}
}

////////////////////////////////////////////////////////////////////////////////
//void estamosRTOS_scheduler(){
//
// C function that Performs the task switch. 
// The SysTick_Handler switches stack pointer spaces
// by moving the MSP to the fits task's allocated buffer.
// The SysTick_Handler is implemented in estamosRTOS_asm.s	
////////////////////////////////////////////////////////////////////////////////

void estamosRTOS_scheduler(){
  if (yielding){
		SysTick_Config(ESTAMOSRTOS_TICKS_TO_SCHEDULER);   // Restore original Tick frequency
		yielding=0;
	}
	running=running->next; // running points to the next TCB
}


////////////////////////////////////////////////////////////////////////////////
// void estamosRTOS_yield()
//
// C function that yields the CPU to the scheduler
// This is done in a suboptimal way: By setting the SysTick timer to go off
// in a few cycles (about 100 ticks). This is done for two reasons:
//
//   1) This delay must be short enough to force the SysTick interrupt for the 
//      scheduler to take control as soon as possible.
//   2) This delay must be long enough to allow the scheduler to set the SysTick 
//      back to its original frequency before it goes off again.
//      
// This delay is set in the ESTAMOSRTOS_TICK_TO_YIELD symbol.
////////////////////////////////////////////////////////////////////////////////

void estamosRTOS_yield(){
  SysTick_Config(ESTAMOSRTOS_TICKS_TO_YIELD);   // Generate interrupt very soon
	yielding=1;
	while(yielding);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright © 2017-2018, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////
