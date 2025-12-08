#include "pch.h"
#include "framework.h"

#pragma warning(disable: 4996)

#include <iostream>
#include <Windows.h>
#include <ctime>


extern "C"
{
	// Длина строки
	int lenght(char* str)										
	{
		return strlen(str);
	}

	// Вывод числа
	int write_int(int p)											
	{
		std::cout << p << std::endl;
		return 0;
	}

	// Вывод строки
	int write_str(char* str)										
	{
		setlocale(LC_ALL, "rus");
		std::cout << str << std::endl;
		return 0;
	}

	char* copy(char* str1, char* str2, int count)
	{
		int i;
		str1 = (char*)malloc(count);
		for (i = 0; i < count; i++) {
			str1[i] = str2[i];
		}
		str1[i] = '\0';

		return str1;
	}

	// Возведение числа в степень
	int powNumber(int num, int num2)
	{
		return pow(num, num2);
	}

	// Получить случайное число в промежутке
	int random(int start, int end)
	{
		srand(time(NULL));
		return rand() % end + start;
	}

	// Получить факториал числа
	int factorialOfNumber(int num)
	{
		int res = 1;
		for (int i = 1; i <= num; i++) {
			res = res * i;
		}
		return res;
	}

	// Извлечь корень
	int squareOfNumber(int num)
	{
		return sqrt(num);
	}
}