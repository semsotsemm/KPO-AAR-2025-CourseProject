#include "Generator.h"
#include "LT.h"
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>

using namespace std;


namespace GN
{
	void GenConstAndData(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file);
	void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file);
	string GenExpression(LT::LexTable& lextable, IT::IdTable& idtable, int& i);

	string GetUniqueLiteralName(const IT::Entry& entry, int index)
	{
		if (entry.iddatatype == IT::INT) return "Lnum_N" + to_string(index);
		else if (entry.iddatatype == IT::STR) return "Lstr_S" + to_string(index);
		else if (entry.iddatatype == IT::BOOL) return "Lbool_B" + to_string(index);
		return "L_Unknown_" + to_string(index);
	}

	void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream)
	{
		*stream << BEGIN;
		GenConstAndData(lextable, idtable, stream);
		*stream << CODE_SEG;
		GenCode(lextable, idtable, stream);
	}

	void GenConstAndData(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file)
	{
		*file << ".const\n";
		*file << "\tnewline byte 13, 10, 0\n";
		*file << "\ttrue_str byte 'true', 0\n";
		*file << "\tfalse_str byte 'false', 0\n";

		int literalIndex = 0;
		for (int i = 0; i < idtable.size; i++)
		{
			if (idtable.table[i].idtype == IT::L)
			{
				string name = GetUniqueLiteralName(idtable.table[i], literalIndex++);
				if (idtable.table[i].iddatatype == IT::INT)
				{
					*file << "\t" << name << " sdword " << idtable.table[i].value.vint << "\n";
				}
				else if (idtable.table[i].iddatatype == IT::STR)
				{
					*file << "\t" << name << " byte '" << idtable.table[i].value.vstr.str << "', 0\n";
				}
				else if (idtable.table[i].iddatatype == IT::BOOL)
				{
					*file << "\t" << name << " dword " << idtable.table[i].value.vint << "\n";
				}
			}
		}

		*file << DATA_SEG;
		*file << "\tswitch_res sdword 0\n";

		for (int i = 0; i < idtable.size; i++)
		{
			if (idtable.table[i].idtype == IT::V)
			{
				string name = idtable.table[i].id;
				if (idtable.table[i].iddatatype == IT::STR)
				{
					*file << "\t" << name << " dword ?\n";
				}
				else
				{
					*file << "\t" << name << " sdword 0\n";
				}
			}
		}
	}

	void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file)
	{
		stack<LabelInfo> labelStack;
		int globalLabelCounter = 0;
		string currentFuncName = "";

		for (int i = 0; i < lextable.size; i++)
		{
			char lex = lextable.table[i].lexema[0];

			if (lex == LEX_MAIN)
			{
				currentFuncName = "main";
				*file << "main PROC\n";
			}
			else if (lex == LEX_FUNCTION)
			{
				i += 2;
				string funcName = idtable.table[lextable.table[i].idxTI].id;
				currentFuncName = funcName;
				*file << funcName << " PROC ";
				i += 2;
				bool firstParam = true;
				while (lextable.table[i].lexema[0] != LEX_RIGHTHESIS)
				{
					if (lextable.table[i].lexema[0] == LEX_ID)
					{
						if (!firstParam) *file << ", ";
						string paramName = idtable.table[lextable.table[i].idxTI].id;
						*file << paramName << " : DWORD";
						firstParam = false;
					}
					i++;
				}
				*file << "\n";
			}
			else if (lex == LEX_RETURN)
			{
				i++;
				string expr = GenExpression(lextable, idtable, i);
				*file << expr;
				if (!expr.empty()) *file << "\tpop eax\n";
				*file << "\tret\n";
			}

			// switch
			else if (lex == LEX_SWITCH)
			{
				i++; 
				string exprCode = GenExpression(lextable, idtable, i);
				*file << exprCode;

				*file << "\tpop eax\n";
				*file << "\tmov switch_res, eax\n";

				int swID = globalLabelCounter++;
				labelStack.push({ swID, L_SWITCH, -1 });
			}

			// case
			else if (lex == LEX_CASE)
			{
				i++; 

				string operandName = "";
				int idx = lextable.table[i].idxTI;

				if (lextable.table[i].lexema[0] == LEX_LITERAL)
				{
					int currentLitIndex = 0;
					for (int k = 0; k < idtable.size; k++) {
						if (idtable.table[k].idtype == IT::L) {
							if (k == idx) break;
							currentLitIndex++;
						}
					}
					operandName = GetUniqueLiteralName(idtable.table[idx], currentLitIndex);
				}
				else if (lextable.table[i].lexema[0] == LEX_ID)
				{
					operandName = idtable.table[idx].id;
				}

				*file << "\tmov eax, switch_res\n";
				*file << "\tcmp eax, " << operandName << "\n";

				int caseFailLabel = globalLabelCounter++;
				int parentSwitch = -1;

				stack<LabelInfo> temp = labelStack;
				while (!temp.empty()) {
					if (temp.top().type == L_SWITCH) { parentSwitch = temp.top().id; break; }
					temp.pop();
				}

				*file << "\tjne L_case_fail_" << caseFailLabel << "\n";

				labelStack.push({ caseFailLabel, L_CASE, parentSwitch });

			}

			// default
			else if (lex == LEX_DEFAULT)
			{
				int defaultID = globalLabelCounter++;
				int parentSwitch = -1;
				stack<LabelInfo> temp = labelStack;
				while (!temp.empty()) {
					if (temp.top().type == L_SWITCH) { parentSwitch = temp.top().id; break; }
					temp.pop();
				}

				labelStack.push({ defaultID, L_DEFAULT, parentSwitch });
			}

			// }
			else if (lex == LEX_RIGHTBRACE)
			{
				bool isEndOfFunc = (i + 1 < lextable.size && lextable.table[i + 1].lexema[0] == LEX_SEMICOLON);

				if (isEndOfFunc)
				{
					if (currentFuncName == "main") {
						*file << "\tpush 0\n\tcall ExitProcess\nmain ENDP\nend main\n";
					}
					else {
						*file << currentFuncName << " ENDP\n";
					}
				}
				else if (!labelStack.empty())
				{
					LabelInfo info = labelStack.top();
					labelStack.pop();

					if (info.type == L_IF)
					{
						bool nextIsElse = (lextable.size > i + 1) && (lextable.table[i + 1].lexema[0] == LEX_ELSE);
						if (nextIsElse) {
							*file << "\tjmp L_end_if_" << info.id << "\n";
							*file << "L_else_" << info.id << ":\n";
							labelStack.push(info);
						}
						else {
							*file << "L_end_if_" << info.id << ":\n";	
						}
					}
					else if (info.type == L_CASE)
					{
						*file << "\tjmp L_switch_end_" << info.parentSwitchID << "\n";
						*file << "L_case_fail_" << info.id << ":\n";
					}
					else if (info.type == L_DEFAULT)
					{
						*file << "\tjmp L_switch_end_" << info.parentSwitchID << "\n";
					}
					else if (info.type == L_SWITCH)
					{
						*file << "L_switch_end_" << info.id << ":\n";
					}
				}
			}

			// if-else
			else if (lex == LEX_IF)
			{
				i++;
				string cond = GenExpression(lextable, idtable, i);
				*file << cond;
				int labelID = globalLabelCounter++;
				labelStack.push({ labelID, L_IF, -1 });

				*file << "\tpop eax\n\tcmp eax, 0\n";
				*file << "\tje L_else_" << labelID << "\n";
			}
			
			else if (lex == LEX_PRINT)
			{
				i++;
				int idxFirstOperand = lextable.table[i].idxTI;
				IT::Entry entry;
				if (idxFirstOperand != LT_TI_NULLIDX) entry = idtable.table[idxFirstOperand];
				i--;
				string exprCode = GenExpression(lextable, idtable, i);
				*file << exprCode;
				*file << "\tpop eax\n";

				if (entry.iddatatype == IT::STR || (entry.idtype == IT::F && entry.iddatatype == IT::STR))
				{
					*file << "\tpush eax\n\tcall write_str\n";
				}
				else
				{
					*file << "\tpush eax\n\tcall write_int\n";
				}
				*file << "\tpush offset newline\n\tcall write_str\n";
			}

			// Присваивание
			else if (lex == LEX_EQUAL && lextable.table[i + 1].lexema[0] != LEX_EQUAL)
			{
				int targetIdx = lextable.table[i - 1].idxTI;
				string targetName = idtable.table[targetIdx].id;
				i++;
				string code = GenExpression(lextable, idtable, i);
				*file << code;
				*file << "\tpop eax\n\tmov " << targetName << ", eax\n";
			}

			// Вызов функции
			else if (lex == LEX_ID && idtable.table[lextable.table[i].idxTI].idtype == IT::F)
			{
				int funcIdx = lextable.table[i].idxTI;
				int argCount = idtable.table[funcIdx].params.count;

				string funcCode = GenExpression(lextable, idtable, i);
				*file << funcCode;

				if (argCount > 0) *file << "\tadd esp, " << argCount * 4 << "\n";

				*file << "\tpop ecx\n";

				while (i < lextable.size && lextable.table[i].lexema[0] != LEX_SEMICOLON) i++;
				i--;
			}
		}
	}

	string GenExpression(LT::LexTable& lextable, IT::IdTable& idtable, int& i)
	{
		string code = "";
		for (; i < lextable.size; i++)
		{
			char lex = lextable.table[i].lexema[0];
			if (lex == LEX_SEMICOLON || lex == LEX_RIGHTHESIS || lex == LEX_COMMA || lex == LEX_LEFTBRACE || lex == LEX_COLON)
			{
				i--; 
				break;
			}
			if (lex == LEX_LITERAL || lex == LEX_ID)
			{
				int idx = lextable.table[i].idxTI;
				IT::Entry entry = idtable.table[idx];
				string name = string(entry.id);
				
				if (entry.idtype == IT::F)
				{
					int paramCount = entry.params.count;
					vector<string> paramCodes;

					int k = i;
					while (k < lextable.size && lextable.table[k].lexema[0] != LEX_RIGHTHESIS) k++;

					int param_i = i + 2;
					for (int arg = 0; arg < paramCount; arg++)
					{
						paramCodes.push_back(GenExpression(lextable, idtable, param_i));

						if (arg < paramCount - 1 && lextable.table[param_i + 1].lexema[0] == LEX_COMMA)
							param_i += 2;
						else if (lextable.table[param_i + 1].lexema[0] == LEX_RIGHTHESIS)
							param_i++;
					}

					std::reverse(paramCodes.begin(), paramCodes.end());
					for (const string& paramCode : paramCodes)
					{
						code += paramCode;
					}

					string funcName = string(entry.id);
					code += "\tcall " + funcName + "\n";

					code += "\tpush eax\n";

					i = k;
				}
				else
				{
					if (entry.idtype == IT::L)
					{
						int currentLitIndex = 0;
						for (int j = 0; j < idtable.size; j++)
						{
							if (idtable.table[j].idtype == IT::L)
							{
								if (j == idx) break;
								currentLitIndex++;
							}
						}
						name = GetUniqueLiteralName(entry, currentLitIndex);
					}

					if (entry.idtype == IT::L && entry.iddatatype == IT::STR) code += "\tpush offset " + name + "\n";
					else code += "\tpush " + name + "\n";
				}
			}
			else if (lex == LEX_PLUS) code += "\tpop ebx\n\tpop eax\n\tadd eax, ebx\n\tpush eax\n";
			else if (lex == LEX_MINUS) code += "\tpop ebx\n\tpop eax\n\tsub eax, ebx\n\tpush eax\n";
			else if (lex == LEX_STAR) code += "\tpop ebx\n\tpop eax\n\timul ebx\n\tpush eax\n";
			else if (lex == LEX_DIRSLASH) code += "\tpop ebx\n\tpop eax\n\tcdq\n\tidiv ebx\n\tpush eax\n";
			else if (lex == LEX_PECENT) code += "\tpop ebx\n\tpop eax\n\tcdq\n\tidiv ebx\n\tpush edx\n";

			// Операторы сравнения

			// == 
			else if (lex == LEX_EQUAL && lextable.table[i + 1].lexema[0] == LEX_EQUAL) {

				i++;
				code += GenExpression(lextable, idtable, i);
				code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n\tmov eax, 0\n\tsete al\n\tpush eax\n";
			}
			// != 
			else if (lex == LEX_EXCLAMATION && lextable.table[i + 1].lexema[0] == LEX_EQUAL) {

				i++;
				code += GenExpression(lextable, idtable, i);
				code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n\tmov eax, 0\n\tsetne al\n\tpush eax\n";
			}
			// > 
			else if (lex == LEX_GREATHER && lextable.table[i + 1].lexema[0] != LEX_EQUAL) {

				i++;
				code += GenExpression(lextable, idtable, i);
				code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n\tmov eax, 0\n\tsetg al\n\tpush eax\n";
			}
			// < 
			else if (lex == LEX_LESS && lextable.table[i + 1].lexema[0] != LEX_EQUAL) {

				i++;
				code += GenExpression(lextable, idtable, i);
				code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n\tmov eax, 0\n\tsetl al\n\tpush eax\n";
			}
			// >= 
			else if (lex == LEX_GREATHER && lextable.table[i + 1].lexema[0] == LEX_EQUAL) {
				i++;
				code += GenExpression(lextable, idtable, i);
				code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n\tmov eax, 0\n\tsetge al\n\tpush eax\n";
			}
			// <= 
			else if (lex == LEX_LESS && lextable.table[i + 1].lexema[0] == LEX_EQUAL) {

				i++;
				code += GenExpression(lextable, idtable, i);
				code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n\tmov eax, 0\n\tsetle al\n\tpush eax\n";
			}
		}
		return code;
	}
}