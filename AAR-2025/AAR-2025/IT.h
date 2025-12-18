#pragma once
#include <string>
#include <stack>
#include <vector>

using namespace std;

#define ID_MAXSIZE			21				// максимальное количество символов в идентификаторе + '\0'
#define TI_MAXSIZE			4096			// максимальное количество эл-ов в таблице идентификаторов 
#define TI_INT_DEFAULT		0x00000000		
#define TI_STR_DEFAULT		0x00			
#define TI_BOOL_DEFAULT		0x00			
#define TI_NULLIDX			0xffffffff		// ссылка, указывающая отсутсвие индефикатора
#define GLOBAL				"global"		
#define TI_STR_MAXSIZE		256				
#define TI_INT_MAX			32768			
#define TI_INT_MIN			-32768			
#define FUNC_COPY			"copy"					
#define FUNC_LEN			"lenght"				
#define FUNC_TIME			"getLocalTimeAndDate"	
#define FUNC_POW			"powNumber"				
#define FUNC_RANDOM			"random"				
#define FUNC_FACTORIAL		"factorialOfNumber"		
#define FUNC_SQUARE			"squareOfNumber"		


#define PARM_ID_DEFAULT_LOCATION		L"C:\\Users\\user\\work_space\\Laboratory_work\\AAR-2025\\Files\\in"
#define PARM_ID_DEFAULT_EXT				L".id.txt" 

namespace IT	
{
	enum class IDDATATYPE { DEF, INT, STR, BOOL};	// типы данных идентификаторов: не определен, int, string, bool
	enum class IDTYPE { D, V, F, P, L};				// типы идентификаторов: не определен, переменная, функция, параметр, литерал

	struct Entry	// строка таблицы идентификаторов
	{
		int idxfirstLE;							// индекс первой строки в таблице лексем
		char areaOfVisibility[ID_MAXSIZE]{};	
		char id[ID_MAXSIZE]{};					// имя идентификатора (автоматически усекается до ID_MAXSIZE)
		IDDATATYPE	iddatatype;					
		IDTYPE	idtype;							
		union
		{
			int vint;							
			struct
			{
				unsigned char len;				
				char str[TI_STR_MAXSIZE];		
			} vstr;								
		}value;									// значение идентификатора
		struct Param
		{
			int count;							
			vector<IDDATATYPE> types;			
		} params;
		string FullName;
		Entry(int idxfirstLE, string areaOfVisibility, const char* id, IDDATATYPE iddatatype, IDTYPE idtype);
		Entry(int idxfirstLE, IDDATATYPE iddatatype, IDTYPE idtype, char* value);
	};
	struct IdTable					
	{
		int maxsize;				
		int current_size;			
		Entry** table;				

		IdTable(int size);
	};

	static Entry len(0, string(GLOBAL), FUNC_LEN, IDDATATYPE::INT, IDTYPE::F);
	static Entry copy(0, string(GLOBAL), FUNC_COPY, IDDATATYPE::STR, IDTYPE::F);
	static Entry time(0, string(GLOBAL), FUNC_TIME, IDDATATYPE::STR, IDTYPE::F);
	static Entry pow(0, string(GLOBAL), FUNC_POW, IDDATATYPE::INT, IDTYPE::F);
	static Entry random(0, string(GLOBAL), FUNC_RANDOM, IDDATATYPE::INT, IDTYPE::F);
	static Entry factorial(0, string(GLOBAL), FUNC_FACTORIAL, IDDATATYPE::INT, IDTYPE::F);
	static Entry square(0, string(GLOBAL), FUNC_SQUARE, IDDATATYPE::INT, IDTYPE::F);

	void Add(IdTable& idtable, Entry* entry);
	Entry GetEntry(IdTable& idtable, int n);
	int IsId(IdTable& idtable, char id[ID_MAXSIZE], stack<string> areaOfVisibility);
	int IsLiteralInt(IdTable& idtable, char* lexema);
	int IsLiteralString(IdTable& idtable, char* lexema);
	int IsLiteralBool(IdTable& idtable, char* lexema);
	void PrintIdTable(IdTable& idtable, const wchar_t* in);
	void Delete(IdTable& idtable);
}