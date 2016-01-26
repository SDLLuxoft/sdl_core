/*
 * Copyright (c) 2016, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "gtest/gtest.h"
#include "utils/timer.h"
#include "utils/mock_timer_task.h"

namespace test {
namespace components {

namespace timer_test {

class TimerTest : public testing::Test {
 public:
  TimerTest() : timer_name_("test_timer"), is_looper_(false), time_out_(1000) {}

 protected:
  void SetUp() OVERRIDE {
    time_out_ = 1000;
    // Will be destroyed in Timer::~Destructor
    test_task_ = new MockTimerTask();
  }

  const std::string timer_name_;
  bool is_looper_;
  MockTimerTask* test_task_;
  uint32_t time_out_;
};

TEST_F(TimerTest, Init_TwoArgs_CorrectInitValues) {
  // Actions
  timer::Timer test_timer(timer_name_, test_task_);
  // Expects
  EXPECT_EQ(timer_name_, test_timer.getTimerName());
  EXPECT_FALSE(test_timer.isLooper());
  EXPECT_EQ(test_task_, test_timer.getTrackedTask().get());
  EXPECT_EQ(0u, test_timer.getTimeOut());
}

TEST_F(TimerTest, Init_ThreeArgs_CorrectInitValues) {
  // Preconditions
  is_looper_ = true;
  // Actions
  timer::Timer test_timer(timer_name_, test_task_, is_looper_);
  // Expects
  EXPECT_EQ(timer_name_, test_timer.getTimerName());
  EXPECT_TRUE(test_timer.isLooper());
  EXPECT_EQ(test_task_, test_timer.getTrackedTask().get());
  EXPECT_EQ(0u, test_timer.getTimeOut());
}

TEST_F(TimerTest, UpdateTimeOut_CorrectValues_CorrectValues) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  ASSERT_EQ(0u, test_timer.getTimeOut());
  time_out_ = 10000;
  // Actions
  test_timer.updateTimeOut(time_out_);
  // Expects
  EXPECT_EQ(time_out_, test_timer.getTimeOut());
}

TEST_F(TimerTest, UpdateTimeOut_Zerro_One) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  ASSERT_EQ(0u, test_timer.getTimeOut());
  time_out_ = 0u;
  // Actions
  test_timer.updateTimeOut(time_out_);
  // Expects
  EXPECT_EQ(1u, test_timer.getTimeOut());
}

TEST_F(TimerTest, Start_NoLoop_OneCall) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  // Expects
  EXPECT_CALL(*test_task_, run());
  // Actions
  test_timer.start(time_out_);
  // Elisey`s feature xD_________(just a joke, comment will be rewriten)
  testing::Mock::AsyncVerifyAndClearExpectations(time_out_ * 2);
}

TEST_F(TimerTest, Start_Loop_3Calls) {
  // Preconditions
  is_looper_ = true;
  timer::Timer test_timer(timer_name_, test_task_, is_looper_);
  // Expects
  EXPECT_CALL(*test_task_, run()).Times(3);
  // Actions
  test_timer.start(time_out_);
  sleep(3);
  test_timer.suspend();
  // Waiting calls in all threads
  testing::Mock::AsyncVerifyAndClearExpectations(5000);
}

TEST_F(TimerTest, Suspend_Loop_OneCall) {
  // Preconditions
  is_looper_ = true;
  timer::Timer test_timer(timer_name_, test_task_, is_looper_);
  // Expects
  EXPECT_CALL(*test_task_, run());
  // Actions
  test_timer.start(time_out_);
  test_timer.suspend();
  // Waiting calls in all threads
  testing::Mock::AsyncVerifyAndClearExpectations(time_out_ * 2);
}

TEST_F(TimerTest, Stop_FirstLoop_NoCall) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  // Expects
  EXPECT_CALL(*test_task_, run()).Times(0);
  // Actions
  test_timer.start(time_out_);
  test_timer.stop();
}

TEST_F(TimerTest, Stop_SecondLoop_OneCall) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  // Expects
  EXPECT_CALL(*test_task_, run());
  // Actions
  test_timer.start(time_out_);
  // Wait to starting second loop
  // (mseconds*1000=useconds)
  usleep(time_out_ * 1000 * 1.5);
  test_timer.stop();
}

TEST_F(TimerTest, IsRunning_NotStarted_False) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  // Expects
  EXPECT_FALSE(test_timer.isRunning());
}

TEST_F(TimerTest, IsRunning_Started_True) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  // Actions
  test_timer.start(time_out_);
  // Expects
  EXPECT_TRUE(test_timer.isRunning());
}

TEST_F(TimerTest, IsRunning_Stoped_False) {
  // Preconditions
  timer::Timer test_timer(timer_name_, test_task_);
  // Actions
  test_timer.start(time_out_);
  test_timer.stop();
  // Expects
  EXPECT_FALSE(test_timer.isRunning());
}

TEST_F(TimerTest, IsRunning_Suspended_False) {
  // Preconditions
  is_looper_ = true;
  timer::Timer test_timer(timer_name_, test_task_, is_looper_);
  // Actions
  test_timer.start(time_out_);
  test_timer.suspend();
  // Expects
  EXPECT_CALL(*test_task_, run());
  EXPECT_TRUE(test_timer.isRunning());
  // Wait to end of loop
  // (mseconds*1000=useconds)
  usleep(time_out_ * 1000 * 1.5);
  EXPECT_FALSE(test_timer.isRunning());
}

}  // namespace timer_test
}  // namespace components
}  // namespace test
