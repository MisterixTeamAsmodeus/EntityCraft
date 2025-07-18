#include "PostgreAdapter/postgreadapter.h"

#include <iostream>

int main()
{
    database_adapter::postgre::settings settings;
    settings.database_name = "Test";
    settings.login = "postgres";
    settings.password = "postgres";
    settings.port = "5432";
    settings.url = "localhost";

    auto driver = std::make_shared<database_adapter::postgre::database_adapter>(settings);

    try {
        driver->prepare("select * from public.test_table where id = $1", "test");
        auto res = driver->exec_prepared({ "1" }, "test");
    } catch(database_adapter::sql_exception e) {
        std::cout << e.what();
    }

    return 0;
}
