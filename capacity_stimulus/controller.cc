#include <iostream>
#include <deque>
#include <cassert>
#include <algorithm>

#include "controller.hh"
#include "timestamp.hh"

#define MIN_WINDOW_SIZE 2
#define MAX_REORDER_MS 10

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
  , outstanding_datagrams()
  , max_packets_in_flight(MIN_WINDOW_SIZE)
  , timestamp_window_last_changed(0)
  , rtt_ewma(100)
  , loss_ewma(0)
  , min_rtt_seen(1000)
{}

/* Get current window size, in datagrams
unsigned int Controller::window_size( void )
{
  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
        << " window size is " << outstanding_datagrams.size() << endl;
  }

  return outstanding_datagrams.size();
}
 */

bool Controller::window_is_open( void )
{
  return outstanding_datagrams.size() < max_packets_in_flight;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  outstanding_datagrams.emplace_back( sequence_number, send_timestamp );

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number
        << " window size is " << outstanding_datagrams.size() << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
    const double ewma_factor = .1;
    while ( ( outstanding_datagrams.front().second + MAX_REORDER_MS ) < send_timestamp_acked ) {
        outstanding_datagrams.pop_front();
        loss_ewma = 1. * ewma_factor + ( 1 - ewma_factor ) * loss_ewma;
    }

    uint64_t previous_sequence_number = 0;
    for ( auto sent_datagram = outstanding_datagrams.begin(); sent_datagram != outstanding_datagrams.end(); sent_datagram++ ) {
        if ( sent_datagram->first == sequence_number_acked ) {
            outstanding_datagrams.erase( sent_datagram );
            // packet delivered
            loss_ewma = 0. * ewma_factor + ( 1 - ewma_factor ) * loss_ewma;
            break;
        }
        if ( sent_datagram->first > sequence_number_acked ) {
            // ack for packet we already considered lost
            break;
        }

        // sanity check sequence numbers monotonic increasing in deque
        assert( previous_sequence_number < sent_datagram->first );
        previous_sequence_number = sent_datagram->first;
    }

    uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
    min_rtt_seen = min( rtt, min_rtt_seen );
    rtt_ewma = (double) rtt * ewma_factor + ( 1 - ewma_factor ) * rtt_ewma;

    //cout << "[";
    //for ( double increment = 0; increment < 1.; increment+= .01 ) {
    //    if (increment < loss_ewma)
    //        cout << "|";
    //    else
    //        cout << " ";
    //}
    //cout << "]" << endl;

    max_packets_in_flight = max( max_packets_in_flight, (uint64_t) MIN_WINDOW_SIZE );

    if ( timestamp_window_last_changed + 10 < timestamp_ack_received ) {
        if ( loss_ewma > .5 ) {
            max_packets_in_flight--;
            max_packets_in_flight--;
        } else if ( loss_ewma > .05 ) {
            max_packets_in_flight--;
        } else if ( loss_ewma > .02 ) {
            // don't change window
        } else if ( rtt_ewma < (min_rtt_seen + 10 ) ) {
            max_packets_in_flight++;
            max_packets_in_flight++;
            max_packets_in_flight++;
        } else if ( rtt_ewma < (min_rtt_seen + 200 ) ) {
            max_packets_in_flight++;
        } else if ( rtt_ewma > 1000 ) {
            max_packets_in_flight--;
        }
        timestamp_window_last_changed = timestamp_ack_received;
    }

    if ( debug_ ) {
        cerr << "At time " << timestamp_ack_received
            << " received ack for datagram " << sequence_number_acked
            << " (send @ time " << send_timestamp_acked
            << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
            << rtt
            << endl;
    }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
int Controller::timeout_ms( void )
{
  return 1500;
}
