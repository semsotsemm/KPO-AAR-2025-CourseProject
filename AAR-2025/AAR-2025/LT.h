#pragma once

#define LEXEMA_FIXSIZE	1			
#define	LT_MAXSIZE		4096		// максимальное количество строк в таблице лексем	
#define	LT_TI_NULLXDX	-1			// нет элемента таблицы идентификаторов	
#define	LT_V_NULL		'?'			

#define LEX_SWITCH		's'
#define LEX_CASE		'a'
#define LEX_DEFAULT		'u'
#define LEX_BREAK		'k'
#define	LEX_INT			't'			
#define	LEX_BOOL		't'			
#define	LEX_STR			't'			
#define	LEX_ID			'i' 		
#define	LEX_LITERAL		'l'			
#define	LEX_FUNCTION	'f'			
#define	LEX_PARAM		'p'			
#define	LEX_VAR			'v'			
#define	LEX_REPEAT		'~'			
#define	LEX_RETURN		'r'			
#define	LEX_WRITE		'w'			
#define	LEX_MAIN		'm'			
#define	LEX_SEMICOLON	';'			
#define	LEX_COMMA		','			
#define	LEX_LEFTBRACE	'{'			
#define	LEX_RIGHTBRACE	'}'			
#define	LEX_LEFTHESIS	'('			
#define	LEX_RIGHTHESIS	')'			
#define	LEX_OPERATOR	'#'			
#define	LEX_EQUAL		'='			
#define	LEX_MORE		'>'			
#define	LEX_LESS		'<'			
#define	LEX_COMPARE		'&'			
#define	LEX_NOTEQUALS	'^'			
#define	LEX_IF			'?' 		
#define LEX_THEN		':'			
#define LEX_ELSE		'!'			
#define LEX_COMMENT		'@'			

#define PARM_LEX_DEFAULT_LOCATION	L"C:\\Users\\user\\work_space\\Laboratory_work\\AAR-2025\\Files\\in"	
#define PARM_LEX_DEFAULT_EXT		L".lex.txt"

namespace LT							
{
	struct Entry	// строка таблицы лексем
	{
		char lexema;					
		char sign;	// знак лексемы v или LT_V_NULL
		int sn;		// номер строки в исходном тексте
		int tn;		// номер токена в строке
		int idxTI;	// индекс в таблице идентификаторов или LT_TI_NULLIDX

		Entry(char lexema, int sn, int tn, int idxTI);		// для идентификатор
		Entry(char lexema, char sign, int sn, int tn);		// для операторов
		Entry(char lexema, int sn, int tn);					// остальные лексемы
		Entry(char lexema);
	};

	struct LexTable						
	{
		int maxsize;					
		int current_size;				
		Entry** table;					// массив указателей на строки таблицы лексем

		LexTable();
		LexTable(int size);			// создать таблицу лексем
	};
	void Add(LexTable& lextable, Entry* entry);						
	Entry GetEntry(LexTable& lextable, int n);						
	void PrintLexTable(LexTable& lextable, const wchar_t* in);		
	void Delete(LexTable& lextable);								
}