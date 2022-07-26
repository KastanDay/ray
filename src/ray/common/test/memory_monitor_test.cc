// Copyright 2022 The Ray Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ray/common/memory_monitor.h"
#include "gtest/gtest.h"

namespace ray {
class MemoryMonitorTest : public ::testing::Test {};

TEST_F(MemoryMonitorTest, TestThresholdZeroAlwaysAboveThreshold) {
  MemoryMonitor monitor(0 /*usage_threshold*/, 0 /*refresh_interval_ms*/,
      [](bool is_usage_above_threshold) {
    FAIL() << "Expected monitor not running";
  });
  ASSERT_TRUE(monitor.IsUsageAboveThreshold());
}

TEST_F(MemoryMonitorTest, TestThresholdOneAlwaysBelowThreshold) {
  MemoryMonitor monitor(1 /*usage_threshold*/, 0 /*refresh_interval_ms*/,
      [](bool is_usage_above_threshold) {
    FAIL() << "Expected monitor not running";
  });
  ASSERT_FALSE(monitor.IsUsageAboveThreshold());
}

TEST_F(MemoryMonitorTest, TestGetNodeAvailableMemoryBytesAlwaysPositive) {
  {
    MemoryMonitor monitor(0 /*usage_threshold*/, 0 /*refresh_interval_ms*/,
        [](bool is_usage_above_threshold) {
      FAIL() << "Expected monitor not running";
    });
    auto [available, total] = monitor.GetNodeAvailableMemoryBytes();
    ASSERT_GT(available, 0);
    ASSERT_GT(total, available);
  }
}

TEST_F(MemoryMonitorTest, TestMonitorPeriodSetCallbackExecuted) {
  std::condition_variable callback_ran; 
  std::mutex callback_ran_mutex;

  MemoryMonitor monitor(1 /*usage_threshold*/, 1 /*refresh_interval_ms*/,
      [&callback_ran](bool is_usage_above_threshold) {
    callback_ran.notify_all();
  });
  std::unique_lock<std::mutex> callback_ran_mutex_lock(callback_ran_mutex);
  callback_ran.wait(callback_ran_mutex_lock);
}

}  // namespace ray

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
