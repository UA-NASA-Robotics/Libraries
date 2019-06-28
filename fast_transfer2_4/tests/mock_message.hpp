#pragma once
#include <stdint.h>
#include <deque>
#include "crc.h"

class MockMessage
{
public:
    static uint8_t const FIRST_BYTE;
    static uint8_t const SECOND_BYTE;

public:
    MockMessage (uint8_t sender, uint8_t receiver, std::deque<uint8_t> content)
        : m_first(FIRST_BYTE),
          m_second(SECOND_BYTE),
          m_sender(sender),
          m_receiver(receiver),
          m_length(content.size()),
          m_content(std::move(content))
    {
        uint8_t * arr = new uint8_t [m_length];
        for (int i = 0; i < m_length; ++i)
            arr[i] = m_content.at(i);
        m_crc = crc(arr, m_length);
        delete [] arr;
    }

    void set_first_byte (uint8_t byte)
    {
        m_first = byte;
    }

    void set_second_byte (uint8_t byte)
    {
        m_second = byte;
    }

    void set_length (uint8_t length)
    {
        m_length = length;
    }

    void set_crc (uint8_t crc_value)
    {
        m_crc = crc_value;
    }

    void populateBuffer (std::deque<uint8_t> & buffer) const
    {
        buffer.push_back(m_first);
        buffer.push_back(m_second);
        buffer.push_back(m_sender);
        buffer.push_back(m_receiver);
        buffer.push_back(m_length);
        std::copy(m_content.begin(), m_content.end(), std::back_inserter(buffer));
        buffer.push_back(m_crc);
    }

private:
    uint8_t m_first;
    uint8_t m_second;
    uint8_t m_sender;
    uint8_t m_receiver;
    uint8_t m_length;
    uint8_t m_crc;
    std::deque<uint8_t> m_content;
};
