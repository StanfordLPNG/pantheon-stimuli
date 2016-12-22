#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>
#include <list>

/* Congestion controller interface */

class Controller
{
private:
  bool debug_; /* Enables debugging output */

  unsigned int window_size_;

  unsigned int datagram_num_;
  std::list< std::pair<uint64_t, uint64_t> > datagram_list_;

public:
  /* Default constructor */
  Controller(const bool debug);

  /* Get current window size, in datagrams */
  unsigned int window_size(void);

  bool window_is_open(void);

  void timer_fires(void);

  /* A datagram was sent */
  void datagram_was_sent(const uint64_t sequence_number,
                         const uint64_t send_timestamp);

  /* An ack was received */
  void ack_received(const uint64_t sequence_number_acked,
                    const uint64_t send_timestamp_acked,
                    const uint64_t recv_timestamp_acked,
                    const uint64_t timestamp_ack_received);

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  int timeout_ms(void);
};

#endif
