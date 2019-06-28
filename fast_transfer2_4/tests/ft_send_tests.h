#pragma once
#include "gtest/gtest.h"
#include "ft.h"
#include "crc.h"
#include "mock_serial.hpp"

#define FT_ADDRESS 2

TEST (FTSendTests, SendPopulatesSerialTransmitBuffer)
{
    FT_t * ft = FT_Create(FT_ADDRESS, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_ToSend(ft, 0x10, -1);
    FT_Send(ft, 1);
    std::deque<uint8_t> buffer = MockSerial_get_transmit_buffer();

    uint8_t packet_data [] = {0x10, 0xFF, 0xFF};
    uint8_t crc_code = crc(packet_data, 3);
    std::deque<uint8_t> expected {0x06, 0x85, FT_ADDRESS, 1, 3, 0x10, 0xFF, 0xFF, crc_code};

    EXPECT_EQ(expected, buffer);

    MockSerial_teardown();
}

TEST (FTSendTests, SendingEmptyBufferSendsNothing)
{
    FT_t * ft = FT_Create(FT_ADDRESS, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Send(ft, 1);
    std::deque<uint8_t> buffer = MockSerial_get_transmit_buffer();
    std::deque<uint8_t> expected = {};

    EXPECT_EQ(expected, buffer);

    MockSerial_teardown();
}
