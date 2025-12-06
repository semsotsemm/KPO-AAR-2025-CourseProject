#include "Generator.h"
#include <string>
#include <vector>
#include <stack>
#include <iostream>

using namespace std;

// Предполагаем, что определения LEX_... и IT::... существуют в LT.h/IT.h

namespace GN
{
	// ... (GetUniqueLiteralName остается без изменений)
	string GetUniqueLiteralName(const IT::Entry& entry, int index)
	{
		if (entry.iddatatype == IT::INT) {
			return "Lnum_N" + to_string(index);
		}
		else if (entry.iddatatype == IT::STR) {
			return "Lstr_S" + to_string(index);
		}
		else if (entry.iddatatype == IT::BOOL) {
			return "Lbool_B" + to_string(index);
		}
		return "L_Unknown_" + to_string(index);
	}

	// Структура для хранения меток циклов и условий
	struct LabelInfo {
		int labelID;
		bool hasElse; // Есть ли ветка else
	};

	void GenConstAndData(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file);
	void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file);
	string GenExpression(LT::LexTable& lextable, IT::IdTable& idtable, int& i);

	void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream)
	{
		*stream << BEGIN;

		// 1. Генерация сегмента констант и данных
		GenConstAndData(lextable, idtable, stream);

		// 2. Генерация сегмента кода
		*stream << CODE_SEG;
		GenCode(lextable, idtable, stream);
	}

	void GenConstAndData(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file)
	{
		// Стандартные константы
		*file << "\tnewline byte 13, 10, 0\n";
		*file << "\ttrue_str byte 'true', 0\n";
		*file << "\tfalse_str byte 'false', 0\n";

		int literalIndex = 0;

		// Генерация литералов (сегмент .const)
		for (int i = 0; i < idtable.size; i++)
		{
			if (idtable.table[i].idtype == IT::L) // Литерал
			{
				string name = GetUniqueLiteralName(idtable.table[i], literalIndex++);

				if (idtable.table[i].iddatatype == IT::INT) {
					*file << "\t" << name << " sdword " << idtable.table[i].value.vint << "\n";
				}
				else if (idtable.table[i].iddatatype == IT::STR) {
					*file << "\t" << name << " byte '" << idtable.table[i].value.vstr.str << "', 0\n";
				}
				else if (idtable.table[i].iddatatype == IT::BOOL) {
					*file << "\t" << name << " dword " << idtable.table[i].value.vint << "\n";
				}
			}
		}

		*file << DATA_SEG;
		// Генерация переменных (сегмент .data)
		for (int i = 0; i < idtable.size; i++)
		{
			// Генерируем только переменные (IT::V)
			if (idtable.table[i].idtype == IT::V)
			{
				string name = idtable.table[i].id;
				// Переменные-указатели (строки)
				if (idtable.table[i].iddatatype == IT::STR) {
					*file << "\t" << name << " dword ?\n";
				}
				// Переменные-значения (Int, Bool)
				else {
					*file << "\t" << name << " sdword 0\n";
				}
			}
		}
	}

	void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file)
	{
		stack<LabelInfo> labelStack;
		int labelCounter = 0;
		string currentFuncName = "";
		int literalIndex = 0;

		for (int i = 0; i < lextable.size; i++)
		{
			char lex = lextable.table[i].lexema[0];

			// --- MAIN ---
			if (lex == LEX_MAIN) // 'm'
			{
				currentFuncName = "main";
				*file << "main PROC\n";
			}

			// --- ОБЪЯВЛЕНИЕ ФУНКЦИИ ---
			else if (lex == LEX_FUNCTION) // 'f'
			{
				// i+2 - ID функции
				i += 2;
				string funcName = idtable.table[lextable.table[i].idxTI].id;
				currentFuncName = funcName;
				*file << funcName << " PROC ";

				// Обработка параметров
				i += 2; // Пропускаем имя и '('
				bool firstParam = true;
				while (lextable.table[i].lexema[0] != LEX_RIGHTHESIS)
				{
					if (lextable.table[i].lexema[0] == LEX_ID)
					{
						if (!firstParam) *file << ", ";
						string paramName = idtable.table[lextable.table[i].idxTI].id;
						*file << paramName << " : DWORD"; // Все параметры передаем как 4 байта
						firstParam = false;
					}
					i++;
				}
				*file << "\n";
			}

			// --- ВОЗВРАТ (RETURN) ---
			else if (lex == LEX_RETURN) // 'r'
			{
				i++; // Переходим к выражению
				string expr = GenExpression(lextable, idtable, i);
				*file << expr;

				// Результат выражения должен быть в EAX для возврата.
				if (!expr.empty()) {
					*file << "\tpop eax\n"; // Результат возврата кладем в EAX
				}
				*file << "\tret\n";
			}

			// Конец функции или main '}' + ';'
			else if (lex == LEX_RIGHTBRACE && lextable.table[i + 1].lexema[0] == LEX_SEMICOLON)
			{
				if (currentFuncName != "main")
				{
					*file << currentFuncName << " ENDP\n";
				}
				else // Закрытие main
				{
					*file << "\tpush 0\n"; // Код возврата 0
					*file << "\tcall ExitProcess\n";
					*file << "main ENDP\n";
					*file << "end main\n";
				}
			}

			// --- PRINT ---
			else if (lex == LEX_PRINT) // 'p'
			{
				i++; // Следующий элемент - то, что печатаем

				// ИСПРАВЛЕНИЕ: Нужно определить тип *результата* выражения, а не только первого операнда.
				// Используем токен, чтобы найти идентификатор и его тип.

				int idxFirstOperand = lextable.table[i].idxTI;
				IT::Entry entry;
				if (idxFirstOperand != LT_TI_NULLIDX) {
					entry = idtable.table[idxFirstOperand];
				}
				// ВАЖНО: Если печатается результат выражения (например, f(a)+b),
				// здесь нельзя полагаться на entry.iddatatype, т.к. это будет только первый ID/LITERAL.
				// Однако, поскольку печать работает с результатом, который помещен в EAX,
				// мы можем использовать тип первого ID/LITERAL как индикатор (INT/STR).

				// Откатываемся к началу выражения для GenExpression
				i--;
				string exprCode = GenExpression(lextable, idtable, i);
				*file << exprCode;

				// Результат выражения в стеке (EAX/offset)
				*file << "\tpop eax\n";

				// Временное решение: использовать тип первого ID/LITERAL, чтобы определить, 
				// что печатать (строку или число). Это работает, если печатается одиночный ID/LITERAL.
				if (entry.iddatatype == IT::STR || (entry.idtype == IT::F && entry.iddatatype == IT::STR))
				{
					*file << "\tpush eax\n"; // EAX содержит адрес строки
					*file << "\tcall write_str\n";
				}
				else // INT, BOOL или результат арифметики
				{
					*file << "\tpush eax\n"; // EAX содержит числовое значение
					*file << "\tcall write_int\n";
				}
				*file << "\tpush offset newline\n\tcall write_str\n";

				// Пропускаем все токены до ; (GenExpression уже сделал i-- в конце)
			}

			// --- ПРИСВАИВАНИЕ (=) ---
			else if (lex == LEX_EQUAL && lextable.table[i + 1].lexema[0] != LEX_EQUAL) // '=' (но не '==')
			{
				// (i-1) - ID, куда пишем
				int targetIdx = lextable.table[i - 1].idxTI;
				string targetName = idtable.table[targetIdx].id;

				i++; // Переходим к выражению
				string code = GenExpression(lextable, idtable, i);
				*file << code;

				// Результат выражения лежит на вершине стека
				*file << "\tpop eax\n";
				*file << "\tmov " << targetName << ", eax\n";
			}

			// --- IF (УСЛОВИЕ) ---
			else if (lex == LEX_IF)
			{
				i++;
				string condCode = GenExpression(lextable, idtable, i);
				*file << condCode;

				int currentLabel = labelCounter++;
				labelStack.push({ currentLabel, false });

				*file << "\tpop eax\n"; // Результат условия (1 или 0)
				*file << "\tcmp eax, 0\n";
				*file << "\tje L_else_" << currentLabel << "\n";
			}

			// --- ELSE ---
			else if (lex == LEX_ELSE)
			{
				if (!labelStack.empty())
				{
					LabelInfo info = labelStack.top();
					// Безусловный переход в конец блока (после else)
					*file << "\tjmp L_end_" << info.labelID << "\n";
					// Метка для перехода, если условие IF было ложным (je L_else_X)
					*file << "L_else_" << info.labelID << ":\n";

					// Устанавливаем флаг hasElse, чтобы знать, где ставить L_end_X
					LabelInfo& currentInfo = labelStack.top();
					currentInfo.hasElse = true;
				}
				}

			// --- ЗАКРЫВАЮЩАЯ СКОБКА БЛОКА } (для IF/ELSE) ---
			else if (lex == LEX_RIGHTBRACE && lextable.table[i + 1].lexema[0] != LEX_SEMICOLON)
			{
				if (!labelStack.empty())
				{
					LabelInfo info = labelStack.top();

					// Проверяем, не является ли следующий токен 'ELSE'
					// В лексеме ОПЗ за закрывающей скобкой блока IF может идти 'e' (LEX_ELSE)
					bool nextIsElse = (lextable.size > i + 1) && (lextable.table[i + 1].lexema[0] == LEX_ELSE);

					if (nextIsElse)
					{
						// Если за IF следует ELSE, метки не ставим. ELSE-блок сам поставит L_else_X:.
					}
					else if (info.hasElse)
					{
						// Это конец блока ELSE. Ставим финальную метку.
						*file << "L_end_" << info.labelID << ":\n";
						labelStack.pop(); // Закрываем if/else
					}
					else // IF без ELSE
					{
						// Это конец блока IF, за которым нет ELSE. 
						// Сюда должен был перейти je L_else_X, поэтому ставим эту метку.
						*file << "L_else_" << info.labelID << ":\n";
						labelStack.pop(); // Закрываем if
					}
				}
				}

			// --- ВЫЗОВ ФУНКЦИИ В КАЧЕСТВЕ ОПЕРАТОРА (func(a,b);) ---
			else if (lex == LEX_ID && idtable.table[lextable.table[i].idxTI].idtype == IT::F)
			{
				i--;
				string funcCode = GenExpression(lextable, idtable, i);
				*file << funcCode;

				// Очищаем стек от возвращаемого значения (которое было запушено в GenExpression)
				*file << "\tpop ecx\n";

				// Пропускаем все токены до ;
				while (i < lextable.size && lextable.table[i].lexema[0] != LEX_SEMICOLON) {
					i++;
				}
				i--;
			}
		}
	}

	// Генерация кода для вычисления выражений (ОПЗ)
	string GenExpression(LT::LexTable& lextable, IT::IdTable& idtable, int& i)
	{
		string code = "";
		int paramCount = 0; // Считает, сколько операндов *выражения* запущено в стек

		for (; i < lextable.size; i++)
		{
			char lex = lextable.table[i].lexema[0];

			// Обнаружение конца выражения
			if (lex == LEX_SEMICOLON || lex == LEX_COLON || lex == LEX_RIGHTHESIS || lex == LEX_COMMA || lex == LEX_COLON)
			{
				i--;
				break;
			}

			if (lex == LEX_LITERAL || lex == LEX_ID)
			{
				int idx = lextable.table[i].idxTI;
				IT::Entry entry = idtable.table[idx];

				string name = string(entry.id);

				if (entry.idtype == IT::L) { // Литерал
					int currentLitIndex = 0;
					for (int j = 0; j < idtable.size; j++) {
						if (idtable.table[j].idtype == IT::L) {
							if (j == idx) break;
							currentLitIndex++;
						}
					}
					name = GetUniqueLiteralName(entry, currentLitIndex);
				}

				if (entry.idtype == IT::F) // Вызов функции
				{
					// Вызов функции (аргументы должны быть уже в стеке)
					code += "\tcall " + name + "\n";

					// ИСПРАВЛЕНИЕ: Очистка стека должна быть по числу параметров функции, 
					// а не по paramCount (который сбивается в сложных выражениях)
					int funcParamCount = entry.params.count;
					code += "\tadd esp, " + to_string(funcParamCount * 4) + "\n";

					// paramCount сбрасывается до 1, т.к. результат функции теперь является одним операндом
					paramCount = 1;

					code += "\tpush eax\n"; // Результат функции (в EAX) пушим в стек
				}
				else // Переменная (IT::V, IT::P) или Литерал (IT::L)
				{
					if (entry.idtype == IT::L && (entry.iddatatype == IT::INT || entry.iddatatype == IT::BOOL)) {
						code += "\tpush " + name + "\n";
					}
					else if (entry.idtype == IT::L && entry.iddatatype == IT::STR) {
						code += "\tpush offset " + name + "\n";
					}
					else { // IT::V или IT::P
						code += "\tpush " + name + "\n";
					}
					paramCount++; // Считаем, сколько операндов/параметров было запушено
				}
			}
			// --- АРИФМЕТИКА И ЛОГИКА ---
			else if (lex == LEX_PLUS)
			{
				code += "\tpop ebx\n\tpop eax\n\tadd eax, ebx\n\tpush eax\n";
				paramCount--;
			}
			else if (lex == LEX_MINUS)
			{
				code += "\tpop ebx\n\tpop eax\n\tsub eax, ebx\n\tpush eax\n";
				paramCount--;
			}
			else if (lex == LEX_STAR)
			{
				code += "\tpop ebx\n\tpop eax\n\timul ebx\n\tpush eax\n";
				paramCount--;
			}
			else if (lex == LEX_DIRSLASH)
			{
				code += "\tpop ebx\n\tpop eax\n\tcdq\n\tidiv ebx\n\tpush eax\n";
				paramCount--;
			}
			else if (lex == LEX_PECENT)
			{
				code += "\tpop ebx\n\tpop eax\n\tcdq\n\tidiv ebx\n\tpush edx\n";
				paramCount--;
			}
			// Логические операции
			else if (lex == LEX_EQUAL && lextable.table[i + 1].lexema[0] == LEX_EQUAL)
			{
				i++;
				code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n";
				code += "\tmov eax, 0\n\tsete al\n\tpush eax\n";
				paramCount--;
			}
		}
		return code;
	}
}