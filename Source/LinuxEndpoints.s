.globl SDLHook_PeepEvents_asm
.globl LoopModeGameHook_getWorldSession_asm
.globl ViewRenderHook_onRenderStart_asm

SDLHook_PeepEvents_asm:
    push %rdi # backup volatile registers used in the original function
    push %rsi
    push %rdx
    push %rcx
    push %r8
    call SDLHook_PeepEvents_cpp
    pop %r8
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi
    jmp *%rax # jump to the original function

LoopModeGameHook_getWorldSession_asm:
    push %rdi # backup volatile rdi used in the original function
    mov 8(%rsp), %rdi # load return address into rdi
    call LoopModeGameHook_getWorldSession_cpp
    pop %rdi
    jmp *%rax # jump to the original function

ViewRenderHook_onRenderStart_asm:
    jmp ViewRenderHook_onRenderStart_cpp
