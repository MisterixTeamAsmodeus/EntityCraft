#include "EntityCraft/column.h"
#include "EntityCraft/table.h"

#include <iostream>

struct Test
{
    int a = 0;
};

int main()
{
    using namespace EntityCraft;

    auto table = make_table<Test>("test", "public",
          make_column("id", &Test::a, primary_key()));

    std::cout << "Table: " << table.table_info().tableName() << std::endl;

    return 0;
}
