#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>
#include <list>
#include <vector>
#include <fstream>
#include <memory>

/* Congestion controller interface */

class Controller
{
private:
  bool debug_; /* Enables debugging output */

  unsigned int window_size_;

  unsigned int datagram_num_;
  std::list< std::pair<uint64_t, uint64_t> > datagram_list_;

  std::unique_ptr<std::ofstream> log_;

public:
  /* Default constructor */
  Controller(const bool debug);

  /* Payload size of every datagram */
  unsigned int payload_size(void);

  /* Get current window size, in datagrams */
  unsigned int window_size(void);

  /* If window is open to send more datagrams */
  bool window_is_open(void);

  /* Set the period in ms of timeout timer (return 0 to disable timer) */
  unsigned int timer_period(void);

  /* Timeout timer fires */
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
