//
// Created by dwkimm01 on 7/19/24.
//
#include <string>
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {

// ---------------------------------------------------------------------------
class my_class_that_takes_a_string_val {
public:
    explicit
    my_class_that_takes_a_string_val(std::string s) : m_s(std::move(s)) {}
    ~my_class_that_takes_a_string_val() = default;
    [[nodiscard]]
    const std::string & s() const { return m_s; }
private:
    std::string m_s;
};

TEST(test_abcdi_27_string_params, sp_string_param_val) {
    service_provider p;
    auto& s = p.make_injected<std::string&>();
    s = "Hello";
    auto & v = p.make_injected<my_class_that_takes_a_string_val&>();
    EXPECT_EQ("Hello", v.s());
}

// ---------------------------------------------------------------------------
class my_class_that_takes_a_string_ref {
public:
    explicit
    my_class_that_takes_a_string_ref(std::string & s) : m_s(s) {}
    ~my_class_that_takes_a_string_ref() = default;
    [[nodiscard]]
    const std::string & s() const { return m_s; }
private:
    std::string m_s;
};

TEST(test_abcdi_27_string_params, sp_string_ref) {
    service_provider p;
    auto& s = p.make_injected<std::string&>();
    s = "Hello";
    auto & v = p.make_injected<my_class_that_takes_a_string_ref&>();
    EXPECT_EQ("Hello", v.s());
}

// ---------------------------------------------------------------------------
class my_class_that_takes_a_string_const_ref {
public:
    explicit
    my_class_that_takes_a_string_const_ref(std::string const & s) : m_s(s) {}
    ~my_class_that_takes_a_string_const_ref() = default;
    [[nodiscard]]
    const std::string & s() const { return m_s; }
private:
    std::string m_s;
};

TEST(test_abcdi_27_string_params, sp_string_const_ref) {
    service_provider p;
    auto& s = p.make_injected<std::string&>();
    s = "Hello";
    auto & v = p.make_injected<my_class_that_takes_a_string_const_ref&>();
    EXPECT_EQ("Hello", v.s());
}

// ---------------------------------------------------------------------------
class my_class_that_takes_a_string_pointer {
public:
    explicit
    my_class_that_takes_a_string_pointer(std::string * s) : m_s(*s) {}
    ~my_class_that_takes_a_string_pointer() = default;
    [[nodiscard]]
    const std::string & s() const { return m_s; }
private:
    std::string m_s;
};

TEST(test_abcdi_27_string_params, sp_string_pointer) {
    service_provider p;
    auto * str_ptr = p.make_injected<std::string*>();
    EXPECT_NE(nullptr, str_ptr);

    auto * & str_ptr_ref = p.make_injected<std::string* &>();
    EXPECT_NE(nullptr, str_ptr_ref);

    EXPECT_EQ(&(*str_ptr), &(*str_ptr_ref));

    auto str_shared_ptr = p.make_injected<std::shared_ptr<std::string>>();

    auto& s = p.make_injected<std::string&>();
    s = "Hello";
    auto & v = p.make_injected<my_class_that_takes_a_string_pointer&>();
    EXPECT_EQ("Hello", v.s());
}

}