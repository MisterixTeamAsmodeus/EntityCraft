#include <SqliteAdapter/sqliteadapter.hpp>

#include <DatabaseAdapter/transaction_guard.hpp>

#include <iostream>
#include <chrono>
#include <vector>

/**
 * @brief Пример использования SqliteAdapter
 * 
 * Демонстрирует:
 * - Создание подключения к SQLite базе данных
 * - Выполнение простых SQL-запросов
 * - Использование prepared statements
 * - Работу с транзакциями
 * - Использование connection pool
 */
int main()
{
    try {
        // Настройки подключения к SQLite базе данных
        database_adapter::sqlite::settings settings;
        settings.url = "example_sqlite.db"; // Путь к файлу базы данных
        std::remove(settings.url.c_str());

        std::cout << "=== Пример использования SqliteAdapter ===\n\n";

        // 1. Создание подключения
        std::cout << "1. Создание подключения к базе данных...\n";
        auto connection = std::make_shared<database_adapter::sqlite::connection>(settings);

        // Проверка валидности подключения
        if(connection->is_valid()) {
            std::cout << "   Подключение успешно установлено\n";
        } else {
            std::cout << "   Ошибка: подключение невалидно\n";
            return 1;
        }

        // 2. Создание таблицы
        std::cout << "\n2. Создание таблицы test_table...\n";
        connection->exec("DROP TABLE IF EXISTS test_table");
        connection->exec("CREATE TABLE test_table (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, value INTEGER)");
        std::cout << "   Таблица создана\n";

        // 3. Выполнение простых запросов (INSERT)
        std::cout << "\n3. Вставка данных через обычные запросы...\n";
        for(int i = 1; i <= 5; ++i) {
            std::string query = "INSERT INTO test_table (name, value) VALUES ('item" + std::to_string(i) + "', " + std::to_string(i * 10) + ")";
            connection->exec(query);
            std::cout << "   Вставлена запись: item" << i << "\n";
        }

        // 4. Выполнение SELECT запроса
        std::cout << "\n4. Выборка данных...\n";
        auto result = connection->exec("SELECT id, name, value FROM test_table");
        std::cout << "   Найдено записей: " << result.size() << "\n";
        for(const auto& row : result) {
            std::cout << "   ID: " << row.at("id") 
                      << ", Name: " << row.at("name") 
                      << ", Value: " << row.at("value") << "\n";
        }

        // 5. Использование prepared statements
        std::cout << "\n5. Использование prepared statements...\n";
        connection->prepare("INSERT INTO test_table (name, value) VALUES (?, ?)", "insert_item");
        
        for(int i = 6; i <= 10; ++i) {
            auto prepared_result = connection->exec_prepared({ "item" + std::to_string(i), std::to_string(i * 10) }, "insert_item");
            std::cout << "   Вставлена запись через prepared statement: item" << i << "\n";
        }

        // 6. Обновление данных
        std::cout << "\n6. Обновление данных...\n";
        connection->exec("UPDATE test_table SET value = value * 2 WHERE id <= 3");
        result = connection->exec("SELECT id, name, value FROM test_table WHERE id <= 3");
        std::cout << "   Обновленные записи:\n";
        for(const auto& row : result) {
            std::cout << "   ID: " << row.at("id") 
                      << ", Name: " << row.at("name") 
                      << ", Value: " << row.at("value") << "\n";
        }

        // 7. Работа с транзакциями
        std::cout << "\n7. Работа с транзакциями...\n";
        {
            database_adapter::transaction_guard guard(connection);
            
            connection->exec("INSERT INTO test_table (name, value) VALUES ('transaction_item1', 100)");
            connection->exec("INSERT INTO test_table (name, value) VALUES ('transaction_item2', 200)");
            
            std::cout << "   Внутри транзакции: " << connection->exec("SELECT COUNT(*) as count FROM test_table").at(0).at("count") << " записей\n";
            
            guard.commit();
            std::cout << "   Транзакция закоммичена\n";
        }

        // Проверка после коммита
        result = connection->exec("SELECT COUNT(*) as count FROM test_table");
        std::cout << "   После коммита: " << result.at(0).at("count") << " записей\n";

        // 8. Откат транзакции
        std::cout << "\n8. Демонстрация отката транзакции...\n";
        size_t count_before = std::stoull(connection->exec("SELECT COUNT(*) as count FROM test_table").at(0).at("count"));
        std::cout << "   Записей до транзакции: " << count_before << "\n";
        
        {
            database_adapter::transaction_guard guard(connection);
            connection->exec("INSERT INTO test_table (name, value) VALUES ('rollback_item', 999)");
            std::cout << "   Внутри транзакции добавлена запись\n";
            guard.rollback();
            std::cout << "   Транзакция откачена\n";
        }

        size_t count_after = std::stoull(connection->exec("SELECT COUNT(*) as count FROM test_table").at(0).at("count"));
        std::cout << "   Записей после отката: " << count_after << "\n";

        // 9. Использование connection pool
        std::cout << "\n9. Использование connection pool...\n";
        database_adapter::sqlite::connection_pool::connection_settings = settings;
        database_adapter::sqlite::connection_pool::start_pool_size = 2;
        database_adapter::sqlite::connection_pool::max_pool_size = 5;
        database_adapter::sqlite::connection_pool::wait_time = std::chrono::seconds(2);
        database_adapter::sqlite::connection_pool::idle_timeout = std::chrono::seconds(300);

        auto pool = database_adapter::sqlite::connection_pool::instance();
        std::cout << "   Пул соединений создан\n";

        // Получение соединений из пула
        std::vector<std::shared_ptr<database_adapter::IConnection>> connections;
        for(int i = 0; i < 3; ++i) {
            auto conn = pool->open_connection();
            if(conn) {
                connections.push_back(conn);
                std::cout << "   Получено соединение " << (i + 1) << " из пула\n";
            }
        }

        // Использование соединений из пула
        for(size_t i = 0; i < connections.size(); ++i) {
            auto result = connections[i]->exec("SELECT COUNT(*) as count FROM test_table");
            std::cout << "   Соединение " << (i + 1) << " видит " << result.at(0).at("count") << " записей\n";
        }

        // Соединения автоматически вернутся в пул при уничтожении shared_ptr
        connections.clear();
        std::cout << "   Соединения возвращены в пул\n";

        // 10. Финальная статистика
        std::cout << "\n10. Финальная статистика...\n";
        result = connection->exec("SELECT COUNT(*) as count, SUM(value) as total FROM test_table");
        std::cout << "   Всего записей: " << result.at(0).at("count") << "\n";
        std::cout << "   Сумма значений: " << result.at(0).at("total") << "\n";

        std::cout << "\n=== Пример завершен успешно ===\n";

    } catch(const database_adapter::sql_exception& e) {
        std::cerr << "Ошибка SQL: " << e.what() << "\n";
        return 1;
    } catch(const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
