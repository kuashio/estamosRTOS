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
    IMPORT  estamosRTOS_scheduler
    IMPORT running


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; estamosRTOS_asm_launch
; Assembly function that Moves the Main Stack 
; Pointer to the first task's allocated stack buffer.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
estamosRTOS_asm_launch
    LDR     R0, =running       ; R0 = pointer to running
    LDR     R1, [R0]           ; R1 = running
    LDR     SP, [R1]           ; SP = running->SP;
    BX      LR                 ; nothing to do here! Return


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SysTick_Handler
; The SysTick_Handler switches stack pointer spaces
; by moving the MSP to the first task's allocated buffer.						
;
; This SysTick Handler code was almost entirely taken from Jon Valvano's RTOS code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SysTick_Handler                ; Save R0-R3,R12,LR,PC,PSR
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

    END
		
;////////////////////////////////////////////////////////////////////////////////////////////////////
;// Copyright © 2017, Eduardo Corpeño
;////////////////////////////////////////////////////////////////////////////////////////////////////
