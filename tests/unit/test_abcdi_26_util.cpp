//
// Created by dwkimm01 on 7/19/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
struct my_test_struct {};
class my_test_class {};
enum my_test_enum { MY_TEST_ENUM1, MY_TEST_ENUM2 };
enum class my_test_class_enum { MY_TEST_CLASS_ENUM1, MY_TEST_CLASS_ENUM2 };

TEST(test_abcdi_26_util, is_simple_made_test) {
    EXPECT_TRUE(is_default_made_v<bool>);
    EXPECT_TRUE(is_default_made_v<char>);
    EXPECT_TRUE(is_default_made_v<unsigned char>);
    EXPECT_TRUE(is_default_made_v<int8_t>);
    EXPECT_TRUE(is_default_made_v<uint8_t>);
    EXPECT_TRUE(is_default_made_v<int16_t>);
    EXPECT_TRUE(is_default_made_v<uint16_t>);
    EXPECT_TRUE(is_default_made_v<int32_t>);
    EXPECT_TRUE(is_default_made_v<uint32_t>);
    EXPECT_TRUE(is_default_made_v<int64_t>);
    EXPECT_TRUE(is_default_made_v<uint64_t>);
    EXPECT_TRUE(is_default_made_v<float>);
    EXPECT_TRUE(is_default_made_v<double>);
    EXPECT_TRUE(is_default_made_v<std::string>);
    EXPECT_TRUE(is_default_made_v<my_test_enum>);
    EXPECT_TRUE(is_default_made_v<my_test_class_enum>);

    EXPECT_TRUE(is_default_made_v<std::shared_ptr<int>>);
    EXPECT_TRUE(is_default_made_v<std::unique_ptr<int>>);
    EXPECT_FALSE(is_default_made_v<my_test_struct>);
    EXPECT_FALSE(is_default_made_v<my_test_class>);
}

TEST(test_abcdi_26_util, is_shared_ptr_test) {
    EXPECT_FALSE(is_shared_ptr<bool>::value);
    EXPECT_FALSE(is_shared_ptr<char>::value);
    EXPECT_FALSE(is_shared_ptr<unsigned char>::value);
    EXPECT_FALSE(is_shared_ptr<int8_t>::value);
    EXPECT_FALSE(is_shared_ptr<uint8_t>::value);
    EXPECT_FALSE(is_shared_ptr<int16_t>::value);
    EXPECT_FALSE(is_shared_ptr<uint16_t>::value);
    EXPECT_FALSE(is_shared_ptr<int32_t>::value);
    EXPECT_FALSE(is_shared_ptr<uint32_t>::value);
    EXPECT_FALSE(is_shared_ptr<int64_t>::value);
    EXPECT_FALSE(is_shared_ptr<uint64_t>::value);
    EXPECT_FALSE(is_shared_ptr<float>::value);
    EXPECT_FALSE(is_shared_ptr<double>::value);

    EXPECT_TRUE(is_shared_ptr<std::shared_ptr<int>>::value);
    EXPECT_FALSE(is_shared_ptr<std::unique_ptr<int>>::value);
    EXPECT_FALSE(is_shared_ptr<std::string>::value);
    EXPECT_FALSE(is_shared_ptr<my_test_struct>::value);
    EXPECT_FALSE(is_shared_ptr<my_test_class>::value);
    EXPECT_FALSE(is_shared_ptr<my_test_enum>::value);
    EXPECT_FALSE(is_shared_ptr<my_test_class_enum>::value);
}

TEST(test_abcdi_26_util, string_test) {
    using str_ptr_t = std::string*;
    EXPECT_TRUE(std::is_pointer_v<str_ptr_t>);
    EXPECT_TRUE(is_default_made_v<std::remove_pointer_t<str_ptr_t>>);
    EXPECT_TRUE(is_string<std::string>::value);
}

TEST(test_abcdi_26_util, vec_test) {

    // EXPECT_FALSE(is_vect<int>::value);
    // EXPECT_TRUE(is_vect<std::vector<int>>::value);

    EXPECT_EQ("std::vector<int>", get_type_name<std::vector<int>>());
}
}
