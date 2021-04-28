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
// Copyright © 2017-2018, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////
*/
#include "estamosRTOS.h"
#include <stdlib.h>

// Change this file for the specific microcontroller
// Used for SysTick configuration (SystemCoreClock functions)

#ifdef MCU_STM32F303X8
  #include "stm32f303x8.h"
#elif defined MCU_SOMEOTHER
    #error Must include MCU-specific .h file
#else
    #error Must include default .h file
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
////////////////////////////////////////////////////////////////////////////////////////////////////

// runqueue is the head pointer of the circular TCB linked list used by the scheduler.
static task *runqueue;

// running is a pointer to the currently running task's TCB in the runqueue
task *running;

// task-count stores the number of active tasks
static uint16_t task_count;

// yielding serves as a blocking variable while yield is going on
static uint8_t yielding;

#ifdef ESTAMOSRTOS_LOGIC_ANALYZER
// sched serves as a watch variable to show scheduler activity in the simulator
uint8_t sched;
#endif

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
	#ifdef ESTAMOSRTOS_LOGIC_ANALYZER
		sched=0;
	#endif
	// Debug block to review priorities in the debugger
	#ifdef ESTAMOSRTOS_DEBUG
	  uint32_t pendsv, systick, svc;
	  pendsv=NVIC_GetPriority(PendSV_IRQn);  
	  systick=NVIC_GetPriority(SysTick_IRQn);
	  svc=NVIC_GetPriority(SVCall_IRQn); 
  #endif 	
	
	NVIC_SetPriority(PendSV_IRQn,0xff);  // lowest priority for PendSV
	NVIC_SetPriority(SysTick_IRQn,0x00); // highest priority for SysTick
	NVIC_SetPriority(SVCall_IRQn,0x00); // highest priority for SVCall
  
	#ifdef ESTAMOSRTOS_DEBUG
	  pendsv=NVIC_GetPriority(PendSV_IRQn);  
	  svc=NVIC_GetPriority(SVCall_IRQn);
	  systick=NVIC_GetPriority(SysTick_IRQn);  
	  pendsv = SCB->SHP[10];
    systick=NVIC_GetPriority(SysTick_IRQn);  
	#endif
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
	
	#ifdef MCU_STM32F303X8
		SystemCoreClockUpdate();  // These two lines are implementation specific
		//SysTick_Config(SystemCoreClock/1000);   // Generate interrupt evey 1 ms 
		SysTick_Config(ESTAMOSRTOS_TICKS_TO_SCHEDULER);   // Generate interrupt at selected frequency
  #elif defined MCU_SOMEOTHER
    #error Must implement MCU-specific code
  #else
    #error Must implement MCU-specific code
	#endif
	
	running->SP = running->buffer+STACK_SIZE; // Flush the first task's stack
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
// The PendSV_Handler switches stack pointer spaces by moving the SP to the 
// next task's allocated buffer.
// The PendSV_Handler is implemented in estamosRTOS_asm.s	
////////////////////////////////////////////////////////////////////////////////

void estamosRTOS_scheduler(){
	running=running->next; // running points to the next TCB
	yielding=0;
	#ifdef ESTAMOSRTOS_LOGIC_ANALYZER
		sched=0;
	#endif	
}


////////////////////////////////////////////////////////////////////////////////
// void SysTick_Handler()
//
// This function sets the PendSV interrupt and exits.
// 
////////////////////////////////////////////////////////////////////////////////

void SysTick_Handler(){
	#ifdef ESTAMOSRTOS_LOGIC_ANALYZER
	  sched=1;
	#endif
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}


////////////////////////////////////////////////////////////////////////////////
// void SVC_Handler_C(unsigned int * svc_args)
//
// C implementation of polymorphic custom SVC calls.
// This function extracts the type of SVC call, technically the operand in the 
// calling SVC instruction, and then allows the user to handle the custom 
// as per function parameter definition.
// 
// These functions can also return values in registers. 
// Here are some excellent resources:
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0179b/ar01s02s07.html
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0491c/BABJIGHA.html
// 
// Custom forms may be defined with the following syntax: 
//  
// void __svc(SVC_XX) svc_one(const char *string);
// void __svc(SVC_XX) svc_one(void);
// void __svc(SVC_XX) svc_one(const uint8_t my_num);
//
////////////////////////////////////////////////////////////////////////////////

void SVC_Handler_C(unsigned int * svc_args){
	unsigned int svc_number;    /*    * Stack contains:    * r0, r1, r2, r3, r12, r14, the return address and xPSR    * First argument (r0) is svc_args[0]    */    
	char *str;
	#ifdef ESTAMOSRTOS_LOGIC_ANALYZER
	  sched=1;
	#endif
	svc_number = ((char *)svc_args[6])[-2]; 
	switch(svc_number){        
		case SVC_KILL:            /* Handle SVC KILL */            
		break;       
		
		case SVC_YIELD:            /* Handle SVC YIELD */            
									SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		break;         
		
		case SVC_MISC:            /* Handle SVC MISC */   
			  str = (char *)(svc_args[0]);
				volatile int i;
				i=(int)str;
		break;       
		
		default:            /* Unknown SVC */            
		break;    
	}
}


////////////////////////////////////////////////////////////////////////////////
// void estamosRTOS_yield()
//
// C function that yields the CPU to the scheduler.
// This is done in an optimal way: By calling the SVC handler to do the same 
// as the SysTick handler.
//
// A measure is taken to prevent double yielding in the unlikely event that the
// SysTick goes off during the brief critical section, resulting in an unwanted 
// first yield. 
////////////////////////////////////////////////////////////////////////////////

void estamosRTOS_yield(){
	// Start of critical section
	yielding=1; 
	#ifdef MCU_STM32F303X8
    SysTick_Config(ESTAMOSRTOS_TICKS_TO_SCHEDULER); // Restore original Tick frequency
	#else
    #error Must implement MCU-specific code
	#endif
	// End of critical section
	if(yielding) // If systick didn't go off, then yield. The context switch resets yielding to 0.
		svc_yield();  
	while(yielding);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright © 2017-2018, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////
