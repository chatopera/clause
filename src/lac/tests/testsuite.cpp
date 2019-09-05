/**
 * Run all tests
 */
#include "gtest/gtest.h"
#include "glog/logging.h"

/**
 * Main Function to Collect all testcase
 */
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
//   gflags::ParseCommandLineFlags(&argc, &argv, true);
  return RUN_ALL_TESTS();
}
