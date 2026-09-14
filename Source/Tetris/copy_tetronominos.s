	AREA |.text|, CODE, READONLY
    EXPORT copy_block

; R0 = dest, R1 = src
copy_block PROC
    PUSH    {R4, R5, LR} 


    LDM     R1, {R2, R3, R4, R5}
    STM     R0, {R2, R3, R4, R5}

    POP     {R4, R5, PC} 
    ENDP
    END