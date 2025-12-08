#include "SemanticAnaliz.h"
#include "Error.h"

using namespace std;

namespace SM
{
	void semAnaliz(LT::LexTable& lextable, IT::IdTable& idtable)
	{
		for (int i = 0; i < lextable.size; i++)
		{
			switch (lextable.table[i].lexema[0])
			{
			case LEX_DIRSLASH: // Проверка возвращаемого значения
			{
				if (lextable.table[i + 1].lexema[0] == LEX_LITERAL)
				{
					if (idtable.table[lextable.table[i + 1].idxTI].value.vint == 0)
					{
						ERROR_THROW_IN(700, lextable.table[i + 1].sn, lextable.table[i + 1].idxTI);
					}
				}
				break;
			}
			case LEX_EQUAL:	// Проверка присваивания
			{
				if (i && lextable.table[i - 1].idxTI != 0xffff)
				{
					IT::IDDATATYPE lefttype = idtable.table[lextable.table[i - 1].idxTI].iddatatype;	
					bool ignore = false;

					for (int k = i + 1; lextable.table[k].lexema[0] != LEX_SEMICOLON; k++)
					{
						if (lextable.table[k].idxTI != LT_TI_NULLIDX) 
						{
							if (k + 1 < lextable.size && lextable.table[k + 1].lexema[0] == LEX_LEFTHESIS && idtable.table[lextable.table[k].idxTI].idtype == IT::IDTYPE::F)
							{
								ignore = true;
								continue;
							}
							if (ignore && lextable.table[k + 1].lexema[0] == LEX_RIGHTHESIS)
							{
								ignore = false;
								continue;
							}
						}
						if (lefttype == IT::IDDATATYPE::STR) 
						{
							char l = lextable.table[k].lexema[0];
							if (l == LEX_PLUS || l == LEX_MINUS || l == LEX_STAR || l == LEX_PECENT || l == LEX_DIRSLASH) 
								ERROR_THROW_IN(702, lextable.table[k].sn, -1);
						}
					}
				}
				break;
			}
			case LEX_ID: // проверка типа возвращаемого значения  
			{
				IT::Entry e;
				e = idtable.table[lextable.table[i].idxTI];

				if (i && i >= 2 && lextable.table[i - 2].lexema[0] == LEX_FUNCTION)	// объявление функции
				{
					for (int k = i + 1; ; k++)
					{
						char l = lextable.table[k].lexema[0];
						if (l == LEX_RETURN)
						{
							int next = lextable.table[k + 1].idxTI; // след. за return
							if (idtable.table[next].iddatatype != e.iddatatype)
								ERROR_THROW_IN(703, lextable.table[k + 1].sn, -1);
							break;
						}
					}
				}
				if (lextable.table[i + 1].lexema[0] == LEX_LEFTHESIS && lextable.table[i - 2].lexema[0] != LEX_FUNCTION) // именно вызов
				{
					if (e.idtype == IT::IDTYPE::F) // точно функция
					{
						int paramscount = 0;
						// проверка передаваемых параметров
						for (int j = i + 1; lextable.table[j].lexema[0] != LEX_RIGHTHESIS; j++)
						{
							// проверка соответствия передаваемых параметров прототипам
							if (lextable.table[j].lexema[0] == LEX_ID || lextable.table[j].lexema[0] == LEX_LITERAL)
							{
								paramscount++;
								IT::IDDATATYPE ctype = idtable.table[lextable.table[j].idxTI].iddatatype;
								if (paramscount > e.params.count)
									ERROR_THROW_IN(705, lextable.table[i].sn, -1);
								if (ctype != e.params.types[paramscount - 1])
									ERROR_THROW_IN(704, lextable.table[j].sn, lextable.table[j].idxTI);
							}
						}
						if (paramscount != e.params.count)
							ERROR_THROW_IN(705, lextable.table[i].sn, -1);
					}
				}
				break;
			}
			}
		}
	}
}