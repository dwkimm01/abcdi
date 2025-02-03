//
// Created by dwkimm01 on 12/27/24.
//
#include <vector>

#include <gtest/gtest.h>

#include "abcdi/abcdi.h"

namespace {
// DMA interface
class idma {
public:
    virtual ~idma() = default;
    virtual int vid() = 0;
    virtual int pid() = 0;
    virtual size_t read(std::vector<uint8_t> & v, size_t max_bytes) = 0;
};

// DMA implmementation 1
class dma1 : public idma {
public:
    dma1() = default;
    ~dma1() override = default;
    int vid() override { return 101; }
    int pid() override { return 201; }
    size_t read(std::vector<uint8_t> & v, size_t max_bytes) override {
        const size_t byte_count = std::min(v.size(), max_bytes);
        for (size_t i = 0; i < byte_count; ++i) {
            v.at(i) = 1;
        }
        return byte_count;
    }
};

// DMA implmementation 2
class dma2 : public idma {
public:
    int vid() override { return 102; }
    int pid() override { return 202; }
    size_t read(std::vector<uint8_t> & v, size_t max_bytes) override {
        const size_t byte_count = std::min(v.size(), max_bytes);
        for (size_t i = 0; i < byte_count; ++i) {
            v.at(i) = 2;
        }
        return byte_count;
    }
};

// Reader interface
class ireader {
public:
    virtual ~ireader() = default;
    virtual int read(size_t bytes) = 0;
    virtual std::shared_ptr<idma> & get_dma() = 0;
};

class reader1 : public ireader {
public:
    reader1(std::shared_ptr<idma> dma) : m_dma(std::move(dma)), m_buffer(10, 0) {}
    ~reader1() override = default;
    int read(size_t bytes) override {
        return m_dma->read(m_buffer, bytes);
    }
    std::shared_ptr<idma> & get_dma() override { return m_dma; }
private:
    std::shared_ptr<idma> m_dma;
    std::vector<uint8_t> m_buffer;
};

class reader2 : public ireader {
public:
    reader2(std::shared_ptr<idma> dma) : m_dma(std::move(dma)), m_buffer(20, 0) {}
    ~reader2() override = default;
    int read(size_t bytes) override {
        return m_dma->read(m_buffer, bytes);
    }
    std::shared_ptr<idma> & get_dma() override { return m_dma; }
private:
    std::shared_ptr<idma> m_dma;
    std::vector<uint8_t> m_buffer;
};



class foo_no_param {
public:
    foo_no_param() = default;
    int get() const { return 1; }
};

TEST(test_abcdi_28_more_classes, many_classes) {
// TODO, broken on macos-latest
    abcdi::service_provider sp;
    sp.bind<idma>().to<dma1>();
    sp.bind<idma>().to<dma2>();

    sp.bind<ireader>().to<reader1>();
    sp.bind<ireader>().to<reader2>();

    // (anonymous namespace)::reader2
    EXPECT_TRUE(sp.make_name_pre_bound("dma2"));

    std::cout << "test_abcdi_28_more_classes::many_classes, contents:\n" << sp << std::endl;

    EXPECT_TRUE(sp.make_name_pre_bound("reader2"));
    auto reader = sp.make_injected<std::shared_ptr<ireader>>();
    EXPECT_NE(nullptr, reader);

    auto bytes_read = reader->read(25);
    EXPECT_EQ(20, bytes_read);
}

TEST(test_abcdi_28_more_classes, many_classes_nothing_forced) {
    abcdi::service_provider sp;
    sp.bind<idma>().to<dma1>();
    sp.bind<idma>().to<dma2>();

    sp.bind<ireader>().to<reader1>();
    sp.bind<ireader>().to<reader2>();

    auto reader = sp.make_injected<std::shared_ptr<ireader>>();
    EXPECT_NE(nullptr, reader);
    EXPECT_NE(nullptr, std::dynamic_pointer_cast<reader1>(reader));
    EXPECT_NE(nullptr, std::dynamic_pointer_cast<dma1>(reader->get_dma()));

    auto bytes_read = reader->read(25);
    EXPECT_EQ(10, bytes_read);
}
}
