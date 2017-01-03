#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>
#include <memory>
#include <fstream>
#include <deque>

/* Congestion controller interface */

class Controller
{
private:
  bool debug_; /* Enables debugging output */
  std::deque<std::pair<uint64_t, uint64_t>> outstanding_datagrams; // (sequence_number, send_timestamp)
  size_t max_packets_in_flight;
  uint64_t timestamp_window_last_changed;
  double rtt_ewma;
  uint64_t min_rtt_seen;
  double short_term_loss_ewma;
  double long_term_loss_ewma;
  std::unique_ptr<std::ofstream> log_;

public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  /* Default constructor */
  Controller( const bool debug );

  /* Payload size of every datagram */
  unsigned int payload_size(void);

  bool window_is_open( void );

  /* Set the period in ms of timeout timer (return 0 to disable timer) */
  unsigned int timer_period(void);

  /* Timeout timer fires */
  void timer_fires(void);

  /* A datagram was sent */
  void datagram_was_sent( const uint64_t sequence_number,
			  const uint64_t send_timestamp );

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
		     const uint64_t send_timestamp_acked,
		     const uint64_t recv_timestamp_acked,
		     const uint64_t timestamp_ack_received );

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  int timeout_ms( void );
};

#endif
