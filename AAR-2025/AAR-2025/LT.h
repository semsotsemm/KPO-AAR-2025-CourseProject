#pragma once

#define LEXEME_FIXSIZE	1
#define LT_MAXSIZE		4096
#define LT_TI_NULLIDX	0xffff

#define LEX_INTEGER		'i'
#define LEX_BOOL        'b'
#define LEX_STRING		's'

#define LEX_ID			'x'
#define LEX_LITERAL		'l'

#define LEX_FUNCTION	'f'
#define LEX_RETURN		'r'
#define LEX_PRINT		'p'
#define LEX_MAIN		'm'
#define LEX_IF	    	'c'
#define LEX_ELSE	    'e'
#define LEX_SWITCH	    'w'
#define LEX_PARAM	    'o'
#define LEX_LET 	    'v'
#define LEX_CASE	    'a'
#define LEX_DEFAULT	    'd'

#define LEX_TRUE	    '1'
#define LEX_FALSE	    '0'

#define LEX_PLUS		'+'
#define LEX_MINUS		'-'
#define LEX_STAR		'*'
#define LEX_PECENT      '%'
#define LEX_DIRSLASH	'/'
#define LEX_EQUAL		'='
#define LEX_EXCLAMATION	'!'
#define LEX_LESS		'<'
#define LEX_GREATHER	'>'

#define LEX_RIGHTHESIS  ')'
#define LEX_SEMICOLON	';'
#define LEX_COLON   	':'
#define LEX_COMMA		','
#define LEX_LEFTBRACE	'{'
#define LEX_RIGHTBRACE	'}'
#define LEX_LEFTHESIS	'('

//таблица лексем
namespace LT {
    struct Entry
    {
        char lexema[LEXEME_FIXSIZE];
        int sn;     //номер строки
        int idxTI;  //индекс  в таблице идентификаторов
        Entry(char lex, int line_num, int ti_idx = LT_TI_NULLIDX)
            : sn(line_num), idxTI(ti_idx)
        {
            lexema[0] = lex;
        }
        Entry() : sn(0), idxTI(LT_TI_NULLIDX)
        {
            lexema[0] = '\0';
        }
    };

    struct LexTable
    {
        int maxsize;    
        int size;
        Entry* table;
    };

    LexTable Create(int size);                  // Создаем таблицу лексем размером size
    void Add(LexTable& lextable, Entry entry);
    Entry GetEntry(LexTable& lextable, int n);
    void Delete(LexTable& lextable);
}
