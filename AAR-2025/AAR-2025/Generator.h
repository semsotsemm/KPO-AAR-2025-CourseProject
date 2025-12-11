#pragma once
#include "LT.h"
#include "IT.h"
#include <iostream>
#include <stack>
#include "PolishNotation.h"

#define BEGIN	".586\n\
.model flat, stdcall\n\
includelib kernel32.lib\n\
includelib libucrt.lib\n\
includelib ../Debug/StaticLib.lib\n\n\
ExitProcess PROTO : DWORD\n\
write_int PROTO C : DWORD\n\
write_str PROTO C : DWORD\n\
out_bool PROTO : DWORD\n\
lenght PROTO C : DWORD\n\
copy PROTO C\n\
powNumber PROTO C \n\
random PROTO C\n\
factorialOfNumber PROTO C : DWORD\n\
squareOfNumber PROTO C : DWORD\n\n\
.stack 4096\n"

#define DATA_SEG ".data\n"
#define CODE_SEG ".code\n"


namespace GN
{
	enum LabelType { L_IF, L_SWITCH, L_CASE, L_DEFAULT };

	struct LabelInfo {
		int id;
		LabelType type;
		int parentSwitchID;
	};

	void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream);
}