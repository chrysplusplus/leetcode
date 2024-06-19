// 10.c attempt 1
// Regular Expression Matching
//
// Given an input string s and a pattern p, implement regular expression
// matching with support for:
//   . matches a single character
//   * matches zero or more of the preceding element
//
// The matching should cover the entire input string
//
// Example:
//   s = "aa", p = "a"
//   Result: false

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  char *matching_expr;
  FANode *next;
} FANode;

typedef struct {
  FANode *storage;
  size_t size;
  bool is_success_state;
} FAState;

typedef struct {
  FAState *states;
  size_t states_size;
  const char *pattern;
} RegexPattern;

#define MAX_PATTERN_LENGTH 20

bool subarray_is_in_bounds(void *begin, void *end, void *sub_begin, void *sub_end) {
  assert(begin < end);
  assert(sub_begin < sub_end);
  return (begin <= sub_begin) && (begin <= sub_end) && (sub_begin < end) && (sub_end <= end);
}

RegexPattern re_compile(const char *pattern) {
  size_t state_buffer_capacity = MAX_PATTERN_LENGTH;
  FAState *state_buffer = (FAState *)malloc(state_buffer_capacity * sizeof (FAState));
  assert(state_buffer != NULL);

  FAState *state_buffer_head = state_buffer;
  const char *search_begin = pattern, *search_end = pattern;

  size_t pattern_length = strlen(pattern);
  const char *pattern_end = pattern + pattern_length;
}

void re_destroy(RegexPattern *pattern) {
  printf("re_destroy not implemented\n");
  exit(1);
}

int main(void) {
  printf("Hello World\n");
  return 0;
}
