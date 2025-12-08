/**
 * @file crud_example.cpp
 * @brief Example of using CRUD API EntityCraft with SQLite database
 * 
 * This example demonstrates:
 * - CREATE (INSERT) - inserting single and multiple entities
 * - READ (SELECT) - retrieving all records, searching by ID, searching with conditions
 * - UPDATE - updating by ID and with conditions
 * - DELETE - deleting by ID and with conditions
 */

#include <EntityCraft/entitycraft.h>
#include <SqliteAdapter/sqliteadapter.hpp>

#include <DatabaseAdapter/databaseadapter.hpp>
#include <iostream>
#include <utility>
#include <vector>

/**
 * @brief Simple logger for outputting SQL queries to console
 * 
 * Implements ILogger interface for logging SQL queries and errors
 * to console with formatting for readability
 */
class ConsoleLogger : public database_adapter::ILogger
{
public:
    /**
     * @brief Logs SQL query to console
     * @param message SQL query or query information
     */
    void log_sql(const std::string& message) override
    {
        std::cout << "\n[SQL] " << message << "\n";
    }

    /**
     * @brief Logs error to console
     * @param message Error message
     */
    void log_error(const std::string& message) override
    {
        std::cerr << "\n[ERROR] " << message << "\n";
    }
};

/**
 * @brief User structure for example
 * 
 * To work with EntityCraft, the structure must have:
 * - Default constructor
 * - Public fields or access methods (getter/setter)
 */
struct User
{
    int id;                 ///< User identifier (primary key, auto_increment)
    std::string name;       ///< User name
    std::string email;      ///< User email
    int age;                ///< User age

    User() = default;

    User(int id, std::string  name, std::string  email, int age)
        : id(id)
        , name(std::move(name))
        , email(std::move(email))
        , age(age)
    {
    }
};

int main()
{
    try {
        // Setup SQL query logging
        auto logger = std::make_shared<ConsoleLogger>();
        database_adapter::sqlite::connection::set_logger(std::move(logger));

        // SQLite connection settings
        database_adapter::sqlite::settings settings;
        settings.database_name = "example.db";
        settings.url = "example.db"; // Path to database file

        std::remove(settings.url.c_str());
        // Create connection
        auto connection = std::make_shared<database_adapter::sqlite::connection>(settings);

        // Create table (if not exists)
        connection->exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "email TEXT NOT NULL,"
            "age INTEGER NOT NULL"
            ")");

        // Define table through EntityCraft
        auto users_table = entity_craft::make_table<User>(
            "users",                    // table name
            "",                         // schema (empty for SQLite)
            entity_craft::make_column("id", &User::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &User::name, entity_craft::not_null()),
            entity_craft::make_column("email", &User::email, entity_craft::not_null()),
            entity_craft::make_column("age", &User::age, entity_craft::not_null()));

        // Create storage for working with table
        auto user_storage = entity_craft::make_storage(connection, users_table);

        std::cout << "=== CRUD API example for SQLite ===\n\n";

        // ============================================
        // CREATE (INSERT)
        // ============================================
        std::cout << "1. CREATE (INSERT):\n";
        std::cout << "-----------------------------------\n";

        // Insert single entity
        User new_user1(0, "Ivan Ivanov", "ivan@example.com", 25);
        User inserted_user1 = user_storage.insert(new_user1);
        std::cout << "A user has been inserted: ID=" << inserted_user1.id
                  << ", Name=" << inserted_user1.name
                  << ", Email=" << inserted_user1.email
                  << ", Age=" << inserted_user1.age << "\n";

        User new_user2(0, "Maria Petrova", "maria@example.com", 30);
        User inserted_user2 = user_storage.insert(new_user2);
        std::cout << "A user has been inserted: ID=" << inserted_user2.id
                  << ", Name=" << inserted_user2.name << "\n";

        // Batch insert
        std::vector<User> batch_users = {
            User(0, "Peter Sidorov", "petr@example.com", 28),
            User(0, "Anna Kozlova", "anna@example.com", 22),
            User(0, "Sergey Smirnov", "sergey@example.com", 35)
        };
        std::vector<User> inserted_batch = user_storage.insert_batch(batch_users);
        std::cout << "Batch inserted users: " << inserted_batch.size() << "\n";
        for(const auto& user : inserted_batch) {
            std::cout << "  - ID=" << user.id << ", Name=" << user.name << "\n";
        }

        std::cout << "\n";

        // ============================================
        // READ (SELECT)
        // ============================================
        std::cout << "2. READ (SELECT) operations:\n";
        std::cout << "-----------------------------------\n";

        // Get all records
        std::vector<User> all_users = user_storage.find_all();
        std::cout << "Total users in database: " << all_users.size() << "\n";
        for(const auto& user : all_users) {
            std::cout << "  ID=" << user.id << ", Name=" << user.name
                      << ", Email=" << user.email << ", Age=" << user.age << "\n";
        }

        // Search by ID
        using namespace query_craft::dsl;
        auto user_by_id = user_storage.find_by_id(inserted_user1.id);
        if(user_by_id != nullptr) {
            std::cout << "\nUser with ID " << inserted_user1.id << ": "
                      << user_by_id->name << " (" << user_by_id->email << ")\n";
        }

        // Search with WHERE condition
        auto users_by_age = user_storage.find_where(col("age") > param(25));
        std::cout << "\nUsers older than 25: " << users_by_age.size() << "\n";
        for(const auto& user : users_by_age) {
            std::cout << "  - " << user.name << " (age: " << user.age << ")\n";
        }

        // Find single record
        auto one_user = user_storage.find_one(col("email") == param(std::string("maria@example.com")));
        if(one_user != nullptr) {
            std::cout << "\nUser with email maria@example.com: "
                      << one_user->name << "\n";
        }

        std::cout << "\n";

        // ============================================
        // UPDATE
        // ============================================
        std::cout << "3. UPDATE operations:\n";
        std::cout << "-----------------------------------\n";

        // Update by ID
        User user_to_update = inserted_user1;
        user_to_update.age = 26;
        user_to_update.email = "ivan.new@example.com";
        User updated_user = user_storage.update(user_to_update);
        std::cout << "User updated: ID=" << updated_user.id
                  << ", New age=" << updated_user.age
                  << ", New email=" << updated_user.email << "\n";

        // Update with WHERE condition
        User update_template(0, "", "updated@example.com", 0);
        User updated_where = user_storage.update_where(
            update_template,
            col("name") == param(std::string("Peter Sidorov")));
        std::cout << "Email updated for Peter Sidorov: " << updated_where.email << "\n";

        std::cout << "\n";

        // ============================================
        // DELETE
        // ============================================
        std::cout << "4. DELETE operations:\n";
        std::cout << "-----------------------------------\n";

        // Delete by ID
        bool deleted = user_storage.remove_by_id(inserted_batch[0].id);
        std::cout << "User with ID=" << inserted_batch[0].id
                  << " deleted: " << (deleted ? "success" : "failed") << "\n";

        // Delete with condition
        bool deleted_where = user_storage.remove_where(col("age") < param(23));
        std::cout << "Users younger than 23 deleted: " << (deleted_where ? "success" : "failed") << "\n";

        // Check remaining users
        std::vector<User> remaining_users = user_storage.find_all();
        std::cout << "\nRemaining users: " << remaining_users.size() << "\n";
        for(const auto& user : remaining_users) {
            std::cout << "  - ID=" << user.id << ", Name=" << user.name << ", Age=" << user.age << "\n";
        }

        std::cout << "\n=== All operations completed successfully! ===\n";

    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

