// 10.c
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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#define _RE_MATCH_EXPR_BUFFER_CAPACITY 1024
#define _RE_NODES_BUFFER_CAPACITY 32
#define _RE_STATES_BUFFER_CAPACITY 16
#define _RE_BACKTRACK_STACK_INITIAL_CAPACITY 8

void _assert(bool condition, const char *msg, const char *file, int line) {
  if (condition) return;
  printf("%s:%d: Assertion failed: %s", file, line, msg);
  exit(EXIT_FAILURE);
}

#define assert(_condition, _msg) (_assert((_condition), (_msg), __FILE__, __LINE__))

struct _REState;

struct _RENode {
  // in an alternate universe this would be a char buffer
  char match_char;
  struct _REState *next;
};

struct _REState {
  struct _RENode *nodes_begin, *nodes_end;
  bool is_success_state;
};

struct _RENodesBuffer {
  // buffer fragment that does not invalidate pointers when expanding
  struct _RENode storage[_RE_NODES_BUFFER_CAPACITY];
  struct _RENodesBuffer *next;
};

struct _REStatesBuffer {
  // buffer fragment that does not invalidate pointers when expanding
  struct _REState storage[_RE_STATES_BUFFER_CAPACITY];
  struct _REStatesBuffer *next;
};

struct RegularExpression {
  struct _RENodesBuffer *nodes_buf_head;
  struct _REStatesBuffer *states_buf_head;
  struct _REState *state_start;
};

bool _re_nodes_buf_must_grow(const struct _RENodesBuffer *buffer_tail, const struct _RENode *next) {
  return (next - buffer_tail->storage == _RE_NODES_BUFFER_CAPACITY);
}

bool _re_states_buf_must_grow(const struct _REStatesBuffer *buffer_tail, const struct _REState *next) {
  return (next - buffer_tail->storage == _RE_STATES_BUFFER_CAPACITY);
}

struct _RENodesBuffer *_re_nodes_buf_grow(struct _RENodesBuffer *buffer_tail) {
  assert(buffer_tail->next == NULL, "Can only grow buffer from tail");
  struct _RENodesBuffer *next = malloc(sizeof (struct _RENodesBuffer));
  assert(next != NULL, "Get more memory, forehead");
  buffer_tail->next = next;
  next->next = NULL;
  return next;
}

struct _REStatesBuffer *_re_states_buf_grow(struct _REStatesBuffer *buffer_tail) {
  assert(buffer_tail->next == NULL, "Can only grow buffer from tail");
  struct _REStatesBuffer *next = malloc(sizeof (struct _REStatesBuffer));
  assert(next != NULL, "Get more memory, forehead");
  buffer_tail->next = next;
  next->next = NULL;
  return next;
}

/* Compile for ab*c* currently yields an incorrect state machine
 * node for matching c attaches to the state after matching a because
 * the next state does not get set correctly
 */

/*  re_compile
 *  struct RegularExpression re_compile(const char * pattern)
 *    pattern : Null-terminating pattern string
 *    return : Regular expression structure
 *
 *  Compile a regular expression pattern into a data structure for matching
 *  against strings
 */
struct RegularExpression re_compile(const char *pattern) {
  // set up resources
  //   nodes buffer
  struct _RENodesBuffer *nodes_buf_head = malloc(sizeof (struct _RENodesBuffer));
  assert(nodes_buf_head != NULL, "Get more memory, forehead");
  nodes_buf_head->next = NULL;
  struct _RENodesBuffer *nodes_buf_tail = nodes_buf_head;
  struct _RENode *nodes_buf_next = nodes_buf_tail->storage;

  //   states buffer
  struct _REStatesBuffer *states_buf_head = malloc(sizeof (struct _REStatesBuffer));
  assert(states_buf_head != NULL, "Get more memory, forehead");
  states_buf_head->next = NULL;
  struct _REStatesBuffer *states_buf_tail = states_buf_head;
  struct _REState *states_buf_next = states_buf_tail->storage;

  // parse pattern
  struct _RENode *node_prev = NULL;
  struct _REState *state_prev = NULL;
  for (const char *pattern_char = pattern; *pattern_char != '\0'; ++pattern_char) {
    // assuming per specification that no invalid characters are possible within the pattern string
    if (*pattern_char == '*') {
      assert(state_prev != NULL, "Invalid state");
      assert(node_prev != NULL, "Invalid state");

      // modify previous node to transition to the same state
      node_prev->next = state_prev;

      // add zero-width matching node from previous state to current state
      // if we need to grow the buffer tree, then previous state needs to have its nodes reallocated
      // so that they are stored together in contiguous memory; in this basic system a state will
      // never have more than two match nodes, so I don't feel too bad about the "air gaps" that
      // this reallocation leaves behind
      if (_re_nodes_buf_must_grow(nodes_buf_tail, nodes_buf_next)) {
        nodes_buf_tail = _re_nodes_buf_grow(nodes_buf_tail);
        nodes_buf_next = nodes_buf_tail->storage;
        *nodes_buf_next = *node_prev; // memcpy
        state_prev->nodes_begin = nodes_buf_next++; // reassign state to new memory
      }

      // next node positioned in next available address
      *nodes_buf_next = (struct _RENode){ '\0', states_buf_next }; // '\0' for zero-width match
      state_prev->nodes_end = nodes_buf_next + 1;
      continue;
    }

    // we only need to check growth for the nodes buffer, since the next state needs to be valid
    // when it is incremented in the previous iteration, so will already by valid at this stage,
    // or, if there was no previous iteration, then the next state will still be valid since it
    // is the beginning of a freshly allocated buffer
    if (_re_nodes_buf_must_grow(nodes_buf_tail, nodes_buf_next)) {
      nodes_buf_tail = _re_nodes_buf_grow(nodes_buf_tail);
      nodes_buf_next = nodes_buf_tail->storage;
    }

    // assign pattern character to matching node as the only node on a new state
    *nodes_buf_next = (struct _RENode){ *pattern_char, NULL };
    *states_buf_next = (struct _REState){ nodes_buf_next, nodes_buf_next + 1, false };

    // push state, growing states buffer if needed
    state_prev = states_buf_next++;
    if (_re_states_buf_must_grow(states_buf_tail, states_buf_next)) {
      states_buf_tail = _re_states_buf_grow(states_buf_tail);
      states_buf_next = states_buf_tail->storage;
    }

    // set matching node transition pointer to new next state
    nodes_buf_next->next = states_buf_next;
    node_prev = nodes_buf_next++;
  }

  // assign start state and success states
  struct _REState *state_start = states_buf_head->storage;
  *states_buf_next = (struct _REState){ NULL, NULL, true }; // no need to check state since it guaranteed to be valid
  return (struct RegularExpression){ nodes_buf_head, states_buf_head, state_start };
}

/* re_does_pattern_match
 * Return True if the whole search string matches a compiled regular expression
 */
bool re_does_pattern_match(const struct RegularExpression *regex, const char *search_string) {
  const struct _REState *state = regex->state_start;
  const struct _RENode *node = state->nodes_begin;
  const char *remaining_search = search_string;

  struct MatchContext {
    const struct _REState *state;
    const struct _RENode *node;
    // more detailed implementation might store more information in the match frame
  };

  size_t backtrack_stack_capacity = _RE_BACKTRACK_STACK_INITIAL_CAPACITY;
  struct MatchContext *backtrack_stack = malloc(backtrack_stack_capacity * sizeof (struct MatchContext));
  assert(backtrack_stack != NULL, "Get more memory, forehead");
  struct MatchContext *backtrack_stack_next = backtrack_stack;

  while (*remaining_search != '\0' || (!state->is_success_state)) {
    // iterate through state nodes to find a matching node
    const struct _REState *next_state = NULL;
    for ( ; node != state->nodes_end; ++node) {
      // zero-width match
      if (node->match_char == '\0') {
        next_state = node->next;
        break;
      }

      // literal match
      if (*remaining_search == node->match_char) {
        next_state = node->next;
        ++remaining_search;
        break;
      }
    }

    // push match context to backtracking stack and then set next match context
    if (next_state != NULL) {
      *(backtrack_stack_next++) = (struct MatchContext){ state, node };
      state = next_state;
      node = state->nodes_begin;
    }

    // grow the backtrack stack if necessary
    if (backtrack_stack_next - backtrack_stack == backtrack_stack_capacity) {
      size_t new_capacity = backtrack_stack_capacity * 2;
      struct MatchContext *new_stack = malloc(new_capacity * sizeof (struct MatchContext));
      assert(new_stack != NULL, "Get more memory, forehead!");
      memmove(new_stack, backtrack_stack, backtrack_stack_capacity);
      free(backtrack_stack);
      backtrack_stack = new_stack;
      backtrack_stack_next = new_stack + backtrack_stack_capacity;
      backtrack_stack_capacity = new_capacity;
    }

    // fail if cannot backtrack anymore
    if (next_state == NULL && backtrack_stack_next - backtrack_stack == 0) {
      free(backtrack_stack);
      return false;
    }

    // backtrack if no match found
    if (next_state == NULL) {
      --backtrack_stack_next;
      state = backtrack_stack_next->state;
      node = backtrack_stack_next->node;
      if (node->match_char != '\0')
        --remaining_search;
      assert(search_string <= remaining_search, "Cannot backtrack to before the beginning of search string");
      ++node; // so that the same node isn't just matched again
    }
  }

  // free allocated memory and return success state
  free(backtrack_stack);
  return state->is_success_state;
}

/*  re_destroy
 *  void re_destroy(struct RegularExpression * re_pattern)
 *    re_pattern : regular expression data structure
 *
 *  Free resources used by regular expression data structure
 */
void re_destroy(struct RegularExpression *re_pattern) {
  if (re_pattern == NULL) return;
  // deallocate nodes buffer
  {
    struct _RENodesBuffer *head = re_pattern->nodes_buf_head;
    while (head != NULL) {
      struct _RENodesBuffer *next = head->next;
      free(head);
      head = next;
    }
    re_pattern->nodes_buf_head = NULL;
  }

  // deallocate states buffer
  {
    struct _REStatesBuffer *head = re_pattern->states_buf_head;
    while (head != NULL) {
      struct _REStatesBuffer *next = head->next;
      free(head);
      head = next;
    }
    re_pattern->states_buf_head = NULL;
  }
}

int main(int argc, char *argv[]) {
  char *pattern_str, *search_str;
  switch (argc) {
    case 0:
      exit(EXIT_FAILURE);
      break;
    case 1:
    case 2:
      printf("Usage: %s pattern search_string\n", argv[0]);
      exit(EXIT_FAILURE);
      break;
    case 3:
      pattern_str = argv[1];
      search_str = argv[2];
      break;
    default:
      printf("Error: Too many arguments provided");
      exit(EXIT_FAILURE);
      break;
  }

  struct RegularExpression pattern = re_compile(pattern_str);

  bool result = re_does_pattern_match(&pattern, search_str);
  printf("%s\n", result ? "String matches" : "String does not match");

  re_destroy(&pattern);
  return 0;
}
