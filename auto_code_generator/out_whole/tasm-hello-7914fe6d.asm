; Assemble with TASM and TLINK under DOS

.model small
.stack 100h
.data
    msg db 'Hello, -!$', 0
.code
start:
    mov ax, @data
    mov ds, ax

    mov dx, OFFSET msg
    mov ah, 9
    int 21h

    mov ax, 4C00h
    int 21h
end start