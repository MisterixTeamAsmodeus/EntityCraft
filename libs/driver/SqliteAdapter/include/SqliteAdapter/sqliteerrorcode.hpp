#pragma once

#include "sqliteadapter_global.hpp"
#include <sqlite3.h>

namespace database_adapter {
namespace sqlite {

/**
 * @brief Перечисление кодов ошибок SQLite
 * @details Содержит основные коды ошибок, возвращаемые SQLite API.
 *          Значения соответствуют константам из sqlite3.h
 */
enum class SQLITE_EXPORT sqlite_error_code : int
{
    OK = SQLITE_OK,                    ///< Успешное выполнение операции
    ERROR = SQLITE_ERROR,               ///< Общая ошибка SQL
    INTERNAL = SQLITE_INTERNAL,         ///< Внутренняя ошибка SQLite
    PERM = SQLITE_PERM,                ///< Ошибка доступа
    ABORT = SQLITE_ABORT,              ///< Операция прервана
    BUSY = SQLITE_BUSY,                ///< База данных заблокирована
    LOCKED = SQLITE_LOCKED,            ///< Таблица заблокирована
    NOMEM = SQLITE_NOMEM,              ///< Недостаточно памяти
    READONLY = SQLITE_READONLY,        ///< Попытка записи в базу только для чтения
    INTERRUPT = SQLITE_INTERRUPT,      ///< Операция прервана
    IOERR = SQLITE_IOERR,              ///< Ошибка ввода/вывода
    CORRUPT = SQLITE_CORRUPT,          ///< База данных повреждена
    NOTFOUND = SQLITE_NOTFOUND,        ///< Не найдено
    FULL = SQLITE_FULL,                ///< База данных заполнена
    CANTOPEN = SQLITE_CANTOPEN,        ///< Невозможно открыть файл базы данных
    PROTOCOL = SQLITE_PROTOCOL,        ///< Ошибка протокола базы данных
    EMPTY = SQLITE_EMPTY,              ///< База данных пуста
    SCHEMA = SQLITE_SCHEMA,            ///< Схема базы данных изменилась
    TOOBIG = SQLITE_TOOBIG,            ///< Строка или BLOB слишком большой
    CONSTRAINT = SQLITE_CONSTRAINT,    ///< Нарушение ограничения
    MISMATCH = SQLITE_MISMATCH,        ///< Несоответствие типов данных
    MISUSE = SQLITE_MISUSE,            ///< Неправильное использование библиотеки
    NOLFS = SQLITE_NOLFS,              ///< Используется функция, не поддерживаемая ОС
    AUTH = SQLITE_AUTH,                ///< Ошибка авторизации
    FORMAT = SQLITE_FORMAT,            ///< Неверный формат вспомогательного файла базы данных
    RANGE = SQLITE_RANGE,              ///< Индекс параметра вне допустимого диапазона
    NOTADB = SQLITE_NOTADB,            ///< Файл не является базой данных SQLite
    NOTICE = SQLITE_NOTICE,            ///< Уведомление от SQLite
    WARNING = SQLITE_WARNING,          ///< Предупреждение от SQLite
    ROW = SQLITE_ROW,                  ///< sqlite3_step() вернул строку данных
    DONE = SQLITE_DONE                 ///< sqlite3_step() завершил выполнение
};

/**
 * @brief Преобразует код ошибки SQLite (int) в enum sqlite_error_code
 * @param code Целочисленный код ошибки SQLite
 * @return Соответствующий enum sqlite_error_code
 */
SQLITE_EXPORT sqlite_error_code to_sqlite_error_code(int code);

/**
 * @brief Преобразует enum sqlite_error_code в целочисленный код ошибки SQLite
 * @param code Enum код ошибки
 * @return Целочисленный код ошибки SQLite
 */
SQLITE_EXPORT int to_int(sqlite_error_code code);

/**
 * @brief Проверяет, является ли код ошибки успешным результатом
 * @param code Код ошибки для проверки
 * @return true, если код указывает на успех (OK, ROW, DONE), иначе false
 */
SQLITE_EXPORT bool is_success(sqlite_error_code code);

/**
 * @brief Проверяет, является ли код ошибки критической ошибкой
 * @param code Код ошибки для проверки
 * @return true, если код указывает на критическую ошибку, иначе false
 */
SQLITE_EXPORT bool is_critical_error(sqlite_error_code code);

} // namespace sqlite
} // namespace database_adapter

