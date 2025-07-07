#include "QueryCraft/sortcolumn.h"

query_craft::sort_column query_craft::asc_sort(const column_info& column)
{
    return sort_column { column, sort_column::type::asc };
}

query_craft::sort_column query_craft::desc_sort(const column_info& column)
{
    return sort_column { column, sort_column::type::desc };
}

query_craft::sort_column query_craft::random_sort()
{
    sort_column column;
    column.column.set_alias("RANDOM ()");
    return column;
}