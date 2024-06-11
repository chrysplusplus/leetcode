// utility.h
// Useful functions

#ifndef UTILITY_H
#define UTILITY_H

#include <stddef.h>
#include <assert.h>

void _quicksort(int *pbegin, int *pend) {
  assert(pbegin <= pend);
  if (pend - pbegin < 2)
    // nothing to do
    return;

  // partition array
  int pivot = *pbegin;
  int *left = pbegin - 1;
  int *right = pend;

  // Hoare partition scheme
  while (left < right) {
    do {++left;} while (*left < pivot);
    do {--right;} while (*right > pivot);

    if (left >= right) break;
    int swap = *left;
    *left = *right;
    *right = swap;
  }

  // including right pointer in left partition avoids recursive loop
  _quicksort(pbegin, right + 1);
  _quicksort(right + 1, pend);
}

void quicksort(int *array, size_t size) {
  _quicksort(array, array + size);
}

void array_print(int *array, size_t size) {
  printf("[");
  int *plast = array + size - 1;
  for (int *i = array; i != plast; ++i)
    printf("%d, ", *i);
  printf("%d]", *plast);
}

#endif
