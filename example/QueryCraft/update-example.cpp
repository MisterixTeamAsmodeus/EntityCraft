#include <QueryCraft/querycraft.h>

#include <iostream>

/// Данный пример демонстрирует работу генерации запроса для обновления строк в таблицу

int main()
{
    using namespace query_craft;

    // Объявление информации о таблице
    sql_table table("table_name", "schema_name",
        column_info("c1"),
        column_info("c2"),
        column_info("c3"));

    // Добавление строк для обновления
    table.add_row_args(5, "124", true);
    // Вывод сгенерированного запроса для обновления строк соответсвтующим заданному условию
    std::cout << table.update_sql(table.column("c1") > 5 && table.column("c2") < 7) << "\n";

    // Добавление строк для обновления
    table.add_row_args(5, "874", true);
    // Вывод сгенерированного запроса для обновления строк
    std::cout << table.update_sql() << "\n";

    // Добавление строк для обновления c указанием колонок (пропущена колонка c2, она будет заполнена значением по умолчанию)
    table.add_row_args(5, true);
    // Вывод сгенерированного запроса с явным указанием колонок таблицы
    std::cout << table.update_args_sql({}, { table.column("c1"), table.column("c3") }) << "\n";
}
