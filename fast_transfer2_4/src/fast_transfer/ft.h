#ifndef FT_H
#define FT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "ft_config.h"

typedef struct FastTransferHandle FT_t;
  ///--------------------------------------------------------------------------
  /// FT_t is an alias for a type representing a single fast-transfer handle.
  ///   - "FastTransferHandle" is defined in the implementation file
  ///--------------------------------------------------------------------------

FT_t * FT_Create (uint8_t address,
                  void(*f_put)(uint8_t), uint8_t(*f_get)(), bool(*f_empty)());
  ///--------------------------------------------------------------------------
  /// Allocates heap memory for a fast-transfer instance and returns a handle
  /// to the instance.
  /// 
  /// Parameters:
  ///   - f_put:   Callback function that adds a byte to a transmit buffer.
  ///   - f_get:   Callback function that reads a byte from a receive buffer.
  ///   - f_empty: Callback function that returns true iff receive buffer
  ///              contains 0 bytes.
  ///--------------------------------------------------------------------------

void FT_Destroy (FT_t * handle);
  ///--------------------------------------------------------------------------
  /// Releases resources associated with given fast-transfer handle.
  ///--------------------------------------------------------------------------

int16_t FT_Read (FT_t * handle, uint8_t index);
  ///--------------------------------------------------------------------------
  /// Read a single value from the local fast-transfer array.
  /// As a side-effect, this function unsets the modification flag for the
  /// element at the given index.
  ///--------------------------------------------------------------------------

bool FT_Modified (FT_t * handle, uint8_t index);
  ///--------------------------------------------------------------------------
  /// Read the modification flag, which is TRUE if and only if the data at the
  /// given index has not already been read.
  ///--------------------------------------------------------------------------

void FT_Receive (FT_t * handle);
  ///--------------------------------------------------------------------------
  /// Reads the bytes from the serial buffer, parses them, and updates the
  /// local array if any valid messages were received.
  ///--------------------------------------------------------------------------

void FT_ToSend (FT_t * handle, uint8_t index, int16_t data);
  ///--------------------------------------------------------------------------
  /// Adds the given data to the given handle's next packet to be transmitted.
  ///--------------------------------------------------------------------------

void FT_Send (FT_t * handle, uint8_t address);
  ///--------------------------------------------------------------------------
  /// Transmits the handle's packet to the given address.
  ///--------------------------------------------------------------------------

#ifdef __cplusplus
};
#endif
#endif // FT_H
