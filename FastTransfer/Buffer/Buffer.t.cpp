/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 3, 2018
 ******************************************************************************/
#include "Buffer.h"
#include "gtest/gtest.h"

/*----------------- Setup and Teardown for Common Test Data ------------------*/

class BufferTests : public ::testing::Test {
protected:
    Buffer *buffer;
    Buffer *fullBuffer;
    Buffer *middle;
    static const size_t CAPACITY = 100;

    void SetUp () {
        buffer = createBuffer(CAPACITY);
        fullBuffer = createBuffer(CAPACITY);
        middle = createBuffer(CAPACITY);
        for (int i = 0; i < 100; ++i)
            push(fullBuffer, 1);
        for (int i = 0; i < CAPACITY / 2; ++i) {
            push(middle, 1);
            pop(middle);
        }
    }

    void TearDown () {
        destroyBuffer(buffer);
        destroyBuffer(fullBuffer);
        destroyBuffer(middle);
    }
};

/*---------------------------------- Tests -----------------------------------*/

TEST_F (BufferTests, SizeOfBufferIsInitiallyZero) {
    EXPECT_EQ(0, getSize(buffer));
}

TEST_F (BufferTests, BufferIsInitiallyEmpty) {
    EXPECT_TRUE(isEmpty(buffer));
}

TEST_F (BufferTests, SizeIncreasesWhenElementIsPushed) {
    push(buffer, 1);
    EXPECT_EQ(1, getSize(buffer));
}

TEST_F (BufferTests, BufferIsNotEmptyAfterPush) {
    push(buffer, 1);
    EXPECT_FALSE(isEmpty(buffer));
}

TEST_F (BufferTests, FIFO_Behavior) {
    push(buffer, 1);
    push(buffer, 2);
    push(buffer, 3);
    EXPECT_EQ(1, pop(buffer));
    EXPECT_EQ(2, pop(buffer));
    EXPECT_EQ(3, pop(buffer));
}

TEST_F (BufferTests, BufferIsEmptyAfterPoppingEverything) {
    push(buffer, 1); push(buffer, 2); push(buffer, 3);
    pop(buffer); pop(buffer); pop(buffer);
    EXPECT_TRUE(isEmpty(buffer));
}

TEST_F (BufferTests, IsFullReturnsFalseUntilBufferIsFull) {
    for (int i = 0; i < CAPACITY; ++i) {
        EXPECT_FALSE(isFull(buffer));
        push(buffer, 0);
    }
    EXPECT_TRUE(isFull(buffer));
}

TEST_F (BufferTests, PushHasNoEffectIfBufferIsFull) {
    size_t initialSize = getSize(fullBuffer);
    push(fullBuffer, 2);
    size_t finalSize = getSize(fullBuffer);
    EXPECT_EQ(initialSize, finalSize);
    EXPECT_TRUE(isFull(fullBuffer));
}

TEST_F (BufferTests, RingBufferBehaviorWithWrapping) {
    for (int i = 0; i < CAPACITY; ++i)
        push(middle, (uint8_t)i);
    EXPECT_TRUE(isFull(middle));
    EXPECT_FALSE(isEmpty(middle));
    for (int i = 0; i < CAPACITY; ++i)
        EXPECT_EQ((uint8_t)i, pop(middle));
    EXPECT_FALSE(isFull(middle));
    EXPECT_TRUE(isEmpty(middle));
}

TEST_F (BufferTests, GetValueReturnsCorrectValue) {
    for (int i = 0; i < CAPACITY; ++i)
        push(middle, (uint8_t)i);
    for (int i = 0; i < CAPACITY; ++i)
        EXPECT_EQ((uint8_t)i, getValue(middle, i));
}
