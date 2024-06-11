/* File: main.c
 *
 * Given an integer array `nums` and an integer `k`, return the number of
 * non-empty subarrays that have a sum divisble by `k`.
 *
 * A subarray is a contiguous part of an array
 *
 * Example 1:
 *   Input: nums = [4,5,0,-2,-3,1]; k = 5
 *   Ouput: 7
 *     [4,5,0,-2,-3,1]
 *     [5]
 *     [5,0]
 *     [5,0,-2,-3]
 *     [0]
 *     [0,-2,-3]
 *     [-2,-3]
 *
 * Example 2:
 *   Input: nums = [5]; k = 9
 *   Output: 0
 */

#include <stdio.h>

int *array_end_sentinel(int *p_array, size_t size) {
  return p_array + size;
}

void print_subarray_match(int *p_start, int *p_end) {
  printf("[%d", *(p_start++));
  while (p_start != p_end)
    printf(", %d", *(p_start++));
  printf("]\n");
}

int main(void) {
  const size_t array_size = 6;
  int array[] = { 4, 5, 0, -2, -3, 1 };
  int divisor = 5;

  int *array_end = array_end_sentinel(array, array_size);

  int matches = 0;
  for (int *start = array; start != array_end; ++start) {
    int sum = 0;
    // end is a sentinel so it goes past last element by 1
    for (int *end = start + 1; end != array_end + 1; ++end) {
      sum += *(end - 1); // accumulated sum
      if (sum % divisor == 0) {
        ++matches;
        print_subarray_match(start, end);
      }
    }
  }

  printf("Total matches: %d", matches);
}

