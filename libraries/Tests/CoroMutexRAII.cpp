#include <cassert>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>

#define DECLSPEC
#include "../Tilc/Utils/Coro.h"

std::mutex Mutex;

class TestSuite : public ::testing::Test
{
protected:
    bool m_DoThrow{};

    // This method will be executed before test start
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }

    int TestBuffer(std::string ValidatingString, const std::stringstream& Buffer)
    {
        return (Buffer.str().find(ValidatingString, 0) == 0);
    }
};

co_resource<std::mutex> MakeGuard(std::mutex& m)
{
    std::cout << "Before locking" << std::endl;
    m.lock();
    co_yield m;
    m.unlock();
    std::cout << "After locking" << std::endl;
}

TEST_F(TestSuite, WithoutThrow)
{
    std::stringstream Buffer;
    std::streambuf* OldCoutBuffer = std::cout.rdbuf(Buffer.rdbuf());

    try
    {
        m_DoThrow = false;
        co_resource<std::mutex> lg{ MakeGuard(Mutex) };
        if (m_DoThrow) throw 0;
        std::cout << "Do some work!" << std::endl;
    }
    catch (...) {}

    std::cout.rdbuf(OldCoutBuffer);
    std::cout << Buffer.str() << std::endl;

    ASSERT_TRUE(TestBuffer("Before locking\nDo some work!\nAfter locking", Buffer));
}

TEST_F(TestSuite, WithThrow)
{
    std::stringstream Buffer;
    std::streambuf* OldCoutBuffer = std::cout.rdbuf(Buffer.rdbuf());

    try
    {
        m_DoThrow = true;
        co_resource<std::mutex> lg{ MakeGuard(Mutex) };
        if (m_DoThrow) throw 0;
        std::cout << "Do some work!" << std::endl;
    }
    catch (...) {}

    std::cout.rdbuf(OldCoutBuffer);
    std::cout << Buffer.str() << std::endl;

    ASSERT_TRUE(TestBuffer("Before locking\nAfter locking", Buffer));
}
