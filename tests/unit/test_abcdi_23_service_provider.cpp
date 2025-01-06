//
// Created by Dave on 6/19/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

// ---------------------------------------------------------------------------
class my_no_param_ctor {
public:
    explicit
    my_no_param_ctor() {}
    int get_i() const { return m_i; }
private:
    int m_i {101};
};

// ---------------------------------------------------------------------------
class my_one_int_val_param_ctor {
public:
    explicit
    my_one_int_val_param_ctor(int i) : m_i(i) {}
    int get_i() const { return m_i; }
private:
    int m_i {101};
};

// ---------------------------------------------------------------------------
class my_one_int_ref_param_ctor {
public:
    explicit
    my_one_int_ref_param_ctor(int & i) : m_i(i) {}
    int get_i() const { return m_i; }
private:
    int m_i {101};
};

// ---------------------------------------------------------------------------
class my_one_int_const_ref_param_ctor {
public:
    explicit
    my_one_int_const_ref_param_ctor(int const & i) : m_i(i) {}
    int get_i() const { return m_i; }
private:
    int m_i {101};
};

// ---------------------------------------------------------------------------
class my_one_int_ptr_param_ctor {
public:
   my_one_int_ptr_param_ctor(int * iptr) : m_iptr(iptr) {}
   int* get_i() { return m_iptr; }
private:
   int* m_iptr {nullptr};
};


// ---------------------------------------------------------------------------
class my_one_class_ptr_param_ctor {
public:
   my_one_class_ptr_param_ctor(my_one_int_const_ref_param_ctor * one_int_cref) : m_one_int_cref(one_int_cref) {}
   my_one_int_const_ref_param_ctor * get_ptr() { return m_one_int_cref; }
private:
   my_one_int_const_ref_param_ctor * m_one_int_cref {nullptr};
};

// ---------------------------------------------------------------------------
class my_one_class_shared_ptr_param_ctor {
public:
   my_one_class_shared_ptr_param_ctor(std::shared_ptr<my_one_int_val_param_ctor> p) : m_p(p) {}
   std::shared_ptr<my_one_int_val_param_ctor> get_ptr() { return m_p; }
private:
   std::shared_ptr<my_one_int_val_param_ctor> m_p {nullptr};
};


namespace {
TEST(test_abcdi_23_service_provider, sp_my_no_param_ctor) {
    service_provider p;
    my_no_param_ctor c(p.make_injected<my_no_param_ctor>());
    EXPECT_EQ(101, c.get_i());
}

TEST(test_abcdi_23_service_provider, sp_my_one_int_val_param_ctor) {
    service_provider p;
    my_one_int_val_param_ctor c(p.make_injected<my_one_int_val_param_ctor>());
    EXPECT_EQ(0, c.get_i());
}

TEST(test_abcdi_23_service_provider, sp_my_one_int_ref_param_ctor) {
    service_provider p;
    my_one_int_ref_param_ctor c(p.make_injected<my_one_int_ref_param_ctor>());
    EXPECT_EQ(0, c.get_i());
}

TEST(test_abcdi_23_service_provider, sp_my_one_int_const_ref_param_ctor) {
    service_provider p;

    int i_val = p.make_injected<int>();
    EXPECT_EQ(0, i_val);
    int & i_ref = p.make_injected<int&>();
    int const & i_cref = p.make_injected<int const&>();
    EXPECT_EQ(0, i_cref);

    i_ref = 123;
    EXPECT_EQ(123, i_cref);
    int & i_ref2 = p.make_injected<int&>();
    EXPECT_EQ(123, i_ref2);

    my_one_int_const_ref_param_ctor c(p.make_injected<my_one_int_const_ref_param_ctor>());
    EXPECT_EQ(123, c.get_i());
}

TEST(test_abcdi_23_service_provider, sp_my_one_int_ptr_param_ctor) {
    service_provider p;
    my_one_int_ptr_param_ctor c(p.make_injected<my_one_int_ptr_param_ctor>());
    EXPECT_NE(nullptr, c.get_i());
}

TEST(test_abcdi_23_service_provider, service_provider_my_one_class_ptr_param_ctor) {
    service_provider p;
    my_one_class_ptr_param_ctor m = p.make_injected<my_one_class_ptr_param_ctor>();
    EXPECT_NE(nullptr, m.get_ptr());
}

TEST(test_abcdi_23_service_provider, service_provider_my_one_class_shared_ptr_param_ctor) {
    service_provider p;
    my_one_class_shared_ptr_param_ctor m = p.make_injected<my_one_class_shared_ptr_param_ctor>();
    EXPECT_NE(nullptr, m.get_ptr());
}

// my_one_class_shared_ptr_param_ctor

TEST(test_abcdi_23_service_provider, service_provider_bool_val) {
    service_provider p;
    bool b = p.make_injected<bool>();
    EXPECT_FALSE(b);
}

bool & get() {
    static bool b = true;
    return b;
}

TEST(test_abcdi_23_service_provider, service_provider_bool_ref) {
    bool & x = get();
    EXPECT_TRUE(x);

    service_provider p;
    bool & b = p.make_injected<bool&>();
    EXPECT_FALSE(b);
}


TEST(test_abcdi_23_service_provider, service_provider_shared_ptr_class_ref) {
    service_provider p;
    std::shared_ptr<my_one_int_val_param_ctor> & spcr = p.make_injected<std::shared_ptr<my_one_int_val_param_ctor> &>();

    (void)spcr;
    EXPECT_NE(nullptr, spcr);
}


TEST(test_abcdi_23_service_provider, service_provider_class_two_param_integral_vals) {
    service_provider p;

    class my_class_two_param_integral_vals {
    public:
        my_class_two_param_integral_vals(int i, int j)
            : m_i(i), m_j(j) {}
        int i() const { return m_i; }
        int j() const { return m_j; }
    private:
        int m_i {1};
        int m_j {2};
    };
    my_class_two_param_integral_vals c = p.make_injected<my_class_two_param_integral_vals>();
    EXPECT_EQ(0, c.i());
    EXPECT_EQ(0, c.j());
}

TEST(test_abcdi_23_service_provider, service_provider_class_two_param_one_integral_vals_one_ref_val) {
    service_provider p;

    class my_class_t {
    public:
        my_class_t(int & i, int & j)
            : m_i(i), m_j(j) {}
        int i() const { return m_i; }
        int j() const { return m_j; }
    private:
        int m_i {-1};
        int m_j {-2};
    };
    my_class_t c = p.make_injected<my_class_t>();
    EXPECT_EQ(0, c.i());
    EXPECT_EQ(0, c.j());
}
}

