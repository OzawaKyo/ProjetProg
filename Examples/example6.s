.global main
.text
decr:
    mov r0, #255
    mvn r1, #0
    sub r1, r1, r0
    add r1, r1, #55
    mov r0, #500
    cmn r0, r1
    mov r13, #0
    adc r3, r13, #1
    add r4, r13, #1
    mov r13, #3
    mov r0, #10
    mov r1, #9
    cmn r0, r1
    sbc r5, r13, #1
    sub r6, r13, #1
    cmn r0, r1
    rsc r8, r1, r0
    rsb r9, r1, r0
    mov r0, #1
    mov r1, #2
    and r10, r0, r1
    bic r11, r0, r1
    mvn r12, r0

end:
    swi 0x123456
.data
