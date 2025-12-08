#include "Error.h"

namespace Error
{
    /* 
    серии ошибок:
 	      0 -  99 - системные ошибки
 		100 - 104 - ошибки входных параметров
 		105 - 109 - ошибки при открытии файла
 		110 - 119 - ошибки при чтении файла
 		120 - 140 - ошибки на этапе лексического анализатора
 		600 - 610 - ошибки на этапе синтаксического анализатора
 		700 - 720 - ошибки на этапе семантического анализатора
    */
    ERROR errors[ERROR_MAX_ENTRY] =
    {
        ERROR_ENTRY(0, "Недопустимый код ошибки"),
            ERROR_ENTRY(1, "Системный сбой"),
            
            ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4), ERROR_ENTRY_NODEF(5),
            ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
            ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40), ERROR_ENTRY_NODEF10(50),
            ERROR_ENTRY_NODEF10(60), ERROR_ENTRY_NODEF10(70), ERROR_ENTRY_NODEF10(80), ERROR_ENTRY_NODEF10(90),

            ERROR_ENTRY(100, "Параметр -in должен быть задан"),
            
            ERROR_ENTRY_NODEF(101), ERROR_ENTRY_NODEF(102), ERROR_ENTRY_NODEF(103),

            ERROR_ENTRY(104, "Превышена длина входного параметра"),
            
            ERROR_ENTRY_NODEF(105),ERROR_ENTRY_NODEF(106),ERROR_ENTRY_NODEF(107), ERROR_ENTRY_NODEF(108), ERROR_ENTRY_NODEF(109),

            ERROR_ENTRY(110, "Ошибка при открытии файла с исходным кодом (-in)"),
            ERROR_ENTRY(111, "Недопустимый символ в исходном файле (-in)"),
            ERROR_ENTRY(112, "Ошибка при создании файла протокола(-log)"),
            
            ERROR_ENTRY_NODEF(113), ERROR_ENTRY_NODEF(114), ERROR_ENTRY_NODEF(115),
            ERROR_ENTRY_NODEF(116), ERROR_ENTRY_NODEF(117), ERROR_ENTRY_NODEF(118), ERROR_ENTRY_NODEF(119),

            ERROR_ENTRY(120, "Превышен допустимый размер при создании таблиццы идентификаторов"),
            ERROR_ENTRY(121, "Превышен допустимый размер при создании таблиццы лексем"),
            ERROR_ENTRY(122, "Переполнение таблиццы лексем"),
            ERROR_ENTRY(123, "Переполнение таблиццы лексем"),
            ERROR_ENTRY(124, "Переданный индекс выходит за границы допустимых значений таблиццы лексем"),
            ERROR_ENTRY(125, "Переданный индекс выходит за границы допустимых значений таблиццы лексем"),
            
            ERROR_ENTRY_NODEF(126), ERROR_ENTRY_NODEF(127), ERROR_ENTRY_NODEF(128), ERROR_ENTRY_NODEF(129),
            ERROR_ENTRY_NODEF10(130), ERROR_ENTRY_NODEF10(140), ERROR_ENTRY_NODEF10(150),
            ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170), ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),
            ERROR_ENTRY_NODEF(200),
            
            ERROR_ENTRY(201, "Числовой литерал превышает максимальное значение (65535)"),
            ERROR_ENTRY(202, "Строковый литерал превышает максимальную длину"),
            ERROR_ENTRY(203, "Незакрытый строковый литерал"),
            ERROR_ENTRY(204, "Ожидалась ' (одинарная кавычка) для строкового литерала"),
            ERROR_ENTRY(205, "Незвестный символ"),
            
            ERROR_ENTRY_NODEF(206), ERROR_ENTRY_NODEF(207), ERROR_ENTRY_NODEF(208), ERROR_ENTRY_NODEF(209),
            ERROR_ENTRY_NODEF10(210), ERROR_ENTRY_NODEF10(220), ERROR_ENTRY_NODEF10(230), ERROR_ENTRY_NODEF10(240),
            ERROR_ENTRY_NODEF10(250), ERROR_ENTRY_NODEF10(260), ERROR_ENTRY_NODEF10(270), ERROR_ENTRY_NODEF10(280),
            ERROR_ENTRY_NODEF10(290), ERROR_ENTRY_NODEF100(300), ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500),
            
            ERROR_ENTRY(600, "Неверная структура программы"),
            ERROR_ENTRY(601, "Неверный тип данных: ожидался int, string или bool"),
            ERROR_ENTRY(602, "Ошибочный оператор"),
            ERROR_ENTRY(603, "Ошибка в выражении"),
            ERROR_ENTRY(604, "Ошибка в записи математической операции "),
            ERROR_ENTRY(605, "Ошибка в математической операции: ожидался +,-,* или /"),
            ERROR_ENTRY(606, "Ошибка в параметрах объявления функции"),
            ERROR_ENTRY(607, "Ошибка в части объявления параметра"),
            ERROR_ENTRY(608, "Ошибка в аргументах вызова функции"),
            ERROR_ENTRY(609, "Неверный операнд в условии/инициализации"),
            ERROR_ENTRY(610, "Ошибка в блоке if/else"),
            ERROR_ENTRY(611, "Ошибка в условии (предикате)"),
            ERROR_ENTRY(612, "Ошибка при инициализации переменной"),
            ERROR_ENTRY(613, "Ошибка в switch-case"),
            

            ERROR_ENTRY_NODEF(614), ERROR_ENTRY_NODEF(615),ERROR_ENTRY_NODEF(616), ERROR_ENTRY_NODEF(617), 
            ERROR_ENTRY_NODEF(618),ERROR_ENTRY_NODEF(619), ERROR_ENTRY_NODEF10(620),ERROR_ENTRY_NODEF10(630),
            ERROR_ENTRY_NODEF10(640),ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660),ERROR_ENTRY_NODEF10(670),
            ERROR_ENTRY_NODEF10(680),ERROR_ENTRY_NODEF10(690),

            ERROR_ENTRY(700, "Ошибочное деление на 0"),
            ERROR_ENTRY(701, "Типы данных в выражении не совпадают"),
            ERROR_ENTRY_NODEF(702),
            ERROR_ENTRY(703, "Тип функции и возвращаемое значение не совпадают"),
            ERROR_ENTRY(704, "Несовпадение типов передаваемых параметров"),
            ERROR_ENTRY(705, "Количество ожидаемых функцией и передаваемых параметров не совпадают"),
            ERROR_ENTRY_NODEF(706),

            ERROR_ENTRY_NODEF(707), ERROR_ENTRY_NODEF(708), ERROR_ENTRY_NODEF(709),
            ERROR_ENTRY_NODEF10(710), ERROR_ENTRY_NODEF10(720), ERROR_ENTRY_NODEF10(730), ERROR_ENTRY_NODEF10(740), ERROR_ENTRY_NODEF10(750),
            ERROR_ENTRY_NODEF10(760), ERROR_ENTRY_NODEF10(770), ERROR_ENTRY_NODEF10(780), ERROR_ENTRY_NODEF10(790),
            ERROR_ENTRY_NODEF100(800), ERROR_ENTRY_NODEF100(900)
    };

    ERROR  geterror(int  id)
    {
        ERROR result;

        if (id < 0 || id >= ERROR_MAX_ENTRY)
        {
            result = errors[0];
        }
        else
        {
            result = errors[id];
        }

        return result;
    }
    ERROR  geterrorin(int  id, int  line = -1, int  col = -1)
    {
        ERROR result;

        if (id < 0 || id >= ERROR_MAX_ENTRY)
        {
            result = errors[0];
        }
        else
        {
            result = errors[id];

            result.inext.col = col;
            result.inext.line = line;
        }

        return result;
    }
};