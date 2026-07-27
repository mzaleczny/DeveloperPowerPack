#include <cassert>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#include "../Tilc/Utils/Html.h"
using namespace Tilc;

class TestSuite : public ::testing::Test
{
protected:
    // This method will be executed before test start
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};


TEST_F(TestSuite, HtmlDocument)
{
    THTML doc{
        THead{
            TTitle{"Sample page"}
        },
        THead{
            TP{"Hello world!"},
            TOL{
                TLI{"First position on ordered list"},
                TLI{"Second position on ordered list"},
                TLI{"Third position on ordered list"}
            },
            TUL{
                TLI{"First position on unordered list"},
                TLI{"Second position on unordered list",
                    TUL{
                        TLI{"First subitem"},
                        TLI{"Second subitem"}
                    }
                },
                TLI{"Third position on unordered list"}
            }
        }
    };
    doc.Print();
    std::cout << "DONE." << std::endl;

    ASSERT_TRUE(true);
}
