/*
 * File: char_queue.c
 * Author: Ted Salmon <tass2001@gmail.com>
 * Description:
 *     Implement a FIFO queue to store bytes read from UART into
 */
#include "char_queue.h"

/**
 * CharQueueInit()
 *     Description:
 *         Returns a fresh CharQueue_t object to the caller
 *     Params:
 *         None
 *     Returns:
 *         volatile CharQueue_t *
 */
CharQueue_t CharQueueInit()
{
    CharQueue_t queue;
    // Initialize size and cursors
    CharQueueReset(&queue);
    return queue;
}

/**
 * CharQueueAdd()
 *     Description:
 *         Adds a byte to the queue. If the queue is full, the byte is discarded.
 *     Params:
 *         volatile CharQueue_t *queue - The queue
 *         const unsigned char value - The value to add
 *     Returns:
 *         None
 */
void CharQueueAdd(volatile volatile CharQueue_t *queue, const unsigned char value)
{
    if (queue->size < CHAR_QUEUE_SIZE) {
        if (queue->writeCursor >= CHAR_QUEUE_SIZE) {
            queue->writeCursor = 0;
        }
        queue->data[queue->writeCursor] = value;
        queue->writeCursor++;
        queue->size++;
    }
}

/**
 * CharQueueGet()
 *     Description:
 *         Returns the byte at location idx. Returning the byte does not remove
 *         it from the queue.
 *     Params:
 *         CharQueue_t queue - The queue
 *         uint16_t idx - The index to return data for
 *     Returns:
 *         unsigned char
 */
unsigned char CharQueueGet(volatile CharQueue_t *queue, uint16_t idx)
{
    if (idx >= CHAR_QUEUE_SIZE) {
        return 0x00;
    }
    return queue->data[idx];
}

/**
 * CharQueueGetOffset()
 *     Description:
 *         Returns the byte at the location n steps away from the current index
 *     Params:
 *         CharQueue_t queue - The queue
 *         uint16_t idx - The index to return data for
 *     Returns:
 *         unsigned char
 */
unsigned char CharQueueGetOffset(volatile CharQueue_t *queue, uint16_t offset)
{
    if (offset > queue->size) {
        return 0x00;
    }
    uint16_t offsetCursor = queue->readCursor;
    while (offset > 0) {
        offsetCursor++;
        if (offsetCursor >= CHAR_QUEUE_SIZE) {
            offsetCursor = 0;
        }
        offset--;
    }
    return queue->data[offsetCursor];
}

/**
 * CharQueueNext()
 *     Description:
 *         Shifts the next byte in the queue out, as seen by the read cursor.
 *         Once the byte is returned, it should be considered destroyed from the
 *         queue.
 *     Params:
 *         CharQueue_t queue - The queue
 *     Returns:
 *         unsigned char
 */
unsigned char CharQueueNext(volatile CharQueue_t *queue)
{
    unsigned char data = queue->data[queue->readCursor];
    // Remove the byte from memory
    queue->data[queue->readCursor] = 0x00;
    queue->readCursor++;
    if (queue->readCursor >= CHAR_QUEUE_SIZE) {
        queue->readCursor = 0;
    }
    if (queue->size > 0) {
        queue->size--;
    }
    return data;
}

/**
 * CharQueueRemoveLast()
 *     Description:
 *         Remove the last byte in the buffer
 *     Params:
 *         CharQueue_t queue - The queue
 *     Returns:
 *         void
 */
void CharQueueRemoveLast(volatile CharQueue_t *queue)
{
    if (queue->size > 0) {
        queue->size--;
        queue->data[queue->writeCursor] = 0x00;
        if (queue->writeCursor == 0) {
            queue->writeCursor = CHAR_QUEUE_SIZE - 1;
        } else {
            queue->writeCursor--;
        }
    }
}

/**
 * CharQueueReset()
 *     Description:
 *         Empty a char queue
 *     Params:
 *         CharQueue_t queue - The queue
 *     Returns:
 *         void
 */
void CharQueueReset(volatile CharQueue_t *queue)
{
    queue->size = 0;
    queue->readCursor = 0;
    queue->writeCursor = 0;
    memset((void *) queue->data, 0, CHAR_QUEUE_SIZE);
}

/**
 * CharQueueSeek()
 *     Description:
 *         Checks if a given byte is in the queue and return the length of
 *         characters prior to it.
 *     Params:
 *         volatile CharQueue_t *queue - The queue
 *         const unsigned char needle - The character to look for
 *     Returns:
 *         uint16_t - The length of characters prior to the needle or zero if
 *                   the needle wasn't found
 */
uint16_t CharQueueSeek(volatile CharQueue_t *queue, const unsigned char needle)
{
    uint16_t readCursor = queue->readCursor;
    uint16_t size = queue->size + 1;
    uint16_t cnt = 1;
    while (size > 0) {
        if (queue->data[readCursor] == needle) {
            return cnt;
        }
        readCursor++;
        if (readCursor >= CHAR_QUEUE_SIZE) {
            readCursor = 0;
        }
        cnt++;
        size--;
    }
    return 0;
}
