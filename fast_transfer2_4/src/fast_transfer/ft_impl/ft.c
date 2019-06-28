#include "../ft.h"
#include "buffer.h"
#include "crc.h"
#include "parser.h"
#include <stdlib.h>

/*
 * Type to serve as a handle to FastTransfer data.
 */

struct FastTransferHandle {

  // local data:
  uint8_t  address;
  uint16_t array [ARRAY_SZ];
  bool     flags [ARRAY_SZ];

  // buffers:
  Buffer_t receive_buf;
  Buffer_t transmit_buf;

  // callbacks (for interacting with serial communication hardware):
  void(*put)(uint8_t);
  uint8_t(*get)();
  bool(*empty)();

};

/*
 * Functions for interacting with FastTransfer instances.
 */

FT_t * FT_Create (uint8_t address, void(*put)(uint8_t), uint8_t(*get)(), bool(*empty)())
{
  FT_t * handle = (FT_t*)malloc(sizeof(FT_t));

  // initialize local data:
  handle->address = address;

  int i;
  for (i = 0; i < ARRAY_SZ; ++i) {
    handle->array[i] = 0;
    handle->flags[i] = true;
  }

  // initialize callback function members:
  handle->put = put;
  handle->get = get;
  handle->empty = empty;

  // initialize receive/transmit buffers:
  Buffer_init(&handle->receive_buf);
  Buffer_init(&handle->transmit_buf);

  return handle;
}

int16_t FT_Read (FT_t * handle, uint8_t index)
{
  handle->flags[index] = false;
  return (int16_t)(handle->array[index]);
}

bool FT_Modified (FT_t * handle, uint8_t index)
{
  return handle->flags[index];
}

void FT_Receive (FT_t * handle)
{
  // transfer bytes from serial buffer into local buffer
  while (!handle->empty() && !Buffer_full(&handle->receive_buf))
    Buffer_push(&handle->receive_buf, handle->get());

  // parse/extract messages from local buffer and update local array
  parse(&handle->receive_buf, handle->array, handle->flags, ARRAY_SZ, handle->address);
}


void FT_ToSend (FT_t * handle, uint8_t index, int16_t data)
{
  uint16_t combined = (uint16_t)data;
  uint8_t msb = (combined >> 8);
  uint8_t lsb = combined & 0x00FF;
  Buffer_push(&handle->transmit_buf, index);
  Buffer_push(&handle->transmit_buf, msb);
  Buffer_push(&handle->transmit_buf, lsb);
}

void FT_Send (FT_t * handle, uint8_t address)
{
    size_t payload_sz = Buffer_size(&handle->transmit_buf);

    // abandon send if there is no payload
    if (payload_sz == 0)
        return;

    // populate header
    handle->put(0x06);
    handle->put(0x85);
    handle->put(handle->address);
    handle->put(address);
    handle->put(payload_sz);

    // add the payload
    uint8_t payload [MAX_PCKT_SZ];
    int i;
    for (i = 0; i < payload_sz; ++i)
    {
        uint8_t next_byte = Buffer_pop(&handle->transmit_buf);
        payload[i] = next_byte;
        handle->put(next_byte);
    }

    // add the CRC code
    handle->put(crc(payload, payload_sz));
}

