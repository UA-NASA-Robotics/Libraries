/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 3, 2018
 ******************************************************************************/
#ifndef CONVERT_H
#define CONVERT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*-------------------------- Bytes -> Signed 16-bit --------------------------*/

int16_t toSignedInt (uint8_t msb, uint8_t lsb);

/*------------------------- Bytes -> Unsigned 16-bit -------------------------*/

uint16_t toUnsignedInt (uint8_t msb, uint8_t lsb);

/*-------------------------- Signed 16-bit -> Bytes --------------------------*/

uint8_t getMsbFromSigned (int16_t value);
uint8_t getLsbFromSigned (int16_t value);

/*------------------------- Unsigned 16-bit -> Bytes -------------------------*/

uint8_t getMsbFromUnsigned (uint16_t value);
uint8_t getLsbFromUnsigned (uint16_t value);

#ifdef __cplusplus
};
#endif
#endif // CONVERT_H
