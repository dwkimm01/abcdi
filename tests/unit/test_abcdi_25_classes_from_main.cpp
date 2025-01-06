//
// Created by dwkimm01 on 7/19/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;
namespace {

class foo_no_param {
public:
    foo_no_param() = default;
    int get() const { return 1; }
};

class foo_one_param {
public:
    explicit
    foo_one_param(const int & i) : m_i(i) {
        // std::cout << "foo_one_param @ " << static_cast<void*>(this) << endl;
    }
    int & get() { return m_i; }
private:
    int m_i {-1};
};

class foo_two_param {
public:
    foo_two_param(const int & i, foo_one_param & fop) : m_i(i), m_foo1(fop) {}
    int get() const { return m_i; }
    foo_one_param & get_fop() { return m_foo1; }

private:
    int m_i {-2};
    foo_one_param & m_foo1;
};

TEST(test_abcdi_25_classes_from_main, foos_01) {
    service_provider p;
    int & iref = p.make_injected<int&>();
    iref = 23;
    int ival = p.make_injected<int>();
    EXPECT_EQ(23, ival);
}

TEST(test_abcdi_25_classes_from_main, foos_02) {
    service_provider p;
//    int & iref = p.make_injected<int&>();
//    iref = 23;
//    int ival = p.make_injected<int>();
//    cout << "ival = " << ival << endl;

    auto fop = p.make_injected<foo_one_param>();
    EXPECT_EQ(0, fop.get());

    auto fop_sp = p.make_injected<std::shared_ptr<foo_one_param>>();
    // auto fop_rp = p.make_injected<foo_one_param*>();
    // cout << "fop_rp: " << static_cast<void*>(fop_rp) << endl;

    auto & fop_ref = p.make_injected<foo_one_param&>();
    EXPECT_EQ(0, fop_ref.get());

    auto ftp_sp = p.make_injected<std::shared_ptr<foo_two_param>>();
    EXPECT_EQ((void*)(&fop_ref), (void*)(&(ftp_sp->get_fop())));

    auto ftp_sp2 = p.make_injected<std::shared_ptr<foo_two_param>>();
    EXPECT_EQ(&(*ftp_sp), &(*ftp_sp2));

    ftp_sp->get_fop().get() = 12345;

    auto f0 = p.make_injected<foo_no_param>();
    EXPECT_EQ(1, f0.get());

    auto f1 = p.make_injected<foo_one_param>();
    EXPECT_EQ(12345, f1.get());

    {
        auto f2 = p.make_injected<foo_two_param>();
        EXPECT_EQ(0, f2.get());
        EXPECT_EQ(12345, f2.get_fop().get());
    }

    {
        service_provider p2;
        int & local_i_ref = p2.make_injected<int&>();
        local_i_ref = 12345;

        auto fop2 = p2.make_injected<std::shared_ptr<foo_one_param>>();
        EXPECT_EQ(12345, fop2->get());


        local_i_ref = 876;
        auto f2 = p2.make_injected<foo_two_param>();
        EXPECT_EQ(876, f2.get());
        EXPECT_EQ(12345, f2.get_fop().get());
    }

}
}
