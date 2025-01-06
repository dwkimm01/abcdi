//
// Created by Dave on 6/26/24.
//
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace
{
TEST(test_abcdi_24_binding, sp_bind_01_int_lambda) {
    service_provider p;
    using type_t = int;

    p.bind_to([]() -> type_t { return 54331; });
    const type_t var = p.make_injected<type_t>();
    EXPECT_EQ(54331, var);
}


TEST(test_abcdi_24_binding, sp_bind_01_string_lambda) {
    service_provider p;
    p.bind_to([]() -> std::string { return "Hello"; });
    auto str = p.make_injected<std::string>();
    EXPECT_EQ("Hello", str);
}


int child_dtor_count = 0;

class parent {
public:
    virtual ~parent() = default;
    [[nodiscard]]
    virtual int get() const { return 1; }
    virtual int blah() = 0;
};

class child : public parent {
public:
    ~child() override { ++child_dtor_count; }
    [[nodiscard]]
    int get() const override { return 2; }
    int blah() override { return 101; }
};



TEST(test_abcdi_24_binding, sp_bind_01_interface_shared_ptr) {
    {
        child_dtor_count = 0;

        service_provider p;
        EXPECT_ANY_THROW(p.make_injected<std::shared_ptr<parent>>());

        p.bind<parent>().to<child>();
        auto ptr = p.make_injected<std::shared_ptr<parent>>();
        EXPECT_EQ(2, ptr->get());

        EXPECT_EQ(0, child_dtor_count);
    }
    EXPECT_EQ(1, child_dtor_count);
}


TEST(test_abcdi_24_binding, sp_bind_01_interface_raw_ptr) {
    {
        child_dtor_count = 0;
        service_provider p;
        p.bind<parent>().to<child>();
        auto ptr = p.make_injected<parent*>();
        EXPECT_EQ(2, ptr->get());

        EXPECT_EQ(0, child_dtor_count);
        // Warning, should not call "delete ptr;" anymore
        // EXPECT_EQ(1, child_dtor_count);
    }
    EXPECT_EQ(1, child_dtor_count);
}


TEST(test_abcdi_24_binding, sp_bind_01_fn_lambda_raw_ptr) {
    {
        child_dtor_count = 0;
        service_provider p;

        // Before
        EXPECT_ANY_THROW(p.make_injected<parent*>());
        // Add Raw Ptr Binding
        p.bind_to([]() -> parent* {
            // return new child();
            //parent* parent_ptr = new child();
            //return parent_ptr;
            return new child();
        });

        auto raw_ptr = p.make_injected<parent*>();
        EXPECT_NE(nullptr, raw_ptr);
        EXPECT_EQ(2, raw_ptr->get());

        EXPECT_EQ(0, child_dtor_count);
    }
    EXPECT_EQ(1, child_dtor_count);
}



TEST(test_abcdi_24_binding, sp_bind_01_fn_lambda_shared_ptr) {
    {
        child_dtor_count = 0;
        service_provider p;

        // Before
        EXPECT_ANY_THROW(p.make_injected<std::shared_ptr<parent>>());
        // Add Shared Ptr binding
        p.bind_to([]() {
            return std::shared_ptr<parent>(new child());
        });
        auto shr_ptr = p.make_injected<std::shared_ptr<parent>>();
        EXPECT_NE(nullptr, shr_ptr);
        EXPECT_EQ(2, shr_ptr->get());
        EXPECT_EQ(0, child_dtor_count);
    }
    EXPECT_EQ(1, child_dtor_count);
}


TEST(test_abcdi_24_binding, sp_bind_01_fn_lambda_int) {
    service_provider p;
    p.bind_to([](){ return 42; });
    auto i = p.make_injected<int>();
    EXPECT_EQ(42, i);
}


TEST(test_abcdi_24_binding, sp_bind_01_fn_lambda_int_vec_empty) {
    service_provider p;
    const auto vec = p.make_injected<std::vector<int>>();
    EXPECT_TRUE(vec.empty());
}


TEST(test_abcdi_24_binding, sp_bind_01_fn_lambda_int_vec) {
    service_provider p;
    p.bind_to([](){ return 42; });
    EXPECT_EQ(42, p.make_injected<int>());
    const auto vec = p.make_injected<std::vector<int>>();
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(42, vec.at(0));
}


TEST(test_abcdi_24_binding, sp_bind_01_fn_lambda_int_ptr_vec) {
    service_provider p;
    // Note, not sure this is how we want to force binding to int*
    p.bind_to([]{ return new int(42); });

    int* iptr = p.make_injected<int*>();
    EXPECT_EQ(42, *iptr);

    int ival = p.make_injected<int>();
    EXPECT_EQ(42, ival);

    auto vec = p.make_injected<std::vector<int*>>();
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(42, *vec.at(0));
}



class child2param : public parent {
public:
    child2param(int8_t i, int j) : m_i(i), m_j(j) {}
    ~child2param() override = default;
    [[nodiscard]] int get() const override { return (2 * 100) + (m_i * 10) + (m_j * 1); }
    [[nodiscard]] const int8_t & i() const { return m_i; }
    [[nodiscard]] const int & j() const { return m_j; }

    // virtual int get() const { return 1; }
    int blah() override { return m_i * m_j; }
private:
    int8_t m_i {-1};
    int m_j {-1};
};

TEST(test_abcdi_24_binding, sp_bind_03_fn_using_sp) {
    service_provider p;

    p.bind_to([&]() -> std::shared_ptr<parent> {
        return p.make_injected<std::shared_ptr<child2param>>();
    });

    auto ptr_parent = p.make_injected<std::shared_ptr<parent>>();
    EXPECT_NE(nullptr, ptr_parent);
    EXPECT_EQ(200, ptr_parent->get());

    auto ptr_child = p.make_injected<std::shared_ptr<child2param>>();
    EXPECT_NE(nullptr, ptr_child);
    EXPECT_EQ(&(*ptr_parent), &(*ptr_child));
}


TEST(test_abcdi_24_binding, sp_bind_04_fn_using_sp_param_fiddling) {
    service_provider p;
    p.make_injected<int8_t&>() = 3;
    p.make_injected<int&>() = 4;

    p.bind_to([&]() -> std::shared_ptr<parent> { return p.make_injected<std::shared_ptr<child2param>>(); }); // return std::shared_ptr<parent>(new child2param()); });
    auto ptr = p.make_injected<std::shared_ptr<parent>>();
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(234, ptr->get());

    std::shared_ptr<child2param> child_ptr = std::dynamic_pointer_cast<child2param>(ptr);
    EXPECT_NE(nullptr, child_ptr);

    auto child_ptr_2 = p.make_injected<std::shared_ptr<child2param>>();
    EXPECT_EQ(&(*ptr), &(*child_ptr_2));
}

}
