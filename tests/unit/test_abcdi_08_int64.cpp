//
// Created by Dave on 6/26/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
using TEST_T = int64_t;

TEST(test_abcdi_08_int64, sp_int64_t_value) {
    service_provider p;
    auto b = p.make_injected<TEST_T>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_08_int64, sp_int64_t_const_value) {
    service_provider p;
    const auto b = p.make_injected<const TEST_T>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_08_int64, sp_int64_t_reference) {
    service_provider p;
    auto & b = p.make_injected<TEST_T&>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_08_int64, sp_int64_t_const_reference) {
    service_provider p;
    auto const& b = p.make_injected<TEST_T const&>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_08_int64, sp_int64_t_pointer) {
    service_provider p;
    auto* b = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ(0, *b);

    (*b) = 123;
    auto* c = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(123, *c);
}

TEST(test_abcdi_08_int64, sp_int64_t_pointer_ref) {
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

TEST(test_abcdi_08_int64, sp_int64_t_shared_pointer) {
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

TEST(test_abcdi_08_int64, sp_int64_t_unique_pointer) {
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
