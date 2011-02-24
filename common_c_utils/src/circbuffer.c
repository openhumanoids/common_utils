/*
 * circbuffer.c
 *
 *  Circular buffer of bytes, typically used for serial communication
 */

#include "circbuffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef MIN
#define MIN(a,b)((a < b) ? a : b)
#endif

int circ_buf_create(circular_buffer * cbuf, int size)
{
  //create buffer, and allocate space for size bytes

  cbuf->buf = (char *) malloc(size * sizeof(char));

  cbuf->readOffset = cbuf->writeOffset = cbuf->numBytes = 0;
  cbuf->maxSize = size;
  return 1;
}

int circ_buf_destroy(circular_buffer * cbuf)
{
  //destroy
  cbuf->maxSize = 0;
  free(cbuf->buf);
  return 1;
}

int circ_buf_read(circular_buffer * cbuf, int numBytes, char * buf)
{
  //read numBytes
  int bytes_read = circ_buf_peek(cbuf, numBytes, buf);

  //move readPtr
  cbuf->numBytes -= bytes_read;
  cbuf->readOffset = (cbuf->readOffset + bytes_read) % cbuf->maxSize;

  return bytes_read;

}

int circ_buf_write(circular_buffer * cbuf, int numBytes, char * buf)
{

  //check if there is enough space... maybe this should just wrap around??
  if (numBytes + cbuf->numBytes > cbuf->maxSize) {
    fprintf(stderr, "ERROR not enough space in circular buffer!!!!!!!!\n");
    fprintf(stderr, "Discarding data!!\n");
    numBytes = cbuf->maxSize - cbuf->numBytes;

  }
  //write to wrap around point.
  int bytes_written = MIN(cbuf->maxSize - cbuf->writeOffset, numBytes);
  memcpy(cbuf->buf + cbuf->writeOffset, buf, bytes_written * sizeof(char));
  numBytes -= bytes_written;

  //write the rest from start of buffer
  if (numBytes > 0) {
    memcpy(cbuf->buf, buf + bytes_written, numBytes * sizeof(char));
    bytes_written += numBytes;
  }

  //move writePtr
  cbuf->numBytes += bytes_written;
  cbuf->writeOffset = (cbuf->writeOffset + bytes_written) % cbuf->maxSize;

  return bytes_written;

}

int circ_buf_peek(circular_buffer * cbuf, int numBytes, char * buf)
{
  //read numBytes from start of buffer, but don't move readPtr
  if (numBytes > cbuf->numBytes || numBytes > cbuf->maxSize) {
    fprintf(stderr, "ERROR can't read that many bytes from the circular buffer! \n");
   return -1;
  }
  //read up to wrap around point
  int bytes_read = MIN(cbuf->maxSize - cbuf->readOffset, numBytes);
  memcpy(buf, cbuf->buf + cbuf->readOffset, bytes_read * sizeof(char));
  numBytes -= bytes_read;

  //read again from beginning if there are bytes left
  if (numBytes > 0) {
    memcpy(buf + bytes_read, cbuf->buf, numBytes * sizeof(char));
    bytes_read += numBytes;
  }
  return bytes_read;
}

int circ_buf_flush(circular_buffer * cbuf)
{
  //move pointers to "empty" the read buffer
  cbuf->readOffset = cbuf->writeOffset = cbuf->numBytes = 0;
  return 0;

}

void circ_buf_unit_test()
{
  char * testString = "iuerrlfkladbytes_writtenbytes_writte";
  char comp[1000];
  char * comp_p = comp;
  circular_buffer cbuf;
  circ_buf_create(&cbuf, 13);

  int numWritten = 0;
  int writeAmount = 0;
  int numRead = 0;
  int readAmount = 0;

  writeAmount = 4;
  circ_buf_write(&cbuf, writeAmount, testString + numWritten);
  numWritten += writeAmount;

  writeAmount = 6;
  circ_buf_write(&cbuf, writeAmount, testString + numWritten);
  numWritten += writeAmount;

  readAmount = 9;
  circ_buf_read(&cbuf, readAmount, comp_p + numRead);
  numRead += readAmount;

  writeAmount = 11;
  circ_buf_write(&cbuf, writeAmount, testString + numWritten);
  numWritten += writeAmount;

  readAmount = 12;
  circ_buf_read(&cbuf, readAmount, comp_p + numRead);
  numRead += readAmount;

  printf("at end, there are %d bytes left, should be %d \n", cbuf.numBytes, numWritten - numRead);

  if (strncmp(testString, comp, numRead) == 0)
    printf("WOOOHOO! the strings match :-)\n");
  else
    printf("BOOOO Somethings wrong");

  circ_buf_destroy(&cbuf);
}


