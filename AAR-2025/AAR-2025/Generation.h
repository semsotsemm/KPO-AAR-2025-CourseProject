#pragma once
#include "LT.h"
#include "IT.h"
#include "PolishNotation.h"

#define IT_ENTRY(x)		idtable.table[lextable.table[x]->idxTI]
#define LT_ENTRY(x)		lextable.table[x]


#define BEGIN			".586\n"\
					   << ".model flat, stdcall\n"\
					   << "includelib kernel32.lib\n"\
					   << "includelib libucrt.lib\n"\
					   << "includelib StaticLib.lib\n\n"\
					   << "ExitProcess PROTO: dword\n\n"\

#define EXTERN			 "EXTRN lenght: proc\n"\
					   << "EXTRN write_int: proc\n"\
					   << "EXTRN write_str : proc\n"\
					   << "EXTRN copy: proc\n"\
					   << "EXTRN getLocalTimeAndDate: proc\n"\
					   << "EXTRN random: proc\n"\
					   << "EXTRN squareOfNumber: proc\n"\
					   << "EXTRN factorialOfNumber: proc\n"\
					   << "EXTRN powNumber: proc\n\n"\

#define STACK(value)	".stack " << value << "\n\n"

#define CONST			".const\nnulError byte 'error divided by zero', 0\nnul sdword 0, 0\n"

#define DATA			".data"

#define CODE			".code\n"

#define END				"jmp goodExit\nerrorExit:\npush offset nulError\ncall write_str\ngoodExit:\npush 0\ncall ExitProcess\nmain ENDP\nend main"


namespace GN
{
	enum SwitchEnum { isSwitch };
	struct B // —труктура дл€ отслеживани€ контекста switch
	{
		int openRightbrace; 
		int branchingNum;   
		SwitchEnum swEnum;  
		int currentCaseIdx = 0;
		B(int o, int n, SwitchEnum se) : openRightbrace(o), branchingNum(n), swEnum(se) {}
	};
	enum class IfEnum { thenOrElse, repeat };
	struct A
	{
		int openRightbrace;
		int branchingnNum;
		IfEnum ifEnum;
		A(int open, int branch, IfEnum ifE)
		{
			openRightbrace = open;
			branchingnNum = branch;
			ifEnum = ifE;
		}
	};

	void GenerationASM(std::ostream* stream, LT::LexTable& lextable, IT::IdTable& idtable);
	void GenConstAndData(IT::IdTable& idtable, ostream* file);
	void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file);

	string GenEqualCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i);
	string GenFunctionCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i);
	string GenExitCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, string funcname);
	string GenCallFuncCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i);
	string GenBranchingCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, int branchingnNum);

	string itoS(int x);
}