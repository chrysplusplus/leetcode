// File: 1122.c
//
// Given two arrays:
//   arr1 is an unsorted array
//   arr2 is a set of distinct numbers, all of which are elements of arr1
//
// Sort arr1 so that all elements appear in the same order as arr2, with any
// elements not in arr2 at the end of arr1 in ascending order
//
// Example:
// arr1 = 2, 3, 1, 3, 2, 4, 6, 7, 9, 2, 19
// arr2 = 2, 1, 4, 3, 9, 6
//
// Result:
// arr1 = 2, 2, 2, 1, 4, 3, 3, 9, 6, 7, 19

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"

typedef struct {
  int value;
  size_t count;
} RLENode;

typedef struct {
  RLENode *storage;
  size_t size, capacity;
} RunLengthEncoding;

RunLengthEncoding rle_create(size_t size) {
  RLENode *buffer = (RLENode *)malloc(size * sizeof (RLENode));
  assert(buffer != NULL);
  RunLengthEncoding rle = { buffer, 0, size };
  return rle;
}

void rle_destroy(RunLengthEncoding *rle) {
  free((void *)rle->storage);
  rle->storage = NULL;
  rle->size = 0;
  rle->capacity = 0;
}

void rle_extend(RunLengthEncoding *rle) {
  RLENode *buffer = (RLENode *)malloc(2 * rle->capacity * sizeof (RLENode));
  assert(buffer != NULL);
  memcpy((RLENode *)buffer, (RLENode *)rle->storage, rle->size);
  free((void *)rle->storage);
  rle->storage = buffer;
  rle->capacity = 2 * rle->capacity;
}

void rle_encode(RunLengthEncoding *rle, int *array, size_t size) {
  if (size == 0) return;

  int value = *array;
  size_t count = 1;
  for (int *p = array + 1; p != array + size; ++p) {
    if (*p == value) {
      ++count;
      continue;
    }

    if (rle->size == rle->capacity)
      rle_extend(rle);

    rle->storage[rle->size++] = (RLENode){ value, count };
    value = *p;
    count = 1;
  }

  if (rle->size == rle->capacity)
    rle_extend(rle);

  rle->storage[rle->size++] = (RLENode){ value, count };
}

RLENode *rle_search(RunLengthEncoding *rle, int value) {
  RLENode *pend = rle->storage + rle->size;
  for (RLENode *pnode = rle->storage; pnode != pend; ++pnode)
    if (pnode->value == value) return pnode;
  return NULL;
}

int main(void) {
  int values[] = { 2, 3, 1, 3, 2, 4, 6, 7, 9, 2, 19 };
  size_t values_size = sizeof values / sizeof (int);
  int priorities[] = { 2, 1, 4, 3, 9, 6 };
  size_t priorities_size = sizeof priorities / sizeof (int);

  printf("arr1:\t");
  array_print(values, values_size);
  printf("\narr2:\t");
  array_print(priorities, priorities_size);
  printf("\n");

  quicksort(values, values_size);

  RunLengthEncoding rle = rle_create(values_size);

  rle_encode(&rle, values, values_size);

  // inserting priorities into values array
  int *pvalue = values;
  for (size_t idx = 0; idx < priorities_size; ++idx) {
    RLENode *node = rle_search(&rle, priorities[idx]);
    assert(node != NULL);
    // decrement the count of prioritised values in rle
    do {*(pvalue++) = node->value; } while (--(node->count));
  }

  //printf("arr1:\t");
  //array_print(values, values_size);
  //printf("\narr2:\t");
  //array_print(priorities, priorities_size);
  //printf("\n");

  //// debug
  //size_t idx = 0;
  //for ( ; idx < rle.size-1; ++idx)
  //  printf("%d:%d, ", rle.storage[idx].value, rle.storage[idx].count);
  //printf("%d:%d\n", rle.storage[idx].value, rle.storage[idx].count);

  // appending non-prioritised values
  for (size_t idx = 0; idx < rle.size; ++idx) {
    if (rle.storage[idx].count != 0) {
      do {*(pvalue++) = rle.storage[idx].value; } while (--(rle.storage[idx].count));
    }
  }

  printf("Result:\t");
  array_print(values, values_size);
  printf("\n");

  rle_destroy(&rle);

  return 0;
}

