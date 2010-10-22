/*
 * circbuffer.h
 *
 *  Circular buffer of bytes, typically used for serial communication
 */

#ifndef __circbuf_h__
#define __circbuf_h__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char * buf;
	int readOffset;
	int writeOffset;
	int numBytes;
	int maxSize;
} circular_buffer;

int circ_buf_create(circular_buffer * cbuf, int size); //create buffer, and allocate space for size bytes
int circ_buf_destroy(circular_buffer * cbuf); //destroy
int circ_buf_read(circular_buffer * cbuf, int numBytes, char * buf); //read numBytes from start of buffer, and move readPtr
int circ_buf_write(circular_buffer * cbuf, int numBytes, char * buf); //write numBytes to end of buffer
int circ_buf_peek(circular_buffer * cbuf, int numBytes, char * buf); //read numBytes from start of buffer, but don't move readPtr
int circ_buf_flush(circular_buffer * cbuf); //move pointers to "empty" the read buffer
static inline int circ_buf_available(circular_buffer * cbuf) {
	return cbuf->numBytes;//returns the number of bytes available
}
void circ_buf_unit_test();

#ifdef __cplusplus
}
#endif

#endif
