#include <gtest/gtest.h>
#include <TypeConverterApi/typeconverterapi.h>

#include <list>

#ifdef USE_TYPE_QT
#    include <QQueue>
#endif

TEST(ContainerConverterTest, ConvertToTargetWithEmplaceBack)
{
    using namespace type_converter_api;
    std::vector<int> source = { 1, 2, 3 };
    std::list<int> target;
    container_converter<std::list<int>, int> converter;
    converter.convert_to_target(target, source);

    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target.front(), 1);
    EXPECT_EQ(target.back(), 3);
}

TEST(ContainerConverterTest, ConvertToTargetWithUnsupportedType)
{
    const std::vector<int> source = { 7, 8, 9 };
    std::set<int> target;
    type_converter_api::container_converter<std::set<int>, int> converter;

    EXPECT_THROW(converter.convert_to_target(target, source), std::runtime_error);
}

#ifdef USE_TYPE_QT
TEST(QtContainerConverterTest, ConvertToTargetWithLeftShiftOperator)
{
    std::vector<int> source = { 4, 5, 6 };
    QQueue<int> target;
    type_converter_api::container_converter<QQueue<int>, int> converter;
    converter.convert_to_target(target, source);

    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target.front(), 4);
    EXPECT_EQ(*(--target.end()), 6);
}
#endif
