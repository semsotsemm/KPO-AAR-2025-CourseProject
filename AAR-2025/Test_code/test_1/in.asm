	.586
	.model flat, stdcall
	includelib kernel32.lib
	includelib libucrt.lib
	includelib ../Debug/StaticLib.lib

	ExitProcess PROTO : DWORD
	write_int PROTO C : DWORD
	write_str PROTO C : DWORD
	out_bool PROTO : DWORD
	lenght PROTO C : DWORD
	copy PROTO C
	powNumber PROTO C 
	random PROTO C
	factorialOfNumber PROTO C : DWORD
	squareOfNumber PROTO C : DWORD

	.stack 4096
	.const
		newline byte 13, 10, 0
		true_str byte 'true', 0
		false_str byte 'false', 0
		Lnum_N0 sdword 0
		Lnum_N1 sdword 50
		Lnum_N2 sdword 2
		Lnum_N3 sdword 0
		Lnum_N4 sdword 1
		Lnum_N5 sdword 1
		Lnum_N6 sdword 0
		Lstr_S7 byte '--- Нечетные числа от 0 до 50 ---', 0
		Lstr_S8 byte '--- Конец списка ---', 0
		Lstr_S9 byte '--- Возведение 10 в третью степень ---', 0
		Lnum_N10 sdword 10
		Lnum_N11 sdword 3
		Lnum_N12 sdword 0
	.data
		switch_res sdword 0
		next_number sdword 0
		check_odd sdword 0
		counter sdword 0
		start_val sdword 0
		dsddsd sdword 0
	.code
	print_odd_numbers PROC current : DWORD
		push Lnum_N0
		pop eax
		mov counter, eax
		push current
		push Lnum_N1
		pop ebx
		pop eax
		cmp eax, ebx
		mov eax, 0
		setg al
		push eax
		pop eax
		cmp eax, 0
		je L_else_0
		push current
		pop eax
		ret
		jmp L_end_if_0
	L_else_0:
		push current
		push Lnum_N2
		pop ebx
		pop eax
		cdq
		idiv ebx
		push edx
		pop eax
		mov check_odd, eax
		push check_odd
		push Lnum_N3
		pop ebx
		pop eax
		cmp eax, ebx
		mov eax, 0
		setne al
		push eax
		pop eax
		cmp eax, 0
		je L_else_1
		push current
		pop eax
		push eax
		call write_int
		push offset newline
		call write_str
		jmp L_end_if_1
	L_else_1:
		push counter
		push Lnum_N4
		pop ebx
		pop eax
		add eax, ebx
		push eax
		pop eax
		mov counter, eax
	L_end_if_1:
		push current
		push Lnum_N5
		pop ebx
		pop eax
		add eax, ebx
		push eax
		pop eax
		mov next_number, eax
		push next_number
		call print_odd_numbers
		push eax
		add esp, 4
		pop ecx
	L_end_if_0:
		pop eax
		ret
	print_odd_numbers ENDP
	main PROC
		push Lnum_N6
		pop eax
		mov start_val, eax
		push offset Lstr_S7
		pop eax
		push eax
		call write_str
		push offset newline
		call write_str
		push start_val
		call print_odd_numbers
		push eax
		add esp, 4
		pop ecx
		push offset Lstr_S8
		pop eax
		push eax
		call write_str
		push offset newline
		call write_str
		push offset Lstr_S9
		pop eax
		push eax
		call write_str
		push offset newline
		call write_str
		push Lnum_N11
		push Lnum_N10
		call powNumber
		push eax
		pop eax
		mov dsddsd, eax
		push dsddsd
		pop eax
		push eax
		call write_int
		push offset newline
		call write_str
		push Lnum_N12
		pop eax
		ret
		push 0
		call ExitProcess
	main ENDP
	end main
