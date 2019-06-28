/*******************************************************************************
 * @author      Sean Collins
 * Contact:     sgc29@zips.uakron.edu
 * @date        July 3, 2018
 ******************************************************************************/
#include "Buffer.h"
#include <assert.h>
#include <stdlib.h>

struct BufferRepresentation {
    uint8_t * array;
    size_t    capacity;
    size_t    size;
    size_t    head;
    size_t    tail;
};

Buffer * createBuffer (size_t capacity) {
    Buffer * buffer = (Buffer *)malloc(sizeof(Buffer));
    buffer->array = (uint8_t *)malloc(capacity);
    buffer->capacity = capacity;
    buffer->head = buffer->tail = buffer->size = 0;
    return buffer;
}

void destroyBuffer (Buffer * buffer) {
    free(buffer->array);
    free(buffer);
    buffer = NULL;
}

size_t getSize (Buffer * b) {
    return b->size;
}

bool isFull (Buffer * b) {
    return getSize(b) == b->capacity;
}

bool isEmpty (Buffer * b) {
    return getSize(b) == 0;
}

void push (Buffer * b, uint8_t element) {
    if (isFull(b))
        return;
    b->array[b->tail++] = element;
    ++b->size;
    if (b->tail == b->capacity)
        b->tail = 0;
}

uint8_t pop (Buffer * b) {
    assert(!isEmpty(b));
    --b->size;
    uint8_t * value = &b->array[b->head++];
    if (b->head == b->capacity)
        b->head = 0;
    return *value;
}

uint8_t getValue (Buffer * b, size_t i) {
    assert(i < getSize(b));
    i += b->head;
    i = (i < b->capacity) ? i : i - b->capacity;
    return b->array[i];
}
