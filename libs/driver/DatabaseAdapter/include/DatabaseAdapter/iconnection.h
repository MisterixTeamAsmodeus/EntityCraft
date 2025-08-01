#pragma once

#include "model/queryresult.h"

#include <vector>

namespace database_adapter {
namespace models {
struct database_settings;
} // namespace models
} // namespace database_adapter

namespace database_adapter {

class IConnection
{
public:
    explicit IConnection(const models::database_settings& settings);

    virtual ~IConnection() = default;

    virtual bool is_valid() = 0;

    /**
     * @brief Выполняет SQL-запрос к базе данных.
     * Эта функция должна выполнить указанный SQL-запрос к базе данных
     * и вернуть результат в виде объекта QueryResult.
     * @param query SQL-запрос.
     * @return Результат выполнения SQL-запроса.
     * @throws Выбрасывает исключение sql_exception в случае ошибки выполнения запроса
     */
    virtual models::query_result exec(const std::string& query) = 0;

    /**
     * Выполняет подготовку запроса для возможности динамической подстановки параметров
     * @param query Запрос который необходимо подготовить
     * @param name Именование подготовленного запроса если поддерживается базой
     */
    virtual void prepare(const std::string& query, const std::string& name = "") = 0;

    /**
     * Выполнить подготовленный запрос с подставленными значениями
     * @return Результат выполнения SQL-запроса.
     * @throws Выбрасывает исключение sql_exception в случае ошибки выполнения запроса
     */
    virtual models::query_result exec_prepared(const std::vector<std::string>& params, const std::string& name = "") = 0;
};

} // namespace database_adapter