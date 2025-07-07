#include <QueryCraft/querycraft.h>

#include <iostream>

/// Данный пример демонстрирует работу генерации запроса для удаления строк из таблицы

int main()
{
    using namespace query_craft;

    // Объявление информации о таблице
    const sql_table table("table_name", "schema_name",
        column_info("c1"),
        column_info("c2"),
        column_info("c3"));

    // Вывод сгенерированного запроса с указанием условия удаления
    std::cout << table.remove_sql(
        table.column("c1") > 5
        && table.column("c2") < 7)
              << "\n";
    // Вывод сгенерированного запроса для удаления всех записей
    std::cout << table.remove_sql() << "\n";

    return 0;
}
