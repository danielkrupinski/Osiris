.CODE

systemCall PROC
    mov rax, [rcx] ; load syscall index into rax
    mov r10, [rcx+8] ; load first parameter into r10
    syscall
    ret
systemCall ENDP

END
