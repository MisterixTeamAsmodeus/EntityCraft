#include "SqliteAdapter/sqlitedatabaseadapter.h"

#include <EntityCraft/entitycraft.h>

struct Test
{
    int id = 0;
    std::string name;

    std::string get_name() const { return name; }

    void set_name(const std::string& name) { this->name = name; }
};

namespace QueryCraft {
namespace Helper {

} // namespace Helper
} // namespace QueryCraft

void initDb(std::shared_ptr<DatabaseAdapter::IDataBaseDriver>& adapter)
{
    adapter->exec("CREATE TABLE IF NOT exists Persons ( PersonID int, PersonName varchar);");
}

int main()
{
    using namespace EntityCraft;

    auto dto = make_table<Test>("", "Persons",
        make_column("PersonID", &Test::id, primary_key()),
        make_column<Test, std::string>("PersonName", &Test::set_name, &Test::get_name));

    auto table_info = dto.table_info();

    DatabaseAdapter::SqliteSettings settings;
    settings.url = R"(C:\Project\src\test.db)";

    std::shared_ptr<DatabaseAdapter::IDataBaseDriver> adapter = std::make_shared<DatabaseAdapter::SqliteDatabaseAdapter>(settings);
    adapter->connect();
    // initDb(adapter);

    auto storage = make_storage(adapter, dto);

    Test t;
    t.id = 5;
    t.name = "test";
    storage.insert(t);

    auto value = storage.get_by_id(1);

    if(value == nullptr) {
        std::cout << "NOT FOUND";
    } else {
        std::cout << "id = " << value->id << std::endl;
        std::cout << "name = " << value->get_name() << std::endl;
    }

    return 0;
}
