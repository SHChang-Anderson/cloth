#ifndef ARRAY_H
#define ARRAY_H

typedef struct array {
  void **element;
  long size;
  long index;
} Array;

Array* arrayInitialize(long size);

void arrayInsert(Array* a, void* data);

void* arrayGet(Array* a,long i);

long arrayLen(Array* a);

void arrayReverse(Array* a);

#endif
