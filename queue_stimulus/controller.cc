#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller(const bool debug)
  : debug_(debug)
  , window_size_(10)
  , datagram_num_(0)
  , datagram_list_()
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
  return datagram_num_ < window_size_;
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

  datagram_num_++;
  datagram_list_.emplace_back(sequence_number, send_timestamp);
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

  for (auto it = datagram_list_.begin(); it != datagram_list_.end(); it++) {
    if (it->first == sequence_number_acked) {
      datagram_list_.erase(it);
      break;
    } else if (it->first > sequence_number_acked) {
      break;
    }
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
int Controller::timeout_ms(void)
{
  return -1;
}
