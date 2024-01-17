EXTERN SDLHook_PeepEvents_cpp:PROC
EXTERN LoopModeGameHook_getWorldSession_cpp:PROC
EXTERN ViewRenderHook_onRenderStart_cpp:PROC

.CODE endpts

SDLHook_PeepEvents_asm PROC
    push rcx ; backup volatile registers
    push rdx
    push r8
    push r9
    sub rsp, 40 ; align stack pointer and allocate shadow space for function call
    call SDLHook_PeepEvents_cpp
    add rsp, 40 ; restore stack pointer
    pop r9 ; restore volatile registers
    pop r8
    pop rdx
    pop rcx
    jmp rax ; jump to the original function
SDLHook_PeepEvents_asm ENDP

LoopModeGameHook_getWorldSession_asm PROC
    mov rdx, [rsp] ; load return address into rdx
    push rcx ; backup rcx on stack
    sub rsp, 32 ; allocate shadow space for function call
    call LoopModeGameHook_getWorldSession_cpp
    add rsp, 32 ; deallocate shadow space
    pop rcx ; restore rcx
    jmp rax ; jump to the original function
LoopModeGameHook_getWorldSession_asm ENDP

ViewRenderHook_onRenderStart_asm PROC
    jmp ViewRenderHook_onRenderStart_cpp
ViewRenderHook_onRenderStart_asm ENDP

END
