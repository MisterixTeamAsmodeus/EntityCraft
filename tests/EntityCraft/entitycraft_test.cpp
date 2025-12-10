#include <EntityCraft/entitycraft.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <DatabaseAdapter/databaseadapter.hpp>
#include <memory>
#include <optional>
#include <SqliteAdapter/sqliteadapter.hpp>
#include <string>
#include <vector>

/**
 * @brief Тестовая структура User для тестов EntityCraft
 */
struct User
{
    int id = 0;
    std::string name;
    std::string email;
    int age = 0;

    User() = default;

    User(std::string name, std::string email, int age)
        : name(std::move(name))
        , email(std::move(email))
        , age(age)
    {
    }
};

struct UserTable
{
    static auto dto()
    {
        return entity_craft::make_table<User>(
            "users",
            "",
            entity_craft::make_column("id", &User::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &User::name, entity_craft::not_null()),
            entity_craft::make_column("email", &User::email, entity_craft::not_null()),
            entity_craft::make_column("age", &User::age, entity_craft::not_null()));
    }
};

/**
 * @brief Фикстура для тестов EntityCraft
 */
class EntityCraft : public ::testing::Test
{
    using storage = storage_type(UserTable::dto());

protected:
    void SetUp() override
    {
        _settings.url = "test_entity_craft_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(std::rand()) + ".db";

        std::remove(_settings.url.c_str());
        _connection = std::make_shared<database_adapter::sqlite::connection>(_settings);

        // Создаем таблицу users
        _connection->exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "email TEXT NOT NULL,"
            "age INTEGER NOT NULL"
            ")");

        // Создаем storage для работы с таблицей
        _user_storage = std::make_unique<storage>(entity_craft::make_storage(_connection, UserTable::dto()));
    }

    void TearDown() override
    {
        // Сначала уничтожаем storage, так как его деструктор может обращаться к connection
        _user_storage.reset();
        _connection.reset();

        std::remove(_settings.url.c_str());
    }

protected:
    database_adapter::sqlite::settings _settings;
    std::shared_ptr<database_adapter::sqlite::connection> _connection;
    std::unique_ptr<storage> _user_storage;
};

/**
 * @brief Тест создания table
 */
TEST_F(EntityCraft, Table_Create)
{
    auto users_table = UserTable::dto();

    EXPECT_EQ(users_table.table_name(), "users");
    EXPECT_EQ(users_table.scheme(), "");
    EXPECT_EQ(users_table.property_count(), 4);
}

/**
 * @brief Тест работы table с колонками
 */
TEST_F(EntityCraft, Table_Columns)
{
    auto users_table = UserTable::dto();

    auto columns_name = users_table.columns_name();
    EXPECT_EQ(columns_name.size(), 4);
    EXPECT_TRUE(std::find(columns_name.begin(), columns_name.end(), "id") != columns_name.end());
    EXPECT_TRUE(std::find(columns_name.begin(), columns_name.end(), "name") != columns_name.end());
    EXPECT_TRUE(std::find(columns_name.begin(), columns_name.end(), "email") != columns_name.end());
    EXPECT_TRUE(std::find(columns_name.begin(), columns_name.end(), "age") != columns_name.end());
}

/**
 * @brief Тест column_alias в table
 */
TEST_F(EntityCraft, Table_ColumnAlias)
{
    auto users_table = UserTable::dto();

    EXPECT_EQ(users_table.column_alias("id"), "users_id");
    EXPECT_EQ(users_table.column_alias("name"), "users_name");
}

/**
 * @brief Тест table со схемой
 */
TEST_F(EntityCraft, Table_WithSchema)
{
    auto table_with_schema = entity_craft::make_table<User>(
        "users",
        "public",
        entity_craft::make_column("id", &User::id, entity_craft::primary_key_auto_increment()));

    EXPECT_EQ(table_with_schema.table_name(), "users");
    EXPECT_EQ(table_with_schema.scheme(), "public");
    EXPECT_EQ(table_with_schema.column_alias("id"), "public_users_id");
}

/**
 * @brief Тест вставки одной сущности
 */
TEST_F(EntityCraft, Storage_Insert)
{
    User new_user("Ivan Ivanov", "ivan@example.com", 25);
    User inserted_user = _user_storage->insert(new_user);

    EXPECT_GT(inserted_user.id, 0);
    EXPECT_EQ(inserted_user.name, "Ivan Ivanov");
    EXPECT_EQ(inserted_user.email, "ivan@example.com");
    EXPECT_EQ(inserted_user.age, 25);
}

/**
 * @brief Тест вставки нескольких сущностей (batch insert)
 */
TEST_F(EntityCraft, Storage_InsertBatch)
{
    std::vector<User> batch_users = {
        User("Peter Sidorov", "petr@example.com", 28),
        User("Anna Kozlova", "anna@example.com", 22),
        User("Sergey Smirnov", "sergey@example.com", 35)
    };

    auto inserted_batch = _user_storage->insert_batch(batch_users.begin(), batch_users.end());

    EXPECT_EQ(inserted_batch.size(), 3);
    EXPECT_GT(inserted_batch[0].id, 0);
    EXPECT_GT(inserted_batch[1].id, 0);
    EXPECT_GT(inserted_batch[2].id, 0);
    EXPECT_EQ(inserted_batch[0].name, "Peter Sidorov");
    EXPECT_EQ(inserted_batch[1].name, "Anna Kozlova");
    EXPECT_EQ(inserted_batch[2].name, "Sergey Smirnov");
}

/**
 * @brief Тест поиска сущности по ID
 */
TEST_F(EntityCraft, Storage_FindById)
{
    User new_user("Maria Petrova", "maria@example.com", 30);
    User inserted_user = _user_storage->insert(new_user);

    auto found_user = _user_storage->find_by_id(inserted_user.id);

    EXPECT_NE(found_user, nullptr);
    EXPECT_EQ(found_user->id, inserted_user.id);
    EXPECT_EQ(found_user->name, "Maria Petrova");
    EXPECT_EQ(found_user->email, "maria@example.com");
    EXPECT_EQ(found_user->age, 30);
}

/**
 * @brief Тест поиска несуществующей сущности по ID
 */
TEST_F(EntityCraft, Storage_FindById_NonExistent)
{
    auto found_user = _user_storage->find_by_id(99999);

    EXPECT_EQ(found_user, nullptr);
}

/**
 * @brief Тест получения всех сущностей
 */
TEST_F(EntityCraft, Storage_FindAll)
{
    _user_storage->insert(User("User1", "user1@example.com", 20));
    _user_storage->insert(User("User2", "user2@example.com", 25));
    _user_storage->insert(User("User3", "user3@example.com", 30));

    std::vector<User> all_users = _user_storage->find_all();

    EXPECT_GE(all_users.size(), 3);
}

/**
 * @brief Тест поиска сущностей по условию
 */
TEST_F(EntityCraft, Storage_FindWhere)
{
    _user_storage->insert(User("Young User", "young@example.com", 20));
    _user_storage->insert(User("Old User", "old@example.com", 50));

    using namespace query_craft::dsl;
    auto users = _user_storage->find_where(col("age") > param(25));

    EXPECT_GE(users.size(), 1);
    for(const auto& user : users) {
        EXPECT_GT(user.age, 25);
    }
}

/**
 * @brief Тест поиска одной сущности по условию
 */
TEST_F(EntityCraft, Storage_FindOne)
{
    User inserted = _user_storage->insert(User("Unique User", "unique@example.com", 35));

    using namespace query_craft::dsl;
    auto found = _user_storage->find_one(col("email") == param(std::string("unique@example.com")));

    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Unique User");
    EXPECT_EQ(found->email, "unique@example.com");
}

/**
 * @brief Тест поиска одной несуществующей сущности
 */
TEST_F(EntityCraft, Storage_FindOne_NonExistent)
{
    using namespace query_craft::dsl;
    auto found = _user_storage->find_one(col("email") == param(std::string("nonexistent@example.com")));

    EXPECT_EQ(found, nullptr);
}

/**
 * @brief Тест обновления сущности
 */
TEST_F(EntityCraft, Storage_Update)
{
    User inserted = _user_storage->insert(User("Original Name", "original@example.com", 25));

    inserted.age = 26;
    inserted.email = "updated@example.com";
    User updated = _user_storage->update(inserted);

    EXPECT_EQ(updated.id, inserted.id);
    EXPECT_EQ(updated.age, 26);
    EXPECT_EQ(updated.email, "updated@example.com");

    // Проверяем, что изменения сохранились в БД
    auto found = _user_storage->find_by_id(inserted.id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->age, 26);
    EXPECT_EQ(found->email, "updated@example.com");
}

/**
 * @brief Тест обновления сущностей по условию
 */
TEST_F(EntityCraft, Storage_UpdateWhere)
{
    _user_storage->insert(User("User1", "user1@example.com", 20));
    _user_storage->insert(User("User2", "user2@example.com", 30));

    using namespace query_craft::dsl;
    User update_template;
    update_template.email = "updated@example.com";

    auto updated = _user_storage->update_where(update_template, col("name") == param(std::string("User1")));

    EXPECT_EQ(updated.email, "updated@example.com");

    // Проверяем, что изменения сохранились
    auto found = _user_storage->find_one(col("name") == param(std::string("User1")));
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->email, "updated@example.com");
}

/**
 * @brief Тест удаления сущности
 */
TEST_F(EntityCraft, Storage_Remove)
{
    User inserted = _user_storage->insert(User("To Delete", "delete@example.com", 25));

    bool deleted = _user_storage->remove(inserted);

    EXPECT_TRUE(deleted);

    // Проверяем, что сущность удалена
    auto found = _user_storage->find_by_id(inserted.id);
    EXPECT_EQ(found, nullptr);
}

/**
 * @brief Тест удаления сущности по ID
 */
TEST_F(EntityCraft, Storage_RemoveById)
{
    User inserted = _user_storage->insert(User("To Delete", "delete@example.com", 25));

    bool deleted = _user_storage->remove_by_id(inserted.id);

    EXPECT_TRUE(deleted);

    // Проверяем, что сущность удалена
    auto found = _user_storage->find_by_id(inserted.id);
    EXPECT_EQ(found, nullptr);
}

/**
 * @brief Тест удаления несуществующей сущности по ID
 */
TEST_F(EntityCraft, Storage_RemoveById_NonExistent)
{
    bool deleted = _user_storage->remove_by_id(99999);

    EXPECT_FALSE(deleted);
}

/**
 * @brief Тест удаления сущностей по условию
 */
TEST_F(EntityCraft, Storage_RemoveWhere)
{
    _user_storage->insert(User("Young1", "young1@example.com", 20));
    _user_storage->insert(User("Young2", "young2@example.com", 21));
    _user_storage->insert(User("Old1", "old1@example.com", 40));

    using namespace query_craft::dsl;
    bool deleted = _user_storage->remove_where(col("age") < param(25));

    EXPECT_TRUE(deleted);

    // Проверяем, что молодые пользователи удалены
    auto young_users = _user_storage->find_where(col("age") < param(25));
    EXPECT_EQ(young_users.size(), 0);

    // Проверяем, что старые пользователи остались
    auto old_users = _user_storage->find_where(col("age") >= param(25));
    EXPECT_GE(old_users.size(), 1);
}

/**
 * @brief Тест маппинга строки результата в сущность
 */
TEST_F(EntityCraft, Mapper_MapRowToEntity)
{
    User inserted = _user_storage->insert(User("Mapper Test", "mapper@example.com", 30));

    auto found = _user_storage->find_by_id(inserted.id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Mapper Test");
    EXPECT_EQ(found->email, "mapper@example.com");
    EXPECT_EQ(found->age, 30);
}

/**
 * @brief Тест маппинга результата запроса в вектор сущностей
 */
TEST_F(EntityCraft, Mapper_MapResultToEntities)
{
    _user_storage->insert(User("User1", "user1@example.com", 20));
    _user_storage->insert(User("User2", "user2@example.com", 25));
    _user_storage->insert(User("User3", "user3@example.com", 30));

    std::vector<User> all_users = _user_storage->find_all();

    EXPECT_GE(all_users.size(), 3);
    for(const auto& user : all_users) {
        EXPECT_GT(user.id, 0);
        EXPECT_FALSE(user.name.empty());
        EXPECT_FALSE(user.email.empty());
        EXPECT_GT(user.age, 0);
    }
}

/**
 * @brief Тест полного цикла CRUD операций
 */
TEST_F(EntityCraft, Storage_FullCrudCycle)
{
    // CREATE
    User new_user("Full Cycle", "fullcycle@example.com", 28);
    User inserted = _user_storage->insert(new_user);
    EXPECT_GT(inserted.id, 0);

    // READ
    auto found = _user_storage->find_by_id(inserted.id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Full Cycle");

    // UPDATE
    inserted.name = "Updated Cycle";
    User updated = _user_storage->update(inserted);
    EXPECT_EQ(updated.name, "Updated Cycle");

    // Проверяем обновление
    found = _user_storage->find_by_id(inserted.id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Updated Cycle");

    // DELETE
    bool deleted = _user_storage->remove_by_id(inserted.id);
    EXPECT_TRUE(deleted);

    // Проверяем удаление
    found = _user_storage->find_by_id(inserted.id);
    EXPECT_EQ(found, nullptr);
}

/**
 * @brief Тест работы с пустой таблицей
 */
TEST_F(EntityCraft, Storage_EmptyTable)
{
    std::vector<User> all_users = _user_storage->find_all();
    EXPECT_EQ(all_users.size(), 0);

    auto found = _user_storage->find_by_id(1);
    EXPECT_EQ(found, nullptr);

    using namespace query_craft::dsl;
    auto found_where = _user_storage->find_where(col("age") > param(0));
    EXPECT_EQ(found_where.size(), 0);
}

/**
 * @brief Тест работы с несколькими условиями в WHERE
 */
TEST_F(EntityCraft, Storage_FindWhere_MultipleConditions)
{
    _user_storage->insert(User("User1", "user1@example.com", 25));
    _user_storage->insert(User("User2", "user2@example.com", 30));
    _user_storage->insert(User("User3", "user3@example.com", 25));

    using namespace query_craft::dsl;
    auto users = _user_storage->find_where((col("age") == param(25)) && (col("name") == param(std::string("User1"))));

    EXPECT_GE(users.size(), 1);
    if(!users.empty()) {
        EXPECT_EQ(users[0].age, 25);
        EXPECT_EQ(users[0].name, "User1");
    }
}

/**
 * @brief Тест вставки пустого batch (граничный случай)
 */
TEST_F(EntityCraft, Storage_InsertBatch_Empty)
{
    std::vector<User> empty_batch;
    auto inserted_batch = _user_storage->insert_batch(empty_batch.begin(), empty_batch.end());

    EXPECT_EQ(inserted_batch.size(), 0);
}

/**
 * @brief Тест поиска сущности по ID с блокировкой строк (FOR UPDATE)
 */
TEST_F(EntityCraft, Storage_FindForUpdateById)
{
    User new_user("Locked User", "locked@example.com", 30);
    User inserted_user = _user_storage->insert(new_user);

    auto found_user = _user_storage->find_for_update_by_id(inserted_user.id);

    EXPECT_NE(found_user, nullptr);
    EXPECT_EQ(found_user->id, inserted_user.id);
    EXPECT_EQ(found_user->name, "Locked User");
    EXPECT_EQ(found_user->email, "locked@example.com");
    EXPECT_EQ(found_user->age, 30);
}

/**
 * @brief Тест поиска несуществующей сущности по ID с блокировкой строк
 */
TEST_F(EntityCraft, Storage_FindForUpdateById_NonExistent)
{
    auto found_user = _user_storage->find_for_update_by_id(99999);

    EXPECT_EQ(found_user, nullptr);
}

/**
 * @brief Тест поиска одной сущности по условию с блокировкой строк (FOR UPDATE)
 */
TEST_F(EntityCraft, Storage_FindForUpdateOne)
{
    User inserted = _user_storage->insert(User("Locked One", "lockedone@example.com", 35));

    using namespace query_craft::dsl;
    auto found = _user_storage->find_for_update_one(col("email") == param(std::string("lockedone@example.com")));

    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->id, inserted.id);
    EXPECT_EQ(found->name, "Locked One");
    EXPECT_EQ(found->email, "lockedone@example.com");
}

/**
 * @brief Тест поиска одной несуществующей сущности с блокировкой строк
 */
TEST_F(EntityCraft, Storage_FindForUpdateOne_NonExistent)
{
    using namespace query_craft::dsl;
    auto found = _user_storage->find_for_update_one(col("email") == param(std::string("nonexistent@example.com")));

    EXPECT_EQ(found, nullptr);
}

/**
 * @brief Тест поиска сущностей по условию с блокировкой строк (FOR UPDATE)
 */
TEST_F(EntityCraft, Storage_FindForUpdateWhere)
{
    _user_storage->insert(User("Locked User1", "locked1@example.com", 25));
    _user_storage->insert(User("Locked User2", "locked2@example.com", 30));
    _user_storage->insert(User("Normal User", "normal@example.com", 20));

    using namespace query_craft::dsl;
    auto users = _user_storage->find_for_update_where(col("age") >= param(25));

    EXPECT_GE(users.size(), 2);
    for(const auto& user : users) {
        EXPECT_GE(user.age, 25);
    }
}

/**
 * @brief Тест поиска сущностей с блокировкой строк по пустому условию (все записи)
 */
TEST_F(EntityCraft, Storage_FindForUpdateWhere_All)
{
    _user_storage->insert(User("User1", "user1@example.com", 20));
    _user_storage->insert(User("User2", "user2@example.com", 25));

    using namespace query_craft::dsl;
    auto users = _user_storage->find_for_update_where({});

    EXPECT_GE(users.size(), 2);
}

/**
 * @brief Тест пакетного обновления сущностей
 */
TEST_F(EntityCraft, Storage_UpdateBatch)
{
    User user1 = _user_storage->insert(User("Batch1", "batch1@example.com", 20));
    User user2 = _user_storage->insert(User("Batch2", "batch2@example.com", 25));
    User user3 = _user_storage->insert(User("Batch3", "batch3@example.com", 30));

    user1.age = 21;
    user1.email = "updated1@example.com";
    user2.age = 26;
    user2.email = "updated2@example.com";
    user3.age = 31;
    user3.email = "updated3@example.com";

    std::vector<User> batch_users = { user1, user2, user3 };
    auto updated_batch = _user_storage->update_batch(batch_users.begin(), batch_users.end());

    EXPECT_EQ(updated_batch.size(), 3);
    EXPECT_EQ(updated_batch[0].age, 21);
    EXPECT_EQ(updated_batch[0].email, "updated1@example.com");
    EXPECT_EQ(updated_batch[1].age, 26);
    EXPECT_EQ(updated_batch[1].email, "updated2@example.com");
    EXPECT_EQ(updated_batch[2].age, 31);
    EXPECT_EQ(updated_batch[2].email, "updated3@example.com");

    // Проверяем, что изменения сохранились в БД
    auto found1 = _user_storage->find_by_id(user1.id);
    EXPECT_NE(found1, nullptr);
    EXPECT_EQ(found1->age, 21);
    EXPECT_EQ(found1->email, "updated1@example.com");
}

/**
 * @brief Тест пакетного обновления пустого batch (граничный случай)
 */
TEST_F(EntityCraft, Storage_UpdateBatch_Empty)
{
    std::vector<User> empty_batch;
    auto updated_batch = _user_storage->update_batch(empty_batch.begin(), empty_batch.end());

    EXPECT_EQ(updated_batch.size(), 0);
}

/**
 * @brief Тест пакетного удаления сущностей
 */
TEST_F(EntityCraft, Storage_RemoveBatch)
{
    User user1 = _user_storage->insert(User("Delete1", "delete1@example.com", 20));
    User user2 = _user_storage->insert(User("Delete2", "delete2@example.com", 25));
    User user3 = _user_storage->insert(User("Delete3", "delete3@example.com", 30));

    std::vector<User> batch_users = { user1, user2, user3 };
    bool deleted = _user_storage->remove_batch(batch_users.begin(), batch_users.end());

    EXPECT_TRUE(deleted);

    // Проверяем, что все сущности удалены
    auto found1 = _user_storage->find_by_id(user1.id);
    auto found2 = _user_storage->find_by_id(user2.id);
    auto found3 = _user_storage->find_by_id(user3.id);

    EXPECT_EQ(found1, nullptr);
    EXPECT_EQ(found2, nullptr);
    EXPECT_EQ(found3, nullptr);
}

/**
 * @brief Тест пакетного удаления пустого batch (граничный случай)
 */
TEST_F(EntityCraft, Storage_RemoveBatch_Empty)
{
    std::vector<User> empty_batch;
    bool deleted = _user_storage->remove_batch(empty_batch.begin(), empty_batch.end());

    EXPECT_FALSE(deleted);
}

/**
 * @brief Тест проверки существования сущности по ID (существует)
 */
TEST_F(EntityCraft, Storage_Contains_Exists)
{
    User new_user("Contains Test", "contains@example.com", 30);
    User inserted_user = _user_storage->insert(new_user);

    bool exists = _user_storage->contains(inserted_user.id);

    EXPECT_TRUE(exists);
}

/**
 * @brief Тест проверки существования сущности по ID (не существует)
 */
TEST_F(EntityCraft, Storage_Contains_NotExists)
{
    bool exists = _user_storage->contains(99999);

    EXPECT_FALSE(exists);
}

/**
 * @brief Тест upsert - вставка новой сущности (когда сущность не существует)
 */
TEST_F(EntityCraft, Storage_Upsert_Insert)
{
    User new_user("Upsert Insert", "upsertinsert@example.com", 30);
    new_user.id = 0; // Убеждаемся, что ID пустой

    User result = _user_storage->upsert(new_user);

    EXPECT_GT(result.id, 0);
    EXPECT_EQ(result.name, "Upsert Insert");
    EXPECT_EQ(result.email, "upsertinsert@example.com");
    EXPECT_EQ(result.age, 30);

    // Проверяем, что сущность действительно вставлена
    auto found = _user_storage->find_by_id(result.id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Upsert Insert");
}

/**
 * @brief Тест upsert - обновление существующей сущности (когда сущность существует)
 */
TEST_F(EntityCraft, Storage_Upsert_Update)
{
    User inserted = _user_storage->insert(User("Original Name", "original@example.com", 25));

    User updated_user = inserted;
    updated_user.name = "Updated Name";
    updated_user.email = "updated@example.com";
    updated_user.age = 26;

    User result = _user_storage->upsert(updated_user);

    EXPECT_EQ(result.id, inserted.id);
    EXPECT_EQ(result.name, "Updated Name");
    EXPECT_EQ(result.email, "updated@example.com");
    EXPECT_EQ(result.age, 26);

    // Проверяем, что сущность действительно обновлена
    auto found = _user_storage->find_by_id(inserted.id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Updated Name");
    EXPECT_EQ(found->email, "updated@example.com");
    EXPECT_EQ(found->age, 26);
}

/**
 * @brief Тест upsert - вставка с пустым ID (всегда вставляет)
 */
TEST_F(EntityCraft, Storage_Upsert_InsertWithEmptyId)
{
    User new_user1("Upsert Empty1", "upsertempty1@example.com", 30);
    new_user1.id = 0;
    User result1 = _user_storage->upsert(new_user1);

    User new_user2("Upsert Empty2", "upsertempty2@example.com", 31);
    new_user2.id = 0;
    User result2 = _user_storage->upsert(new_user2);

    EXPECT_GT(result1.id, 0);
    EXPECT_GT(result2.id, 0);
    EXPECT_NE(result1.id, result2.id); // Должны быть разные ID
}

/**
 * @brief Тест upsert - обновление с тем же ID
 */
TEST_F(EntityCraft, Storage_Upsert_UpdateSameId)
{
    User inserted = _user_storage->insert(User("Before Update", "before@example.com", 25));
    int original_id = inserted.id;

    User updated = inserted;
    updated.name = "After Update";
    updated.email = "after@example.com";
    updated.age = 30;

    User result = _user_storage->upsert(updated);

    EXPECT_EQ(result.id, original_id);
    EXPECT_EQ(result.name, "After Update");
    EXPECT_EQ(result.email, "after@example.com");
    EXPECT_EQ(result.age, 30);
}

/**
 * @brief Тест взаимодействия find_for_update и update в транзакции
 */
TEST_F(EntityCraft, Storage_FindForUpdate_TransactionInteraction)
{
    User inserted = _user_storage->insert(User("Transaction Test", "transaction@example.com", 25));

    // Начинаем транзакцию вручную
    _connection->begin_transaction();

    // Находим с блокировкой
    auto locked = _user_storage->find_for_update_by_id(inserted.id);
    EXPECT_NE(locked, nullptr);

    // Обновляем
    locked->age = 30;
    User updated = _user_storage->update(*locked);
    EXPECT_EQ(updated.age, 30);

    // Коммитим транзакцию
    _connection->commit();

    // Проверяем результат
    auto found = _user_storage->find_by_id(inserted.id);
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->age, 30);
}

/**
 * @brief Тест пакетного обновления с частичными изменениями
 */
TEST_F(EntityCraft, Storage_UpdateBatch_PartialChanges)
{
    User user1 = _user_storage->insert(User("Partial1", "partial1@example.com", 20));
    User user2 = _user_storage->insert(User("Partial2", "partial2@example.com", 25));

    // Обновляем только часть полей
    user1.age = 21;
    user2.email = "updated2@example.com";

    std::vector<User> batch_users = { user1, user2 };
    auto updated_batch = _user_storage->update_batch(batch_users.begin(), batch_users.end());

    EXPECT_EQ(updated_batch.size(), 2);

    // Проверяем, что изменения сохранились
    auto found1 = _user_storage->find_by_id(user1.id);
    EXPECT_NE(found1, nullptr);
    EXPECT_EQ(found1->age, 21);
    EXPECT_EQ(found1->name, "Partial1"); // Имя не должно измениться

    auto found2 = _user_storage->find_by_id(user2.id);
    EXPECT_NE(found2, nullptr);
    EXPECT_EQ(found2->email, "updated2@example.com");
    EXPECT_EQ(found2->age, 25); // Возраст не должен измениться
}

/**
 * @brief Тест пакетного удаления части сущностей
 */
TEST_F(EntityCraft, Storage_RemoveBatch_Partial)
{
    User user1 = _user_storage->insert(User("Keep1", "keep1@example.com", 20));
    User user2 = _user_storage->insert(User("Delete1", "delete1@example.com", 25));
    User user3 = _user_storage->insert(User("Delete2", "delete2@example.com", 30));

    // Удаляем только user2 и user3
    std::vector<User> batch_users = { user2, user3 };
    bool deleted = _user_storage->remove_batch(batch_users.begin(), batch_users.end());

    EXPECT_TRUE(deleted);

    // Проверяем, что user1 остался, а user2 и user3 удалены
    auto found1 = _user_storage->find_by_id(user1.id);
    auto found2 = _user_storage->find_by_id(user2.id);
    auto found3 = _user_storage->find_by_id(user3.id);

    EXPECT_NE(found1, nullptr);
    EXPECT_EQ(found2, nullptr);
    EXPECT_EQ(found3, nullptr);
}