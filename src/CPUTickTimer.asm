global GetCPUTicks

section .text


;-----------------------------------------------------------------------
; Return: number of cpu ticks passed
;-----------------------------------------------------------------------
GetCPUTicks:
    RDTSC ; edx:eax = ticks
    shl  rdx, 32 ; eax = ticks
    add  rax, rdx
    ret
