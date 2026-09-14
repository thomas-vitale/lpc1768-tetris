	AREA LFSR_area, CODE, READONLY
	EXPORT LFSR_step
		
LFSR_step PROC
		
		MOV R12, SP
		STMFD sp!, {r4-r8, r10, r11, lr}
		AND R4, R0, R1
		EOR R5, R4, R4, LSR #4
		EOR R5, R5, R5, LSR #2
		EOR R5, R5, R5, LSR #1
		AND R5, R5, #1
		LSL R5, R5, #7
		AND R3, R0, #1
		STR R3, [R2]
		LSR R0, R0, #1
		ORR R0, R0, R5
		
		LDMFD sp!, {r4-r8, r10, r11, pc}
		BX LR
		ENDP
		END