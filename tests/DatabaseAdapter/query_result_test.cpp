#include "mock/databaseadapter.hpp"

#include <DatabaseAdapter/model/queryresult.hpp>
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Тест конструктора по умолчанию
 */
TEST_F(DatabaseAdapter, QueryResult_DefaultConstructor)
{
    database_adapter::query_result result;
    EXPECT_TRUE(result.empty());
    EXPECT_EQ(result.size(), 0);
}

/**
 * @brief Тест конструктора с вектором строк
 */
TEST_F(DatabaseAdapter, QueryResult_ConstructorWithVector)
{
    std::vector<database_adapter::query_result::row> rows = { _row1, _row2 };
    database_adapter::query_result result(rows);

    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 2);
}

/**
 * @brief Тест добавления строк
 */
TEST_F(DatabaseAdapter, QueryResult_AddRow)
{
    database_adapter::query_result result;
    EXPECT_TRUE(result.empty());

    result.add(_row1);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);

    result.add(_row2);
    EXPECT_EQ(result.size(), 2);

    result.add(_row3);
    EXPECT_EQ(result.size(), 3);
}

/**
 * @brief Тест получения данных (копия)
 */
TEST_F(DatabaseAdapter, QueryResult_GetData)
{
    database_adapter::query_result result;
    result.add(_row1);
    result.add(_row2);

    auto data = result.data();
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data[0]["id"], "1");
    EXPECT_EQ(data[1]["id"], "2");

    // Изменение копии не должно влиять на оригинал
    data[0]["id"] = "999";
    auto original_data = result.data();
    EXPECT_EQ(original_data[0]["id"], "1");
}

/**
 * @brief Тест получения изменяемых данных
 */
TEST_F(DatabaseAdapter, QueryResult_GetMutableData)
{
    database_adapter::query_result result;
    result.add(_row1);

    auto& mutable_data = result.mutable_data();
    EXPECT_EQ(mutable_data.size(), 1);
    EXPECT_EQ(mutable_data[0]["id"], "1");

    // Изменение должно влиять на оригинал
    mutable_data[0]["id"] = "999";
    auto data = result.data();
    EXPECT_EQ(data[0]["id"], "999");
}

/**
 * @brief Тест проверки на пустоту
 */
TEST_F(DatabaseAdapter, QueryResult_Empty)
{
    database_adapter::query_result result;
    EXPECT_TRUE(result.empty());

    result.add(_row1);
    EXPECT_FALSE(result.empty());
}

/**
 * @brief Тест получения размера
 */
TEST_F(DatabaseAdapter, QueryResult_Size)
{
    database_adapter::query_result result;
    EXPECT_EQ(result.size(), 0);

    result.add(_row1);
    EXPECT_EQ(result.size(), 1);

    result.add(_row2);
    EXPECT_EQ(result.size(), 2);

    result.add(_row3);
    EXPECT_EQ(result.size(), 3);
}

/**
 * @brief Тест константных итераторов
 */
TEST_F(DatabaseAdapter, QueryResult_ConstIterators)
{
    database_adapter::query_result result;
    result.add(_row1);
    result.add(_row2);
    result.add(_row3);

    size_t count = 0;
    for(auto it = result.cbegin(); it != result.cend(); ++it) {
        EXPECT_FALSE(it->empty());
        ++count;
    }
    EXPECT_EQ(count, 3);
}

/**
 * @brief Тест неконстантных итераторов
 */
TEST_F(DatabaseAdapter, QueryResult_NonConstIterators)
{
    database_adapter::query_result result;
    result.add(_row1);
    result.add(_row2);

    size_t count = 0;
    for(auto it = result.begin(); it != result.end(); ++it) {
        it->at("id") = "modified";
        ++count;
    }
    EXPECT_EQ(count, 2);

    auto data = result.data();
    EXPECT_EQ(data[0]["id"], "modified");
    EXPECT_EQ(data[1]["id"], "modified");
}

/**
 * @brief Тест получения строки по индексу (константная версия)
 */
TEST_F(DatabaseAdapter, QueryResult_AtConst)
{
    database_adapter::query_result result;
    result.add(_row1);
    result.add(_row2);

    const auto& row = result.at(0);
    EXPECT_EQ(row.at("id"), "1");
    EXPECT_EQ(row.at("name"), "Test1");

    const auto& row2 = result.at(1);
    EXPECT_EQ(row2.at("id"), "2");
    EXPECT_EQ(row2.at("name"), "Test2");
}

/**
 * @brief Тест получения строки по индексу (неконстантная версия)
 */
TEST_F(DatabaseAdapter, QueryResult_AtNonConst)
{
    database_adapter::query_result result;
    result.add(_row1);

    auto& row = result.at(0);
    EXPECT_EQ(row["id"], "1");

    row["id"] = "modified";
    auto data = result.data();
    EXPECT_EQ(data[0]["id"], "modified");
}

/**
 * @brief Тест исключения при выходе за границы (константная версия)
 */
TEST_F(DatabaseAdapter, QueryResult_AtOutOfRangeConst)
{
    database_adapter::query_result result;
    result.add(_row1);

    EXPECT_THROW(result.at(1), std::out_of_range);
    EXPECT_THROW(result.at(100), std::out_of_range);
}

/**
 * @brief Тест исключения при выходе за границы (неконстантная версия)
 */
TEST_F(DatabaseAdapter, QueryResult_AtOutOfRangeNonConst)
{
    database_adapter::query_result result;
    result.add(_row1);

    EXPECT_THROW(result.at(1), std::out_of_range);
    EXPECT_THROW(result.at(100), std::out_of_range);
}

/**
 * @brief Тест работы с NULL значениями
 */
TEST_F(DatabaseAdapter, QueryResult_NullValues)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = NULL_VALUE;
    row["value"] = "100";

    database_adapter::query_result result;
    result.add(row);

    auto data = result.data();
    EXPECT_EQ(data[0]["name"], NULL_VALUE);
}

/**
 * @brief Тест работы с большим количеством данных
 */
TEST_F(DatabaseAdapter, QueryResult_LargeDataSet)
{
    database_adapter::query_result result;

    const size_t count = 1000;
    for(size_t i = 0; i < count; ++i) {
        database_adapter::query_result::row row;
        row["id"] = std::to_string(i);
        row["value"] = std::to_string(i * 10);
        result.add(row);
    }

    EXPECT_EQ(result.size(), count);
    EXPECT_EQ(result.at(0)["id"], "0");
    EXPECT_EQ(result.at(count - 1)["id"], std::to_string(count - 1));
}

/**
 * @brief Тест range-based for loop
 */
TEST_F(DatabaseAdapter, QueryResult_RangeBasedForLoop)
{
    database_adapter::query_result result;
    result.add(_row1);
    result.add(_row2);
    result.add(_row3);

    size_t count = 0;
    for(const auto& row : result.data()) {
        EXPECT_FALSE(row.empty());
        ++count;
    }
    EXPECT_EQ(count, 3);
}
