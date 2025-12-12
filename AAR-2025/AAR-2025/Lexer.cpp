#include "Lexer.h"
#include "Error.h"
#include "In.h"

#include <map>
#include <string>
#include <iostream>


namespace Lexer
{

    bool IsStandardFunction(std::string name) {
        return (name == "lenght" || name == "copy" || name == "powNumber" ||
            name == "random" || name == "factorialOfNumber" ||
            name == "squareOfNumber" || name == "write_int" || name == "write_str" || name == "out_bool");
    }

    void RegisterStandardFunctions(IT::IdTable& it)
    {
        std::vector<std::tuple<std::string, IT::IDDATATYPE, int>> std_functions = {
            {"lenght", IT::INT, 1},             
            {"copy", IT::STR, 3},               
            {"powNumber", IT::INT, 2},          
            {"random", IT::INT, 2},             
            {"factorialOfNumber", IT::INT, 1},  
            {"squareOfNumber", IT::INT, 1},     
            {"write_int", IT::UNDEF, 1},        
            {"write_str", IT::UNDEF, 1},        
            {"out_bool", IT::UNDEF, 1}          
        };

        for (const auto& func : std_functions)
        {
            IT::Entry entry;

            std::string name = std::get<0>(func);
            IT::IDDATATYPE returnType = std::get<1>(func);
            int paramCount = std::get<2>(func);

            strcpy_s(entry.id, name.c_str());
            entry.idtype = IT::F;
            entry.iddatatype = returnType;
            if (entry.iddatatype == IT::INT)
            {
                entry.value.vint = 0;
            }
            else
            {
                entry.value.vstr.len = 5;
                strcpy_s(entry.value.vstr.str, "UNDEF");
            }
            entry.params.count = paramCount;
            entry.params.types.clear();

            entry.idxfirstLE = -1;

            IT::Add(it, entry);
        }
    }

    void Analyze(In::IN& in, LT::LexTable& lt, IT::IdTable& it)
    {
        RegisterStandardFunctions(it);
        std::map<std::string, char> keywords;
        bool isFunctionDeclaration = false;
        int currentFunctionId = TI_NULLIDX;
        bool isParameterDeclaration = false;
        int currentDataType;

        keywords["int"] = LEX_INTEGER;
        keywords["string"] = LEX_STRING;
        keywords["bool"] = LEX_BOOL;

        keywords["def"] = LEX_FUNCTION;
        keywords["return"] = LEX_RETURN;
        keywords["print"] = LEX_PRINT;
        keywords["case"] = LEX_CASE;
        keywords["default"] = LEX_DEFAULT;
        keywords["main"] = LEX_MAIN;
        keywords["if"] = LEX_IF;
        keywords["switch"] = LEX_SWITCH;
        keywords["param"] = LEX_PARAM;
        keywords["let"] = LEX_LET;
        keywords["else"] = LEX_ELSE;
        keywords[":"] = LEX_COLON;
        keywords["true"] = LEX_TRUE;
        keywords["false"] = LEX_FALSE;

        int line = 1;
        int position = 0;

        for (int i = 0; i < in.size; i++, position++)
        {
            char c = in.text[i];

            if (c == '|')
            {
                ++line;
                position = 0;
                continue;
            }

            else if (isspace(c))
            {
                position++;
                continue;
            }

            else if (isalpha(c))
            {
                std::string word = "";
                while (i < in.size && (isalpha(in.text[i]) || in.text[i] == '_' || isdigit(in.text[i])))
                {
                    position++;
                    word += in.text[i];
                    ++i;
                }
                position--;
                --i;   
                if (keywords.count(word))   // Ключевое слово?
                {
                    char lexema = keywords[word];
                    if (word == "int")
                    {
                        currentDataType = IT::IDDATATYPE::INT;
                    }
                    else if (word == "string")
                    {
                        currentDataType = IT::IDDATATYPE::STR;
                    }
                    else if (word == "bool")
                    {   
                        currentDataType = IT::IDDATATYPE::BOOL;
                    }
                    else if (word == "def")
                    {
                        isFunctionDeclaration = true;
                        isParameterDeclaration = false;
                    }
                    else if (word == "param")
                    {
                        isParameterDeclaration = true;
                    }
                    else
                    {
                        currentDataType = IT::UNDEF; 
                    }
                    LT::Entry entry = { keywords[word], line, LT_TI_NULLIDX };
                    LT::Add(lt, entry);
                }
                else    // Индетификатор (имя переменной, функции и тд)
                {
                    if (word.length() > ID_MAXSIZE)
                    {
                        word = word.substr(0, ID_MAXSIZE); // Сокращаем длину индетификатора 
                    }

                    int identId = IT::IsId(it, (char*)word.c_str()); // Использовался ли ранее

                    if (identId == TI_NULLIDX)  
                    {
                        IT::Entry entry;
                        IT::IDDATATYPE currentType = (IT::IDDATATYPE)currentDataType;

                        strcpy_s(entry.id, word.c_str());
                        IT::IDTYPE determinedIdType = IT::V; // По умолчанию - переменная

                        if (isFunctionDeclaration )  // Имя функции
                        {
                            determinedIdType = IT::F;
                            isFunctionDeclaration = false;
                            entry.params.count = 0;
                            entry.params.types.clear();

                        }
                        else if (isParameterDeclaration) // Параметр
                        {
                            determinedIdType = IT::P;       
                            isParameterDeclaration = false; 
                        }

                        entry.idtype = determinedIdType;
                        entry.iddatatype = currentType;
                        if (currentType == IT::STR)
                        {
                            entry.value.vstr.len = 4;
                            strcpy_s(entry.value.vstr.str, TI_STR_DEFAULT);
                        }
                        else if (currentType == IT::INT)
                        {
                            entry.value.vint = TI_INT_DEFAULT;
                        }
                        else if (currentType == IT::BOOL)
                        {
                            entry.value.vstr.len = 5;
                            strcpy_s(entry.value.vstr.str, "UNDEF");
                        }
                        else
                        {
                            entry.value.vstr.len = 5;
                            strcpy_s(entry.value.vstr.str, "UNDEF");
                        }
                        entry.idxfirstLE = lt.size + 1;     // Новый номер лексемы

                        IT::Add(it, entry);
                        identId = it.size - 1;  // Новый номер лексемы

                        if (determinedIdType == IT::F)
                        {
                            currentFunctionId = identId;
                        }
                        if (determinedIdType == IT::P && currentFunctionId != TI_NULLIDX)
                        {
                            IT::Entry funcEntry = IT::GetEntry(it, currentFunctionId);

                            funcEntry.params.count++;
                            funcEntry.params.types.push_back(currentType);

                            it.table[currentFunctionId] = funcEntry;
                        }
                        currentDataType = IT::UNDEF;
                    }
                    else if (lt.table[lt.size - 2].lexema[0] == LEX_LET || 
                        lt.table[lt.size - 2].lexema[0] == LEX_FUNCTION ||
                        lt.table[lt.size - 2].lexema[0] == LEX_PARAM)
                    {
                        ERROR_THROW(206);
                    }

                    LT::Entry lexEntry = { LEX_ID, line, identId }; 
                    LT::Add(lt, lexEntry);  
                }
            }

            else if (isdigit(c))
            {
                std::string number = "";
                while (i < in.size && isdigit(in.text[i]))
                {
                    position++;
                    number += in.text[i];
                    ++i;
                    position--;
                }
                position--;
                --i;

                int literal = std::stoi(number);

                if (literal > 32767)
                {
                    ERROR_THROW(201);
                }
                if (literal < -32767)
                {
                    ERROR_THROW(201);
                }

                IT::Entry entry;
                strcpy_s(entry.id, "lnum");
                entry.value.vint = literal;
                entry.idtype = IT::L;
                entry.iddatatype = IT::INT;
                entry.idxfirstLE = lt.size + 1;

                IT::Add(it, entry);
                int literalId = it.size - 1;

                LT::Entry lexEntry = { LEX_LITERAL, line, literalId };
                LT::Add(lt, lexEntry);
            }

            else if (c == '\'')
            {
                std::string literal = "";
                i++;    

                while (i < in.size && in.text[i] != '\'')
                {
                    literal += in.text[i];
                    position++;
                    ++i;
                }

                if (i == in.size || in.text[i] != '\'')
                {
                    ERROR_THROW(203);
                    ERROR_THROW_IN(204, line, position);
                }

                if (literal.size() > TI_STR_MAXSIZE)
                {
                    ERROR_THROW(202);
                }

                IT::Entry entry;
                strcpy_s(entry.id, "Lstr");
                strcpy_s(entry.value.vstr.str, literal.c_str());
                entry.value.vstr.len = (char)literal.length();
                entry.idtype = IT::L;
                entry.iddatatype = IT::STR;
                entry.idxfirstLE = lt.size + 1;

                IT::Add(it, entry);
                int literalId = it.size - 1;

                LT::Entry lexEntry = { LEX_LITERAL, line, literalId };
                LT::Add(lt, lexEntry);

                continue;
            }
            // Операции или скобки
            else
            {
                position++;
                char symbol = 0;
                
                switch (in.text[i])
                {
                case ';': symbol = LEX_SEMICOLON;
                    break;
                case ':': symbol = LEX_COLON;
                    break;
                case ',': symbol = LEX_COMMA;
                    break;
                case '{': symbol = LEX_LEFTBRACE;
                    break;
                case '}': symbol = LEX_RIGHTBRACE;
                    break;
                case '(': symbol = LEX_LEFTHESIS;
                    break;
                case ')': 
                    symbol = LEX_RIGHTHESIS;
                    currentFunctionId = TI_NULLIDX;
                    break;
                case '+': symbol = LEX_PLUS;
                    break;
                case '-': symbol = LEX_MINUS;
                    break;
                case '*': symbol = LEX_STAR;
                    break;
                case '%': symbol = LEX_PECENT;
                    break;
                case '/': symbol = LEX_DIRSLASH;
                    break;
                case '=': symbol = LEX_EQUAL;
                    break;
                case '>': symbol = LEX_GREATHER;
                    break;
                case '<': symbol = LEX_LESS;
                    break;
                case '!': symbol = LEX_EXCLAMATION;
                    break;
                }
                if (symbol == 0)
                {
                    std::cout << in.text[i];
                    ERROR_THROW(205, line, position);
                }
                LT::Entry lexEntry = { symbol, line, LT_TI_NULLIDX };
                LT::Add(lt, lexEntry);
            }
        }
    }
}