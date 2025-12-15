#include <gtest/gtest.h>
#include "common/common.h"

TEST(BasicTest, CommonIncludeWorks) {
    EXPECT_EQ(PORT, 6969);
}

