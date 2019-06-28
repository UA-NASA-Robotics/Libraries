#pragma once
#include "gtest/gtest.h"
#include "ft.h"
#include "crc.h"
#include "mock_serial.hpp"

#define FT_ADDRESS 2

TEST (FTModifiedTests, ReadFunctionUnsetsModifiedFlag)
{
    FT_t * ft = FT_Create(FT_ADDRESS, MockSerial_put, MockSerial_get, MockSerial_empty);
    EXPECT_TRUE(FT_Modified(ft, 0));
    FT_Read(ft, 0);
    EXPECT_FALSE(FT_Modified(ft, 0));
}

TEST (FTModifiedTests, ReceiveFunctionSetsModifiedFlag)
{
    FT_t * ft = FT_Create(FT_ADDRESS, MockSerial_put, MockSerial_get, MockSerial_empty);
    FT_Read(ft, 0);

    EXPECT_FALSE(FT_Modified(ft, 0));

    MockMessage m (0x01, FT_ADDRESS, {0x00, 0xFF, 0xFF});
    MockSerial_populate_receive_buffer({m});

    FT_Receive(ft);

    EXPECT_TRUE(FT_Modified(ft, 0));
    
    MockSerial_teardown();
}
