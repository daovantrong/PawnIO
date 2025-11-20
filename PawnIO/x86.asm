; x86 (32-bit) assembly implementations
; Provides low-level functions for Dell SMM and control register access

.686p
.model flat, stdcall
.code

; Dell SMM function (stdcall convention for x86)
; Prototype: uint32_t __stdcall __dell(uint32_t eax_val)
PUBLIC ___dell@4
___dell@4 PROC
    push ebx
    push ecx
    push edx
    push esi
    push edi
    
    ; Get parameter from stack (stdcall)
    mov eax, [esp + 24]  ; Skip 5 pushed registers (20 bytes) + return address (4 bytes)
    
    ; Dell SMM I/O port
    mov dx, 0B2h
    out dx, al
    
    ; Save result
    push eax
    
    ; Restore registers
    pop eax
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    
    ret 4  ; stdcall: clean up 4 bytes (1 parameter)
___dell@4 ENDP

; Control/Debug register access function
; Prototype: uint32_t __cdecl _crdr(uint32_t reg_num, uint32_t value)
PUBLIC __crdr
PUBLIC _crdr
_crdr PROC
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    
    mov eax, [ebp + 8]   ; reg_num
    mov ecx, [ebp + 12]  ; value
    mov edx, [ebp + 16]  ; write flag
    
    ; Check if write operation
    test edx, edx
    jnz write_reg
    
    ; Read operation
    cmp eax, 0
    je read_cr0
    cmp eax, 2
    je read_cr2
    cmp eax, 3
    je read_cr3
    cmp eax, 4
    je read_cr4
    cmp eax, 8
    je read_cr8
    
    ; Debug registers
    cmp eax, 100h
    je read_dr0
    cmp eax, 101h
    je read_dr1
    cmp eax, 102h
    je read_dr2
    cmp eax, 103h
    je read_dr3
    cmp eax, 106h
    je read_dr6
    cmp eax, 107h
    je read_dr7
    
    xor eax, eax
    jmp done
    
read_cr0:
    mov eax, cr0
    jmp done
    
read_cr2:
    mov eax, cr2
    jmp done
    
read_cr3:
    mov eax, cr3
    jmp done
    
read_cr4:
    mov eax, cr4
    jmp done
    
read_cr8:
    ; CR8 not available in 32-bit mode
    xor eax, eax
    jmp done
    
read_dr0:
    mov eax, dr0
    jmp done
    
read_dr1:
    mov eax, dr1
    jmp done
    
read_dr2:
    mov eax, dr2
    jmp done
    
read_dr3:
    mov eax, dr3
    jmp done
    
read_dr6:
    mov eax, dr6
    jmp done
    
read_dr7:
    mov eax, dr7
    jmp done
    
write_reg:
    ; Write operation
    cmp eax, 0
    je write_cr0
    cmp eax, 2
    je write_cr2
    cmp eax, 3
    je write_cr3
    cmp eax, 4
    je write_cr4
    cmp eax, 8
    je write_cr8
    
    ; Debug registers
    cmp eax, 100h
    je write_dr0
    cmp eax, 101h
    je write_dr1
    cmp eax, 102h
    je write_dr2
    cmp eax, 103h
    je write_dr3
    cmp eax, 106h
    je write_dr6
    cmp eax, 107h
    je write_dr7
    
    xor eax, eax
    jmp done
    
write_cr0:
    mov cr0, ecx
    mov eax, 1
    jmp done
    
write_cr2:
    mov cr2, ecx
    mov eax, 1
    jmp done
    
write_cr3:
    mov cr3, ecx
    mov eax, 1
    jmp done
    
write_cr4:
    mov cr4, ecx
    mov eax, 1
    jmp done
    
write_cr8:
    ; CR8 not available in 32-bit mode
    xor eax, eax
    jmp done
    
write_dr0:
    mov dr0, ecx
    mov eax, 1
    jmp done
    
write_dr1:
    mov dr1, ecx
    mov eax, 1
    jmp done
    
write_dr2:
    mov dr2, ecx
    mov eax, 1
    jmp done
    
write_dr3:
    mov dr3, ecx
    mov eax, 1
    jmp done
    
write_dr6:
    mov dr6, ecx
    mov eax, 1
    jmp done
    
write_dr7:
    mov dr7, ecx
    mov eax, 1
    jmp done
    
done:
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
_crdr ENDP

__crdr EQU _crdr

END
