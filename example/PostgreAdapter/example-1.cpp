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

    int batch = 100;
    int repeate = 500;
    int pause = 50;

    long long time1 = 0;
    std::cout << "start prepare\n";
    for(int i = 0; i < repeate; i++) {
        try {
            auto driver = std::make_shared<database_adapter::postgre::database_adapter>(settings);
            driver->exec("delete from public.test_table");
        } catch(database_adapter::sql_exception e) {
            std::cout << e.what();
        }

        try {
            auto driver = std::make_shared<database_adapter::postgre::database_adapter>(settings);
            for(int i = 1; i <= batch; i++) {
                auto start = std::chrono::system_clock::now();
                driver->exec("insert into public.test_table values (" + std::to_string(i) + ", NULL)");
                auto end = std::chrono::system_clock::now();
                time1 += (end - start).count();
                // std::cout << "Exec not prepare " << (end - start).count() << "\n";
                // std::cout << "---------------------------------"
                // << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(pause));

        } catch(database_adapter::sql_exception e) {
            std::cout << e.what();
        }

        // std::cout << i << "\n";
    }
    std::cout << "pause\n";

    std::cout << "start not prepare\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    long long time = 0;
    for(int i = 0; i < repeate; i++) {
        try {
            auto driver = std::make_shared<database_adapter::postgre::database_adapter>(settings);
            driver->exec("delete from public.test_table");
        } catch(database_adapter::sql_exception e) {
            std::cout << e.what();
        }
        try {
            auto driver = std::make_shared<database_adapter::postgre::database_adapter>(settings);
            driver->prepare("insert into public.test_table values ($1, NULL)", "test");
            for(int i = 1; i <= batch; i++) {
                auto start = std::chrono::system_clock::now();
                driver->exec_prepared({ std::to_string(i) }, "test");
                auto end = std::chrono::system_clock::now();
                time += (end - start).count();
                // std::cout << "Exec prepare " << (end - start).count() << "\n";
                // std::cout << "---------------------------------"
                // << "\n";
            }
        } catch(database_adapter::sql_exception e) {
            std::cout << e.what();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(pause));

        // std::cout << i << "\n";
    }

    std::cout << "Exec prepare avg " << time / repeate << "\n";
    std::cout << "Exec not prepare avg " << time1 / repeate << "\n";

    return 0;
}
