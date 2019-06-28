/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 17, 2018
 ******************************************************************************/
#include "FT.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <deque>

using namespace std;
using Message = vector<uint8_t>;

/*----------------------------- Serial Buffer 1 ------------------------------*/

static deque<uint8_t> rx_1; // Receive Buffer
static deque<uint8_t> tx_1; // Transmit Buffer

static uint8_t get_1 () {
    uint8_t byte = rx_1.front();
    rx_1.pop_front();
    return byte;
}

static void put_1 (uint8_t byte) {
    tx_1.push_back(byte);
}

static bool empty_1 () {
    return rx_1.empty();
}

/*----------------------------- Serial Buffer 2 ------------------------------*/

static deque<uint8_t> rx_2; // Receive Buffer
static deque<uint8_t> tx_2; // Transmit Buffer

static uint8_t get_2 () {
    uint8_t byte = rx_2.front();
    rx_2.pop_front();
    return byte;
}

static void put_2 (uint8_t byte) {
    tx_2.push_back(byte);
}

static bool empty_2 () {
    return rx_2.empty();
}

/*-------------- Setup and Teardown that is Common to All Tests --------------*/

class FastTransferTests : public ::testing::Test {
protected:
    // Variables made available to all tests
    Port * port_1, * port_2;
    uint8_t localAddress;

    // Completed before every test
    void SetUp () {
        // Start with empty buffers
        rx_1.clear(); tx_1.clear();
        rx_2.clear(); tx_2.clear();

        // Allocate a port
        port_1 = FT_NewPort(put_1, get_1, empty_1);
        port_2 = FT_NewPort(put_2, get_2, empty_2);

        // Set local address
        localAddress = 0x01;

        // Initialize library
        FT_Initialize(localAddress);
    }

    // Completed after every test
    void TearDown () {
        FT_DestroyPort(port_1);
    }

    void receive_1 (initializer_list<uint8_t> const & x) {
        copy(x.begin(), x.end(), back_inserter(rx_1));
    }

    void receive_2 (initializer_list<uint8_t> const & x) {
        copy(x.begin(), x.end(), back_inserter(rx_2));
    }

    bool check (deque<uint8_t> const & lhs, initializer_list<uint8_t> const & rhs) {
        return equal(lhs.begin(), lhs.end(), rhs.begin());
    }
};

/*--------------------------- Check Initialization ---------------------------*/

TEST_F (FastTransferTests, InitialValuesInDataArrayAreAllZero) {
    for (uint16_t i = 0; i < ARRAY_SIZE; ++i)
        EXPECT_EQ(0, FT_ReadArray(i));
}

/*------------------------------- Test Parsing -------------------------------*/

TEST_F (FastTransferTests, ParsingValidMessageModifiesCorrectValue) {
    // Receive a single valid message
    receive_1({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0x1F});
    FT_ScanPort(port_1);

    // Verify that message data was written
    EXPECT_EQ(-1, FT_ReadArray(1));
}

TEST_F (FastTransferTests, ParsingMultipleValidMessagesModifiesCorrectValues) {
    // Receive two valid messages with different sizes
    receive_1({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0x1F});
    receive_1({0x06, 0x85, 0x03, 0x01, 0x08, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0xEF});
    FT_ScanPort(port_1);

    // Verify that message data was written
    EXPECT_EQ(-1, FT_ReadArray(1));
    EXPECT_EQ(1, FT_ReadArray(2));
    EXPECT_EQ(2, FT_ReadArray(3));
}

TEST_F (FastTransferTests, ParsingMessageWithInvalidCRCDoesNotChangeArray) {
    // Receive a message with an incorrect CRC code
    receive_1({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0xFF});
    FT_ScanPort(port_1);

    // Verify that the message data was NOT written
    EXPECT_EQ(0, FT_ReadArray(1));
}

TEST_F (FastTransferTests, GargabeBeforeValidMessageDoesNotBreakParsing) {
    // Receive some garbage, then receive a valid message.
    receive_1({0x01, 0x02, 0x03, 0x04, 0x05});
    receive_1({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0x1F});
    FT_ScanPort(port_1);

    // Verify that the data from valid message was written
    EXPECT_EQ(-1, FT_ReadArray(1));
}

TEST_F (FastTransferTests, GarbageBetweenValidMessagesDoesNotBreakParsing) {
    // Receive two valid messages with some garbage in between
    receive_1({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0x1F});
    receive_1({0x01, 0x02, 0x03, 0x04, 0x05});
    receive_1({0x06, 0x85, 0x03, 0x01, 0x08, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0xEF});
    FT_ScanPort(port_1);

    // Verify that the data from valid messages was written
    EXPECT_EQ(-1, FT_ReadArray(1));
    EXPECT_EQ(1, FT_ReadArray(2));
    EXPECT_EQ(2, FT_ReadArray(3));
}

/*----------------------- Test Restricted Write Access -----------------------*/

TEST_F (FastTransferTests, RestrictAccessToSinglePort) {
    // Fill both receive buffers with same message
    receive_1({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0x1F});
    receive_2({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0x1F});

    // Limit access to an index to port_2, then try to write to that index by
    // reading from port_1.
    FT_LimitAccess(0x01, port_2);

    // Verify that the data does not change when we scan port that does not have
    // write permissions
    FT_ScanPort(port_1);
    EXPECT_FALSE(FT_ReadFlag(1));
    EXPECT_EQ(0, FT_ReadArray(1));

    // Verify that the data does change when we scan port that has write perms
    FT_ScanPort(port_2);
    EXPECT_TRUE(FT_ReadFlag(1));
    EXPECT_EQ(-1, FT_ReadArray(1));
}

/*------------------------------ Modified Flags ------------------------------*/

TEST_F (FastTransferTests, InitiallyModifiedFlagsAreAllFalse) {
    // Make sure that the FT_ReadFlag flags are all cleared initially
    for (uint16_t i = 0; i < ARRAY_SIZE; ++i)
        EXPECT_FALSE(FT_ReadFlag(i));
}

TEST_F (FastTransferTests, ModifiedFlagSetWhenValueChanges) {
    // Receive a valid message
    receive_1({0x06, 0x85, 0x03, 0x01, 0x08, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0xEF});
    FT_ScanPort(port_1);

    // Verify that the FT_ReadFlag flag was set for (only) written indices
    EXPECT_FALSE(FT_ReadFlag(0x01));
    EXPECT_TRUE(FT_ReadFlag(0x02));
    EXPECT_TRUE(FT_ReadFlag(0x03));
}

TEST_F (FastTransferTests, ModifiedFlagClearedAfterRead) {
    // Receive a single valid message
    receive_1({0x06, 0x85, 0x02, 0x01, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0x1F});
    FT_ScanPort(port_1);

    // Verify that the FT_ReadFlag flag is cleared after reading
    FT_ReadArray(0x01);
    EXPECT_FALSE(FT_ReadFlag(0x01));
}

/*------------------------------- Transmission -------------------------------*/

TEST_F (FastTransferTests, SendPacketPopulatesTransmitBuffer) {
    // Assemble a packet and transmit it
    FT_ToSend(0, 10);
    FT_ToSend(1, 20);
    FT_Send(port_1, 1);

    // Verify that the transmit buffer contains the correct content
    vector<uint8_t> msg {0x06, 0x85, localAddress, 1, 8, 0, 0, 0, 10, 0, 1, 0, 20, 0xEA};
    EXPECT_TRUE(equal(tx_1.begin(), tx_1.end(), msg.begin()));
}
