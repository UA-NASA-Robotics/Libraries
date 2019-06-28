#pragma once
#include "gtest/gtest.h"
#include "ft.h"
#include "crc.h"
#include "mock_serial.hpp"

#define FT_ADDRESS 2

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                     FT_Receive testing (valid messages)                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

TEST (FTReceiveTests, ReceiveMessageWithOneValue)
{
    // Initialize the mock serial buffer module
    MockMessage m (0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m});

    FT_t * ft = FT_Create(FT_ADDRESS, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    EXPECT_EQ(-1, FT_Read(ft, 1));

    // Reset the mock serial buffer
    MockSerial_teardown();
}

TEST (FTReceiveTests, ReceiveMessageWithTwoValues)
{
    // Initialize the mock serial buffer module
    MockMessage m (0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF, 0x10, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    EXPECT_EQ(-1, FT_Read(ft, 0x01));
    EXPECT_EQ(-1, FT_Read(ft, 0x10));

    // Reset the mock serial buffer
    MockSerial_teardown();
}

TEST (FTReceiveTests, ReceiveTwoSeparateMessages)
{
    MockMessage m1 (0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF, 0x10, 0xFF, 0xFF});
    MockMessage m2 (0x01, FT_ADDRESS, {0x40, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m1, m2});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    // values from first message
    EXPECT_EQ(-1, FT_Read(ft, 0x01));
    EXPECT_EQ(-1, FT_Read(ft, 0x10));

    // value from second message
    EXPECT_EQ(-1, FT_Read(ft, 0x40));

    MockSerial_teardown();
}

TEST (FTReceiveTests, ValidMessageStraddlingTwoCallsToReceiveFunction)
{
    // construct a message and separate it into its first half and its second half
    MockMessage m (0x01, FT_ADDRESS, {0x10, 0xFF, 0xFF, 0x20, 0xFF, 0xFF});
    std::deque<uint8_t> bytes;
    m.populateBuffer(bytes);
    int mid_index = bytes.size() / 2;
    std::deque<uint8_t> first_half (bytes.begin(), bytes.begin() + mid_index);
    std::deque<uint8_t> second_half (bytes.begin() + mid_index, bytes.end());

    // create the handle for FastTransfer
    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);

    // receive the first half
    MockSerial_populate_receive_buffer(first_half);
    FT_Receive(ft);

    EXPECT_EQ(0, FT_Read(ft, 0x10));
    EXPECT_EQ(0, FT_Read(ft, 0x20));

    // receive the second half
    MockSerial_populate_receive_buffer(second_half);
    FT_Receive(ft);

    EXPECT_EQ(-1, FT_Read(ft, 0x10));
    EXPECT_EQ(-1, FT_Read(ft, 0x20));

    MockSerial_teardown();
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                    FT_Receive testing (invalid messages)                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

TEST (FTReceiveTests, MessageWithWrongAddressDoesNotChangeArray)
{
    // Initialize the mock serial buffer module
    MockMessage m (0x01, 1 + FT_ADDRESS, {0x01, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    for (int i = 0; i < ARRAY_SZ; ++i)
        EXPECT_EQ(0, FT_Read(ft, i));

    // Reset the mock serial buffer
    MockSerial_teardown();
}

TEST (FTReceiveTests, MessageWithInvalidLengthDoesNotChangeArray)
{
    // Initialize the mock serial buffer module
    MockMessage m (0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF});
    m.set_length(4);
    MockSerial_populate_receive_buffer({m});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    for (int i = 0; i < ARRAY_SZ; ++i)
        EXPECT_EQ(0, FT_Read(ft, i));

    // Reset the mock serial buffer
    MockSerial_teardown();
}

TEST (FTReceiveTests, MessageWithWrongCRCDoesNotChangeArray)
{
    // Initialize the mock serial buffer module
    MockMessage m (0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF});
    m.set_crc(0xFF);
    MockSerial_populate_receive_buffer({m});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    for (int i = 0; i < ARRAY_SZ; ++i)
        EXPECT_EQ(0, FT_Read(ft, i));

    // Reset the mock serial buffer
    MockSerial_teardown();
}

TEST (FTReceiveTests, MessageWithWrongFirstByteDoesNotChangeArray)
{
    // Initialize the mock serial buffer module
    MockMessage m (0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF});
    m.set_first_byte(0xFF);
    MockSerial_populate_receive_buffer({m});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    for (int i = 0; i < ARRAY_SZ; ++i)
        EXPECT_EQ(0, FT_Read(ft, i));

    // Reset the mock serial buffer
    MockSerial_teardown();
}

TEST (FTReceiveTests, MessageWithWrongSecondByteDoesNotChangeArray)
{
    // Initialize the mock serial buffer module
    MockMessage m (0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF});
    m.set_second_byte(0xFF);
    MockSerial_populate_receive_buffer({m});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    for (int i = 0; i < ARRAY_SZ; ++i)
        EXPECT_EQ(0, FT_Read(ft, i));

    // Reset the mock serial buffer
    MockSerial_teardown();
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//         FT_Receive testing (mixture of valid and invalid messages)         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

TEST (FTReceiveTests, ValidMessageAfterMessageWithWrongAddress)
{
    MockMessage m1(0x01, 0xFF, {0x01, 0xFF, 0xFF});
    MockMessage m2(0x01, FT_ADDRESS, {0x10, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m1, m2});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    EXPECT_EQ(0, FT_Read(ft, 0x01));
    EXPECT_EQ(-1, FT_Read(ft, 0x10));

    MockSerial_teardown();
}

TEST (FTReceiveTests, ValidMessageAfterMessageWithWrongCRC)
{
    MockMessage m1(0x01, FT_ADDRESS, {0x01, 0xFF, 0xFF});
    m1.set_crc(0xFF);
    MockMessage m2(0x01, FT_ADDRESS, {0x10, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m1, m2});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    EXPECT_EQ(0, FT_Read(ft, 0x01));
    EXPECT_EQ(-1, FT_Read(ft, 0x10));

    MockSerial_teardown();
}

TEST (FTReceiveTests, MessageWithWrongCRCBetweenTwoValidMessages)
{
    MockMessage m1(0x01, FT_ADDRESS, {0x10, 0xFF, 0xFF});
    MockMessage m2(0x01, FT_ADDRESS, {0x20, 0xFF, 0xFF});
    m2.set_crc(0xFF);
    MockMessage m3(0x01, FT_ADDRESS, {0x30, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m1, m2, m3});

    FT_t * ft = FT_Create(2, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Receive(ft);

    // value from first message
    EXPECT_EQ(-1, FT_Read(ft, 0x10));

    // no change from second message
    EXPECT_EQ(0, FT_Read(ft, 0x20));

    // value from third message
    EXPECT_EQ(-1, FT_Read(ft, 0x30));

    MockSerial_teardown();
}
