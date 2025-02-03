//
// Created by dwkimm01 on 11/1/24.
//
#include <vector>
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
using TEST_T = std::vector<int>;

TEST(test_abcdi_13_vector, sp_vec_bool_empty_value) {
    service_provider p;
    EXPECT_TRUE(p.make_injected<std::vector<bool>>().empty());
}

TEST(test_abcdi_13_vector, sp_vec_int_empty_value) {
    service_provider p;
    auto b = p.make_injected<TEST_T>();
    EXPECT_TRUE(b.empty());
}

TEST(test_abcdi_13_vector, sp_vec_int_one_value) {
    service_provider p;
    int & i = p.make_injected<int&>();
    i = 42;
    auto b = p.make_injected<TEST_T>();
    EXPECT_FALSE(b.empty());
    EXPECT_EQ(42, b.at(0));
}

struct foo_abcdi_13_vector {
    int i {12};
};

TEST(test_abcdi_13_vector, sp_vec_foo_empty) {
    service_provider p;
    std::vector<foo_abcdi_13_vector> & foo_vec = p.make_injected<std::vector<foo_abcdi_13_vector>&>();
    EXPECT_TRUE(foo_vec.empty());
}

TEST(test_abcdi_13_vector, sp_vec_foo_not_empty) {
// TODO, broken on  macos-latest
    service_provider p;
    auto ptr = p.make_injected<std::shared_ptr<foo_abcdi_13_vector>>();
    ptr->i = 43;
    std::vector<foo_abcdi_13_vector> & foo_vec = p.make_injected<std::vector<foo_abcdi_13_vector>&>();
    EXPECT_FALSE(foo_vec.empty());
    EXPECT_EQ(43, foo_vec.at(0).i);
}
}
