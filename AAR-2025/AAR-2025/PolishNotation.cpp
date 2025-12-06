#include "PolishNotation.h"
#include <stack>
#include <queue>
#include <iostream>

using namespace std;

namespace PN
{
	int priority(char v);

	bool polishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable)
	{
		stack<LT::Entry> stk;
		queue<LT::Entry> result;
		bool function = false;
		int quantityParm = 0;
		int i = lextable_pos; // Индекс входа в выражение (после '=')

		// 1. ПРЕОБРАЗОВАНИЕ В ОПЗ
		for (; i < lextable.size && lextable.table[i].lexema[0] != LEX_SEMICOLON &&
			(lextable.table[i].lexema[0] != LEX_RIGHTHESIS || !stk.empty()); i++)
		{
			char current_lexema = lextable.table[i].lexema[0];

			switch (current_lexema)
			{
			case LEX_ID:
			case LEX_LITERAL:
			case LEX_TRUE:
			case LEX_FALSE:
				// Если это функция
				if (current_lexema == LEX_ID && idtable.table[lextable.table[i].idxTI].idtype == IT::IDTYPE::F)
				{
					quantityParm = 0;
					function = true;
					result.push(lextable.table[i]);
				}
				else
				{
					// Если это параметр функции
					if (function && !quantityParm)
						quantityParm++;
					result.push(lextable.table[i]);
				}
				break;

				// ИСПРАВЛЕНО: Каждый case должен быть отдельно
			case LEX_PLUS:
			case LEX_MINUS:
			case LEX_STAR:
			case LEX_PECENT:
			case LEX_DIRSLASH:
			case '<':  // Добавлены операторы сравнения
			case '>':
			{
				int current_priority = priority(current_lexema);

				while (!stk.empty() &&
					stk.top().lexema[0] != LEX_LEFTHESIS &&
					priority(stk.top().lexema[0]) >= current_priority)
				{
					result.push(stk.top());
					stk.pop();
				}
				stk.push(lextable.table[i]);
			}
			break;

			case LEX_LEFTHESIS:
				stk.push(lextable.table[i]);
				break;

			case LEX_RIGHTHESIS:
				// Выталкиваем всё до открывающей скобки
				while (!stk.empty() && stk.top().lexema[0] != LEX_LEFTHESIS)
				{
					result.push(stk.top());
					stk.pop();
				}

				// Удаляем открывающую скобку '(' из стека (она не идет в результат)
				if (!stk.empty())
					stk.pop();
				else
					return false; // Ошибка: лишняя закрывающая скобка

				if (function)
				{
					// Добавляем служебные символы для функции
					result.push(LT::Entry('@', lextable.table[i].sn));
					result.push(LT::Entry('0' + quantityParm, lextable.table[i].sn));
					function = false;
				}
				break;

			case LEX_COMMA:
				if (function)
					quantityParm++;
				while (!stk.empty() && stk.top().lexema[0] != LEX_LEFTHESIS)
				{
					result.push(stk.top());
					stk.pop();
				}
				break;

				// Игнорируем другие символы, если попадутся (например, пробелы, если они есть)
			default:
				break;
			}
		}

		// Выталкиваем оставшиеся операции
		while (!stk.empty())
		{
			if (stk.top().lexema[0] == LEX_LEFTHESIS) return false; // Ошибка скобок
			result.push(stk.top());
			stk.pop();
		}

		// ----------------------------------------------------------------
		// 2. ЗАПИСЬ РЕЗУЛЬТАТА И ОЧИСТКА МУСОРА
		// ----------------------------------------------------------------

		int j_current = lextable_pos; // Начинаем перезапись с начала выражения

		while (!result.empty())
		{
			LT::Entry entry_ptr = result.front();

			// Перезаписываем лексему
			lextable.table[j_current] = entry_ptr;

			result.pop();
			j_current++;
		}

		// Затираем пробелами всё, что осталось от старого выражения до точки с запятой (i)
		// Это удаляет лишние скобки, запятые и операнды, которые ушли в ОПЗ
		for (int j = j_current; j < i; j++)
		{
			lextable.table[j] = LT::Entry();     // Обнуляем
			lextable.table[j].lexema[0] = ' ';   // Ставим пробел (игнорируется при выводе)
			lextable.table[j].sn = lextable.table[i].sn; // Сохраняем номер строки для красоты
		}

		return true;
	}

	int priority(char v)
	{
		switch (v)
		{
		case '(':
		case ')': return 1;
		case ',': return 2;
		case '+':
		case '-': return 3;
		case '*':
		case '/':
		case '%': return 4;
		case '<':
		case '>': return 0; // Низкий приоритет для сравнения
		}
		return 0;
	}

	int findExpression(LT::LexTable& lextable)
	{
		for (static int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].lexema[0] == LEX_EQUAL)
				return ++i;
		}
		return 0;
	}
}