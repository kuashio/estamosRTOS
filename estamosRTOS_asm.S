;////////////////////////////////////////////////////////////////////////////////////////////////////
;// estamosRTOS is distributed under the DWYWADC license (Do Whatever You Want And Don't Complain).
;// 
;// By using estamosRTOS, you agree to the following terms:
;// 
;//                       - Do whatever you want and don't complain.
;//
;// enjoy!
;//
;// Copyright © 2017-2018, Eduardo Corpeño
;////////////////////////////////////////////////////////////////////////////////////////////////////

#include "estamosRTOS_MCU.h"
	
	PRESERVE8 
	THUMB

    AREA    |.text|, CODE, READONLY 
    EXPORT  PendSV_Handler
    EXPORT  SVC_Handler
    EXPORT  estamosRTOS_asm_launch
    EXPORT  estamosRTOS_mutex_lock
    EXPORT  estamosRTOS_mutex_unlock
    IMPORT  estamosRTOS_scheduler
    IMPORT  running
	IMPORT  SVC_Handler_C    

;#ifdef ESTAMOSRTOS_LOGIC_ANALYZER
;    IMPORT  sched
;#endif	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; estamosRTOS_asm_launch
; Assembly function that Moves the Main Stack 
; Pointer to the first task's allocated stack buffer.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
estamosRTOS_asm_launch PROC
    LDR     R0, =running       ; R0 = pointer to running
    LDR     R1, [R0]           ; R1 = running
    LDR     SP, [R1]           ; SP = running->SP;
    BX      LR                 ; nothing to do here! Return
	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PendSV_Handler
; The PendSV_Handler switches stack pointer spaces
; by moving the MSP (for now) to the next task's allocated buffer.						
;
; This Handler code was mostly borrowed from Jon Valvano's RTOS code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PendSV_Handler     PROC        ; R0-R3,R12,LR,PC,PSR saved by SysTick or other 
                               ; tail-chained interrupt/exception.
;    CPSID   I                  ; Prevent interrupt during switch
    PUSH    {R4-R11}           ; Save remaining regs r4-11
    LDR     R0, =running       ; R0 = pointer to running, old thread
    LDR     R1, [R0]           ; R1 = running
    STR     SP, [R1]           ; Save SP into TCB
    PUSH    {R0,LR}
    BL      estamosRTOS_scheduler  ; call the scheduler
    POP     {R0,LR}
    LDR     R1, [R0]           ; R1 = running, new thread
    LDR     SP, [R1]           ; new thread SP; SP = running->SP;

;#ifdef ESTAMOSRTOS_LOGIC_ANALYZER
;	LDR     R4,=0
;	LDR     R5,=sched
;	STR		R4,[R5]
;#endif
	
    POP     {R4-R11}           ; restore regs r4-11
;    CPSIE   I                  ; tasks run with interrupts enabled
    BX      LR                 ; restore R0-R3,R12,LR,PC,PSR

	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; uint32_t estamosRTOS_mutex_lock(estamosRTOS_mutex *);  // Assembly function that locks a mutex. 
;
; estamosRTOS_mutex_lock
; Assembly function that attempts to lock a mutex. 
; Receives a pointer to an estamosRTOS_mutex.
; Returns success as 1 or failure as 0 in a 32 bit integer.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
estamosRTOS_mutex_lock PROC
	LDR		R2, =ESTAMOSRTOS_MUTEX_LOCKED
	LDREX	R1, [R0]
	CMP 	R1, R2
	BEQ 	was_locked

	STREX 	R1, R2, [R0]
	CMP 	R1, #0        ; 0 means success in STREX
	BNE 	was_locked
	
	MOV 	R0, #ESTAMOSRTOS_EXIT_SUCCESS	
	BX      LR                 ; Return success

was_locked
	CLREX
	MOV 	R0, #ESTAMOSRTOS_EXIT_FAILURE
	BX      LR                 ; Return failure
	ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; void estamosRTOS_mutex_unlock(estamosRTOS_mutex *);  // Assembly function that unlocks a mutex. 
; estamosRTOS_mutex_unlock
; Assembly function that unlocks a mutex. 
; Receives a pointer to an estamosRTOS_mutex.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
estamosRTOS_mutex_unlock PROC
	LDR		R2, =ESTAMOSRTOS_MUTEX_UNLOCKED
	STR 	R2, [R0]
	BX      LR                 
	ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SVC_Handler
; The SVC_Handler copies the calling stack pointer to r0 and calls the 
; C implementation for the remainder of the handler.						
;
; Notice the branch nature of this handler. This is part of the conventions
; used by the __svc() compiler keyword operations. The return is performed by 
; the compiler through a pop {...,pc} instruction.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SVC_Handler
    TST lr, #4    
	MRSEQ r0, MSP    
	MRSNE r0, PSP    
	B SVC_Handler_C

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	END
;////////////////////////////////////////////////////////////////////////////////////////////////////
;// Copyright © 2017-2018, Eduardo Corpeño
;////////////////////////////////////////////////////////////////////////////////////////////////////
