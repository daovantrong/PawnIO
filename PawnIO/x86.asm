; PawnIO - Input-output driver
; Copyright (C) 2023  namazso <admin@namazso.eu>
; 
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License along
; with this program; if not, write to the Free Software Foundation, Inc.,
; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

.686
.model flat
.code

; Dell SMM function - manually decorated for stdcall
PUBLIC __dell@4
__dell@4 PROC
    push ebx
    push esi
    push edi
    
    mov edx, [esp+16]  ; Get parameter (adjusted for pushes)
    
    mov eax, [edx]
    mov ecx, [edx+4]
    push edx
    mov edx, [edx+8]
    push eax
    mov eax, [esp+8]   ; Reload edx pointer
    mov ebx, [eax+12]
    mov esi, [eax+16]
    mov edi, [eax+20]
    pop eax
    
    out 0b2h, al
    out 084h, al
    
    pop edx            ; Get edx pointer back
    mov [edx], eax
    mov [edx+4], ecx
    push eax
    mov eax, edx
    pop edx
    mov [eax+8], edx
    mov [eax+12], ebx
    mov [eax+16], esi
    mov [eax+20], edi
    
    setb al
    movzx eax, al
    
    pop edi
    pop esi
    pop ebx
    
    ret 4
__dell@4 ENDP

; CR/DR access function
PUBLIC __crdr
__crdr PROC
    lea eax, begin
    and ecx, 1f8h
    add eax, ecx
    jmp eax
ALIGN 8
begin:
    ; mov eax, dr[0-7]
    DB 0fh, 21h, 0c0h
    ret
    ALIGN 8
    DB 0fh, 21h, 0c8h
    ret
    ALIGN 8
    DB 0fh, 21h, 0d0h
    ret
    ALIGN 8
    DB 0fh, 21h, 0d8h
    ret
    ALIGN 8
    DB 0fh, 21h, 0e0h
    ret
    ALIGN 8
    DB 0fh, 21h, 0e8h
    ret
    ALIGN 8
    DB 0fh, 21h, 0f0h
    ret
    ALIGN 8
    DB 0fh, 21h, 0f8h
    ret
    ALIGN 8
    
    ; mov eax, cr[0-7]
    DB 0fh, 20h, 0c0h
    ret
    ALIGN 8
    DB 0fh, 20h, 0c8h
    ret
    ALIGN 8
    DB 0fh, 20h, 0d0h
    ret
    ALIGN 8
    DB 0fh, 20h, 0d8h
    ret
    ALIGN 8
    DB 0fh, 20h, 0e0h
    ret
    ALIGN 8
    DB 0fh, 20h, 0e8h
    ret
    ALIGN 8
    DB 0fh, 20h, 0f0h
    ret
    ALIGN 8
    DB 0fh, 20h, 0f8h
    ret
    ALIGN 8
    
    ; mov dr[0-7], edx
    DB 0fh, 23h, 0c2h
    ret
    ALIGN 8
    DB 0fh, 23h, 0cah
    ret
    ALIGN 8
    DB 0fh, 23h, 0d2h
    ret
    ALIGN 8
    DB 0fh, 23h, 0dah
    ret
    ALIGN 8
    DB 0fh, 23h, 0e2h
    ret
    ALIGN 8
    DB 0fh, 23h, 0eah
    ret
    ALIGN 8
    DB 0fh, 23h, 0f2h
    ret
    ALIGN 8
    DB 0fh, 23h, 0fah
    ret
    ALIGN 8
    
    ; mov cr[0-7], edx
    DB 0fh, 22h, 0c2h
    ret
    ALIGN 8
    DB 0fh, 22h, 0cah
    ret
    ALIGN 8
    DB 0fh, 22h, 0d2h
    ret
    ALIGN 8
    DB 0fh, 22h, 0dah
    ret
    ALIGN 8
    DB 0fh, 22h, 0e2h
    ret
    ALIGN 8
    DB 0fh, 22h, 0eah
    ret
    ALIGN 8
    DB 0fh, 22h, 0f2h
    ret
    ALIGN 8
    DB 0fh, 22h, 0fah
    ret
    ALIGN 8
__crdr ENDP

END
