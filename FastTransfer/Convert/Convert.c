/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 3, 2018
 ******************************************************************************/
#include "Convert.h"

/*------------------------------- Helper Union -------------------------------*/

static union {
    int16_t signedInt;
    uint16_t unsignedInt;
    struct {
        uint8_t lsb;
        uint8_t msb;
    };
} converter;

/*-------------------------- Bytes -> Signed 16-bit --------------------------*/

int16_t toSignedInt (uint8_t msb, uint8_t lsb) {
    converter.msb = msb;
    converter.lsb = lsb;
    return converter.signedInt;
}

/*------------------------- Bytes -> Unsigned 16-bit -------------------------*/

uint16_t toUnsignedInt (uint8_t msb, uint8_t lsb) {
    converter.msb = msb;
    converter.lsb = lsb;
    return converter.unsignedInt;
}

/*-------------------------- Signed 16-bit -> Bytes --------------------------*/

uint8_t getMsbFromSigned (int16_t n) {
    converter.signedInt = n;
    return converter.msb;
}

uint8_t getLsbFromSigned (int16_t n) {
    converter.signedInt = n;
    return converter.lsb;
}

/*------------------------- Unsigned 16-bit -> Bytes -------------------------*/

uint8_t getMsbFromUnsigned (uint16_t n) {
    converter.unsignedInt = n;
    return converter.msb;
}

uint8_t getLsbFromUnsigned (uint16_t n) {
    converter.unsignedInt = n;
    return converter.lsb;
}
