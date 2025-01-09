.globl SDLHook_PeepEvents_asm
.globl ViewRenderHook_onRenderStart_asm
.globl PlayerPawn_sceneObjectUpdater_asm
.globl Weapon_sceneObjectUpdater_asm

.hidden SDLHook_PeepEvents_asm
.hidden ViewRenderHook_onRenderStart_asm
.hidden PlayerPawn_sceneObjectUpdater_asm
.hidden Weapon_sceneObjectUpdater_asm

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

ViewRenderHook_onRenderStart_asm:
    jmp ViewRenderHook_onRenderStart_cpp

.align 2 # scene object updater is stored in a tagged pointer on Linux so we align the address to ensure the tag bit is not set
PlayerPawn_sceneObjectUpdater_asm:
    jmp PlayerPawn_sceneObjectUpdater_cpp

.align 2 # scene object updater is stored in a tagged pointer on Linux so we align the address to ensure the tag bit is not set
Weapon_sceneObjectUpdater_asm:
    jmp Weapon_sceneObjectUpdater_cpp
