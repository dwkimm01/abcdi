//
// Created by dwkimm01 on 7/9/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi_always_create.h"
using namespace abcdi;

namespace {
TEST(test_abcdi_31_always_create, sp_bool_value) {
    spv4 p;
    bool b = p.make_injected<bool>();
    EXPECT_FALSE(b);
}

TEST(test_abcdi_31_always_create, sp_bool_const_value) {
    spv4 p;
    const bool b = p.make_injected<const bool>();
    EXPECT_FALSE(b);
}

TEST(test_abcdi_31_always_create, sp_bool_reference) {
    spv4 p;
    bool & b = p.make_injected<bool&>();
    EXPECT_FALSE(b);
}

TEST(test_abcdi_31_always_create, sp_bool_const_reference) {
    spv4 p;
    bool const& b = p.make_injected<bool const&>();
    EXPECT_FALSE(b);
}

TEST(test_abcdi_31_always_create, sp_bool_pointer) {
    spv4 p;
    bool* b = p.make_injected<bool*>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);

    (*b) = true;
    bool* c = p.make_injected<bool*>();
    EXPECT_NE(nullptr, c);
    EXPECT_FALSE(*c);
}

TEST(test_abcdi_31_always_create, sp_bool_shared_pointer) {
    spv4 p;
    auto b = p.make_injected<std::shared_ptr<bool>>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);
}

TEST(test_abcdi_31_always_create, sp_bool_unique_pointer) {
    spv4 p;
    auto b = p.make_injected<std::unique_ptr<bool>>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);
}
}

