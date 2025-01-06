//
// Created by dwkimm01 on 7/19/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
using TEST_T = unsigned char;

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_value) {
    service_provider p;
    auto b = p.make_injected<TEST_T>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_const_value) {
    service_provider p;
    const auto b = p.make_injected<const TEST_T>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_reference) {
    service_provider p;
    auto & b = p.make_injected<TEST_T&>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_const_reference) {
    service_provider p;
    auto const& b = p.make_injected<TEST_T const&>();
    EXPECT_EQ(0, b);
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_pointer) {
    service_provider p;
    auto* b = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ(0, *b);

    (*b) = 123;
    auto* c = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(123, *c);
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_pointer_ref) {
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

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_shared_pointer) {
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

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_shared_pointer_ref) {
    service_provider p;
    auto & b = p.make_injected<std::shared_ptr<TEST_T>&>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ(0, *b);
    EXPECT_EQ(1, b.use_count());

    (*b) = 123;
    auto c = p.make_injected<std::shared_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(123, *c);
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_shared_pointer_pointer) {
    service_provider p;
    auto * b = p.make_injected<std::shared_ptr<TEST_T>*>();
    EXPECT_NE(nullptr, b);
    EXPECT_TRUE((*b));
    EXPECT_EQ(0, *(*b));
    EXPECT_EQ(1, (*b).use_count());

    (*(*b)) = 123;
    auto * c = p.make_injected<std::shared_ptr<TEST_T>*>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
    EXPECT_EQ(123, *(*c));
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_shared_pointer_pointer_reference) {
    service_provider p;
    auto * & b = p.make_injected<std::shared_ptr<TEST_T>* &>();
    EXPECT_NE(nullptr, b);
    EXPECT_TRUE((*b));
    EXPECT_EQ(0, *(*b));
    EXPECT_EQ(1, (*b).use_count());

    (*(*b)) = 123;
    auto * & c = p.make_injected<std::shared_ptr<TEST_T>* &>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
    EXPECT_EQ(123, *(*c));
}

TEST(test_abcdi_03_unsigned_char, sp_unsigned_char_unique_pointer) {
    service_provider p;
    auto b = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ(0, *b);

    (*b) = 12;
    auto c = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(0, *c);
}
}
