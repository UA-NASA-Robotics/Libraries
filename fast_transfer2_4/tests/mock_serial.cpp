#include "mock_serial.hpp"
#include <algorithm>
#include <iostream>

/*
 * Implement the MockSerial module
 */

static std::deque<uint8_t> r_buf;
    /// mock receive buffer

static std::deque<uint8_t> t_buf;
    /// mock transmit buffer

void MockSerial_populate_receive_buffer (std::vector<MockMessage> const & msgs)
{
    for (auto const & msg : msgs)
        msg.populateBuffer(r_buf);
}

void MockSerial_populate_receive_buffer (std::deque<uint8_t> const & bytes)
{
    std::copy(bytes.begin(), bytes.end(), std::back_inserter(r_buf));
}

void MockSerial_teardown ()
{
    r_buf.clear();
    t_buf.clear();
}

void MockSerial_put (uint8_t byte)
{
    t_buf.push_back(byte);
}

uint8_t MockSerial_get ()
{
    uint8_t byte = r_buf.front();
    r_buf.pop_front();
    return byte;
}

bool MockSerial_empty ()
{
    return r_buf.empty();
}

std::deque<uint8_t> MockSerial_get_receive_buffer ()
{
    return r_buf;
}

std::deque<uint8_t> MockSerial_get_transmit_buffer ()
{
    return t_buf;
}
