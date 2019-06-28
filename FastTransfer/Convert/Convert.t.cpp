/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 3, 2018
 ******************************************************************************/
#include "Convert.h"
#include "gtest/gtest.h"

/*------------------------------- Test Fixture -------------------------------*/

class ConvertTests : public ::testing::Test {
protected:
    int16_t zero;
    int16_t negativeOne;
    void SetUp () {
        zero = 0;
        negativeOne = -1;
    }
};

/*---------------- Test Signed Int to MSB and LSB Conversions ----------------*/

TEST_F (ConvertTests, MostSignificantByteOfZeroIsZero) {
    EXPECT_EQ(0, getMsbFromSigned(zero));
}

TEST_F (ConvertTests, LeastSignificantByteOfZeroIsZero) {
    EXPECT_EQ(0, getLsbFromSigned(zero));
}

TEST_F (ConvertTests, NegativeOneIsAllOnes) {
    EXPECT_EQ(0b11111111, getMsbFromSigned(negativeOne));
    EXPECT_EQ(0b11111111, getLsbFromSigned(negativeOne));
}

TEST_F (ConvertTests, ExtractBytesForSomeNegativeValues) {
    EXPECT_EQ(0b11111111, getMsbFromSigned(-10));
    EXPECT_EQ(0b11110110, getLsbFromSigned(-10));
    EXPECT_EQ(0b10110001, getMsbFromSigned(-20000));
    EXPECT_EQ(0b11100000, getLsbFromSigned(-20000));
}

TEST_F (ConvertTests, ExtractBytesForSomePositiveValues) {
    EXPECT_EQ(0b00000000, getMsbFromSigned(10));
    EXPECT_EQ(0b00001010, getLsbFromSigned(10));
    EXPECT_EQ(0b01001110, getMsbFromSigned(20000));
    EXPECT_EQ(0b00100000, getLsbFromSigned(20000));
}

/*------------------- Test Bytes to Signed Int Conversions -------------------*/

TEST_F (ConvertTests, CombiningZerosResultsInZero) {
    EXPECT_EQ(0, toSignedInt(0b00000000, 0b00000000));
}

TEST_F (ConvertTests, CombiningSomeNegativeValues) {
    EXPECT_EQ(-10,    toSignedInt(0b11111111, 0b11110110));
    EXPECT_EQ(-20000, toSignedInt(0b10110001, 0b11100000));
}

TEST_F (ConvertTests, CombiningSomePositiveValues) {
    EXPECT_EQ(10,    toSignedInt(0b00000000, 0b00001010));
    EXPECT_EQ(20000, toSignedInt(0b01001110, 0b00100000));
}
