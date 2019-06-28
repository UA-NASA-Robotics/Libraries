#include "gtest/gtest.h"
#include "buffer.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                              Buffer_size tests                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

TEST (BufferSize, SizeIsInitiallyZero)
{
  Buffer_t b;
  Buffer_init(&b);
  EXPECT_EQ(0, Buffer_size(&b));
}

TEST (BufferSize, SizeIncreasesAfterPush)
{
  Buffer_t b;
  Buffer_init(&b);
  Buffer_push(&b, 1);
  EXPECT_EQ(1, Buffer_size(&b));
}

TEST (BufferSize, SizeDecreasesAfterPop)
{
  Buffer_t b;
  Buffer_init(&b);
  Buffer_push(&b, 1);
  EXPECT_EQ(1, Buffer_size(&b));
  Buffer_pop(&b);
  EXPECT_EQ(0, Buffer_size(&b));
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                              Buffer_full tests                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

TEST (BufferFull, FullInitiallyReturnsFalse)
{
  Buffer_t b;
  Buffer_init(&b);
  EXPECT_FALSE(Buffer_full(&b));
}

TEST (BufferFull, FullReturnsTrueAfterPushingMaxAmount)
{
  Buffer_t b;
  Buffer_init(&b);
  for (int i = 0; i < 256; ++i)
    Buffer_push(&b, 1);
  EXPECT_TRUE(Buffer_full(&b));
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                             Buffer_empty tests                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

TEST (BufferEmpty, EmptyInitiallyReturnsTrue)
{
  Buffer_t b;
  Buffer_init(&b);
  EXPECT_TRUE(Buffer_empty(&b));
}

TEST (BufferEmpty, EmptyReturnsFalseAfterPushing)
{
  Buffer_t b;
  Buffer_init(&b);
  Buffer_push(&b, 1);
  EXPECT_FALSE(Buffer_empty(&b));
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                              Buffer_get tests                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

TEST (BufferGet, GetReturnsExpectedValues)
{
  Buffer_t b;
  Buffer_init(&b);
  Buffer_push(&b, 1);
  Buffer_push(&b, 2);
  Buffer_push(&b, 3);
  EXPECT_EQ(1, Buffer_get(&b, 0));
  EXPECT_EQ(2, Buffer_get(&b, 1));
  EXPECT_EQ(3, Buffer_get(&b, 2));
}
