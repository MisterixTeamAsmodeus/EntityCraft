#include <EntityCraft/entitycraft.h>
#include <SqliteAdapter/sqliteadapter.hpp>

#include <ostream>
#include <string>

struct A
{
    int id = 0;
    std::string info;
    int b_id;
};

struct ATableInfo
{
    static constexpr auto id = "id";
    static constexpr auto info = "info";
    static constexpr auto b_id = "b_id";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<A>("", "A",
            make_column(id, &A::id, query_craft::primary_key()),
            make_column(info, &A::info, query_craft::not_null()),
            make_column(b_id, &A::b_id));
    }
};

struct AStorage
{
    using Storage = storage_type(ATableInfo::dto());

    explicit AStorage(const std::shared_ptr<database_adapter::IConnection>& adapter)
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
        _storage.database()->exec("CREATE TABLE IF NOT exists A (id INTEGER NOT NULL, info varchar NOT NULL, b_id INTEGER, CONSTRAINT A_PK PRIMARY KEY (id), CONSTRAINT A_B_FK FOREIGN KEY (b_id) REFERENCES B(id));");
    }

private:
    Storage _storage;
};

struct B
{
    int id;
    std::string text;
    A a;
};

struct BTableInfo
{
    static constexpr auto id = "id";
    static constexpr auto text = "text";

    static auto dto()
    {
        using namespace entity_craft;

        return make_table<B>("", "B",
            make_column(id, &B::id, query_craft::primary_key()),
            make_column(text, &B::text),
            make_reference_column(ATableInfo::b_id, &B::a, ATableInfo::dto(), relation_type::one_to_one_inverted));
    }

    static query_craft::table table_info()
    {
        return dto().table_info();
    }
};


struct BStorage
{
    using Storage = storage_type(BTableInfo::dto());

    explicit BStorage(const std::shared_ptr<database_adapter::IConnection>& adapter)
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
        _storage.database()->exec("CREATE TABLE IF NOT exists B (id INTEGER NOT NULL, text VARCHAR, CONSTRAINT B_PK PRIMARY KEY (id));");
    }

private:
    Storage _storage;
};

class Logger final : public database_adapter::ILogger
{
public:
    ~Logger() override = default;

    void log_error(const std::string& message) override
    {
        std::cout << "LOG_ERROR : " << message << "\n";
    }

    void log_sql(const std::string& message) override
    {
        std::cout << "LOG_DEBUG : " << message << "\n";
    }
};

int main()
{
    using namespace entity_craft;

    database_adapter::sqlite::connection::set_logger(std::make_shared<Logger>());

    database_adapter::sqlite::settings settings;
    settings.url = "example-5.db";
    std::remove(settings.url.c_str());

    std::shared_ptr<database_adapter::IConnection> adapter = std::make_shared<database_adapter::sqlite::connection>(settings);

    auto a_storage = AStorage(adapter);
    auto b_storage = BStorage(adapter);

    B b;
    b.id = 1;
    b.text = "text";
    b.a.id = 1;
    b.a.info = "test";
    b.a.b_id = b.id;

    b_storage().insert(b);

    for(const auto& data : b_storage().select()) {
        std::cout << "id - " << data.id << "\n";
        std::cout << "text - " << data.text << "\n";
        std::cout << "a_id - " << data.a.id << "\n";
        std::cout << "a_info - " << data.a.info << "\n";
        std::cout << "a_b_id - " << data.a.b_id << "\n";
        std::cout << "\n";
    }

    b.text = "text_update";
    b.a.info = "test_update";

    b_storage().update(b);

    for(const auto& data : b_storage().select()) {
        std::cout << "id - " << data.id << "\n";
        std::cout << "text - " << data.text << "\n";
        std::cout << "a_id - " << data.a.id << "\n";
        std::cout << "a_info - " << data.a.info << "\n";
        std::cout << "a_b_id - " << data.a.b_id << "\n";
        std::cout << "\n";
    }
}
