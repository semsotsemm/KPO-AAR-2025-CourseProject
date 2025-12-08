#pragma once
#include "LT.h"
#include "IT.h"
#include <iostream>
#include <stack>
#include "PolishNotation.h"

#define BEGIN	".586\n						\
.model flat, stdcall\n						\
includelib kernel32.lib\n					\
includelib libucrt.lib\n					\
includelib ../Debug/StaticLib.lib\n\n		\
ExitProcess PROTO : DWORD\n					\
write_int PROTO C : SDWORD\n				\
write_str PROTO C : DWORD\n					\
out_bool PROTO : DWORD\n\n					\
.stack 4096\n"
#define DATA_SEG ".data\n"
#define CODE_SEG ".code\n"


namespace GN
{
	// Типы областей видимости для генерации меток
	enum LabelType { L_IF, L_SWITCH, L_CASE, L_DEFAULT };

	struct LabelInfo {
		int id;			    // Уникальный номер метки
		LabelType type;     // Тип (IF, SWITCH, CASE)
		int parentSwitchID; // ID свича, к которому относится кейс (чтобы прыгнуть в конец)
	};

	void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream);
}