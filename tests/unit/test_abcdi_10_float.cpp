//
// Created by dwkimm01 on 7/19/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
using TEST_T = float;

TEST(test_abcdi_10_float, sp_float_value) {
    service_provider p;
    auto v = p.make_injected<TEST_T>();
    EXPECT_EQ(0, v);
}

TEST(test_abcdi_10_float, sp_float_const_value) {
    service_provider p;
    const auto v = p.make_injected<const TEST_T>();
    EXPECT_EQ(0, v);
}

TEST(test_abcdi_10_float, sp_float_reference) {
    service_provider p;
    auto & v = p.make_injected<TEST_T&>();
    EXPECT_EQ(0, v);
}

TEST(test_abcdi_10_float, sp_float_const_reference) {
    service_provider p;
    auto const& v = p.make_injected<TEST_T const&>();
    EXPECT_EQ(0, v);
}

TEST(test_abcdi_10_float, sp_float_pointer) {
    service_provider p;
    auto* v = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, v);
    EXPECT_EQ(0, *v);

    (*v) = 123.4;
    auto* c = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, c);
    EXPECT_NEAR(123.4, *c, 0.0001);
}

TEST(test_abcdi_10_float, sp_float_pointer_ref) {
    service_provider p;
    auto* & b = p.make_injected<TEST_T* &>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ(0, *b);

    (*b) = 123;
    auto* & c = p.make_injected<TEST_T* &>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(123, *c);

    auto & d = p.make_injected<TEST_T&>();
    EXPECT_EQ(123, d);

    auto * e = p.make_injected<TEST_T*>();
    EXPECT_EQ(123, *e);
}

TEST(test_abcdi_10_float, sp_float_shared_pointer) {
    service_provider p;
    auto b = p.make_injected<std::shared_ptr<TEST_T>>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ(0, *b);
    EXPECT_EQ(2, b.use_count());

    (*b) = 123;
    auto c = p.make_injected<std::shared_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(123, *c);
    EXPECT_EQ(3, b.use_count());
    EXPECT_EQ(3, c.use_count());
}

TEST(test_abcdi_10_float, sp_float_unique_pointer) {
    service_provider p;
    auto b = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ(0, *b);

    (*b) = 123;
    auto c = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(0, *c);
}
}

