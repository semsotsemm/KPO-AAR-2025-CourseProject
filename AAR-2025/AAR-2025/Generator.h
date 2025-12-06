#pragma once
#include <iostream>

#include "LT.h"
#include "IT.h"

// Определения для ASM
#define BEGIN \
    ".586\n" \
    ".model flat, stdcall\n" \
    "includelib kernel32.lib\n" \
    "includelib libucrt.lib\n" \
    "includelib ../Debug/StaticLib.lib\n\n" \
    "ExitProcess PROTO : DWORD\n" \
    "write_int PROTO C : SDWORD\n"  /* <-- ИСПРАВЛЕНО */ \
    "write_str PROTO C : DWORD\n"  /* <-- ИСПРАВЛЕНО */ \
    "out_bool PROTO : DWORD\n\n" \
    ".stack 4096\n" \
    ".const\n"

#define DATA_SEG ".data\n"
#define CODE_SEG ".code\n"

namespace GN
{
    void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream);
    std::string GetUniqueLiteralName(const IT::Entry& entry, int index);
}