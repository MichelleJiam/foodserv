#include <gtest/gtest.h>
#include "../../src/config/nginx_config.hpp"
// #include <gmock/gmock.h>

int Factorial(int n);

// Test syntax
/*
    TEST(TestSuiteName, TestName) {
    ... test body ...
    }
*/

// Demonstrate some basic assertions.
TEST(HelloTestSuite, HelloTest) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

// Testing with function from different file
// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(Factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(Factorial(1), 1);
  EXPECT_EQ(Factorial(2), 2);
  EXPECT_EQ(Factorial(3), 6);
  EXPECT_EQ(Factorial(8), 40320);
}

// Shows how to correctly path to a config file that you should have copied
// into this test folder.
// ".." is because the test executable is run from within build/.
TEST(WebServTest, ThisIsHowYouTest) {
	NginxConfig config("../default.conf");

	(void)config;
}
