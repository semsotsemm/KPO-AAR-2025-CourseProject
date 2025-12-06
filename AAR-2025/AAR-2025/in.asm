.586
.model flat, stdcall
includelib kernel32.lib
includelib libucrt.lib
includelib ../Debug/StaticLib.lib

ExitProcess PROTO : DWORD
out_int PROTO : SDWORD
out_str PROTO : DWORD
out_bool PROTO : DWORD

.stack 4096
.const
	newline byte 13, 10, 0
	true_str byte 'true', 0
	false_str byte 'false', 0
	Lnum_N0 sdword 1
	Lnum_N1 sdword 10
	Lnum_N2 sdword 5
	Lstr_S3 byte 'Result from function 1:', 0
	Lstr_S4 byte 'TrueString', 0
	Lstr_S5 byte 'FalseString', 0
	Lnum_N6 sdword 15
	Lstr_S7 byte 'A is less than 15.', 0
	Lstr_S8 byte 'A is not less than 15.', 0
	Lnum_N9 sdword 50
	Lstr_S10 byte 'Case: 50', 0
	Lstr_S11 byte 'Case: A', 0
	Lnum_N12 sdword 0
.data
	calcu sdword 0
	sum_v sdword 0
	prod_ sdword 0
	conca dword ?
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
	push s2
	pop eax
	mov resul, eax
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
	mov a, eax
	push Lnum_N2
	pop eax
	mov b, eax
	push calcu
	push a
	push b
	pop eax
	mov resul, eax
	push offset Lstr_S3
	call out_str
	push offset newline
	call out_str
	push offset resul
	call out_str
	push offset newline
	call out_str
	push conca
	push offset Lstr_S4
	push offset Lstr_S5
	push check
	pop eax
	mov final, eax
	push a
	push Lnum_N6
	pop eax
	cmp eax, 0
	je L_else_1
	push offset Lstr_S7
	call out_str
	push offset newline
	call out_str
	push offset Lstr_S8
	call out_str
	push offset newline
	call out_str
	push offset Lstr_S10
	call out_str
	push offset newline
	call out_str
	push offset Lstr_S11
	call out_str
	push offset newline
	call out_str
	push Lnum_N12
	pop eax
	ret
	push 0
	call ExitProcess
main ENDP
end main
