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
				// Проверка возвращаемого значения
				case LEX_DIRSLASH:
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
				// Проверка присваивания
				case LEX_EQUAL:	
				{
					if (i && lextable.table[i - 1].idxTI != 0xffff)
					{
						IT::IDDATATYPE lefttype = idtable.table[lextable.table[i - 1].idxTI].iddatatype;	
						bool ignore = false;
						if (lextable.table[i + 1].lexema[0] != '-')
						{
							for (int k = i + 1; lextable.table[k].lexema[0] != LEX_SEMICOLON; k++)
							{
								// Функция стандартной библиотеки
								if (lextable.table[k].idxTI != LT_TI_NULLIDX)
								{
									// Функция
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
								// Математические операции над строкой 
								if (lefttype == IT::IDDATATYPE::STR)
								{
									char l = lextable.table[k].lexema[0];
									if (l == LEX_PLUS || l == LEX_MINUS || l == LEX_STAR || l == LEX_PECENT || l == LEX_DIRSLASH)
									{
										ERROR_THROW_IN(706, lextable.table[k].sn, -1);
									}
								}
								if (lefttype != idtable.table[lextable.table[i + 1].idxTI].iddatatype)
								{
									ERROR_THROW_IN(702, lextable.table[k].sn, -1);
								}
							}
						}
						if (lextable.table[i + 1].lexema[0] == LEX_MINUS)
						{
							if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::STR)
							{
								ERROR_THROW_IN(707, lextable.table[i + 2].sn, -1);
							}

							if (idtable.table[lextable.table[i + 2].idxTI].iddatatype == IT::IDDATATYPE::BOOL)
							{
								ERROR_THROW_IN(708, lextable.table[i + 2].sn, -1);
							}
						}
						break;
					}
				}
				// Проверка типа возвращаемого значения  
				case LEX_ID: 
				{
					IT::Entry e;
					e = idtable.table[lextable.table[i].idxTI];
					// Объявление функции
					if (i && i >= 2 && lextable.table[i - 2].lexema[0] == LEX_FUNCTION)	
					{
						for (int k = i + 1; ; k++)
						{
							char l = lextable.table[k].lexema[0];
							if (l == LEX_RETURN)
							{
								int next = lextable.table[k + 1].idxTI; // след. за return
								if (idtable.table[next].iddatatype != e.iddatatype)
								{
									ERROR_THROW_IN(703, lextable.table[k + 1].sn, -1);
								}
								break;
							}
						}
					}
					// Вызов функции
					if (lextable.table[i + 1].lexema[0] == LEX_LEFTHESIS && lextable.table[i - 2].lexema[0] != LEX_FUNCTION)
					{
						if (e.idtype == IT::IDTYPE::F)
						{
							int paramscount = 0;
							for (int j = i + 1; lextable.table[j].lexema[0] != LEX_RIGHTHESIS; j++)
							{
								if (lextable.table[j].lexema[0] == LEX_ID || lextable.table[j].lexema[0] == LEX_LITERAL)
								{
									paramscount++;
									IT::IDDATATYPE ctype = idtable.table[lextable.table[j].idxTI].iddatatype;
									if (paramscount > e.params.count)
									{
										ERROR_THROW_IN(705, lextable.table[i].sn, -1);
									}
								}
							}
							if (paramscount != e.params.count)
							{
								ERROR_THROW_IN(705, lextable.table[i].sn, -1);
							}
						}
					}

					// Неинициализированная переменная 
					if ((e.iddatatype == IT::STR || e.iddatatype == IT::BOOL) && strcmp(e.value.vstr.str, "UNDEF") == 0)
					{
						if (lextable.table[i - 2].lexema[0] != LEX_LET)
						{
							ERROR_THROW_IN(709, lextable.table[i].sn, -1);
						}
					}
					if (e.iddatatype == IT::INT && e.value.vint == -32768)
					{
						if (lextable.table[i - 2].lexema[0] != LEX_LET)
						{
							ERROR_THROW_IN(709, lextable.table[i].sn, -1);
						}
					}
					break;
				}
			}
		}
	}
}