#include "SqliteAdapter/sqlitedatabaseadapter.h"

#include <EntityCraft/entitycraft.h>

struct WorkerInfo
{
    int id;
    std::string name;
    bool isActiveWorker;

    std::string get_name() const { return name; }

    void set_name(const std::string& name) { this->name = name; }
};

int main()
{
    using namespace EntityCraft;

    auto dto = make_table<WorkerInfo>("", "WorkerInfo",
        make_column(create_column_name(&WorkerInfo::id), QueryCraft::primary_key()),
        make_column("name", &WorkerInfo::set_name, &WorkerInfo::get_name),
        make_column(create_column_name(&WorkerInfo::isActiveWorker)));

    auto table_info = dto.table_info();

    DatabaseAdapter::SqliteSettings settings;
    settings.url = R"(./db/example-1.db)";

    std::shared_ptr<DatabaseAdapter::IDataBaseDriver> adapter = std::make_shared<DatabaseAdapter::SqliteDatabaseAdapter>(settings);
    adapter->connect();

    adapter->exec("CREATE TABLE IF NOT exists WorkerInfo ( id int, name varchar, isActiveWorker bool);");

    auto storage = make_storage(adapter, dto);

    WorkerInfo worker1 { 1, "worker1", true };
    WorkerInfo worker2 { 2, "worker2", false };
    WorkerInfo worker3 { 3, "worker3", true };

    std::vector<WorkerInfo> workers = {
        worker1,
        worker2,
        worker3
    };

    storage.transaction();

    storage.insert(workers.begin(), workers.end());

    std::cout << "count in database = " << storage.select().size() << "\n";

    storage.remove(worker3);

    std::cout << "count in database = " << storage.select().size() << "\n";

    worker2.name += " update";
    storage.update(worker2);

    std::cout << "Workers in transaction:\n";
    for(const auto& worker : storage.select()) {
        std::cout << "Worker id = " << worker.id << "\n";
        std::cout << "Worker name = " << worker.name << "\n";
        std::cout << "Worker isActiveWorker = " << worker.isActiveWorker << "\n";
        std::cout << "\n";
    }

    if(storage.commit()) {
        std::cout << "commit succsesful\n";
    } else {
        std::cout << "commit error\n";
    }

    std::cout << "Workers in database:\n";
    std::cout << "count in database = " << storage.select().size() << "\n";

    storage.removeAll();

    return 0;
}
