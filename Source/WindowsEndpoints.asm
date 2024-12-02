EXTERN DllMain_cpp:PROC
EXTERN SDLHook_PeepEvents_cpp:PROC
EXTERN ViewRenderHook_onRenderStart_cpp:PROC
EXTERN PlayerPawn_sceneObjectUpdater_cpp:PROC
EXTERN Weapon_sceneObjectUpdater_cpp:PROC

EXTERNDEF textSectionStartMarker:DWORD
EXTERNDEF textSectionEndMarker:DWORD

OPTION DOTNAME

.DATA
textSectionLength QWORD 0
textSectionReferenceCount QWORD -1
textSectionLock DWORD 0

.CODE .text$a
textSectionStartMarker DWORD 0

.CODE .text$z
textSectionEndMarker DWORD 0

.CODE endpts

lockTextSection:
    mov eax, 1
tryLock:
    xchg [textSectionLock], eax
    test eax, eax
    jz return
spin:
    pause
    cmp [textSectionLock], 0
    je tryLock
    jmp spin

makeTextSectionExecutable:
    cmp textSectionLength, 0
    jz return
    call lockTextSection
    inc textSectionReferenceCount
    jnz unlockTextSection
    mov r9, 20h ; PAGE_EXECUTE_READ
    jmp changeTextSectionProtection

makeTextSectionNotExecutable:
    cmp textSectionLength, 0
    jz return
    call lockTextSection
    dec textSectionReferenceCount
    jns unlockTextSection
    mov r9, 4h ; PAGE_READWRITE
    jmp changeTextSectionProtection

return:
    ret

changeTextSectionProtection:
    ; expects protection constant (e.g. PAGE_EXECUTE_READ) in r9 register
    mov r10, -1 ; first syscall parameter (handle to the current process)
    lea rax, textSectionStartMarker ; base address to protect
    push rax
    mov rdx, rsp ; second syscall parameter (pointer to base address to protect)
    push textSectionLength
    mov r8, rsp ; third syscall parameter (pointer to the number of bytes to protect)
    push rax ; align stack
    push rax ; space for storing old protection
    push rsp ; fifth syscall parameter (pointer to old protection)
    sub rsp, 40 ; align stack pointer and allocate shadow space
    mov rax, 80 ; load syscall index of NtProtectVirtualMemory
    syscall
    add rsp, 80

unlockTextSection:
    mov textSectionLock, 0
    ret

DllMain PROC
    sub rsp, 40 ; align stack pointer and allocate shadow space for function call
    call makeTextSectionExecutable
    call DllMain_cpp
    mov [rsp + 32], rax ; backup rax as the next call will destroy it
    call makeTextSectionNotExecutable
    add rsp, 32
    pop rax
    mov textSectionLength, rax
    mov eax, 1
    ret
DllMain ENDP

SDLHook_PeepEvents_asm PROC
    push rcx ; backup volatile registers used in the original function
    push rdx
    push r8
    push r9
    sub rsp, 40 ; align stack pointer and allocate shadow space for function call
    call makeTextSectionExecutable
    call SDLHook_PeepEvents_cpp
    mov [rsp + 32], rax ; backup rax as the next call will destroy it
    call makeTextSectionNotExecutable
    add rsp, 32
    pop rax
    pop r9
    pop r8
    pop rdx
    pop rcx
    jmp rax ; jump to the original function
SDLHook_PeepEvents_asm ENDP

ViewRenderHook_onRenderStart_asm PROC
    push rcx ; backup volatile rcx
    sub rsp, 32 ; allocate shadow space for function call
    call makeTextSectionExecutable
    mov rcx, [rsp + 32]
    call ViewRenderHook_onRenderStart_cpp
    call makeTextSectionNotExecutable
    add rsp, 40
    ret
ViewRenderHook_onRenderStart_asm ENDP

PlayerPawn_sceneObjectUpdater_asm PROC
    push rcx ; backup volatile registers used in the original function
    push rdx
    push r8
    sub rsp, 32 ; allocate shadow space for function call
    call makeTextSectionExecutable
    mov r8, [rsp + 32]
    mov rdx, [rsp + 40]
    mov rcx, [rsp + 48]
    call PlayerPawn_sceneObjectUpdater_cpp
    mov [rsp + 48], rax ; backup rax as the next call will destroy it
    call makeTextSectionNotExecutable
    add rsp, 48
    pop rax
    ret
PlayerPawn_sceneObjectUpdater_asm ENDP

Weapon_sceneObjectUpdater_asm PROC
    push rcx ; backup volatile registers used in the original function
    push rdx
    push r8
    sub rsp, 32 ; allocate shadow space for function call
    call makeTextSectionExecutable
    mov r8, [rsp + 32]
    mov rdx, [rsp + 40]
    mov rcx, [rsp + 48]
    call Weapon_sceneObjectUpdater_cpp
    mov [rsp + 48], rax ; backup rax as the next call will destroy it
    call makeTextSectionNotExecutable
    add rsp, 48
    pop rax
    ret
Weapon_sceneObjectUpdater_asm ENDP

END
