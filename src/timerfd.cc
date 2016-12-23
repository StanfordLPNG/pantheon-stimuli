#include "timerfd.hh"
#include <unistd.h>
#include "util.hh"

using namespace std;

Timerfd::Timerfd(int flags)
  : FileDescriptor(SystemCall("timerfd_create",
                              timerfd_create(CLOCK_REALTIME, flags)))
{}

void Timerfd::arm(unsigned int first_exp_ms, unsigned int interval_ms)
{
  unsigned int first_exp_s = first_exp_ms / 1000;
  unsigned int first_exp_ns = (first_exp_ms - first_exp_s * 1000) * 1000000;

  unsigned int interval_s = interval_ms / 1000;
  unsigned int interval_ns = (interval_ms - interval_s * 1000) * 1000000;

  struct itimerspec ts;
  ts.it_value.tv_sec = first_exp_s;
  ts.it_value.tv_nsec = first_exp_ns;
  ts.it_interval.tv_sec = interval_s;
  ts.it_interval.tv_nsec = interval_ns;

  SystemCall("timerfd_settime", timerfd_settime(fd_num(), 0, &ts, NULL));
}

bool Timerfd::is_disarmed()
{
  struct itimerspec curr_ts;
  SystemCall("timerfd_gettime", timerfd_gettime(fd_num(), &curr_ts));
  return (curr_ts.it_value.tv_sec == 0 && curr_ts.it_value.tv_nsec == 0);
}

unsigned int Timerfd::expirations()
{
  uint64_t num_exp = 0;

  if (SystemCall("read", ::read(fd_num(), &num_exp, sizeof(uint64_t)) != sizeof(uint64_t))) {
    perror("expirations:read");
    exit(EXIT_FAILURE);
  }

  register_read();
  return (unsigned int) num_exp;
}

