#include "EntityCraft/column.h"
#include "EntityCraft/table.h"

#include <iostream>

struct Test
{
    int a = 0;
};

int main()
{
    auto table = EntityCraft::make_table<Test>("public", "test",
          EntityCraft::make_column("id", &Test::a));
}
