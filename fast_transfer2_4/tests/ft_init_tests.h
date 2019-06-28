#pragma once
#include "gtest/gtest.h"
#include "ft.h"
#include "mock_serial.hpp"

#define FT_ADDRESS 2

TEST (FTCreateTests, ArrayIsZeroInitialized)
{
  FT_t * ft = FT_Create(FT_ADDRESS, MockSerial_put, MockSerial_get, MockSerial_empty);
  for (int i = 0; i < ARRAY_SZ; ++i)
    EXPECT_EQ(0, FT_Read(ft, i));
}

TEST (FTCreateTests, ModificationFlagsAreInitializedToTrue)
{
  FT_t * ft = FT_Create(FT_ADDRESS, MockSerial_put, MockSerial_get, MockSerial_empty);
  for (int i = 0; i < ARRAY_SZ; ++i)
    EXPECT_TRUE(FT_Modified(ft, i));
}
