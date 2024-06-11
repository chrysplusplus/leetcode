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
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// from 1051.c
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

// interface function
void quicksort(int *array, size_t size) {
  _quicksort(array, array + size);
}

typedef struct {
  int key;
  int *pointer;
} IntArrayLookupNode;

typedef struct {
  IntArrayLookupNode *pbegin;
  size_t size, capacity;
} IntArrayLookup;

#define LOOKUP_INITIAL_CAPACITY 16

IntArrayLookup int_array_lookup_create() {
  void *buffer = malloc(LOOKUP_INITIAL_CAPACITY * sizeof (IntArrayLookupNode));
  assert(buffer != NULL);
  IntArrayLookup lookup = { (IntArrayLookupNode *)buffer, 0, LOOKUP_INITIAL_CAPACITY };
  return lookup;
}

void int_array_lookup_destroy(IntArrayLookup *lookup) {
  free((void *)lookup->pbegin);
  lookup->pbegin = NULL;
  lookup->size = 0;
  lookup->capacity = 0;
}

void int_array_lookup_extend(IntArrayLookup *lookup) {
  lookup->capacity = lookup->capacity * 2;
  void *buffer = malloc(lookup->capacity * sizeof (IntArrayLookupNode));
  assert(buffer != NULL);
  memcpy(buffer, (void *)lookup->pbegin, lookup->size);
  free((void *)lookup->pbegin);
  lookup->pbegin = (IntArrayLookupNode *)buffer;
}

void int_array_lookup_add_lookup(IntArrayLookup *lookup, int key, int *pointer) {
  if (lookup->size == lookup->capacity)
    int_array_lookup_extend(lookup);

  lookup->pbegin[lookup->size] = (IntArrayLookupNode){ key, pointer };
  lookup->size++;
}

int *int_array_lookup_get(const IntArrayLookup lookup, const int key) {
  for (size_t idx = 0; idx < lookup.size; ++idx)
    if (lookup.pbegin[idx].key == key)
      return lookup.pbegin[idx].pointer;

  return NULL;
}

int *int_array_binary_search(int *array, size_t size, int value) {
  if (size == 0) return NULL;

  int *begin = array;
  int *end = array + size;
  int *last_search = NULL;
  int *search = begin + size/2;

  while (search != last_search) {
    if (*search < value)
      begin = search;
    else if (*search == value)
      return search;
    else
      end = search;

    last_search = search;
    search = begin + (end - begin)/2;
  }

  return NULL;
}

void print_array(int *array, size_t size) {
  printf("[");
  size_t idx = 0;
  for ( ; idx < size - 1; ++idx)
    printf("%d, ", array[idx]);
  printf("%d]", array[idx]);
}

int main(void) {
  int arr1[] = { 2, 3, 1, 3, 2, 4, 6, 7, 9, 2, 19 };
  size_t arr1_size = sizeof arr1 / sizeof (int);

  printf("arr1:\t");
  print_array(arr1, arr1_size);
  printf("\n");

  int arr2[] = { 2, 1, 4, 3, 9, 6 };
  size_t arr2_size = sizeof arr2 / sizeof (int);

  printf("arr2:\t");
  print_array(arr2, arr2_size);
  printf("\n");

  quicksort(arr1, arr1_size);

  {
    int *priority = (int *)malloc(arr1_size * sizeof (int));
    int *normal = (int *)malloc(arr1_size * sizeof (int));

    int *ptr_arr1 = arr1;
    int *ptr_arr2 = arr2;
    int *ppriority = priority;
    int *pnormal = normal;
    for ( ; ptr_arr2 != arr2 + arr2_size; ++ptr_arr2) {
      // skip elements not included
      while (*ptr_arr1 < *ptr_arr2) {
        assert(ptr_arr1 != arr1 + arr1_size);
        *(pnormal++) = *(ptr_arr1++);
      }

      // prioritise included elements
      while (*ptr_arr1 == *ptr_arr2) {
        assert(ptr_arr1 != arr1 + arr1_size);
        *(ppriority++) = *(ptr_arr1++);
      }
    }

    // combine priority and normal buffers
    ptr_arr1 = arr1;
    for (int *i = priority; i != ppriority; ++i) {
      assert(ptr_arr1 != arr1 + arr1_size);
      *(ptr_arr1++) = *i;
    }
    for (int *i = normal; i != pnormal; ++i) {
      assert(ptr_arr1 != arr1 + arr1_size);
      *(ptr_arr1++) = *i;
    }

    free((void *)priority);
    free((void *)normal);
  }

  printf("Result:\t");
  print_array(arr1, arr1_size);
  printf("\n");

  return 0;
}
