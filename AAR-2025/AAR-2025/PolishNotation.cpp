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
		int i = lextable_pos;

		for (; i < lextable.size && lextable.table[i].lexema[0] != LEX_SEMICOLON &&
			(lextable.table[i].lexema[0] != LEX_RIGHTHESIS || !stk.empty()); i++)
		{
			char current_lexema = lextable.table[i].lexema[0];
			char next_lexema = (i >= 1) ? lextable.table[i - 1].lexema[0] : ' ';

			switch (current_lexema)
			{
			case LEX_ID:
			case LEX_LITERAL:
			case LEX_TRUE:
			case LEX_FALSE:
				if (current_lexema == LEX_ID && idtable.table[lextable.table[i].idxTI].idtype == IT::IDTYPE::F)
				{
					quantityParm = 0;
					function = true;
					result.push(lextable.table[i]);
				}
				else
				{
					if (function && !quantityParm)
						quantityParm++;
					result.push(lextable.table[i]);
				}
				break;

			case LEX_PLUS:
			case LEX_MINUS:
			case LEX_STAR:
			case LEX_PECENT:
			case LEX_DIRSLASH:
			case LEX_LESS:
			case LEX_GREATHER:
			case LEX_EQUAL:
			case LEX_EXCLAMATION:
			{
				char operator_to_push = current_lexema;

				if (current_lexema == LEX_EQUAL && next_lexema == LEX_EQUAL)
				{
					operator_to_push = OP_EQUAL_EQUAL;
					i++; 
				}
				else if (current_lexema == LEX_EXCLAMATION && next_lexema == LEX_EQUAL)
				{
					operator_to_push = LEX_EXCLAMATION;
					i++; 
				}
				else if (current_lexema == LEX_GREATHER && next_lexema == LEX_EQUAL)
				{
					operator_to_push = OP_GREATER_EQUAL;
					i++; 
				}
				else if (current_lexema == LEX_LESS && next_lexema == LEX_EQUAL)
				{
					operator_to_push = OP_LESS_EQUAL;
					i++;
				}
				else if (current_lexema == LEX_EQUAL)
				{
					continue;
				}

				int current_priority = priority(operator_to_push);

				while (!stk.empty() &&
					stk.top().lexema[0] != LEX_LEFTHESIS &&
					priority(stk.top().lexema[0]) >= current_priority)
				{
					result.push(stk.top());
					stk.pop();
				}
				LT::Entry operator_entry;
				operator_entry.lexema[0] = operator_to_push;
				operator_entry.sn = lextable.table[i].sn;

				stk.push(operator_entry);
			}
			break;

			case LEX_LEFTHESIS:
				stk.push(lextable.table[i]);
				break;

			case LEX_RIGHTHESIS:
				while (!stk.empty() && stk.top().lexema[0] != LEX_LEFTHESIS)
				{
					result.push(stk.top());
					stk.pop();
				}

				if (!stk.empty())
					stk.pop();
				else
					return false;

				if (function)
				{
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

			default:
				break;
			}
		}

		while (!stk.empty())
		{
			if (stk.top().lexema[0] == LEX_LEFTHESIS) return false;
			result.push(stk.top());
			stk.pop();
		}


		int j_current = lextable_pos;

		while (!result.empty())
		{
			LT::Entry entry_ptr = result.front();

			lextable.table[j_current] = entry_ptr;

			result.pop();
			j_current++;
		}

		for (int j = j_current; j < i; j++)
		{
			lextable.table[j] = LT::Entry();
			lextable.table[j].lexema[0] = ' ';
			lextable.table[j].sn = lextable.table[i].sn;
		}

		return true;
	}

	int priority(char v)
	{
		switch (v)
		{
		case '(':
		case ')': return 0;
		case '+':
		case '-': return 1;
		case '*':
		case '/':
		case '%': return 2;
		case OP_EQUAL_EQUAL:
		case LEX_EXCLAMATION:
		case OP_GREATER_EQUAL:
		case OP_LESS_EQUAL:
		case LEX_GREATHER:
		case LEX_LESS: return 3;
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