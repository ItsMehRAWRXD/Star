BITS 64

; Simple x64 shellcode to print message
; This would be the decrypted payload

section .text
global _start

_start:
    ; sys_write(1, message, length)
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    lea rsi, [rel msg]  ; message
    mov rdx, msg_len    ; length
    syscall
    
    ; sys_exit(0)
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; exit code 0
    syscall

section .data
msg: db "Fileless payload executed!", 0x0a
msg_len equ $ - msg