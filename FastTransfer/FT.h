/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 17, 2018
 ******************************************************************************/
#ifndef FT_H
#define FT_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>

/*------------------------ FastTransfer Configuration ------------------------*/

/// Change these constants to adjust the library to your needs/limitations.

/**
 * @brief   Specifies the number of elements in the FastTransfer array. Since
 *          the array indices are unsigned, 16-bit integers, the theoretical
 *          maximum is 2^16 = 65536. Prefer a smaller array size in order to
 *          conserve memory.
 */
#define ARRAY_SIZE 200 

/**
 * @brief   Specifies the maximum number of bytes that can be transmitted in
 *          a single packet, including the overhead associated with the packet's
 *          header and CRC code. Smallest possible packet is 10 bytes.
 */
#define MX_PACKET_SIZE 100

/*----------------------------------- Port -----------------------------------*/

/**
 * @brief   The Port type serves as an interface between the FastTransfer
 *          library and a pair of serial buffers (the transmit buffer and the
 *          receive buffer). Each Port bundles together a set of function
 *          pointers, which provide write access to the transmit buffer and read
 *          access to the receive buffer. Clients create a Port by passing these
 *          function pointers to the "createPort" function.
 */
typedef struct PortRepresentation Port;

/// Provided these aliases in order to de-clutter the function prototypes below.
typedef void (*PutFunc) (uint8_t);
typedef uint8_t (*GetFunc) ();
typedef bool (*EmptyFunc) ();

/**
 * @brief   Allocates heap storage for a new Port object and assigns function
 *          pointers for accessing a transmit buffer and a receive buffer.
 * 
 * @param   putc    Pointer to func. that writes 1 byte to transmit buffer
 * @param   getc    Pointer to func. that reads 1 byte from receive buffer
 * @param   empty   Pointer to func. that returns true if and only if the
 *                  receive buffer does not contain any more bytes at this time.
 * 
 * @return Port*    Pointer to a new Port object.
 */
Port *
FT_NewPort (PutFunc putc, GetFunc getc, EmptyFunc empty);

/**
 * @brief   Deallocate heap storage for a Port object located at given address
 */
void
FT_DestroyPort (Port *port);

/*------------------------------ Initialization ------------------------------*/

/**
 * @brief   Initializes the local FastTransfer node by assigning it an address
 *          and initializing all of the values in the FastTransfer array to 0.
 */
void
FT_Initialize (uint8_t address);

/*------------------------ Accessing the Local Array -------------------------*/

/**
 * @brief   Reads the data from the given position in the local FastTransfer
 *          node's array.
 * 
 * @param   index   A position in the local FastTransfer array.
 */
int16_t
FT_ReadArray (uint16_t index);

/**
 * @brief   Queries whether or not the value at the given index in the local
 *          FastTransfer array has been modified since the last read.
 * 
 * @return  Returns true iff the value has been modified since last read.
 */
bool
FT_ReadFlag (uint16_t index);

/*---------------------------- Receiving Packets -----------------------------*/

/**
 * @brief   By default, the "parse" function will write to any position in the
 *          local FastTransfer array if it receives a packet with data addressed
 *          to that position. This function is provided in order to restrict
 *          write access, so that only packets from the given Port will be able
 *          to modify the value at the given index.
 * 
 * @note    Use this function in a for-loop to restrict access to a range of
 *          indices.
 * 
 * @param   index   A position in the local FastTransfer array.
 * @param   port    Pointer to the Port that will have exclusive write
 *                  permissions to the given index.
 */
void
FT_LimitAccess (uint16_t index, Port *port);

/**
 * @brief   Accesses a receive buffer (via the given Port), scanning for any
 *          valid FastTransfer packets that are addressed to the local node.
 *          Writes data from such packets to the local FastTransfer array.
 */
void
FT_ScanPort (Port *port);

/*--------------------------- Transmitting Packets ---------------------------*/

/**
 * @brief   Adds another frame to the next packet to be transmitted. The frame
 *          consists of an unsigned 16-bit index followed by a signed 16-bit
 *          data value. Only one packet can be assembled at a time.
 * 
 * @param   index   Index that will be written to in the receivers array.
 * @param   data    Value that will be written.
 */
void
FT_ToSend (uint16_t index, int16_t data);

/**
 * @brief   Clears the contents of the next packet to be transmitted.
 */
void
FT_ClearPacket ();

/**
 * @brief   Transmits the packet by writing each byte to a transmit buffer.
 * 
 * @post    Automatically clears the packet buffer so that the client can
 *          begin to assemble the next packet.
 * 
 * @param   port        Port that contains a function pointer with write access
 *                      to a serial transmit buffer.
 * @param   address     The address of the FastTransfer node for which this
 *                      package intended.
 */
void
FT_Send (Port * port, uint8_t address);

#ifdef __cplusplus
};
#endif
#endif // FT_H
