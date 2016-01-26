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

#include "utils/timer.h"

namespace timer {

void handler_wrapper(sigval_t val) {
  if (NULL != val.sival_ptr){
    Timer *object = (Timer *)val.sival_ptr;
    object->onTimeOut();
  }
}

Timer::Timer(const std::string &name,
             utils::SharedPtr<TimerTask> task_for_tracking, bool is_looper)
    : timer_name_(name),
      tracked_task_(task_for_tracking),
      time_out_miliseconds_(0),
      is_loop_(is_looper),
      is_running_(false),
      is_must_be_stoped(false),
      posix_timer_(NULL){
  LOG4CXX_AUTO_TRACE(logger_);
}

Timer::~Timer() {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_INFO(logger_, "Timer is to be destroyed " << timer_name_);
  if (isRunning()) {
    stop();
  }
}

void Timer::start(const uint32_t time_out) {
  LOG4CXX_AUTO_TRACE(logger_);
  set_time_out(time_out);
  if (isRunning()) {
    restart(time_out_miliseconds_);
  } else {
    // CREATE NEW POSIX_TIMER
    startPosixTimer();
    is_running_ = true;
  }
}

void Timer::restart(const uint32_t new_time_out) {
  LOG4CXX_AUTO_TRACE(logger_);
  stop();
  start(new_time_out);
}

void Timer::stop() {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_DEBUG(logger_, "Stopping timer  " << timer_name_);
  //  DESTROING OLD POSIX_TIMER
  destructPosixTimer();

  is_running_ = false;
  time_out_miliseconds_ = 0;
}

void Timer::suspend() {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_DEBUG(logger_, "Suspend timer " << timer_name_ << " after next loop");
  is_must_be_stoped = true; }

bool Timer::isRunning() { return is_running_; }

void Timer::updateTimeOut(const uint32_t new_time_out) {
  LOG4CXX_AUTO_TRACE(logger_);
  if (isRunning()) {
    restart(new_time_out);
  } else {
    set_time_out(new_time_out);
  }
}

uint32_t Timer::getTimeOut() { return time_out_miliseconds_; }

const std::string&Timer::getTimerName(){ return timer_name_; }

bool Timer::isLooper() { return is_loop_; }

const utils::SharedPtr<TimerTask> Timer::getTrackedTask(){
  return tracked_task_;
}

void Timer::onTimeOut() {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_DEBUG(logger_,
                "Timer has finished counting. Timeout(ms): " << time_out_miliseconds_);
  tracked_task_->run();
  if (is_loop_ && !is_must_be_stoped) {
    LOG4CXX_INFO(logger_, "Restart timer " << timer_name_);
    restart(time_out_miliseconds_);
  } else {
    stop();
  }
}
void Timer::set_time_out(const uint32_t new_time_out) {
  // There would be no way to stop thread if timeout in lopper will be 0
  time_out_miliseconds_ = (new_time_out > 0) ? new_time_out : 1;
}

void Timer::startPosixTimer() {
  LOG4CXX_AUTO_TRACE(logger_);
  LOG4CXX_INFO(logger_, "Creating posix_timer in " << timer_name_);
  struct itimerspec itimer;
  itimer.it_interval.tv_sec = time_out_miliseconds_ / 1000;
  itimer.it_interval.tv_nsec = (time_out_miliseconds_ % 1000) * 1000000;
  itimer.it_value.tv_sec = itimer.it_interval.tv_sec;
  itimer.it_value.tv_nsec = itimer.it_interval.tv_nsec;

  signal_event_.sigev_notify = SIGEV_THREAD;
  signal_event_.sigev_notify_attributes = NULL;
  signal_event_.sigev_value.sival_ptr = (void *)this;
  signal_event_.sigev_notify_function = handler_wrapper;

  if (timer_create(CLOCK_REALTIME, &signal_event_, &posix_timer_) < 0) {
    LOG4CXX_ERROR(logger_, "Can`t create posix_timer");
  }
  if (timer_settime(posix_timer_, 0, &itimer, NULL) < 0) {
    LOG4CXX_ERROR(logger_, "Can`t set time_out to posix_timer");
  }
}

void Timer::destructPosixTimer() {
  LOG4CXX_AUTO_TRACE(logger_);
  if (NULL != posix_timer_ && timer_delete(posix_timer_) < 0) {
    LOG4CXX_ERROR(logger_, "Can`t delete posix_timer");
  }
}

}  // namespace timer
