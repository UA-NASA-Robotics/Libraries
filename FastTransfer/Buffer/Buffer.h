/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 3, 2018
 ******************************************************************************/
#ifndef BUFFER_H
#define BUFFER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct BufferRepresentation Buffer;

/**
 * @brief   Allocate memory for a ring buffer and return a pointer to the object
 * @param   capacity    Number of bytes that can fit in the ring buffer
 * @return  Returns a pointer to the ring buffer
 */
Buffer * createBuffer (size_t capacity);

/**
 * @brief   Deallocates dynamic memory used by ring buffer at the given address
 * @param   buffer      Address of the ring buffer whose memory will be freed
 */
void destroyBuffer (Buffer * buffer);

/**
 * @param   buffer  Address of a buffer object
 * @return  Number of elements in the given buffer
 */
size_t getSize (Buffer * buffer);

/**
 * @param   buffer  Address of a buffer object
 * @return  Returns true if number of elements in the buffer is equal to the
 *          buffer's capacity; otherwise, returns false.
 */
bool isFull (Buffer * buffer);

/**
 * @param   buffer  Address of a buffer object
 * @return  Returns true if there are no elements in the buffer; otherwise,
 *          returns false.
 */
bool isEmpty (Buffer * buffer);

/**
 * @brief   Adds an element to the end of the buffer.
 * @note    If the buffer is already full prior to the call to this function,
 *          then this function will have no effect on the buffer.
 * @param   buffer      Address of a buffer object
 * @param   element     Byte that will be added to end of buffer
 */
void push (Buffer * buffer, uint8_t element);

/**
 * @brief   Removes an element from the front of the buffer and returns the
 *          value of this element.
 * @pre     Buffer must not be empty prior to the call to this function.
 *          Undefined behavior results if this function passed a pointer to an
 *          empty buffer.
 * @return  Returns the value of the element that was at the front of the buffer
 */
uint8_t pop (Buffer * buffer);

/**
 * @brief   Provides access to the value of any element in the buffer.
 * @pre     Undefined behavior results if the offset is greater than or equal to
 *          the number of elements in the buffer.
 * @param   buffer      Address of a buffer object
 * @param   offset      Position of the desired element relative to the position
 *                      of the element at the front of the buffer. For example,
 *                      the offset for the element at the front is 0, and the
 *                      offset for the third element in the buffer is 2, etc.
 * @return  Value of the byte corresponding to the given offset
 */
uint8_t getValue (Buffer * buffer, size_t offset);

#ifdef __cplusplus
};
#endif
#endif // BUFFER_H
