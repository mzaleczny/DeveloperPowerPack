#include <cassert>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include "../Tilc/Thread/ThreadPool.h"

std::mutex Mutex;

void OutputString(const std::string& s)
{
    std::unique_lock lk(Mutex);
    std::cout << s;
}

void Task1()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 700));
    OutputString("This is task 1\n");
}
void Task2()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 700));
    OutputString("This is task 2\n");
}
void Task3()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 700));
    OutputString("This is task 3\n");
}
void Task4()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 700));
    OutputString("This is task 4\n");
}



class TestSuite : public ::testing::Test
{
protected:
    // This method will be executed before test start
    void SetUp() override
    {
        srand(time(nullptr));
    }
    void TearDown() override
    {
    }

    int CountPhrase(std::string Phrase, const std::stringstream& Buffer)
    {
        size_t pos = 0;
        int Count = 0;
        while ((pos = Buffer.str().find(Phrase, pos)) != std::string::npos)
        {
            ++Count;
            ++pos;
        }
        return Count;
    }
};


TEST_F(TestSuite, RunningThreads)
{
    std::stringstream Buffer;
    std::streambuf* OldCoutBuffer = std::cout.rdbuf(Buffer.rdbuf());
    {
        TThreadPool tpool;
        tpool.submit(Task1);
        tpool.submit(Task1);
        tpool.submit(Task2);
        tpool.submit(Task2);
        tpool.submit(Task3);
        tpool.submit(Task3);
        tpool.submit(Task4);
        tpool.submit(Task4);
        while (!tpool.IsDone())
        {
            std::this_thread::yield();
        }
    }
    std::cout.rdbuf(OldCoutBuffer);
    std::cout << Buffer.str() << std::endl;

    ASSERT_EQ(CountPhrase("1", Buffer), 2);
    ASSERT_EQ(CountPhrase("2", Buffer), 2);
    ASSERT_EQ(CountPhrase("3", Buffer), 2);
    ASSERT_EQ(CountPhrase("4", Buffer), 2);
}
