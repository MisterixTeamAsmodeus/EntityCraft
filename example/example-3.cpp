#include "SqliteAdapter/sqlitedatabaseadapter.h"

#include <EntityCraft/entitycraft.h>

#include <ostream>
#include <string>

struct A
{
    int id = 0;
    int b_id;
    std::string info;
};

struct ATableInfo
{
    static constexpr auto id = "id";
    static constexpr auto b_id = "b_id";
    static constexpr auto info = "info";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<A>("", "A",
            make_column(id, &A::id, query_craft::primary_key()),
            make_column(b_id, &A::b_id, query_craft::not_null()),
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
        _storage.database()->exec("CREATE TABLE IF NOT exists A (id INTEGER NOT NULL, b_id INTEGER NOT NULL, info varchar, CONSTRAINT A_PK PRIMARY KEY (id), CONSTRAINT A_B_FK FOREIGN KEY (b_id) REFERENCES B(id) ON DELETE CASCADE);");
    }

private:
    Storage _storage;
};

struct B
{
    int id;
    std::string t;
    std::list<A> a;
};

struct BTableInfo
{
    static constexpr auto id = "id";
    static constexpr auto t = "t";

    static auto dto()
    {
        using namespace entity_craft;

        return make_table<B>("", "B",
            make_column(id, &B::id, query_craft::primary_key()),
            make_column(t, &B::t),
            make_reference_column(ATableInfo::b_id, &B::a, ATableInfo::dto(), relation_type::one_to_many));
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
        _storage.database()->exec("CREATE TABLE IF NOT exists B (id INTEGER NOT NULL,a_id INTEGER, t varchar, CONSTRAINT B_PK PRIMARY KEY (id),CONSTRAINT B_A_FK FOREIGN KEY (a_id) REFERENCES A(id));");
    }

private:
    Storage _storage;
};

int main()
{
    using namespace entity_craft;

    database_adapter::sqlite_settings settings;
    settings.url = R"(./db/example-3.db)";

    std::shared_ptr<database_adapter::IDataBaseDriver> adapter = std::make_shared<database_adapter::sqlite_database_adapter>(settings);

    auto b_storage = BStorage(adapter);
    auto a_storage = AStorage(adapter);

    b_storage().remove();
    a_storage().remove();

    B b;
    b.id = 1;

    A a;
    a.id = 1;
    a.b_id = b.id;
    a.info = "test";

    b.a.emplace_back(a);

    b_storage().upsert(b);

    for(const auto& data : b_storage().select()) {
        std::cout << "b id = " << data.id << "\n";
        for(const auto& a : data.a) {
            std::cout << "  a id = " << a.id << "\n";
            std::cout << "  a b_id = " << a.b_id << "\n";
            std::cout << "  a info = " << a.info << "\n ";
        }
        std::cout << "\n";
    }
}
