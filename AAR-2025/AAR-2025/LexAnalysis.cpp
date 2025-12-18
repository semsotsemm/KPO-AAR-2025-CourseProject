#include "stdafx.h"
#include <stack>

#include "LexAnalysis.h"
#include "Error.h"
#include "FST.h"
#include "graphs.h"

using namespace std;

void LexAnalysis(In::IN& in, LT::LexTable& lex, IT::IdTable& id)
{
	IT::IDDATATYPE iddatatype = IT::IDDATATYPE::DEF;			
	IT::IDTYPE idtype = IT::IDTYPE::D;							
	IT::Entry* IT_ENTRY = NULL;

	IT::len.params = IT::Entry::Param{ 1, vector<IT::IDDATATYPE>{ IT::IDDATATYPE::STR} };
	IT::Add(id, &IT::len);

	IT::copy.params = IT::Entry::Param{ 3, vector<IT::IDDATATYPE>{IT::IDDATATYPE::STR, IT::IDDATATYPE::STR, IT::IDDATATYPE::INT} };
	IT::Add(id, &IT::copy);

	IT::time.params = IT::Entry::Param{ 0, vector<IT::IDDATATYPE>{} };
	IT::Add(id, &IT::time);

	IT::pow.params = IT::Entry::Param{ 2, vector<IT::IDDATATYPE>{IT::IDDATATYPE::INT, IT::IDDATATYPE::INT} };
	IT::Add(id, &IT::pow);

	IT::random.params = IT::Entry::Param{ 2, vector<IT::IDDATATYPE>{IT::IDDATATYPE::INT, IT::IDDATATYPE::INT} };
	IT::Add(id, &IT::random);

	IT::square.params = IT::Entry::Param{ 1, vector<IT::IDDATATYPE>{IT::IDDATATYPE::INT} };
	IT::Add(id, &IT::square);

	IT::factorial.params = IT::Entry::Param{ 1, vector<IT::IDDATATYPE>{IT::IDDATATYPE::INT} };
	IT::Add(id, &IT::factorial);
	ID_RESET
		 
	char lexema[TI_STR_MAXSIZE];
	int currentRow = 1, currentLex = 0, indexIdTable = 7;
	bool lexID = true, lexInt = false, lexBool = false, lexComment = false;
	int main = 0;
	stack<string> areaOfVisibility;
	areaOfVisibility.push(GLOBAL);

	while (FindLexema(in, lexema))
	{
		currentLex++;
		lexID = true;
		lexInt = false;
		lexBool = false;

		switch (*lexema)
		{
			case LEX_NOTEQUALS:
			case LEX_COMPARE:
			case LEX_RIGHTBRACE:
			case LEX_SEMICOLON:
			case LEX_COMMA:
			case LEX_LEFTBRACE:
			case LEX_LEFTHESIS:
			case LEX_RIGHTHESIS:
			case LEX_EQUAL:
			case LEX_MORE:
			case LEX_LESS:
			{
				LT::Add(lex, new LT::Entry(*lexema, currentRow, currentLex));
				lexID = false;

				if (*lexema == LEX_LEFTBRACE)
				{
					char unique_suffix[10];
					static int block_count = 0; 
					sprintf(unique_suffix, "%d", block_count++); 

					char scope_name[TI_STR_MAXSIZE]; // Используем больший буфер
					strcpy(scope_name, "block");		
					strcat(scope_name, unique_suffix);	// block_n

					areaOfVisibility.push(scope_name);
				}

				if (*lexema == LEX_RIGHTBRACE)
				{
					areaOfVisibility.pop();
				}
				break;
			}
			case IN_CODE_VERTICAL_LINE:
				currentRow++;
				currentLex = 0;
				lexID = false;
				if (lexComment)
				{
					lexComment = false;
				}
				break;

			case '#':
			{
				FST::FST fst_comment = FST_COMMENT(lexema);

				if (FST::execute(fst_comment))
				{
					lexComment = true;
				}
				break;
			}
			case 'c':
			{
				FST::FST fst_case = FST_CASE(lexema); 
				if (FST::execute(fst_case) && !lexComment)
				{
					LT::Add(lex, new LT::Entry(LEX_CASE, currentRow, currentLex));
					lexID = false;
				}
				break;
			}
			case 's':
			{
				FST::FST fst_string = FST_STR(lexema);

				// string
				if (FST::execute(fst_string) && !lexComment)
				{
					LT::Add(lex, new LT::Entry(LEX_STR, currentRow, currentLex));
					iddatatype = IT::IDDATATYPE::STR;
					lexID = false;
				}

				// switch
				if (lexID)
				{
					FST::FST fst_switch = FST_SWITCH(lexema); 
					if (FST::execute(fst_switch) && !lexComment)
					{
						LT::Add(lex, new LT::Entry(LEX_SWITCH, currentRow, currentLex));
						lexID = false;
					}
				}
				break;
			}
			case 'b':
			{
				FST::FST fst_bool = FST_BOOL(lexema);

				// bool
				if (FST::execute(fst_bool) && !lexComment) 
				{
					LT::Add(lex, new LT::Entry(LEX_BOOL, currentRow, currentLex));
					iddatatype = IT::IDDATATYPE::BOOL;
					lexID = false;
				}

				// break
				if (lexID) 
				{
					FST::FST fst_break = FST_BREAK(lexema);
					if (FST::execute(fst_break) && !lexComment)
					{
						LT::Add(lex, new LT::Entry(LEX_BREAK, currentRow, currentLex));
						lexID = false;
					}
				}

				// b
				if (lexID) 
				{
					FST::FST fst_literal_i2 = FST_LITERAL_I2(lexema);		// целочисленный литерал в 2-система счисления
					if (FST::execute(fst_literal_i2) && !lexComment)
					{
						lexID = false; 
						lexInt = true;
						BaseToDecimal(lexema, 2);
					}
				}
				break;
			}
			case 'd':
			{
				FST::FST fst_function = FST_FUNCTION(lexema);
				if (FST::execute(fst_function) && !lexComment)
				{
					LT::Add(lex, new LT::Entry(LEX_FUNCTION, currentRow, currentLex));
					idtype = IT::IDTYPE::F;
					lexID = false;
				}
				if (lexID)
				{
					FST::FST fst_default = FST_DEFAULT(lexema);
					if (FST::execute(fst_default) && !lexComment)
					{
						LT::Add(lex, new LT::Entry(LEX_DEFAULT, currentRow, currentLex));
						lexID = false;
					}
				}
				break;
			}
			case 'i':
			{
				FST::FST fst_int = FST_INT(lexema);
				if (FST::execute(fst_int) && !lexComment)
				{
					LT::Add(lex, new LT::Entry(LEX_INT, currentRow, currentLex));
					iddatatype = IT::IDDATATYPE::INT;
					lexID = false;
				}

				if (lexID)
				{
					FST::FST fst_if = FST_IF(lexema);
					if (FST::execute(fst_if) && !lexComment)
					{
						LT::Add(lex, new LT::Entry(LEX_IF, currentRow, currentLex));
						lexID = false;
					}
				}
				break;
			}
			case 'm':
			{
				FST::FST fst_main = FST_MAIN(lexema);
				if (FST::execute(fst_main) && !lexComment)
				{
					main++;
					LT::Add(lex, new LT::Entry(LEX_MAIN, currentRow, currentLex));
					areaOfVisibility.push(GLOBAL);
					lexID = false;
				}
				break;
			}
			case 'p':
			{
				if (*(lexema + 1) == 'a')
				{
					FST::FST fst_param = FST_PARAM(lexema);
					if (FST::execute(fst_param) && !lexComment)
					{
						LT::Add(lex, new LT::Entry(LEX_PARAM, currentRow, currentLex));
						idtype = IT::IDTYPE::P;

						if (IT_ENTRY)
						{
							IT_ENTRY->params.count++;
							IT_ENTRY->params.types.push_back(iddatatype);
						}

						lexID = false;
					}
				}
				else if (*(lexema + 1) == 'r')
				{
					FST::FST fst_write = FST_WRITE(lexema);
					if (FST::execute(fst_write) && !lexComment)
					{
						LT::Add(lex, new LT::Entry(LEX_WRITE, currentRow, currentLex));
						lexID = false;
					}
				}
				break;
			}

			case 'r':
			{
				FST::FST fst_return = FST_RETURN(lexema);
				if (FST::execute(fst_return) && !lexComment)
				{
					LT::Add(lex, new LT::Entry(LEX_RETURN, currentRow, currentLex));
					lexID = false;
				}
				if (lexID)
				{
					FST::FST fst_var = FST_REPEAT(lexema);
					if (FST::execute(fst_var) && !lexComment)
					{
						char str[10];
						static int count = 0;
						sprintf(str, "%d", count++);
						LT::Add(lex, new LT::Entry(LEX_REPEAT, currentRow, currentLex));
						lexID = false;
					}
				}
				break;
			}
			case 'l':
			{
				FST::FST fst_var = FST_VAR(lexema);
				if (FST::execute(fst_var) && !lexComment)
				{
					LT::Add(lex, new LT::Entry(LEX_VAR, currentRow, currentLex));
					idtype = IT::IDTYPE::V;
					lexID = false;
				}
				break;
			}
			case 'h':
			{
				FST::FST fst_literal_i16 = FST_LITERAL_I16(lexema);		// целочисленный литерал в 16-система счисления
				if (FST::execute(fst_literal_i16) && !lexComment)
				{
					lexID = false; lexInt = true;
					BaseToDecimal(lexema, 16);
				}
				break;
			}
			case 'o':
			{
				FST::FST fst_literal_i8 = FST_LITERAL_I8(lexema);		// целочисленный литерал в 8-система счисления
				if (FST::execute(fst_literal_i8) && !lexComment)
				{
					lexID = false; lexInt = true;
					BaseToDecimal(lexema, 8);
				}
				break;
			}
			case ':':
			{
				FST::FST fst_write = FST_THEN(lexema);
				if (FST::execute(fst_write) && !lexComment)
				{
					char str[10];
					static int count = 0;
					sprintf(str, "%d", count++);
					LT::Add(lex, new LT::Entry(LEX_THEN, currentRow, currentLex));
					lexID = false;
				}
				break;
			}
			case 'e':
			{
				FST::FST fst_write = FST_ELSE(lexema);
				if (FST::execute(fst_write) && !lexComment)
				{
					char str[10];
					static int count = 0;
					sprintf(str, "%d", count++);
					LT::Add(lex, new LT::Entry(LEX_ELSE, currentRow, currentLex));
					lexID = false;
				}
				break;
			}
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
			{
				FST::FST fst_literal_minus_i = FST_LITERAL_MINUS_I(lexema);		// отрицательный целочисленный литерал
				if (FST::execute(fst_literal_minus_i) && !lexComment)
				{
					lexID = false; lexInt = true;
				}
				if (lexID)
				{
					FST::FST fst_operator = FST_OPERATOR(lexema);				
					if (FST::execute(fst_operator) && !lexComment)
					{
						LT::Add(lex, new LT::Entry(LEX_OPERATOR, *lexema, currentRow, currentLex));
						lexID = false;
					}
				}
				break;
			}
			case 't':
			{
				FST::FST fst_true = FST_TRUE(lexema);
				if (FST::execute(fst_true) && !lexComment)
				{
					lexID = false;
					lexBool = true;
				}
				break;
			}
			case 'f':
			{
				FST::FST fst_false = FST_FALSE(lexema);
				if (FST::execute(fst_false) && !lexComment)
				{
					lexID = false;
					lexBool = true;
				}
				break;
			}
			case IN_CODE_QUOTES: // строковые литералы
			{
				FST::FST fst_literal_s = FST_LITERAL_S(lexema);
				if (FST::execute(fst_literal_s) && !lexComment)
				{
					lexID = false;
					int IsLiteralString = IT::IsLiteralString(id, lexema);	// возвращает -1 если нет такого литерала, иначе указываем не него
					if (IsLiteralString + 1)
					{
						LT::Add(lex, new LT::Entry(LEX_LITERAL, currentRow, currentLex, IsLiteralString));
					}
					else
					{
						LT::Add(lex, new LT::Entry(LEX_LITERAL, currentRow, currentLex, indexIdTable++));

						LITERAL_STR
							IT::Add(id, new IT::Entry(lex.current_size - 1, iddatatype, idtype, lexema));
						ID_RESET
					}
				}
				break;
			}
			default:
			{
				FST::FST fst_literal_i = FST_LITERAL_I(lexema);
				if (FST::execute(fst_literal_i))
				{
					if (!(TI_INT_MIN < atoi(lexema) && atoi(lexema) < TI_INT_MAX))
					{
						throw ERROR_THROW_IN(135, currentRow, currentLex);
					}
					lexID = false;
					lexInt = true;
				}
			}
		}

		if (lexInt && !lexComment)
		{
			int IsLiteralInt = IT::IsLiteralInt(id, lexema);	// возвращает -1 если нет такого целочисленного литерала, иначе указываем не него
			if (IsLiteralInt + 1)
			{
				LT::Add(lex, new LT::Entry(LEX_LITERAL, currentRow, currentLex, IsLiteralInt));
			}
			else
			{
				LT::Add(lex, new LT::Entry(LEX_LITERAL, currentRow, currentLex, indexIdTable++));

				LITERAL_INT
				IT::Add(id, new IT::Entry(lex.current_size - 1, iddatatype, idtype, lexema));
				ID_RESET
			}
		}
		if (lexBool && !lexComment)
		{
			int bool_value = GetBoolValueFromLexema(lexema);

			char bool_lexema[TI_STR_MAXSIZE];
			sprintf(bool_lexema, "%d", bool_value);

			int IsLiteralBool = IT::IsLiteralBool(id, bool_lexema);

			if (IsLiteralBool + 1)
			{
				LT::Add(lex, new LT::Entry(LEX_LITERAL, currentRow, currentLex, IsLiteralBool));
			}
			else
			{
				iddatatype = IT::IDDATATYPE::BOOL;
				LT::Add(lex, new LT::Entry(LEX_LITERAL, currentRow, currentLex, indexIdTable++));

				LITERAL_BOOL 
				IT::Add(id, new IT::Entry(lex.current_size - 1, iddatatype, idtype, bool_lexema));
				ID_RESET
			}
		}

		if (lexID && !lexComment) 	// идентификатор
		{
			FST::FST fst_id = FST_ID(lexema);
			if (FST::execute(fst_id))
			{
				int isId = IT::IsId(id, lexema, areaOfVisibility); // возвращает -1 если нет в таблице идентификаторов
				if (isId + 1)
				{
					if (idtype != IT::IDTYPE::D)
					{
						if (!strcmp(id.table[isId]->areaOfVisibility, areaOfVisibility.top().c_str()))
						{
							throw ERROR_THROW_IN(131, currentRow, currentLex);
						}
						if (iddatatype == IT::IDDATATYPE::DEF)
						{
							throw ERROR_THROW_IN(121, currentRow, currentLex);
						}

						LT::Add(lex, new LT::Entry(LEX_ID, currentRow, currentLex, indexIdTable++));
						IT::Add(id, new IT::Entry(lex.current_size - 1, areaOfVisibility.top(), lexema, iddatatype, idtype));

						ID_RESET
					}
					else
					{
						LT::Add(lex, new LT::Entry(LEX_ID, currentRow, currentLex, isId));
					}
				}
				else
				{
					if (iddatatype == IT::IDDATATYPE::DEF)
					{
						throw ERROR_THROW_IN(121, currentRow, currentLex);
					}
					if (idtype == IT::IDTYPE::D)
					{
						throw ERROR_THROW_IN(132, currentRow, currentLex);
					}

					LT::Add(lex, new LT::Entry(LEX_ID, currentRow, currentLex, indexIdTable++));
					IT::Add(id, new IT::Entry(lex.current_size - 1, areaOfVisibility.top(), lexema, iddatatype, idtype));

					if (idtype == IT::IDTYPE::F)
					{
						areaOfVisibility.push(lexema);
						IT_ENTRY = id.table[id.current_size - 1];
					}

					ID_RESET
				}
			}
			else
				throw ERROR_THROW_IN(120, currentRow, currentLex);
		}
	}

	if (main == 0)
	{
		throw ERROR_THROW(133);
	}
	if (main > 1)
	{
		throw ERROR_THROW(134);
	}
	LT::Add(lex, new LT::Entry('$', currentRow, currentLex));
}

bool FindLexema(In::IN& in, char* lexema)
{
	bool s_literal = false;
	static int i = 0; 
	int indexLexema = 0;
	for (; i < in.size && in.text[i] != IN_CODE_SPACE || s_literal; i++)
	{
		if (indexLexema >= TI_STR_MAXSIZE - 1)
		{
			throw ERROR_THROW(125)
		}
		lexema[indexLexema++] = in.text[i];
		if (in.text[i] == IN_CODE_QUOTES) 
		{
			s_literal = !s_literal;
		}
	}
	lexema[indexLexema] = TI_STR_DEFAULT;
	i++;

	return indexLexema;
}

void BaseToDecimal(char* lexema, int base)
{
	int number = 0;
	int k;
	for (int i = 1; lexema[i] != '\0'; i++)
	{
		if (lexema[i] <= '9' && lexema[i] >= '0') k = lexema[i] - '0';
		else if (lexema[i] >= 'A' && lexema[i] <= 'F') k = lexema[i] - 'A' + 10;
		else if (lexema[i] >= 'a' && lexema[i] <= 'f') k = lexema[i] - 'a' + 10;
		else continue;
		number = base * number + k;
	}
	sprintf(lexema, "%d", number);
}

int GetBoolValueFromLexema(const char* lexema)
{
	if (strcmp(lexema, "true") == 0)
		return 1;
	if (strcmp(lexema, "false") == 0)
		return 0;

	int number = atoi(lexema);

	return (number != 0) ? 1 : 0;
}