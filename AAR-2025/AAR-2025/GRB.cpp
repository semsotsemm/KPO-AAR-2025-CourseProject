#include "GRB.h"

#include <vector>
#include <string> 
#include <cstdarg>

#define GRB_ERROR_SERIES 600

namespace GRB
{
#define NS(n) Rule::Chain::N(n)
#define TS(n) Rule::Chain::T(n)

	Greibach greibach(NS('S'), TS('$'), {	// стартовый символ, дно стека
			Rule(NS('S'), GRB_ERROR_SERIES + 0, {	// Неверная структура программы
				// main функция 
				{ TS('m'), TS('{'), NS('N'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';'), NS('S') },
				{ TS('m'), TS('{'), NS('N'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';')},
				{ TS('m'), TS('{'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';'), NS('S')},
				{ TS('m'), TS('{'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';')},

				// Пользовательские функции
				{ TS('f'),NS('T'), TS('x'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';'), NS('S')},
				{ TS('f'),NS('T'), TS('x'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';')},
				{ TS('f'),NS('T'), TS('x'), TS('('), NS('F'), TS(')'), TS('{'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';')},
				{ TS('f'),NS('T'), TS('x'), TS('('), NS('F'), TS(')'), TS('{'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';'), NS('S')},
				{ TS('f'),NS('T'), TS('x'), TS('('), TS(')'), TS('{'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';'), NS('S')},
				{ TS('f'),NS('T'), TS('x'), TS('('), TS(')'), TS('{'), TS('r'), NS('U'), TS(';'), TS('}'), TS(';')},
			}),
			Rule(NS('T'), GRB_ERROR_SERIES + 1, { // Типы данных
				{TS('i'),},
				{TS('b'),},
				{TS('s'),},
			}),
			Rule(NS('N'), GRB_ERROR_SERIES + 2, {	// Операторы
				{ TS('v'), NS('T'), TS('x'), TS(';')},
				{ TS('v'), NS('T'), TS('x'), TS(';'), NS('N')},
				{ TS('v'), NS('T'), TS('x'), TS('='), NS('E'), TS(';'), NS('N')},
				{ TS('v'), NS('T'), TS('x'), TS('='), NS('E'), TS(';')},
				{ TS('v'), TS('i'), TS('='), NS('L'), NS('T'), TS(';')},
				{ TS('v'), TS('i'), TS('='), NS('L'), NS('T'), TS(';'), NS('N')},
				{ TS('x'), TS('='), NS('E'), TS(';')},
				{ TS('x'), TS('='), NS('E'), TS(';'), NS('N')},
				{ TS('x'), TS('('), NS('W'), TS(')'), TS(';'), NS('N')},
				{ TS('x'), TS('('), NS('W'), TS(')'),  TS(';')},

				{ TS('w'), TS('('), NS('E'), TS(')'), TS('{'), NS('C'), TS('}'), NS('N') },
				{ TS('w'), TS('('), NS('E'), TS(')'), TS('{'), NS('C'), TS('}') },

				{ TS('p'), NS('E'), TS(';') }, 
				{ TS('p'), NS('E'), TS(';'), NS('N') },
				{ TS('c'), NS('K'), NS('J')},
				{ TS('c'), NS('K'), NS('J'), NS('N') }
			}),

			Rule(NS('E'), GRB_ERROR_SERIES + 3, {	// выражения
				{ TS('x'), NS('O'), NS('E') },
				{ TS('x')},
				{ TS('l'), NS('O'), NS('E') },
				{ TS('l')},
				{ TS('0')},
				{ TS('1')},
				{ TS('('), NS('E'), TS(')')},
				{ TS('('), NS('E'), TS(')'), NS('O'), NS('E') },
				{ TS('x'), TS('('), NS('W'), TS(')')},
				{ TS('x'), TS('('), TS(')')},
				{ TS('x'), TS('('), NS('W'), TS(')'), NS('O'), NS('E') },
			}),
			Rule(NS('M'), GRB_ERROR_SERIES + 4, {	
				{ TS('O'), NS('E')},
			}),
			Rule(NS('O'), GRB_ERROR_SERIES + 5, {	
				{ TS('+') },
				{ TS('-') },
				{ TS('*') },
				{ TS('/') },
				{ TS('%') },
			}),
			Rule(NS('F'), GRB_ERROR_SERIES + 6,	{	
				{ TS('o'), NS('T'), TS('x')},
				{ TS('o'), NS('T'), TS('x'), TS(','), NS('F')}
			}),
			Rule(NS('P'), GRB_ERROR_SERIES + 7,	{	
				{TS('o'), TS('x')}
			}),
			Rule(NS('W'), GRB_ERROR_SERIES + 8,	{
				{ TS('x')},
				{ TS('l')},
				{ TS('x'), TS(','), NS('W')},
				{ TS('l'), TS(','), NS('W')},
				{ },
			}),
			Rule(NS('U'), GRB_ERROR_SERIES + 9,	{
				{ TS('l')},
				{ TS('x')},
				{ TS('0')},
				{ TS('1')},
				{ TS('l'), NS('U')},
				{ TS('x'), NS('U')},
				{ TS('x'), TS(','), NS('U')},
				{ TS('l'), TS(','), NS('U')},
				{ TS('O'), TS('l')},
				{ TS('O'), TS('l'), NS('U')},
				{ TS('O'), TS('l'), TS(','), NS('U')}
			}),
			Rule(NS('J'), GRB_ERROR_SERIES + 10, {
				{TS(':'), TS('{'), NS('N'), TS('}')},
				{TS(':'), TS('{'), NS('N'), TS('}'), TS('e'), TS('{'), NS('N'), TS('}')},
			}),

			Rule(NS('K'), GRB_ERROR_SERIES + 11,{
				{TS('('), NS('U'), TS('='), TS('='), NS('U'), TS(')')},
				{TS('('), NS('U'), TS(')')},
				{TS('('), NS('U'), TS('!'), TS('='), NS('U'), TS(')')},
				{TS('('), NS('U'), TS('<'), NS('U'), TS(')')},
				{TS('('), NS('U'), TS('>'), NS('U'), TS(')')},
			}),
			Rule(NS('L'), GRB_ERROR_SERIES + 12,{
				{TS('x')},
				{TS('l')},
				{TS('x'), TS('('), NS('U'), TS(')')},
				{TS('x'), TS('('), TS(')')},
				{TS('O'), TS('x')},
				{TS('O'), TS('l')},
				{TS('O'), TS('x'), NS('L')},
				{TS('O'), TS('l'), NS('L')},
				{TS('x'), NS('L')},
				{TS('l'), NS('L')}
			}),
			Rule(NS('C'), GRB_ERROR_SERIES + 13,{
				{TS('a'), NS('E'), TS(':'), TS('{'), NS('N'), TS('}'), NS('C')},
				{TS('a'), NS('E'), TS(':'), TS('{'), NS('N'), TS('}')},
			})
		});
	Greibach getGreibach()
	{
		return greibach;
	};

	short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
	{
		for (int i = 0; i < rules.size(); i++)
		{
			if (rules[i].nn == pnn)
			{
				prule = rules[i];
				return i;
			}
		}
		return -1;
	};

	Rule Greibach::getRule(short n)
	{
		if (n >= 0 && n < rules.size())
		{
			return rules[n];
		}
		return Rule();
	}

	char* Rule::getCRule(char* b, short nchain)
	{
		if (nchain >= 0 && nchain < chains.size())
		{
			char bchain[200];
			b[0] = Chain::alphabet_to_char(nn);
			b[1] = '-';
			b[2] = '>';
			b[3] = 0x00;
			chains[nchain].getCChain(bchain);
			strcat_s(b, 205, bchain);
			return b;
		}
		b[0] = 0x00;
		return b;
	};

	short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
	{
		for (short i = j; i < chains.size(); i++)
		{
			if (!chains[i].nt.empty() && chains[i].nt[0] == t)
			{
				pchain = chains[i];
				return i;
			}
		}
		return -1;
	};

	char* Rule::Chain::getCChain(char* b)
	{
		for (int i = 0; i < nt.size(); i++)
		{
			b[i] = Chain::alphabet_to_char(nt[i]);
		}
		b[nt.size()] = 0x00;
		return b;
	}
}