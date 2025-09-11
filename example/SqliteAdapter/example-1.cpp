#include "SqliteAdapter/sqliteadapter.h"

#include <iostream>

int main()
{
    database_adapter::sqlite::settings settings;
    settings.url = R"(C:\Users\Alex\Documents\test.db)";

    auto driver = std::make_shared<database_adapter::sqlite::connection>(settings);

    try {
        auto res = driver->exec("select * from test_table where id = 2");
    } catch(database_adapter::sql_exception e) {
        std::cout << e.what();
    }

    try {
        driver->prepare("select * from test_table where id = ?", "test");
        auto res = driver->exec_prepared({ "2" }, "test");
    } catch(database_adapter::sql_exception e) {
        std::cout << e.what();
    }

    return 0;
}
