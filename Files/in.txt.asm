.586
.model flat, stdcall
includelib kernel32.lib
includelib libucrt.lib
includelib StaticLib.lib

ExitProcess PROTO: dword

EXTRN lenght: proc
EXTRN write_int: proc
EXTRN write_str : proc
EXTRN copy: proc
EXTRN getLocalTimeAndDate: proc
EXTRN random: proc
EXTRN squareOfNumber: proc
EXTRN factorialOfNumber: proc
EXTRN powNumber: proc

.stack 4096

.const
nulError byte 'error divided by zero', 0
nul sdword 0, 0

	L0 byte "Входные числа: ", 0
	L1 byte "X равно Y: ", 0
	L2 byte "X больше Y: ", 0
	L3 byte "X меньше Y: ", 0
	L4 byte "X не равно Y: ", 0
	L5 sdword 0
	L6 byte "--- Арифметические операции ---", 0
	L7 byte "Сложение (a + b): ", 0
	L8 byte "Вычитание (a - b): ", 0
	L9 byte "Умножение (a * b): ", 0
	L10 byte "Деление (a / b): ", 0
	L11 byte "Остаток от деления: ", 0
	L12 byte "--- Демонстрация switch/case ---", 0
	L13 sdword 1
	L14 byte "Выбран Кейс 1: Проверка целочисленных литералов", 0
	L15 sdword 13
	L16 sdword 51
	L17 byte "Двоичный b1101 (13): ", 0
	L18 byte "Восьмеричный h33 (27): ", 0
	L19 sdword 2
	L20 byte "Выбран Кейс 2: Проверка функций времени и случайных чисел", 0
	L21 byte "Текущее время: ", 0
	L22 sdword 10
	L23 byte "Случайное число от 1 до 10: ", 0
	L24 byte "Дефолтный Кейс: Неверный выбор", 0
	L25 byte " ", 0
	L26 sdword 50
	L27 sdword 5
	L28 byte "--- Сравнения ---", 0
	L29 sdword 15
	L30 byte "--- Встроенные Функции для Чисел ---", 0
	L31 sdword 4
	L32 sdword 3
	L33 byte "4 в степени 3 (pow(4, 3)): ", 0
	L34 byte "Факториал 4: ", 0
	L35 sdword 9
	L36 byte "Квадратный корень 9: ", 0
	L37 byte "--- Функции для Строк и Цикл ---", 0
	L38 byte "TestString", 0
	L39 byte "Строка: ", 0
	L40 byte "Длина строки: ", 0
	L41 byte "Скопированные 4 символа: ", 0
	L42 byte "Цикл repeat (вывод чисел от 1 до 5):", 0
	L43 sdword 6
	L44 byte "--- Представление логического литарала ---", 0
	L45 byte "flag = false", 0
	L46 byte "Тест не пройден", 0
	L47 byte "Тест пройден", 0
.data
	resultEq_block0 sdword 0
	sum_block8 sdword 0
	difference_block8 sdword 0
	product_block8 sdword 0
	division_block8 sdword 0
	remainder_block8 sdword 0
	binaryVal_block11 sdword 0
	octalVal_block11 sdword 0
	rTime_block12 dword ?
	rResult_block12 sdword 0
	separator_block14 dword ?
	firstoperand_block14 sdword 0
	secondoperand_block14 sdword 0
	logA_block14 sdword 0
	logB_block14 sdword 0
	pResult_block14 sdword 0
	factorial_block14 sdword 0
	square_block14 sdword 0
	myString_block14 dword ?
	len_block14 sdword 0
	destStr_block14 dword ?
	i_block14 sdword 0
	flag_block14 sdword 0
.code
LogicAndBitwiseDemo PROC,	x_LogicAndBitwiseDemo : sdword, y_LogicAndBitwiseDemo : sdword  
push ebx
push edx


push offset L0
call write_str

push x_LogicAndBitwiseDemo
call write_int

push y_LogicAndBitwiseDemo
call write_int

mov edx, x_LogicAndBitwiseDemo
cmp edx, y_LogicAndBitwiseDemo

jz true0
jnz false0
true0:

push offset L1
call write_str

jmp next0

false0:
mov edx, x_LogicAndBitwiseDemo
cmp edx, y_LogicAndBitwiseDemo

jg true1
jle false1
true1:

push offset L2
call write_str

jmp next1

false1:
mov edx, x_LogicAndBitwiseDemo
cmp edx, y_LogicAndBitwiseDemo

jl true2
jge false2
true2:

push offset L3
call write_str

jmp next2

false2:
mov edx, x_LogicAndBitwiseDemo
cmp edx, y_LogicAndBitwiseDemo

jnz true3
jmp next3
true3:

push offset L4
call write_str

next3:

next2:

next1:

next0:

pop edx
pop ebx

mov eax, L5
ret
LogicAndBitwiseDemo ENDP
ArithmeticDemo PROC,	a_ArithmeticDemo : sdword, b_ArithmeticDemo : sdword  
push ebx
push edx


push offset L6
call write_str


push offset L0
call write_str

push a_ArithmeticDemo
call write_int

push b_ArithmeticDemo
call write_int

push a_ArithmeticDemo
push b_ArithmeticDemo
pop ebx
pop eax
add eax, ebx
push eax
pop eax
mov sum_block8, eax


push offset L7
call write_str

push sum_block8
call write_int

push a_ArithmeticDemo
push b_ArithmeticDemo
pop ebx
pop eax
sub eax, ebx
push eax
pop eax
mov difference_block8, eax


push offset L8
call write_str

push difference_block8
call write_int

push a_ArithmeticDemo
push b_ArithmeticDemo
pop ebx
pop eax
imul eax, ebx
push eax
pop eax
mov product_block8, eax


push offset L9
call write_str

push product_block8
call write_int

push a_ArithmeticDemo
push b_ArithmeticDemo
pop ebx
pop eax
cmp nul, ebx
cdq
idiv ebx
push eax
pop eax
mov division_block8, eax


push offset L10
call write_str

push division_block8
call write_int

push a_ArithmeticDemo
push b_ArithmeticDemo
pop ebx
pop eax
cdq
idiv ebx
push edx
pop eax
mov remainder_block8, eax


push offset L11
call write_str

push remainder_block8
call write_int

pop edx
pop ebx

mov eax, sum_block8
ret
ArithmeticDemo ENDP
SwitchDemo PROC,	choice_SwitchDemo : sdword  
push ebx
push edx


push offset L12
call write_str

mov eax, choice_SwitchDemo
cmp eax, L13
je switch_case_4_0
cmp eax, L19
je switch_case_4_1
jmp switch_default_4

switch_case_4_0:


push offset L14
call write_str

push L15
pop eax
mov binaryVal_block11, eax

push L16
pop eax
mov octalVal_block11, eax


push offset L17
call write_str

push binaryVal_block11
call write_int


push offset L18
call write_str

push octalVal_block11
call write_int

switch_case_4_1:


push offset L20
call write_str

call getLocalTimeAndDate
mov rTime_block12, eax

push offset L21
call write_str

push rTime_block12
call write_str

push L22
push L13
call random
push eax
pop eax
mov rResult_block12, eax


push offset L23
call write_str

push rResult_block12
call write_int

jmp switch_end_4

switch_default_4:


push offset L24
call write_str

jmp switch_end_4

switch_end_4:

pop edx
pop ebx

mov eax, choice_SwitchDemo
ret
SwitchDemo ENDP
main PROC

mov separator_block14, offset L25
push separator_block14
call write_str

push L26
pop eax
mov firstoperand_block14, eax

push L27
pop eax
mov secondoperand_block14, eax

push secondoperand_block14
push firstoperand_block14
call ArithmeticDemo
push eax
pop eax
mov secondoperand_block14, eax

push separator_block14
call write_str


push offset L28
call write_str

push L22
pop eax
mov logA_block14, eax

push L22
pop eax
mov logB_block14, eax

push logB_block14
push logA_block14
call LogicAndBitwiseDemo
push eax
pop eax
mov logB_block14, eax

push separator_block14
call write_str

push L29
pop eax
mov logB_block14, eax

push logB_block14
push logA_block14
call LogicAndBitwiseDemo
push eax
pop eax
mov logB_block14, eax

push separator_block14
call write_str

push L13
call SwitchDemo
push eax
pop eax
mov logB_block14, eax

push separator_block14
call write_str

push L19
call SwitchDemo
push eax
pop eax
mov logB_block14, eax

push separator_block14
call write_str


push offset L30
call write_str

push separator_block14
call write_str

push L32
push L31
call powNumber
push eax
pop eax
mov pResult_block14, eax


push offset L33
call write_str

push pResult_block14
call write_int

push L31
call factorialOfNumber
push eax
pop eax
mov factorial_block14, eax


push offset L34
call write_str

push factorial_block14
call write_int

push L35
call squareOfNumber
push eax
pop eax
mov square_block14, eax


push offset L36
call write_str

push square_block14
call write_int

push separator_block14
call write_str


push offset L37
call write_str

push separator_block14
call write_str

mov myString_block14, offset L38
push myString_block14
call lenght
push eax
pop eax
mov len_block14, eax


push offset L39
call write_str

push myString_block14
call write_str


push offset L40
call write_str

push len_block14
call write_int

push L31
push myString_block14
push destStr_block14
call copy
mov destStr_block14, eax

push offset L41
call write_str

push destStr_block14
call write_str

push separator_block14
call write_str


push offset L42
call write_str

push L13
pop eax
mov i_block14, eax

cyclenext5:
mov edx, i_block14
cmp edx, L43
jge cycle5
push i_block14
call write_int

push i_block14
push L13
pop ebx
pop eax
add eax, ebx
push eax
pop eax
mov i_block14, eax

jmp cyclenext5
cycle5:

push separator_block14
call write_str


push offset L44
call write_str

push L13
pop eax
cmp eax, 0
je bool_conv_575_false
mov eax, 1
jmp bool_conv_575_end
bool_conv_575_false:
mov eax, 0
bool_conv_575_end:
mov flag_block14, eax


push offset L45
call write_str

mov edx, flag_block14
cmp edx, 0

jnz true6
jz false6
true6:

push offset L46
call write_str

jmp next6

false6:

push offset L47
call write_str

next6:

jmp goodExit
errorExit:
push offset nulError
call write_str
goodExit:
push 0
call ExitProcess
main ENDP
end main