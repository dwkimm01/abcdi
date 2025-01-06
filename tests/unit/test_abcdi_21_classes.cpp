//
// Created by Dave on 7/14/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {

class my_class {
public:
   int & get_i() { return m_i; }
private:
   int m_i {42};
};

TEST(test_abcdi_21_classes, sp_class) {

    std::shared_ptr<my_class> mc;
    {
        service_provider p;
        mc = p.make_injected<std::shared_ptr<my_class>>();
        EXPECT_NE(nullptr, mc);
        EXPECT_EQ(42, mc->get_i());
        mc->get_i() = 123;
        EXPECT_EQ(123, mc->get_i());
        EXPECT_EQ(2, mc.use_count());


        auto mc2 = p.make_injected<std::shared_ptr<my_class>>();
        EXPECT_NE(nullptr, mc2);
        EXPECT_EQ(123, mc2->get_i());
        EXPECT_EQ(&(*mc), &(*mc2));
        EXPECT_EQ(3, mc.use_count());
        EXPECT_EQ(3, mc2.use_count());
    }

    EXPECT_EQ(1, mc.use_count());
}


TEST(test_abcdi_21_classes, sp_class_ref) {
    service_provider p;

    auto & mc = p.make_injected<my_class&>();
    EXPECT_EQ(42, mc.get_i());
    mc.get_i() = 123;
    EXPECT_EQ(123, mc.get_i());

    auto & mc2 = p.make_injected<my_class&>();
    EXPECT_EQ(123, mc2.get_i());
}


TEST(test_abcdi_21_classes, sp_class_ptr) {
    service_provider p;

    auto * mc = p.make_injected<my_class*>();
    EXPECT_EQ(42, mc->get_i());
    mc->get_i() = 123;
    EXPECT_EQ(123, mc->get_i());

    auto * mc2 = p.make_injected<my_class*>();
    EXPECT_EQ(123, mc2->get_i());
}

TEST(test_abcdi_21_classes, up_class) {
    service_provider p;

    auto mc = p.make_injected<std::unique_ptr<my_class>>();
    EXPECT_EQ(42, mc->get_i());
    mc->get_i() = 223;
    EXPECT_EQ(223, mc->get_i());

    auto mc2 = p.make_injected<std::unique_ptr<my_class>>();
    EXPECT_EQ(42, mc2->get_i());
}

TEST(test_abcdi_21_classes, up_class_ref) {
    service_provider p;

    auto & mc = p.make_injected<std::unique_ptr<my_class>&>();
    EXPECT_EQ(42, mc->get_i());
    mc->get_i() = 223;
    EXPECT_EQ(223, mc->get_i());

    auto & mc2 = p.make_injected<std::unique_ptr<my_class>&>();
    EXPECT_EQ(223, mc2->get_i());
}

TEST(test_abcdi_21_classes, rp_class_ref) {
    service_provider p;

    auto * & mc = p.make_injected<my_class* &>();
    EXPECT_EQ(42, mc->get_i());
    mc->get_i() = 223;
    EXPECT_EQ(223, mc->get_i());

    auto * & mc2 = p.make_injected<my_class* &>();
    EXPECT_EQ(223, mc2->get_i());
}
}
