.global main
.text
decr:
    mov r0, #1
    mov r1, #2
    add r2, r0, r1
    add r3, r2, r2
    sub r4, r3, r0
    and r5, r1, r0, LSL #1
    mov r6, r5
    eor r6, r0, r1, LSR #1
    rsb r7, r1, r2
    orr r8, r1, r0

end:
    swi 0x123456
.data
