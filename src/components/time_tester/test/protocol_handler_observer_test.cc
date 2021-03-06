/*
 * Copyright (c) 2015, Ford Motor Company
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
#include "time_manager.h"
#include "include/time_manager_mock.h"
#include "protocol_handler/time_metric_observer.h"

namespace test {
namespace components {
namespace time_tester_test {

using namespace time_tester;
using ::testing::_;

TEST(ProtocolHandlerObserverTest, MessageProcess) {
  TimeManagerMock time_manager_mock;

  ProtocolHandlerObserver pr_handler(&time_manager_mock);
  uint32_t message_id = 1;
  TimevalStruct start_time;
  start_time.tv_sec = 1;
  start_time.tv_usec = 0;
  pr_handler.StartMessageProcess(message_id, start_time);

  typedef protocol_handler::PHMetricObserver::MessageMetric MetricType;
  utils::SharedPtr<MetricType> message_metric = new MetricType();
  message_metric->message_id = 1;
  EXPECT_CALL(time_manager_mock, SendMetric(_));
  pr_handler.EndMessageProcess(message_metric);
}

TEST(ProtocolHandlerObserverTest, MessageProcessWithZeroMessageId) {
  TimeManagerMock time_manager_mock;

  ProtocolHandlerObserver pr_handler(&time_manager_mock);
  uint32_t message_id = 0;
  TimevalStruct start_time;
  start_time.tv_sec = 1;
  start_time.tv_usec = 0;
  pr_handler.StartMessageProcess(message_id, start_time);

  typedef protocol_handler::PHMetricObserver::MessageMetric MetricType;
  utils::SharedPtr<MetricType> message_metric = new MetricType();
  message_metric->message_id = 0;
  EXPECT_CALL(time_manager_mock, SendMetric(_)).Times(0);
  pr_handler.EndMessageProcess(message_metric);
}

}  // namespace time_tester
}  // namespace components
}  // namespace test
