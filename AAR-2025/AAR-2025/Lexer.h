#pragma once

#include "In.h"
#include "LT.h"
#include "IT.h"

#include <string>

namespace Lexer {
    void Analyze(In::IN& in, LT::LexTable& lt, IT::IdTable& it);
    bool IsStandardFunction(std::string name);
}