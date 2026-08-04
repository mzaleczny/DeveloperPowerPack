#include <cassert>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <sstream>

#define DECLSPEC
#define BUILD_WITHOUT_GRAPHICS 1
#include "../Tilc/Globals.h"
#include "../Tilc/Utils/ScopeGuard.h"

class TestSuite : public ::testing::Test
{
protected:
    void SetUp() override
    {
        srand(time(nullptr));
    }
    void TearDown() override
    {
    }
};

void RollbackOperation1(int Code)
{
    std::cout << "Rollback operation 1 with code: " << Code << std::endl;
}

TEST_F(TestSuite, ScopeGuard)
{
    bool Operation1_Success = false;
    bool Operation2_Success = false;
    try
    {
        Operation1_Success = (rand() % 2 == 0);
        if (!Operation1_Success) throw std::exception("Operation1 exception");

        // Add rollback action for operation 1. It will be called if operation 2 fails.
        int Code = rand();
        std::cout << "Code passed: " << Code << std::endl;
        Tilc::TScopeGuard SG{ Tilc::MakeGuard(&RollbackOperation1, Code) };

        Operation2_Success = (rand() % 2 == 0);
        if (!Operation2_Success) throw std::exception("Operation2 exception");

        SG.Commit();

        ON_SCOPE_EXIT{ std::cout << "FINALIZED with default value: " << arg << std::endl; };
        ON_SCOPE_EXIT_WITH_VALUE(12) { std::cout << "FINALIZED with value: " << arg << std::endl; };
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception catched: " << e.what() << std::endl;
    }

    if (!Operation1_Success || !Operation2_Success)
    {
        std::cout << "Transaction error: ";
        if (!Operation1_Success)
        {
            std::cout << "Operation #1 failed. ";
        }
        else
        {
            std::cout << "Operation #1 succeeded. ";
            if (!Operation2_Success)
            {
                std::cout << "Operation #2 failed. ";
            }
            else
            {
                std::cout << "Operation #2 succeeded. ";
            }
        }
        std::cout << std::endl << "Rollback." << std::endl;
    }
    else
    {
        std::cout << "Transaction committed successfully" << std::endl;
    }

    ASSERT_TRUE(true);
}
