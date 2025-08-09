; Build with MASM (example): ml /c /coff hello.asm && link /SUBSYSTEM:CONSOLE hello.obj
.386
.model flat, stdcall
option casemap:none

includelib msvcrt.lib
printf PROTO C :PTR SBYTE, :VARARG

.data
    fmt db "Hello, %s!", 10, 0
    who db "rentRot", 0

.code
main PROC
    push OFFSET who
    push OFFSET fmt
    call printf
    add esp, 8
    xor eax, eax
    ret
main ENDP

END main