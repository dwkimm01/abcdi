//
// Created by dwkimm01 on 7/19/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
using TEST_T = std::string;

TEST(test_abcdi_12_string, sp_string_value) {
    service_provider p;
    auto v = p.make_injected<TEST_T>();
    EXPECT_EQ("", v);
}

TEST(test_abcdi_12_string, sp_string_const_value) {
    service_provider p;
    const auto v = p.make_injected<const TEST_T>();
    EXPECT_EQ("", v);
}

TEST(test_abcdi_12_string, sp_string_reference) {
    service_provider p;
    auto & v = p.make_injected<TEST_T&>();
    EXPECT_EQ("", v);
}

TEST(test_abcdi_12_string, sp_string_const_reference) {
    service_provider p;
    auto const& v = p.make_injected<TEST_T const&>();
    EXPECT_EQ("", v);

    p.make_injected<TEST_T &>() = "Hello";
    EXPECT_EQ("Hello", v);
}

TEST(test_abcdi_12_string, sp_string_pointer) {
    service_provider p;
    auto* v = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, v);
    EXPECT_EQ("", *v);

    (*v) = "Hello";
    auto* c = p.make_injected<TEST_T*>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ("Hello", *c);
}

TEST(test_abcdi_12_string, sp_string_pointer_ref) {
    service_provider p;
    auto* & b = p.make_injected<TEST_T* &>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ("", *b);

    (*b) = "123";
    auto* & c = p.make_injected<TEST_T* &>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ("123", *c);

    auto & d = p.make_injected<TEST_T&>();
    EXPECT_EQ("123", d);

    auto * e = p.make_injected<TEST_T*>();
    EXPECT_EQ("123", *e);
}

TEST(test_abcdi_12_string, sp_string_shared_pointer) {

    std::shared_ptr<TEST_T> b;
    {
        service_provider p;
        b = p.make_injected<std::shared_ptr<TEST_T>>();
        EXPECT_NE(nullptr, b);
        EXPECT_EQ("", *b);
        EXPECT_EQ(2, b.use_count());

        (*b) = "123";
        auto c = p.make_injected<std::shared_ptr<TEST_T>>();
        EXPECT_NE(nullptr, c);
        EXPECT_EQ("123", *c);
        EXPECT_EQ(3, b.use_count());
        EXPECT_EQ(3, c.use_count());
    }

    EXPECT_EQ(1, b.use_count());
}

TEST(test_abcdi_12_string, sp_string_shared_pointer_ref) {
    service_provider p;
    auto & b = p.make_injected<std::shared_ptr<TEST_T>&>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ("", *b);
    EXPECT_EQ(1, b.use_count());

    (*b) = "Hello";
    auto c = p.make_injected<std::shared_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ("Hello", *c);
}

TEST(test_abcdi_12_string, sp_string_shared_pointer_pointer) {
    service_provider p;
    auto * b = p.make_injected<std::shared_ptr<TEST_T>*>();
    EXPECT_NE(nullptr, b);
    EXPECT_TRUE((*b));
    EXPECT_EQ("", *(*b));
    EXPECT_EQ(1, (*b).use_count());

    (*(*b)) = "Hello";
    auto * c = p.make_injected<std::shared_ptr<TEST_T>*>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
    EXPECT_EQ("Hello", *(*c));
}

TEST(test_abcdi_12_string, sp_string_shared_pointer_pointer_reference) {
    service_provider p;
    auto * & b = p.make_injected<std::shared_ptr<TEST_T>* &>();
    EXPECT_NE(nullptr, b);
    EXPECT_TRUE((*b));
    EXPECT_EQ("", *(*b));
    EXPECT_EQ(1, (*b).use_count());

    (*(*b)) = "Hello";
    auto * & c = p.make_injected<std::shared_ptr<TEST_T>* &>();
    EXPECT_NE(nullptr, c);
    EXPECT_TRUE(*c);
    EXPECT_EQ("Hello", *(*c));
}

TEST(test_abcdi_12_string, sp_string_unique_pointer) {
    service_provider p;
    auto b = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, b);
    EXPECT_EQ("", *b);

    (*b) = "123";
    auto c = p.make_injected<std::unique_ptr<TEST_T>>();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ("", *c);
}
}
