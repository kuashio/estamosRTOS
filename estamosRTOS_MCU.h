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


           DO NOT USE NON-PREPROCESSOR CODE IN THIS FILE!

           ONLY USE #define, #ifdef, #else, and so on.
					 DO NOT USE typedef, char, uint8_t, int func(int), and so on.
					 DO NOT USE C CODE!
							
					 THIS FILE IS SEEN BY THE PREPROCESSOR TO DEFINE SYMBOLS
           FOR BOTH THE C COMPILER AND THE ASSEMBLER!!!
*/

#ifndef ESTAMOSRTOS_MCU_H
#define ESTAMOSRTOS_MCU_H


// Uncomment the following line to enable debug mode
// #define ESTAMOSRTOS_DEBUG
// ... or the following line to only enable logic analyzer signals
//#define ESTAMOSRTOS_LOGIC_ANALYZER


// Specific MCU definition
#ifndef MCU_STM32F303X8
  #define MCU_STM32F303X8
#endif

#ifdef ESTAMOSRTOS_DEBUG
	#ifndef ESTAMOSRTOS_LOGIC_ANALYZER
		#define ESTAMOSRTOS_LOGIC_ANALYZER
	#endif
#endif
	
// State size in 32 bit words, NOT in bytes
#define STATE_SIZE 16U

// Stack size in 32 bit words, NOT in bytes
#ifndef ESTAMOSRTOS_STACK_SIZE
  #define ESTAMOSRTOS_STACK_SIZE (STATE_SIZE * 4U)
#endif  // ESTAMOSRTOS_STACK_SIZE 

#define STACK_SIZE ESTAMOSRTOS_STACK_SIZE 

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


#ifdef EXIT_SUCCESS
  #define ESTAMOSRTOS_EXIT_SUCCESS EXIT_SUCCESS
#else
  #define ESTAMOSRTOS_EXIT_SUCCESS 0
#endif

#ifdef EXIT_FAILURE
  #define ESTAMOSRTOS_EXIT_FAILURE EXIT_FAILURE
#else
  #define ESTAMOSRTOS_EXIT_FAILURE 1
#endif


#endif // ESTAMOSRTOS_MCU_H

////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright © 2017-2018, Eduardo Corpeño
////////////////////////////////////////////////////////////////////////////////////////////////////


