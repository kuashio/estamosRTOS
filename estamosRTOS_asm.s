;////////////////////////////////////////////////////////////////////////////////////////////////////
;// estamosRTOS is distributed under the DWYWADC license (Do Whatever You Want And Don't Complain).
;// 
;// By using estamosRTOS, you agree to the following terms:
;// 
;//                       - Do whatever you want and don't complain.
;//
;// enjoy!
;//
;// Copyright © 2017, Eduardo Corpeño
;////////////////////////////////////////////////////////////////////////////////////////////////////

	PRESERVE8 
	THUMB

    AREA    |.text|, CODE, READONLY 

    EXPORT  SysTick_Handler
    EXPORT  estamosRTOS_asm_launch
    EXPORT  estamosRTOS_mutex_lock
    EXPORT  estamosRTOS_mutex_unlock
    IMPORT  estamosRTOS_scheduler
    IMPORT  running
		
	EXPORT  ESTAMOSRTOS_MUTEX_LOCKED
	EXPORT  ESTAMOSRTOS_MUTEX_UNLOCKED

; These two symbols need to be defined in estamosRTOS.h with the same values!!!!
ESTAMOSRTOS_MUTEX_LOCKED   EQU 1
ESTAMOSRTOS_MUTEX_UNLOCKED EQU 0
; These two symbols need to be defined in estamosRTOS.h with the same values!!!!
EXIT_SUCCESS EQU 0
EXIT_FAILURE EQU 1

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
; SysTick_Handler
; The SysTick_Handler switches stack pointer spaces
; by moving the MSP to the first task's allocated buffer.						
;
; This SysTick Handler code was almost entirely taken from Jon Valvano's RTOS code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SysTick_Handler    PROC        ; Save R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; Prevent interrupt during switch
    PUSH    {R4-R11}           ; Save remaining regs r4-11
    LDR     R0, =running       ; R0 = pointer to running, old thread
    LDR     R1, [R0]           ; R1 = running
    STR     SP, [R1]           ; Save SP into TCB
    PUSH    {R0,LR}
    BL      estamosRTOS_scheduler  ; call the scheduler
    POP     {R0,LR}
    LDR     R1, [R0]           ; R1 = running, new thread
    LDR     SP, [R1]           ; new thread SP; SP = running->SP;
    POP     {R4-R11}           ; restore regs r4-11
    CPSIE   I                  ; tasks run with interrupts enabled
    BX      LR                 ; restore R0-R3,R12,LR,PC,PSR

	ENDP
		

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; uint32_t estamosRTOS_mutex_lock(estamosRTOS_mutex *);  // Assembly function that locks a mutex. 
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
	
	MOV 	R0, #EXIT_SUCCESS	
	BX      LR                 ; Return success

was_locked
	CLREX
	MOV 	R0, #EXIT_FAILURE
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




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	END
;////////////////////////////////////////////////////////////////////////////////////////////////////
;// Copyright © 2017-2018, Eduardo Corpeño
;////////////////////////////////////////////////////////////////////////////////////////////////////
