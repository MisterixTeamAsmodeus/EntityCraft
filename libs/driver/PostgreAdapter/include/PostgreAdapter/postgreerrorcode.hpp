#pragma once

#include "postgreadapter_global.hpp"
#include <libpq-fe.h>

namespace database_adapter {
namespace postgre {

/**
 * @brief Перечисление кодов ошибок PostgreSQL (ExecStatusType)
 * @details Содержит основные коды статуса выполнения запросов PostgreSQL.
 *          Значения соответствуют константам из libpq-fe.h
 */
enum class POSTGRE_EXPORT postgre_exec_status : int
{
    EMPTY_QUERY = PGRES_EMPTY_QUERY,              ///< Пустой запрос
    COMMAND_OK = PGRES_COMMAND_OK,                ///< Команда выполнена успешно
    TUPLES_OK = PGRES_TUPLES_OK,                  ///< Запрос вернул кортежи успешно
    COPY_OUT = PGRES_COPY_OUT,                    ///< Копирование данных из сервера
    COPY_IN = PGRES_COPY_IN,                      ///< Копирование данных в сервер
    BAD_RESPONSE = PGRES_BAD_RESPONSE,            ///< Неверный ответ от сервера
    NONFATAL_ERROR = PGRES_NONFATAL_ERROR,        ///< Нефатальная ошибка
    FATAL_ERROR = PGRES_FATAL_ERROR,              ///< Фатальная ошибка
    COPY_BOTH = PGRES_COPY_BOTH,                  ///< Копирование в обе стороны
    SINGLE_TUPLE = PGRES_SINGLE_TUPLE             ///< Один кортеж (асинхронный режим)
};

/**
 * @brief Перечисление кодов статуса подключения PostgreSQL (PGconnStatusType)
 * @details Содержит основные коды статуса подключения к PostgreSQL.
 *          Значения соответствуют константам из libpq-fe.h
 */
enum class POSTGRE_EXPORT postgre_conn_status : int
{
    OK = CONNECTION_OK,                                 ///< Подключение установлено успешно
    BAD = CONNECTION_BAD,                               ///< Подключение не установлено
    STARTED = CONNECTION_STARTED,                       ///< Подключение начато
    MADE = CONNECTION_MADE,                             ///< Подключение установлено
    AWAITING_RESPONSE = CONNECTION_AWAITING_RESPONSE,   ///< Ожидание ответа
    AUTH_OK = CONNECTION_AUTH_OK,                       ///< Авторизация успешна
    SETENV = CONNECTION_SETENV,                         ///< Установка переменных окружения
    SSL_STARTUP = CONNECTION_SSL_STARTUP,               ///< Инициализация SSL
    NEEDED = CONNECTION_NEEDED                          ///< Требуется подключение
};

/**
 * @brief Преобразует код статуса выполнения запроса PostgreSQL (ExecStatusType) в enum postgre_exec_status
 * @param status Целочисленный код статуса выполнения запроса
 * @return Соответствующий enum postgre_exec_status
 */
POSTGRE_EXPORT postgre_exec_status to_postgre_exec_status(ExecStatusType status);

/**
 * @brief Преобразует enum postgre_exec_status в целочисленный код статуса выполнения запроса PostgreSQL
 * @param status Enum код статуса
 * @return Целочисленный код статуса выполнения запроса PostgreSQL
 */
POSTGRE_EXPORT ExecStatusType to_exec_status_type(postgre_exec_status status);

/**
 * @brief Преобразует код статуса подключения PostgreSQL (PGconnStatusType) в enum postgre_conn_status
 * @param status Целочисленный код статуса подключения
 * @return Соответствующий enum postgre_conn_status
 */
POSTGRE_EXPORT postgre_conn_status to_postgre_conn_status(ConnStatusType status);

/**
 * @brief Преобразует enum postgre_conn_status в целочисленный код статуса подключения PostgreSQL
 * @param status Enum код статуса
 * @return Целочисленный код статуса подключения PostgreSQL
 */
POSTGRE_EXPORT ConnStatusType to_conn_status_type(postgre_conn_status status);

/**
 * @brief Проверяет, является ли код статуса выполнения запроса успешным результатом
 * @param status Код статуса для проверки
 * @return true, если код указывает на успех (COMMAND_OK, TUPLES_OK), иначе false
 */
POSTGRE_EXPORT bool is_success(postgre_exec_status status);

/**
 * @brief Проверяет, является ли код статуса выполнения запроса критической ошибкой
 * @param status Код статуса для проверки
 * @return true, если код указывает на критическую ошибку (FATAL_ERROR, BAD_RESPONSE), иначе false
 */
POSTGRE_EXPORT bool is_critical_error(postgre_exec_status status);

/**
 * @brief Проверяет, является ли код статуса подключения успешным
 * @param status Код статуса для проверки
 * @return true, если код указывает на успешное подключение (OK), иначе false
 */
POSTGRE_EXPORT bool is_connection_ok(postgre_conn_status status);

} // namespace postgre
} // namespace database_adapter

