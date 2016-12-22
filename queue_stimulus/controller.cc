#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller(const bool debug)
  : debug_(debug)
  , window_size_(10)
  , window_is_open_(false)
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size(void)
{
  if (debug_) {
    cerr << "At time " << timestamp_ms()
    << " window size is " << window_size_ << endl;
  }

  return window_size_;
}

bool Controller::window_is_open(void)
{
  return window_is_open_;
}

/* A datagram was sent */
void Controller::datagram_was_sent(
    const uint64_t sequence_number,
    /* of the sent datagram */
    const uint64_t send_timestamp)
    /* in milliseconds */
{
  if (debug_) {
    cerr << "At time " << send_timestamp
    << " sent datagram " << sequence_number << endl;
  }
}

/* An ack was received */
void Controller::ack_received(
    const uint64_t sequence_number_acked,
    /* what sequence number was acknowledged */
    const uint64_t send_timestamp_acked,
    /* when the acknowledged datagram was sent (sender's clock) */
    const uint64_t recv_timestamp_acked,
    /* when the acknowledged datagram was received (receiver's clock) */
    const uint64_t timestamp_ack_received)
    /* when the ack was received (by sender) */
{
  if (debug_) {
    cerr << "At time " << timestamp_ack_received
    << " received ack for datagram " << sequence_number_acked
    << " (send @ time " << send_timestamp_acked
    << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
    << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms(void)
{
  return 1000; /* timeout of one second */
}
