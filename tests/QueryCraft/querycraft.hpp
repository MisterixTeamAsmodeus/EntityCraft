#pragma once

#include <gtest/gtest.h>
#include <QueryCraft/querycraft.h>

class QueryCraft : public ::testing::Test
{
protected:
    void SetUp() override
    {
        postgres_dialect_ = std::make_shared<query_craft::postgres_dialect>();
        sqlite_dialect_ = std::make_shared<query_craft::sqlite_dialect>();
    }

protected:
    std::shared_ptr<query_craft::postgres_dialect> postgres_dialect_;
    std::shared_ptr<query_craft::sqlite_dialect> sqlite_dialect_;
};