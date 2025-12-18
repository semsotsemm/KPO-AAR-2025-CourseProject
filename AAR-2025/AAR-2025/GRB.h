#pragma once
#include "Error.h"

typedef short GRBALPHABET;	// символы алфавита грамматики терминалы > 0, нетерминалы < 0

namespace GRB
{
	struct Rule 
	{
		GRBALPHABET nn;		
		int iderror;		
		short size;			// количество цепочек - правых частей правила

		struct Chain		// цепочка (права€ часть правила)
		{
			short size;			
			GRBALPHABET* nt;	// цепочка терминалов (>0) и нетерминалов (<0)

			Chain()
			{
				size = 0;
				nt = new GRBALPHABET[1]{};
			};

			Chain(short symbolCount,	// количество символов в цепочке
				GRBALPHABET s, ...);	// символы (терминал или нетерминал)

			char* getCChain(char* b);		//получить правую сторону правила
			static GRBALPHABET T(char t) { return GRBALPHABET(t); }		// терминал
			static GRBALPHABET N(char n) { return -GRBALPHABET(n); }	// нетерминал
			static bool isT(GRBALPHABET s) { return s > 0; }			// терминал?
			static bool isN(GRBALPHABET s) { return !isT(s); }			// нетерминал?
			static char alphabet_to_char(GRBALPHABET s) { return (isT(s) ? char(s) : char(-s)); }	// CRBALPHABET->char
		}*chains;		// массив цепочек - правых частей правила

		Rule()
		{
			nn = 0;
			size = 0;
		}
		Rule(GRBALPHABET pnn,	
			int iderror,		
			short psize,		
			Chain c, ...);		

		char* getCRule(			// получить правило в виде N->цепочка
			char* b,			// буфер
			short nchain);		// номер цепочки (правой части) в правиле

		short getNextChain(			// получить следующую за j подход€щую цепочку, вернуть ее номер или -1
			GRBALPHABET t,			// первый символ цепочки	
			Rule::Chain& pchain,	// возвращаема€ цепочка
			short j);				// номер цепочки
	};

	struct Greibach // грамматика √рейбах
	{
		short size;				// количество правил 
		GRBALPHABET startN;		// стартовый символ
		GRBALPHABET stbottomT;  // дно стека
		Rule* rules;			// множество правил

		Greibach()
		{
			size = 0;
			startN = 0;
			stbottomT = 0;
			rules = 0;
		};
		Greibach(GRBALPHABET pstartN,	// стартовый символ
			GRBALPHABET pstbottomT,		// дно стека
			short psize,				// количество правил
			Rule r, ...);				// правила

		short getRule(				// получить правило, возвращаетс€ номер правила или -1
			GRBALPHABET pnn,		// левый символ правила
			Rule& prule);			// возвращаемое правило грамматики

		Rule getRule(short n);		// получить правило по номеру
	};

	Greibach getGreibach();		// получить грамматику
}
