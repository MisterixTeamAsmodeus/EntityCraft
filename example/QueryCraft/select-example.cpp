#include <QueryCraft/querycraft.h>

#include <iostream>

/// Данный пример демонстрирует работу генерации запроса для запроса строк из таблицы

int main()
{
    using namespace query_craft;

    // Объявление информации о таблице
    const sql_table table_info("table_name", "schema_name",
        column_info("c1"),
        column_info("c2"),
        column_info("c3"),
        column_info("c4"),
        column_info("c5"),
        column_info("c6"),
        column_info("c7"),
        column_info("c8"),
        column_info("c9"),
        column_info("c10"),
        column_info("c11"),
        column_info("c12"));

    // Список для проверки условия in (...)
    const auto list = { "first", "second", "third", "fourth", "fifth" };

    // Получение сгенерированного запроса для запроса строк
    const auto sql = table_info.select_args_sql(
        { { join_column::type::left, static_cast<table>(table_info), table_info.column("c1").is_null() } },
        (table_info.column("c1") > 5 && (table_info.column("c2") < 7) || table_info.column("c3") >= 6
            || table_info.column("c4") <= 6 || table_info.column("c5") == 6 || table_info.column("c6") != 6)
            || table_info.column("c7").is_null() || table_info.column("c8").not_null() || table_info.column("c9").in(5, 4, 7, 8)
            || table_info.column("c11").notIn(78, 57, 57, 4, 1) || (table_info.column("c10").in_list(list.begin(), list.end()) && table_info.column("c12").not_in_list(list.begin(), list.end())),
        { { table_info.column("c3"), sort_column::type::desc }, { table_info.column("c8") } },
        10,
        45);

    std::cout << sql << "\n";

    return 0;
}
