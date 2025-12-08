#include "SemanticAnaliz.h"
#include "PolishNotation.h"
#include "Generator.h"
#include "Lexer.h"
#include "Error.h"
#include "MFST.h"	
#include "Parm.h"
#include "GRB.h"	
#include "Log.h"
#include "LT.h"
#include "IT.h"
#include "In.h"

#include <iostream>
#include <tchar.h>
#include <fstream>     
#include <cstdlib>     
#include <locale>


int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL, "rus");

    Log::LOG log = Log::INITLOG;

    LT::LexTable lextable = LT::Create(LT_MAXSIZE);
    IT::IdTable idtable = IT::Create(TI_MAXSIZE);

    try
    {
        Parm::PARM parm = Parm::getparm(argc, argv);    // Генерируем файлы

        log = Log::getlog(parm.log);    //  Открываем поток для записи
        Log::WriteLog(log);
        Log::WriteParm(log, parm);

        In::IN in = In::getin(parm.in, parm.out);   // Пробуем прочитать файл

        Log::WriteLine(log, (char*)"---------- Лексический анализ --------\n", "");
        Lexer::Analyze(in, lextable, idtable);
         
        Log::WriteLine(log, (char*)"Лексический анализ прошел успешно.\n", "");

        Log::WriteLine(log, (char*)"--------- Результаты анализа --------\n", "");
        Log::WriteLine(log, (char*)"Таблица лексем:\n", "");
        Log::WriteLexTable(log, lextable);
        Log::WriteLine(log, (char*)"Таблица идентификаторов:\n", "");
        Log::WriteIdTable(log, idtable);
        Log::WriteLine(log, (char*)"---------- Синтаксический анализ --------\n", "");

        MFST_TRACE_START
            MFST::Mfst mfst(lextable, GRB::getGreibach());

        bool ok = mfst.start();
        if (ok)
        {

            Log::WriteLine(log, (char*)"Синтаксический анализ прошел успешно.\n", "");

            Log::WriteLine(log, (char*)"--------- Результат анализа --------\n", "");
            Log::WriteLine(log, (char*)"Таблица лексем:\n", "");
            Log::WriteLexTable(log, lextable);
            Log::WriteLine(log, (char*)"---------- Семантический анализ --------\n", "");

            SM::semAnaliz(lextable, idtable);

            Log::WriteLine(log, (char*)"Семантический анализ прошел успешно.\n", "");
            Log::WriteLine(log, (char*)"--------- Результат анализа --------\n", "");

            Log::WriteLine(log, (char*)"Таблица лексем:\n", "");
            Log::WriteLexTable(log, lextable);

            Log::WriteLine(log, (char*)"--------- Преобразование кода в обратную польскую запись ---------\n", "");
            int pos = PN::findExpression(lextable);
            while (pos != 0)
            {
                PN::polishNotation(pos, lextable, idtable);
                pos = PN::findExpression(lextable);
            }

            Log::WriteLine(log, (char*)"Преобразование кода в ОПЗ прошло успешно.\n", "");
            Log::WriteLine(log, (char*)"--------- Результат преобразования в ОПЗ ---------\n", "");
            Log::WriteLine(log, (char*)"Таблица лексем:\n", "");
            Log::WriteLexTable(log, lextable);
            Log::WriteLine(log, (char*)"---------- Генерация кода в Ассемблер --------\n", "");

            std::ofstream asmFile(parm.out);
            if (!asmFile.is_open())
            {
                std::cout << "Ошибка: Не удалось открыть файл " << parm.out << std::endl;
            }

            GN::Generate(lextable, idtable, &asmFile);

            asmFile.close();

            Log::WriteLine(log, (char*)"Ассемблер успешно сгенерирован.\n", "");
            Log::WriteLine(log, (char*)"---------- Результат генерации --------\n", "");
            WriteLine(log, (wchar_t*)L"Файл с исходным кодом на языке ассемблер находится по пути: ", (wchar_t*)parm.out, L"");
        }
        else
        {
            Log::WriteLine(log, (char*)"Синтаксический анализ обнаружил ошибки.\n", "");
        }

        mfst.savededucation();
        mfst.printrules();


        LT::Delete(lextable);
        IT::Delete(idtable);
    }
    catch (Error::ERROR e)
    {
        std::cout << "ОШИБКА: " << e.message << std::endl;
        Log::WriteError(log, e);
        Log::Close(log);
        return -1;
    }

    return 0;
}