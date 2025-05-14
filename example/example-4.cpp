#include "SqliteAdapter/sqlitedatabaseadapter.h"

#include <EntityCraft/entitycraft.h>

#include <ostream>
#include <string>

struct A
{
    int id = 0;
    std::string info;
};

struct ATableInfo
{
    static constexpr auto id = "id";
    static constexpr auto info = "info";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<A>("", "A",
            make_column(id, &A::id, query_craft::primary_key()),
            make_column(info, &A::info, query_craft::not_null()));
    }
};

struct AStorage
{
    using Storage = storage_type(ATableInfo::dto());

    explicit AStorage(const std::shared_ptr<database_adapter::IDataBaseDriver>& adapter)
        : _storage(make_storage(adapter, ATableInfo::dto()))
    {
        create_table();
    }

    auto storage() const
    {
        return _storage;
    }

    auto& operator()()
    {
        return _storage;
    }

private:
    void create_table() const
    {
        _storage.database()->exec("CREATE TABLE IF NOT exists A (id INTEGER NOT NULL, info varchar NOT NULL, CONSTRAINT A_PK PRIMARY KEY (id));");
    }

private:
    Storage _storage;
};

struct B
{
    int id;
    A a;
};

struct BTableInfo
{
    static constexpr auto id = "id";
    static constexpr auto a_id = "a_id";

    static auto dto()
    {
        using namespace entity_craft;

        return make_table<B>("", "B",
            make_column(id, &B::id, query_craft::primary_key()),
            make_reference_column(a_id, &B::a, ATableInfo::dto(), relation_type::many_to_one));
    }

    static query_craft::table table_info()
    {
        return dto().table_info();
    }
};

struct BStorage
{
    using Storage = storage_type(BTableInfo::dto());

    explicit BStorage(const std::shared_ptr<database_adapter::IDataBaseDriver>& adapter)
        : _storage(make_storage(adapter, BTableInfo::dto()))
    {
        create_table();
    }

    auto storage() const
    {
        return _storage;
    }

    auto& operator()()
    {
        return _storage;
    }

private:
    void create_table() const
    {
        _storage.database()->exec("CREATE TABLE IF NOT exists B (id INTEGER NOT NULL,a_id INTEGER, CONSTRAINT B_PK PRIMARY KEY (id),CONSTRAINT B_A_FK FOREIGN KEY (a_id) REFERENCES A(id));");
    }

private:
    Storage _storage;
};

int main()
{
    using namespace entity_craft;

    database_adapter::sqlite_settings settings;
    settings.url = R"(./db/example-4.db)";

    std::shared_ptr<database_adapter::IDataBaseDriver> adapter = std::make_shared<database_adapter::sqlite_database_adapter>(settings);

    auto a_storage = AStorage(adapter);
    auto b_storage = BStorage(adapter);

    a_storage().remove();
    b_storage().remove();

    A a;
    a.id = 1;
    a.info = "info - 1";

    a_storage().insert(a);

    A a1;
    a1.id = 2;
    a1.info = "info - 2";

    a_storage().insert(a1);

    B b;
    b.id = 1;

    b_storage().insert(b);

    a1.info += " update";

    B b1;
    b1.id = 2;
    b1.a = a1;

    b_storage().upsert(b1);

    for(const auto& data : b_storage().select()) {
        std::cout << "id - " << data.id << "\n";
        std::cout << "a_id - " << data.a.id << "\n";
        std::cout << "a_info - " << data.a.info << "\n";
        std::cout << "\n";
    }
}
