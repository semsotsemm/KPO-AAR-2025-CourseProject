#pragma once
#include <vector>

#define ID_MAXSIZE 5
#define TI_MAXSIZE 4096
#define TI_INT_DEFAULT 0
#define TI_NULLIDX 0xffffffff
#define TI_STR_MAXSIZE 255


namespace IT {
    enum IDDATATYPE { INT = 1, STR = 2, BOOL = 3, UNDEF = 3 };
    enum IDTYPE { V = 1, F = 2, P = 3, L = 4 };

    struct Entry {
        int idxfirstLE;                
        char id[ID_MAXSIZE + 1];
        IDDATATYPE iddatatype;
        IDTYPE idtype;
        union {
            int vint;
            struct {
                char len;
                char str[TI_STR_MAXSIZE - 1];
            } vstr;
        } value;

        struct Param
        {
            int count;							// количество параметров функции
            std::vector<IDDATATYPE> types;			// типы параметров функции
        } params;
    };

    struct IdTable {
        int maxsize;
        int size;
        Entry* table;
    };


    IdTable Create(int size);                       // Создаем таблицу идентификаторов размером size
    void Add(IdTable& idtable, Entry& entry);
    Entry& GetEntry(IdTable& idtable, int n);
    int IsId(IdTable& idtable, char id[ID_MAXSIZE]);
    const char* GetDataTypeString(int iddatatype);
    const char* GetIdTypeString(int idtype);
    void Delete(IdTable& idtable);
}