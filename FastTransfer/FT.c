/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 17, 2018
 ******************************************************************************/
#include "FT.h"
#include "Buffer.h"
#include "Convert.h"
#include <stdlib.h>

#define HEADER_0 0x06
#define HEADER_1 0x85
#define HEADER_SIZE 5
#define SIZE_INDEX 4

/*----------------------------- Local Variables ------------------------------*/

static uint8_t  s_address;
static int16_t  s_array [ARRAY_SIZE];
static bool     s_flags [ARRAY_SIZE];
static void*    s_permissions [ARRAY_SIZE];
static uint8_t  s_packet [MX_PACKET_SIZE];
static uint8_t  s_packetIndex = 0;

/*----------------------------------- Port -----------------------------------*/

struct PortRepresentation
{
    PutFunc put;
    GetFunc get;
    EmptyFunc empty;
    Buffer * buffer;
};

Port * FT_NewPort(PutFunc put, GetFunc get, EmptyFunc empty) {
    // Allocate heap storage for port
    Port * port = (Port *)malloc(sizeof(Port));

    // Assign function pointers for rx/tx serial buffer access
    port->put = put;
    port->get = get;
    port->empty = empty;

    // allocate ring buffer for parsing
    port->buffer = createBuffer(MX_PACKET_SIZE);
    return port;
}

void FT_DestroyPort(Port *port) {
    destroyBuffer(port->buffer);
    free(port);
}

/*------------------------------ Initialization ------------------------------*/

void FT_Initialize(uint8_t address) {
    s_address = address;

    // initialize every value in array to zero
    for (uint16_t i = 0; i < ARRAY_SIZE; ++i) {
        s_array[i] = 0;
        s_flags[i] = false;
        s_permissions[i] = NULL;
    }
}

/*------------------------ Accessing the Local Array -------------------------*/

int16_t FT_ReadArray(uint16_t index) {
    s_flags[index] = false;
    return s_array[index];
}

bool FT_ReadFlag(uint16_t index) {
    return s_flags[index];
}

/*---------------------------- Receiving Packets -----------------------------*/


/// See "Parsing Helping Functions" section below for definitions
void getBytesFromUART (Port *);
void trashBytesBeforeHeader (Buffer *);
bool validHeader (Buffer *);
bool correctCRC (Buffer *);
uint8_t crc (uint8_t const *, uint8_t);
void writeData (Port *, size_t);

/**
 * @brief   Transfers contents of a receive buffer to the given port's ring
 *          buffer, then scans this buffer for valid packets. Transfers data
 *          from all valid packets to the local FastTransfer array.
 */
void FT_ScanPort(Port *port) {
    // Transfer bytes in receive buffer to the port's ring buffer.
    getBytesFromUART(port);

    // Scan for valid packets until the ring buffer is empty
    while (!isEmpty(port->buffer)) {
        trashBytesBeforeHeader(port->buffer);

        // Stop parsing if there are not enough bytes in the ring buffer to
        // determine whether or not there is a valid FastTransfer header.
        if (getSize(port->buffer) < HEADER_SIZE)
            return;

        if (validHeader(port->buffer)) {
            // Determine length of packet based on the information in header
            size_t packetSize = getValue(port->buffer, SIZE_INDEX) + HEADER_SIZE + 1;

            // Stop parsing if the ring buffer does not yet contain enough bytes
            if (getSize(port->buffer) < packetSize)
                return;
            
            // Verify the CRC code
            if (correctCRC(port->buffer)) {

                // Write the packet's data into the local FastTransfer array.
                writeData(port, packetSize);

                // Parsing is complete, so remove the packet from ring buffer.
                for (size_t i = 0; i < packetSize; ++i)
                    pop(port->buffer);
            }
            else {
                // Incorrect CRC, so trash the first byte and start looking for
                // the next potential packet.
                pop(port->buffer);
            }
        }
        else {
            // Invalid header, so trash the first byte and start looking for the
            // next potential packet.
            pop(port->buffer);
        }
    }
}

void FT_LimitAccess (uint16_t index, Port * port) {
    if (index < ARRAY_SIZE)
        s_permissions[index] = (void *)port;
}

/*------------------------- Parsing Helper Functions -------------------------*/

/**
 * @brief   Transfers all bytes out of the receive buffer and into the port's
 *          ring buffer.
 */
void getBytesFromUART (Port * port) {
    Buffer * buffer = port->buffer;
    while (!port->empty() && !isFull(buffer))
        push(buffer, port->get());
}

/**
 * @brief   Throws out all bytes in given buffer that come before the sentinel
 *          value that indicates the potential beginning of a FastTransfer
 *          packet.
 */
void trashBytesBeforeHeader (Buffer * buffer) {
    while (!isEmpty(buffer) && getValue(buffer, 0) != HEADER_0)
        pop(buffer);
}

/**
 * @brief   Evaluates whether or not the first 5 bytes in the given ring buffer
 *          constitute a valid header of a FastTransfer packet.
 * @note    Headers that are not addressed to the local FastTransfer node are
 *          not considered valid.
 */
bool validHeader (Buffer * buffer) {
    return (getSize(buffer) >= HEADER_SIZE)
        && (getValue(buffer, 0) == HEADER_0)
        && (getValue(buffer, 1) == HEADER_1)
        && (getValue(buffer, 3) == s_address)
        && (getValue(buffer, SIZE_INDEX) % 4 == 0)
        && (getValue(buffer, SIZE_INDEX) + HEADER_SIZE + 1 <= MX_PACKET_SIZE);
}

/**
 * @brief   Verifies that the ring buffer contains a FastTransfer packet with
 *          the correct CRC.
 * @pre     Function assumes that the first byte in the ring buffer is the
 *          first byte in the header of a FastTransfer packet
 */
bool correctCRC (Buffer * buffer) {
    static uint8_t arr [MX_PACKET_SIZE];
    uint8_t size = getValue(buffer, SIZE_INDEX);
    for (size_t i = 0; i < size; ++i)
        arr[i] = getValue(buffer, i + HEADER_SIZE);
    return crc(arr, size) == getValue(buffer, size + HEADER_SIZE);
}

/**
 * @brief   Calculates the CRC code for a packet.
 * 
 * @param   arr     Pointer to the first element in a FastTransfer packet's
 *                  data segment.
 * @param   size    Number of bytes in the packet's data segment.
 * 
 * @return  Returns the CRC code for the given packet.
 */
uint8_t crc (uint8_t const *arr, const uint8_t size) {
    const uint8_t POLYNOMIAL = 0x8C;
    uint8_t value = 0x00;
    for (uint8_t i = 0; i < size; ++i) {
        uint8_t data = arr[i];
        for (uint8_t j = 0; j < 8; ++j) {
            uint8_t sum = (value ^ data) & (uint8_t)0x01;
            value >>= 1;
            if (sum)
                value ^= POLYNOMIAL;
            data >>= 1;
        }
    }
    return value;
}

/**
 * @brief   Copies data from the ring buffer into the local FastTransfer node's
 *          array.
 * 
 * @pre     First byte in the given ring buffer is the first byte of a valid
 *          FastTransfer packet that is addressed to the local FastTransfer node
 */
void writeData (Port * port, size_t packetSize) {
    Buffer * buffer = port->buffer;
    static uint8_t frame [4];
    for (size_t i = HEADER_SIZE; i < packetSize - 1; i += 4) {
        frame[0] = getValue(buffer, i);
        frame[1] = getValue(buffer, i + 1);
        frame[2] = getValue(buffer, i + 2);
        frame[3] = getValue(buffer, i + 3);
        uint16_t index = toUnsignedInt(frame[0], frame[1]);
        int16_t data = toSignedInt(frame[2], frame[3]);
        if (index < ARRAY_SIZE) {
            // Only write the data if the given port has write permissions for
            // the given index
            if (s_permissions[index] == 0 || s_permissions[index] == (void *)port) {
                s_array[index] = data;
                s_flags[index] = true;
            }
        }
    }
}

/*--------------------------- Transmitting Packets ---------------------------*/

// Loading the fasttransfer packets
void FT_ToSend(uint16_t index, int16_t data) {
    s_packet[s_packetIndex++] = getMsbFromUnsigned(index);
    s_packet[s_packetIndex++] = getLsbFromUnsigned(index);
    s_packet[s_packetIndex++] = getMsbFromSigned(data);
    s_packet[s_packetIndex++] = getLsbFromSigned(data);
}

void FT_ClearPacket() {
    s_packetIndex = 0;
}
// Sending the FastTransfer Packets
void FT_Send(Port *port, uint8_t address) {
    port->put(HEADER_0);
    port->put(HEADER_1);
    port->put(s_address);
    port->put(address);
    port->put(s_packetIndex);
    for (int i = 0; i < s_packetIndex; ++i)
        port->put(s_packet[i]);
    port->put(crc(s_packet, s_packetIndex));
    FT_ClearPacket();
}
