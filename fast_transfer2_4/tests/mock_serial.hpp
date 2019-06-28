#pragma once
#include "mock_message.hpp"
#include <deque>
#include <vector>

void MockSerial_populate_receive_buffer (std::vector<MockMessage> const & messages);
    /// Populate the mock receive buffer with the contents of the
    /// given messages in the same order as in the given vector.

void MockSerial_populate_receive_buffer (std::deque<uint8_t> const & bytes);
    /// Populate the mock receive buffer with the given bytes.
   
void MockSerial_teardown ();
    /// Clears out the mock receive and transmit buffers.

void MockSerial_put (uint8_t byte);
    /// Pushes the given byte to the mock transmit buffer.

uint8_t MockSerial_get ();
    /// Gets the next byte from the mock receive buffer.

bool MockSerial_empty ();
    /// Returns true if and only if there are no more bytes in the

std::deque<uint8_t> MockSerial_get_receive_buffer ();
    /// Returns the mock receive buffer.

std::deque<uint8_t> MockSerial_get_transmit_buffer ();
    /// Returns the mock transmit buffer.
