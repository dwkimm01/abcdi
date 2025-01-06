//
// Created by Dave on 6/26/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
using TEST_T = bool;

TEST(test_abcdi_01_bool, sp_bool_value) {
    service_provider p;
    auto b = p.make_injected<TEST_T>();
    EXPECT_FALSE(b);
}

TEST(test_abcdi_01_bool, sp_bool_const_value) {
    service_provider p;
    const auto b = p.make_injected<const TEST_T>();
    EXPECT_FALSE(b);
}

TEST(test_abcdi_01_bool, sp_bool_reference) {
    service_provider p;
    auto & b = p.make_injected<TEST_T&>();
    EXPECT_FALSE(b);
}

TEST(test_abcdi_01_bool, sp_bool_const_reference) {
    service_provider p;
    auto const& v = p.make_injected<TEST_T const&>();
    EXPECT_FALSE(v);

    p.make_injected<TEST_T &>() = true;
    EXPECT_EQ(true, v);
}

TEST(test_abcdi_01_bool, sp_bool_pointer) {
    service_provider p;
    auto* b = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);

    (*b) = true;
    auto* c = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
}

TEST(test_abcdi_01_bool, sp_bool_pointer_ref) {
    service_provider p;
    auto* & b = p.make_injected<TEST_T* &>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);

    (*b) = true;
    auto* & c = p.make_injected<TEST_T* &>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);

    auto & d = p.make_injected<TEST_T&>();
    EXPECT_TRUE(d);

    auto * e = p.make_injected<TEST_T*>();
    EXPECT_TRUE(*e);
}

TEST(test_abcdi_01_bool, sp_bool_shared_pointer) {
    service_provider p;
    auto b = p.make_injected<std::shared_ptr<TEST_T>>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);
    EXPECT_EQ(2, b.use_count());

    (*b) = true;
    auto c = p.make_injected<std::shared_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
    EXPECT_EQ(3, b.use_count());
    EXPECT_EQ(3, c.use_count());
}

TEST(test_abcdi_01_bool, sp_bool_shared_pointer_ref) {
    service_provider p;
    auto & b = p.make_injected<std::shared_ptr<TEST_T>&>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);
    EXPECT_EQ(1, b.use_count());

    (*b) = true;
    auto c = p.make_injected<std::shared_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
}

TEST(test_abcdi_01_bool, sp_bool_shared_pointer_pointer) {
    service_provider p;
    auto * b = p.make_injected<std::shared_ptr<TEST_T>*>();
    EXPECT_NE(nullptr, b);
    EXPECT_TRUE((*b));
    EXPECT_FALSE(*(*b));
    EXPECT_EQ(1, (*b).use_count());

    (*(*b)) = true;
    auto * c = p.make_injected<std::shared_ptr<TEST_T>*>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
    EXPECT_TRUE(*(*c));
}

TEST(test_abcdi_01_bool, sp_bool_shared_pointer_pointer_reference) {
    service_provider p;
    auto * & b = p.make_injected<std::shared_ptr<TEST_T>* &>();
    EXPECT_NE(nullptr, b);
    EXPECT_TRUE((*b));
    EXPECT_FALSE(*(*b));
    EXPECT_EQ(1, (*b).use_count());

    (*(*b)) = true;
    auto * & c = p.make_injected<std::shared_ptr<TEST_T>* &>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
    EXPECT_TRUE(*(*c));
}

TEST(test_abcdi_01_bool, sp_bool_unique_pointer) {
    service_provider p;
    auto b = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, b);
    EXPECT_FALSE(*b);

    (*b) = true;
    auto c = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_FALSE(*c);
}
}
