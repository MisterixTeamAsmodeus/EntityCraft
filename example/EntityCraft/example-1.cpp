#include <EntityCraft/entitycraft.h>

#include <fstream>

struct Id
{
    int id;

    Id() = default;

    explicit Id(const int id)
        : id(id)
    {
    }
};

struct WorkerInfo : Id
{
    std::shared_ptr<std::string> name;
    bool isActiveWorker;

    std::shared_ptr<std::string> get_name() const { return name; }

    void set_name(const std::shared_ptr<std::string>& name) { this->name = name; }

    WorkerInfo() = default;

    WorkerInfo(const int id, std::shared_ptr<std::string> name, const bool is_active_worker)
        : Id(id)
        , name(std::move(name))
        , isActiveWorker(is_active_worker)
    {
    }
};

struct WorkerTableInfo
{
    static constexpr auto id = "id";
    static constexpr auto name = "name";
    static constexpr auto is_active_worker = "isActiveWorker";

    static auto dto()
    {
        using namespace entity_craft;

        return make_table<WorkerInfo>("", "WorkerInfo",
            make_column(id, static_cast<int WorkerInfo::*>(&WorkerInfo::id), query_craft::primary_key() | query_craft::column_settings::auto_increment),
            make_column(name, &WorkerInfo::set_name, &WorkerInfo::get_name),
            make_column(is_active_worker, &WorkerInfo::isActiveWorker, query_craft::not_null()));
    }

    static query_craft::table table_info()
    {
        return dto().table_info();
    }
};

struct WorkerStorage
{
    using Storage = storage_type(WorkerTableInfo::dto());

    explicit WorkerStorage(const std::shared_ptr<database_adapter::IDataBaseDriver>& adapter)
        : _storage(make_storage(adapter, WorkerTableInfo::dto()))
    {
        create_table();
    }

    Storage storage() const
    {
        return _storage;
    }

    Storage& operator()()
    {
        return _storage;
    }

    std::vector<WorkerInfo> get_all_active_workers()
    {
        return _storage
            .condition_group(WorkerTableInfo::table_info().column(WorkerTableInfo::is_active_worker) == true)
            .select();
    }

private:
    void create_table() const
    {
        _storage.database()->exec("CREATE TABLE IF NOT exists WorkerInfo ( id integer primary key autoincrement, name varchar, isActiveWorker bool);");
    }

private:
    Storage _storage;
};

int main()
{
    using namespace entity_craft;

    database_adapter::sqlite::settings settings;
    settings.url = "example-1.db";
    std::remove(settings.url.c_str());

    auto worker_storage = WorkerStorage(std::make_shared<database_adapter::sqlite::database_adapter>(settings));

    WorkerInfo worker1 { 0, std::make_shared<std::string>("worker1"), true };
    WorkerInfo worker2 { 0, nullptr, false };
    WorkerInfo worker3 { 0, std::make_shared<std::string>("worker2"), true };

    std::vector<WorkerInfo> workers = {
        worker1,
        worker2,
        worker3
    };

    worker_storage().transaction();

    worker_storage().insert(workers.begin(), workers.end());

    worker1 = workers[0];
    worker2 = workers[1];
    worker3 = workers[2];

    std::cout << "count in database = " << worker_storage().select().size() << "\n";

    worker_storage().remove(worker3);

    std::cout << "count in database = " << worker_storage().select().size() << "\n";

    worker2.name = std::make_shared<std::string>(" update");
    worker_storage().update(worker2);

    std::cout << "Workers in transaction:\n";
    for(const auto& worker : worker_storage().select()) {
        std::cout << "Worker id = " << worker.id << "\n";
        std::cout << "Worker name = " << *worker.name << "\n";
        std::cout << "Worker isActiveWorker = " << worker.isActiveWorker << "\n";
        std::cout << "\n";
    }

    worker_storage().commit();

    std::cout << "Workers in database:\n";
    std::cout << "count in database = " << worker_storage().select().size() << "\n";

    return 0;
}
