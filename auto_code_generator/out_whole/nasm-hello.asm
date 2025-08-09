; nasm -felf64 hello.asm && ld -o hello hello.o && ./hello

section .data
    msg db "Hello, -!", 10
    len equ $ - msg

section .text
    global _start

_start:
    mov rax, 1          ; sys_write
    mov rdi, 1          ; fd = stdout
    mov rsi, msg        ; buf
    mov rdx, len        ; count
    syscall

    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; status = 0
    syscall