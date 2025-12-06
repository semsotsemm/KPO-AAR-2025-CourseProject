.586
.model flat, stdcall
includelib kernel32.lib
includelib libucrt.lib
includelib ../Debug/StaticLib.lib

ExitProcess PROTO : DWORD
write_int PROTO C : SDWORD
write_str PROTO C : DWORD
out_bool PROTO : DWORD

.stack 4096
.const
	newline byte 13, 10, 0
	true_str byte 'true', 0
	false_str byte 'false', 0
	Lnum_N0 sdword 1
	Lnum_N1 sdword 0
	Lnum_N2 sdword 10
	Lnum_N3 sdword 5
	Lstr_S4 byte '--- TEST 1: Func Call and Int ---', 0
	Lstr_S5 byte 'a+b * b % (a+b) result:', 0
	Lstr_S6 byte '--- TEST 2: Concatenate (True) ---', 0
	Lstr_S7 byte 'Result of True case:', 0
	Lstr_S8 byte 'TrueString_Case', 0
	Lstr_S9 byte 'FalseString_Case', 0
	Lstr_S10 byte '--- TEST 3: Concatenate (False) ---', 0
	Lstr_S11 byte 'Result of False case:', 0
	Lstr_S12 byte 'TrueString_Case', 0
	Lstr_S13 byte 'FalseString_Case', 0
	Lstr_S14 byte '--- TEST 4: If-Else (a==15 is False) ---', 0
	Lnum_N15 sdword 15
	Lstr_S16 byte 'This should NOT be printed.', 0
	Lstr_S17 byte 'If-Else: False branch executed.', 0
	Lstr_S18 byte '--- TEST 5: Switch (result_int is 5) ---', 0
	Lnum_N19 sdword 50
	Lstr_S20 byte 'Switch: Case 50 is wrong.', 0
	Lstr_S21 byte 'Switch: Case A is wrong.', 0
	Lnum_N22 sdword 5
	Lstr_S23 byte 'Switch: Correct Case 5 executed.', 0
	Lstr_S24 byte '--- TEST 6: Bool Print ---', 0
	Lnum_N25 sdword 0
.data
	sum_v sdword 0
	prod_ sdword 0
	resul dword ?
	a sdword 0
	b sdword 0
	final dword ?
	check sdword 0
.code
calcu PROC x : DWORD, y : DWORD
	push x
	push y
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	mov sum_v, eax
	push x
	push y
	pop ebx
	pop eax
	imul ebx
	push eax
	push sum_v
	pop ebx
	pop eax
	cdq
	idiv ebx
	push edx
	pop eax
	mov prod_, eax
	push prod_
	pop eax
	ret
calcu ENDP
conca PROC s1 : DWORD, s2 : DWORD, condi : DWORD
	push condi
	pop eax
	cmp eax, 0
	je L_else_0
	push s1
	pop eax
	mov resul, eax
	jmp L_end_0
L_else_0:
	push s2
	pop eax
	mov resul, eax
L_end_0:
	push resul
	pop eax
	ret
conca ENDP
main PROC
	push Lnum_N0
	pop eax
	mov check, eax
	push Lnum_N1
	pop eax
	mov check, eax
	push Lnum_N2
	pop eax
	mov a, eax
	push Lnum_N3
	pop eax
	mov b, eax
	push offset Lstr_S4
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S5
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push a
	pop ebx
	pop eax
	add eax, ebx
	push eax
	push b
	pop eax
	push eax
	call write_int
	push offset newline
	call write_str
	push offset Lstr_S6
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S7
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	call conca
	add esp, 12
	push eax
	push offset Lstr_S8
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S10
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S11
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	call conca
	add esp, 12
	push eax
	push offset Lstr_S12
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S14
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push a
	push Lnum_N15
	pop eax
	cmp eax, 0
	je L_else_1
	push offset Lstr_S16
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	jmp L_end_1
L_else_1:
	push offset Lstr_S17
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
L_end_1:
	push offset Lstr_S18
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S20
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S21
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S23
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push offset Lstr_S24
	pop eax
	push eax
	call write_str
	push offset newline
	call write_str
	push check
	pop eax
	push eax
	call write_int
	push offset newline
	call write_str
	push Lnum_N25
	pop eax
	ret
	push 0
	call ExitProcess
main ENDP
end main
