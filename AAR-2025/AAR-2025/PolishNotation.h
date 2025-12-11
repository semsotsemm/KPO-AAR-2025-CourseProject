#pragma once
#include <string>

#include "LT.h"
#include "IT.h"

#define OP_EQUAL_EQUAL     '#' 
#define OP_GREATER_EQUAL   'G' 
#define OP_LESS_EQUAL      'L' 

#include <queue>

namespace PN
{
	int findExpression(LT::LexTable& lextable);
	bool polishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable);
	int priority(char v);
	bool IsStandardFunction(std::string name);
}