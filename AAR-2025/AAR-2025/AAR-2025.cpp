#include "stdafx.h"
#include "Parm.h"	
#include "In.h"		
#include "Log.h"	
#include "Out.h"	
#include "FST.h"
#include "LT.h"
#include "IT.h"
#include "graphs.h"
#include "Error.h"
#include "LexAnalysis.h"
#include "GRB.h"
#include "MFST.h"
#include "SemanticAnaliz.h"
#include "PolishNotation.h"
#include "Generation.h"

using namespace std;

int wmain(int argc, wchar_t* argv[])
{
	setlocale(LC_ALL, "rus");
	Log::LOG log;
	Out::OUT out;
	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);				
		log = Log::getlog(parm.log);								
		Log::WriteLog(log.stream);									
		Log::WriteParm(log.stream, parm);							

		In::IN in = In::getin(parm.in);								
		Log::WriteIn(log.stream, in);								

		LT::LexTable lex(LT_MAXSIZE);								
		IT::IdTable id(TI_MAXSIZE);									

		LexAnalysis(in, lex, id);
		Log::WriteLine(&std::cout, "Лексический анализ завершен без ошибок", "");
		Delete(in);

		PrintLexTable(lex, L"Table");
		PrintIdTable(id, L"Table");
		
		MFST_TRACE_START(log.stream)
		MFST::MFST sintaxAnaliz(lex, GRB::getGreibach());
		bool syntax_ok = sintaxAnaliz.start(log.stream);
		if (!syntax_ok)
		{
			Log::WriteLine(log.stream, "Синтаксический анализ завершен с ошибками", "");
			Log::WriteLine(&std::cout, "Синтаксический анализ завершен с ошибками\n", "Выполнение программы остановлено", "");
			return 0;
		}
		Log::WriteLine(&std::cout, "Синтаксический анализ завершен без ошибок", "");
		sintaxAnaliz.printRules(log.stream);

		SM::semAnaliz(lex, id);
		Log::WriteLine(&std::cout, "Семантический анализ завершен без ошибок", "");

		out = Out::getout(parm.out);										
		GN::GenerationASM(out.stream, lex, id);
		Log::WriteLine(log.stream, "\nПрограмма успешно завершена!", "");
		Log::WriteLine(&std::cout, "\nПрограмма успешно завершена!", "");


		Delete(lex);
		Delete(id);
		Out::Close(out);
		Log::Close(log);
	}

	catch (Error::ERROR e)
	{
		if (e.id >= PARM_BEGIN && e.id <= PARM_END)
			cout << "Ошибка " << e.id << ": " << e.message << endl << endl;
		
		else
		{
			Log::WriteError(log.stream, e);
			Log::Close(log);
			cout << e.message << endl << "Подробнее в протоколе (log файле)" << endl;
		}
	}

	return 0;
}
