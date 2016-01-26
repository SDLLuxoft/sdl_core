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

#ifndef SRC_COMPONENTS_UTILS_INCLUDE_UTILS_TIMER_H_
#define SRC_COMPONENTS_UTILS_INCLUDE_UTILS_TIMER_H_

#include <stdint.h>
#include <string>
#include <signal.h>
#include <time.h>

#include "utils/lock.h"
#include "utils/timer_task.h"
#include "utils/shared_ptr.h"

namespace timer {

CREATE_LOGGERPTR_GLOBAL(logger_, "Utils")

class Timer {
 public:
  Timer(const std::string& name, utils::SharedPtr<TimerTask> task_for_tracking,
        bool is_looper = false);

  virtual ~Timer();

  virtual void start(const uint32_t time_out);

  virtual void stop();

  virtual void suspend();

  virtual bool isRunning();

  virtual void updateTimeOut(const uint32_t new_time_out);

  // getters
  uint32_t getTimeOut();
  const std::string& getTimerName();
  bool isLooper();
  const utils::SharedPtr<TimerTask> getTrackedTask();

 protected:
  virtual void restart(const uint32_t new_time_out);

  virtual void onTimeOut();

 private:
  void set_time_out(const uint32_t new_time_out);

  void startPosixTimer();

  void destructPosixTimer();

  const std::string timer_name_;
  utils::SharedPtr<TimerTask> tracked_task_;
  uint32_t time_out_miliseconds_;
  bool is_loop_;
  bool is_running_;
  bool is_must_be_stoped;
  timer_t posix_timer_;
  sigevent signal_event_;

  friend void handler_wrapper(sigval_t val);
};
}  // namespace timer

#endif  // SRC_COMPONENTS_UTILS_INCLUDE_UTILS_TIMER_H_
