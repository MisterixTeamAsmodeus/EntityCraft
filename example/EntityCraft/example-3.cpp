#include <EntityCraft/entitycraft.h>
#include <SqliteAdapter/sqliteadapter.h>

#include <ostream>
#include <string>

struct C
{
    int id = 0;
    int a_id;
    std::string info;
};

struct CTableInfo
{
    static constexpr auto id = "id";
    static constexpr auto a_id = "a_id";
    static constexpr auto info = "info";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<C>("", "C",
            make_column(id, &C::id, query_craft::primary_key()),
            make_column(a_id, &C::a_id, query_craft::not_null()),
            make_column(info, &C::info, query_craft::not_null()));
    }
};

struct CStorage
{
    using Storage = storage_type(CTableInfo::dto());

    explicit CStorage(const std::shared_ptr<database_adapter::IConnection>& adapter)
        : _storage(make_storage(adapter, CTableInfo::dto()))
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
        _storage.database()->exec("CREATE TABLE IF NOT exists C (id INTEGER NOT NULL, a_id INTEGER NOT NULL, info varchar, CONSTRAINT C_PK PRIMARY KEY (id), CONSTRAINT C_A_FK FOREIGN KEY (a_id) REFERENCES A(id) ON DELETE CASCADE);");
    }

private:
    Storage _storage;
};

struct A
{
    int id = 0;
    int b_id;
    std::string info;
    std::list<C> array;
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
            make_column(info, &A::info, query_craft::not_null()),
            make_reference_column(CTableInfo::a_id, &A::array, CTableInfo::dto(), relation_type::one_to_many));
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
        _storage.database()->exec("CREATE TABLE IF NOT exists B (id INTEGER NOT NULL, t varchar, CONSTRAINT B_PK PRIMARY KEY (id));");
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
    settings.url = "example-3.db";
    std::remove(settings.url.c_str());

    std::shared_ptr<database_adapter::IConnection> adapter = std::make_shared<database_adapter::sqlite::connection>(settings);

    auto b_storage = BStorage(adapter);
    auto a_storage = AStorage(adapter);
    auto c_storage = CStorage(adapter);

    B b;
    b.id = 1;

    A a;
    a.id = 1;
    a.b_id = b.id;
    a.info = "test";

    a.array.push_back({ 1, a.id, "123" });
    a.array.push_back({ 2, a.id, "321" });

    b.a.emplace_back(a);

    a.id = 2;
    a.b_id = b.id;
    a.info = "test2";
    a.array.clear();
    a.array.push_back({ 3, a.id, "test" });

    b.a.emplace_back(a);

    b_storage().insert(b);

    for(const auto& data : b_storage().select()) {
        std::cout << "b id = " << data.id << "\n";
        for(const auto& a : data.a) {
            std::cout << "  a id = " << a.id << "\n";
            std::cout << "  a b_id = " << a.b_id << "\n";
            std::cout << "  a info = " << a.info << "\n ";

            for(const auto& c : a.array) {
                std::cout << "      c id = " << c.id << "\n";
                std::cout << "      c a_id = " << c.a_id << "\n";
                std::cout << "      c info = " << c.info << "\n ";
            }
        }
        std::cout << "\n";
    }
}
