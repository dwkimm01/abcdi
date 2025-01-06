//
// Created by Dave on 6/19/24.
//
#include <memory>
#include <string>
#include <gtest/gtest.h>
#include "abcdi/abcdi.h"
using namespace abcdi;

namespace {
class db_connect_str {
public:
//   db_connect_str(int i, int j, int k) {
//       (void)i; (void)j; (void)k;
   db_connect_str() {
      // std::cout << "db_connect_str @ " << (void*)this << std::endl;
   }
   ~db_connect_str() = default;

   std::string & get_val() { return val; }

private:
   std::string val;
};

class db {
public:
   db(std::shared_ptr<db_connect_str> conn_str)
      : m_conn_str(conn_str) {

      // std::cout << "db @ " << (void*)this << std::endl;
   }

   std::shared_ptr<db_connect_str> get_conn_str() { return m_conn_str; }

private:
   std::shared_ptr<db_connect_str> m_conn_str;
};


TEST(test_abcdi_22_multi_classes, sp_multi_classes) {
    service_provider p;
    std::shared_ptr<db_connect_str> conn_str = p.make_injected<std::shared_ptr<db_connect_str>>();
    EXPECT_NE(nullptr, conn_str);
    conn_str->get_val() = ":in_mem:";

    std::shared_ptr<db> db_ptr = p.make_injected<std::shared_ptr<db>>();
    EXPECT_NE(nullptr, db_ptr);
    EXPECT_NE(nullptr, db_ptr->get_conn_str());
    EXPECT_EQ(":in_mem:", db_ptr->get_conn_str()->get_val());
}
}

