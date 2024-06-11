// File: 1051.c
// Height Checker
//
// A school is trying to take an annual photo of all the students. The students
// are asked to stand in a single-file lne in non-decreasing order by height.
// Let this ordering be represented by the integer array `expected` where
// `expected[i]` is the expected height of the ith student in line.
//
// You are given an integer array `heights` representing the current order that
// the students are standing in. Each `heights[i]` is the height of the ith
// student in line (0-indexed).
//
// Return the number of indices where `heights[i] != expected[i]`.
//
// Example 1:
//   Input: heights = [1,1,4,2,1,3]
//   Output: 3 (elems 2, 4 and 5)
// ...

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

void print_int_array(int *array, size_t size);

void quicksort(int *pbegin, int *pend) {
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
  quicksort(pbegin, right + 1);
  quicksort(right + 1, pend);
}

void print_int_array(int *array, size_t size) {
  int *p = array;
  printf("[%d", *(p++));
  while (p != array + size)
    printf(", %d", *(p++));
  printf("]");
}

int main(void) {
  int array[] = { 1, 1, 4, 2, 1, 3 };
  size_t array_size = 6;

  // copy into sorted array and sort
  int sorted_array[6];
  for (size_t idx = 0; idx < array_size; ++idx)
    sorted_array[idx] = array[idx];

  quicksort(sorted_array, sorted_array + array_size);

  int number_of_mismatches = 0;
  for (size_t idx = 0; idx < array_size; ++idx)
    if (sorted_array[idx] != array[idx])
      ++number_of_mismatches;

  printf("Heights:\t");
  print_int_array(array, array_size);
  printf("\nExpected:\t");
  print_int_array(sorted_array, array_size);
  printf("\nThere are %d mismatches\n", number_of_mismatches);

  return 0;
}
