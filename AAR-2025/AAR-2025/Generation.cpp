	#include "stdafx.h"
	#include <vector>
	#include <stack>
	#include <sstream>
	#include "Generation.h"

	using namespace std;

	struct CaseInfo {
    string operandName; 
    string label;
    bool isDefault = false;
};

	vector<CaseInfo> findSwitchCases(LT::LexTable& lextable, IT::IdTable& idtable, int start_index, int switch_num)
	{
		vector<CaseInfo> cases;
		int depth = 1;
		int j = start_index;
		int case_idx = 0; // Порядковый номер кейса для уникальности метки

		while (j < lextable.current_size)
		{
			if (LT_ENTRY(j)->lexema == LEX_LEFTBRACE) depth++;
			else if (LT_ENTRY(j)->lexema == LEX_RIGHTBRACE) {
				depth--;
				if (depth == 0) break;
			}
			else if (depth == 1)
			{
				if (LT_ENTRY(j)->lexema == LEX_CASE)
				{
					// Проверяем и литерал, и переменную (ID)
					if (LT_ENTRY(j + 2)->lexema == LEX_LITERAL || LT_ENTRY(j + 2)->lexema == LEX_ID)
					{
						string opName = IT_ENTRY(j + 2)->FullName;
						cases.push_back({
							opName,
							"switch_case_" + GN::itoS(switch_num) + "_" + GN::itoS(case_idx++),
							false
							});
						j += 5; continue;
					}
				}
				else if (LT_ENTRY(j)->lexema == LEX_DEFAULT)
				{
					cases.push_back({ "", "switch_default_" + GN::itoS(switch_num), true });
					j += 1; continue;
				}
			}
			j++;
		}
		return cases;
	}

	namespace GN
	{
		void GenerationASM(std::ostream* stream, LT::LexTable& lextable, IT::IdTable& idtable)
		{
			ostream* file = stream;
			*file << BEGIN;
			*file << EXTERN;
			*file << STACK(4096);
			GenConstAndData(idtable, file);
			*file << CODE;
			GenCode(lextable, idtable, file);
			*file << END;
		}

		void GenConstAndData(IT::IdTable& idtable, ostream* file)
		{
			vector <string> result;
			vector <string> con;  con.push_back(CONST);
			vector <string> data;  data.push_back(DATA);

			for (int i = 0; i < idtable.current_size; i++)
			{
				string str = "\t" + idtable.table[i]->FullName;

				if (idtable.table[i]->idtype == IT::IDTYPE::L)	// литерал - в .const
				{
					switch (idtable.table[i]->iddatatype)
					{
					case IT::IDDATATYPE::INT:  str += " sdword " + itoS(idtable.table[i]->value.vint);  break;
					case IT::IDDATATYPE::BOOL: 
						str += " sdword " + itoS(idtable.table[i]->value.vint);
						break;
					case IT::IDDATATYPE::STR:  str += " byte " + string(idtable.table[i]->value.vstr.str) + ", 0";  break;
					}
					con.push_back(str);
				}
				else if (idtable.table[i]->idtype == IT::IDTYPE::V)	// переменная - в .data
				{
					switch (idtable.table[i]->iddatatype)
					{
					case IT::IDDATATYPE::INT: str += " sdword 0";  break; //sdword 0 - 4 байта со знаком
					case IT::IDDATATYPE::BOOL: // <-- ДОБАВЛЕНО
						str += " sdword 0"; // sdword 0 - 4 байта со знаком
						break;
					case IT::IDDATATYPE::STR: str += " dword ?";  break; //dword - 4 байта без знака
					}
					data.push_back(str);
				}
			}
			result.insert(result.end(), con.begin(), con.end());  //result.push_back("\n");
			result.insert(result.end(), data.begin(), data.end()); //result.push_back("\n");
			for (auto r : result)
				*file << r << endl;
		}

		void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file)
		{
			string str;
			string funcName;	// имя текущей функции
			int branchingnNum = -1, open = 0;
			stack<A> kol;
			stack<B> kolSwitch; 

			for (int i = 0; i < lextable.current_size; i++)
			{
				switch (LT_ENTRY(i)->lexema)
				{
				case LEX_MAIN:
				{
					str = "main PROC\n";
					open++;
					break;
				}
				case LEX_FUNCTION:
				{
					funcName = IT_ENTRY(i + 1)->FullName;
					str = GenFunctionCode(lextable, idtable, i);
					break;
				}
				case LEX_RETURN:
				{
					str = GenExitCode(lextable, idtable, i, funcName);
					break;
				}
				case LEX_ID: // вызов функции
				{
					if (LT_ENTRY(i + 1)->lexema == LEX_LEFTHESIS && LT_ENTRY(i + 1)->lexema != LEX_FUNCTION) // не объявление, а вызов
						str = GenCallFuncCode(lextable, idtable, i);
					break;
				}
				case LEX_IF: // условие
				{
					branchingnNum++;
					str = GenBranchingCode(lextable, idtable, i, branchingnNum);
					break;
				}
				case LEX_LEFTBRACE:
				{
					open++;
					break;
				}
				case LEX_RIGHTBRACE:	// переход на метку в конце кондишна
				{
					open--;
					if (LT_ENTRY(i + 1)->lexema == LEX_ELSE)
					{
						kol.pop();
						kol.push(A(open, branchingnNum, IfEnum::thenOrElse));
						str += "jmp next" + itoS(kol.top().branchingnNum) + '\n';
					}
					else
						if (!kol.empty())
							if (kol.top().openRightbrace == open)
							{
								if (kol.top().ifEnum == IfEnum::repeat)
								{
									str += "jmp cyclenext" + itoS(kol.top().branchingnNum) + '\n';
									str += "cycle" + itoS(kol.top().branchingnNum) + ":\n";
									kol.pop();
								}
								else
								{
									str += "next" + itoS(kol.top().branchingnNum) + ":\n";
									kol.pop();
								}
							}
					if (!kolSwitch.empty())
					{
						if (kolSwitch.top().openRightbrace == open + 2)
						{
							str += "switch_end_" + itoS(kolSwitch.top().branchingNum) + ":\n";
							kolSwitch.pop();
						}
					}
					break;
				}
				case LEX_THEN: // условие верно (метка)
				{
					kol.push(A(open, branchingnNum, IfEnum::thenOrElse));
					str += "true" + itoS(branchingnNum) + ":";
					break;
				}
				case LEX_ELSE: // условие неверно(метка)
				{
					str += "false" + itoS(branchingnNum) + ":";
					break;
				}
				case LEX_REPEAT: // цикл с условием (метка)
				{
					branchingnNum++;
					kol.push(A(open, branchingnNum, IfEnum::repeat));
					str += "cyclenext" + itoS(kol.top().branchingnNum) + ":\n";
					str += GenBranchingCode(lextable, idtable, i, kol.top().branchingnNum);

					break;
				}

				case LEX_SWITCH:
				{
					branchingnNum++;
					kolSwitch.push(B(open, branchingnNum, SwitchEnum::isSwitch));
					int switch_num = kolSwitch.top().branchingNum;

					IT::Entry* e_id = IT_ENTRY(i + 2);
					str += "mov eax, " + string(e_id->FullName) + "\n";

					vector<CaseInfo> case_list = findSwitchCases(lextable, idtable, i + 5, switch_num);

					bool has_default = false;
					for (const auto& case_info : case_list)
					{
						if (!case_info.isDefault)
						{
							str += "cmp eax, " + case_info.operandName + "\n";
							str += "je " + case_info.label + "\n";
						}
						else has_default = true;
					}

					if (has_default) str += "jmp switch_default_" + itoS(switch_num) + "\n";
					else str += "jmp switch_end_" + itoS(switch_num) + "\n";

					i += 3;
					break;
				}
				case LEX_CASE:
				{
					if (kolSwitch.empty()) break;

					int switch_num = kolSwitch.top().branchingNum;
					int case_idx = kolSwitch.top().currentCaseIdx++;


					str += "switch_case_" + itoS(switch_num) + "_" + itoS(case_idx) + ":\n";

					i += 5;
					break;
				}

				case LEX_BREAK:
				{
					if (!kolSwitch.empty())
					{
						str += "jmp switch_end_" + itoS(kolSwitch.top().branchingNum) + "\n";
					}
					break;
				}

				case LEX_DEFAULT:
				{
					if (kolSwitch.empty()) break;
					str += "switch_default_" + itoS(kolSwitch.top().branchingNum) + ":\n";
					i += 1;
					break;
				}
				case LEX_EQUAL: // присваивание (вычисление выражений)
				{
					PN::polishNotation(i, lextable, idtable);
					str = GenEqualCode(lextable, idtable, i);
					break;
				}
				case LEX_WRITE: // вывод
				{
					IT::Entry* e = IT_ENTRY(i + 1);
					switch (e->iddatatype)
					{
					case IT::IDDATATYPE::INT:
					case IT::IDDATATYPE::BOOL:
						str += "push " + string(e->FullName) + "\ncall write_int\n";
						break;
					case IT::IDDATATYPE::STR:
						if (e->idtype == IT::IDTYPE::L)  str += "\npush offset " + string(e->FullName) + "\ncall write_str\n";
						else  str += "push " + string(e->FullName) + "\ncall write_str\n";
						break;
					}
					break;
				}
				}

				if (!str.empty())
				{
					*file << str << endl;
					str.clear();
				}
			}
		}

		string GenEqualCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i)
		{
			string str;
			IT::Entry* e1 = IT_ENTRY(i - 1); // левый операнд
			i++;
			switch (e1->iddatatype)
			{
			case IT::IDDATATYPE::INT:
			case IT::IDDATATYPE::BOOL:
			{
				// --- 1. Вычисление выражения (результат в стеке) ---
				for (; LT_ENTRY(i)->lexema != LEX_SEMICOLON; i++)
				{
					switch (LT_ENTRY(i)->lexema)
					{
					case LEX_LITERAL:
					case LEX_ID:
					{
						if (IT_ENTRY(i)->idtype == IT::IDTYPE::F) // если в выражении вызов функции
						{
							str = str + GenCallFuncCode(lextable, idtable, i); // функция возвращает результат в eax
							str = str + "push eax\n";				// результат выражения в стек для дальнейшего вычисления выражения
							break;
						}
						else
							str = str + "push " + IT_ENTRY(i)->FullName + "\n";
						break;
					}
					case LEX_OPERATOR:
						switch (LT_ENTRY(i)->sign)
						{
						case '+':
							str += "pop ebx\npop eax\nadd eax, ebx\npush eax\n"; break;
						case '-':
							str += "pop ebx\npop eax\nsub eax, ebx\npush eax\n"; break;
						case '*':
							str += "pop ebx\npop eax\nimul eax, ebx\npush eax\n"; break;
						case '/':
							str += "pop ebx\npop eax\ncmp nul, ebx\ncdq\nidiv ebx\npush eax\n"; break;
						case '%':
							str += "pop ebx\npop eax\ncdq\nidiv ebx\npush edx\n"; break;
						}
					}
				}
				// --- Конец вычисления. Результат в стеке. ---

				// --- 2. Извлечение результата из стека в EAX ---
				str += "pop eax\n"; // <--- Теперь эта команда извлечет РЕЗУЛЬТАТ выражения.

				// ЕСЛИ ЭТО INT, ТО ЗДЕСЬ НАДО СРАЗУ СОХРАНЯТЬ
				if (e1->iddatatype == IT::IDDATATYPE::INT)
				{
					str += "mov " + string(e1->FullName) + ", eax\n";
					break;
				}

				// --- 3. Приведение значения к 0 или 1 (Только для BOOL) ---

				// Примечание: Эта логика приведения нужна, если вы разрешаете присваивать INT переменным BOOL, 
				// например, bool_var = 100 * 20;

				string bool_label_prefix = "bool_conv_" + GN::itoS(i);

				str += "cmp eax, 0\n";
				str += "je " + bool_label_prefix + "_false\n"; // Если равно 0, переходим к false

				// Если не 0 (true):
				str += "mov eax, 1\n"; // Устанавливаем EAX = 1
				str += "jmp " + bool_label_prefix + "_end\n";

				// Если 0 (false):
				str += bool_label_prefix + "_false:\n";
				str += "mov eax, 0\n"; // Оставляем EAX = 0

				// Завершение
				str += bool_label_prefix + "_end:\n";

				// 4. Сохранение результата (0 или 1)
				str += "mov " + string(e1->FullName) + ", eax\n";
				break;
			}
			case IT::IDDATATYPE::STR:// разрешить присваивать строкам только строки, литералы и вызовы функций
			{
				char lex = LT_ENTRY(i)->lexema;
				IT::Entry* e2 = IT_ENTRY(i);
				if (lex == LEX_ID && (e2->idtype == IT::IDTYPE::F)) // вызов функции
				{
					str += GenCallFuncCode(lextable, idtable, i);
					str += "mov " + string(e1->FullName) + ", eax";
				}
				else if (lex == LEX_LITERAL) // литерал
				{
					str = +"mov " + string(e1->FullName) + ", offset " + string(e2->FullName);
				}
				else // ид(переменная) - через регистр
				{
					str += "mov ecx, " + string(e2->FullName) + "\nmov " + string(e1->FullName) + ", ecx";
				}
			}
			}
			return str;
		}

		string GenFunctionCode (LT::LexTable& lextable, IT::IdTable& idtable, int& i)
		{
			string str = "";

			str += string(IT_ENTRY(i + 1)->FullName) + string(" PROC,\t");
			//дальше параметры
			i += 3; // начало - то что сразу после открывающей скобки

			while (LT_ENTRY(i)->lexema != LEX_RIGHTHESIS) // пока параметры не кончатся
			{
				if (LT_ENTRY(i)->lexema == LEX_ID) // параметр
					str += string(IT_ENTRY(i)->FullName) + (IT_ENTRY(i)->iddatatype == IT::IDDATATYPE::INT ? " : sdword, " : " : dword, ");
				i++;
			}
			int f = str.rfind(',');
			if (f > 0)
				str[f] = ' ';

			str += "\npush ebx\npush edx\n";

			return str;
		}

		string GenExitCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, string funcname)
		{
			string str = "pop edx\npop ebx\n\n";
			if (LT_ENTRY(i + 1)->lexema != LEX_SEMICOLON)	// выход из функции (вернуть значение)
			{
				str += "mov eax, " + string(IT_ENTRY(i + 1)->FullName) + "\n";
			}
			str += "ret\n";
			str += funcname + " ENDP";
			return str;
		}

		string GenCallFuncCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i)
		{
			string str;

			IT::Entry* e = IT_ENTRY(i); // идентификатор вызываемой функции
			stack <IT::Entry*> temp;

			for (i++; LT_ENTRY(i)->lexema != '@'; i++)
			{
				if (LT_ENTRY(i)->lexema == LEX_ID || LT_ENTRY(i)->lexema == LEX_LITERAL)
					temp.push(IT_ENTRY(i)); // // заполняем стек в прямом порядке	
			}

			// раскручиваем стек
			while (!temp.empty())
			{
				if (temp.top()->idtype == IT::IDTYPE::L && temp.top()->iddatatype == IT::IDDATATYPE::STR)
					str += "push offset " + string(temp.top()->FullName) + "\n";
				else   str += "push " + string(temp.top()->FullName) + "\n";
				temp.pop();
			}

			str += "call " + string(e->FullName) + '\n';
			i++;

			return str;
		}

		string GenBranchingCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, int branchingnNum)
		{
			string str;
			IT::Entry* lft = IT_ENTRY(i + 2); // Левый операнд (всегда i+2)

			// --- Параметры для динамического определения ---
			IT::Entry* rgt = nullptr;
			char comparison_lexema = '\0';
			bool is_simple_bool_check = false;

			// --- 1. Определение типа условия и получение операндов/оператора ---

			// Проверяем, является ли условие простым if(переменная)
			if (LT_ENTRY(i + 3)->lexema == LEX_RIGHTHESIS)
			{
				is_simple_bool_check = true;
				// Оператор: неявно != 0 (LEX_NOTEQUALS)
				comparison_lexema = LEX_NOTEQUALS;
			}
			else // Сложное условие: if(a OP b)
			{
				rgt = IT_ENTRY(i + 4); // Правый операнд (i+4)
				comparison_lexema = LT_ENTRY(i + 3)->lexema; // Оператор сравнения (i+3)
			}

			// --- 2. Генерация кода сравнения в ASM ---

			if (is_simple_bool_check)
			{
				// Сценарий: if(переменная) -> mov edx, [переменная]; cmp edx, 0
				str += "mov edx, " + lft->FullName + "\n";
				str += "cmp edx, 0\n"; // Сравниваем с нулем
			}
			else
			{
				// Сценарий: if(a OP b) -> mov edx, [a]; cmp edx, [b]
				str += "mov edx, " + lft->FullName + "\n";
				str += "cmp edx, " + rgt->FullName + "\n";
			}

			// --- 3. Определение команд перехода (tstr, fstr) ---

			string tstr, fstr;

			// Если это простое булево условие, используем команды jnz/jz,
			// которые соответствуют логике "не равно нулю" (True) и "равно нулю" (False)
			if (is_simple_bool_check)
			{
				tstr = "jnz";
				fstr = "jz";
			}
			else
			{
				// Используем стандартный switch для сложных условий
				switch (comparison_lexema)
				{
				case LEX_MORE:		tstr = "jg";   fstr = "jle";   break; // JLE - более надежная инверсия, чем jl
				case LEX_LESS:		tstr = "jl";   fstr = "jge";   break; // JGE - более надежная инверсия
				case LEX_COMPARE:	tstr = "jz";   fstr = "jnz";   break;
				case LEX_NOTEQUALS:	tstr = "jnz";   fstr = "jz";   break;
				}
			}

			// --- 4. Генерация переходов ---

			bool has_then = false, has_else = false;
			// Сдвиг для пропуска условия: 4 токена для if(var), 6 для if(a op b)
			int start_j = i + (is_simple_bool_check ? 4 : 6);

			// Поиск меток THEN/ELSE в теле условия
			for (int j = start_j; LT_ENTRY(j - 2)->lexema != LEX_RIGHTBRACE; j++)
			{
				if (LT_ENTRY(j)->lexema == LEX_THEN)
					has_then = true;
				if (LT_ENTRY(j)->lexema == LEX_ELSE)
					has_else = true;
			}

			if (LT_ENTRY(i)->lexema != LEX_REPEAT)
			{
				// IF/ELSE
				if (has_then)
					str += "\n" + tstr + " true" + itoS(branchingnNum);

				if (has_else)
					str += "\n" + fstr + " false" + itoS(branchingnNum);

				// Если никуда не перешли, прыгаем в конец
				if (!has_then || !has_else)
					str += "\njmp next" + itoS(branchingnNum);
			}
			else
			{
				// REPEAT (цикл)
				// При FALSE-условии нужно выйти из цикла.
				str += fstr + " cycle" + itoS(branchingnNum);
			}

			return str;
		}

		string itoS(int x) //чтобы избежать дублирования меток
		{
			stringstream r;  r << x;  return r.str();
		}
	}