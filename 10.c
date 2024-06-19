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
#include <assert.h>

#define _RE_MATCH_EXPR_BUFFER_CAPACITY 1024
#define _RE_NODES_BUFFER_CAPACITY 32
#define _RE_STATES_BUFFER_CAPACITY 16

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
  // dynamic buffer, allows reallocation
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
  assert(((void)"Get more memory, forehead", nodes_buf_head != NULL));
  nodes_buf_head->next = NULL;
  struct _RENodesBuffer *nodes_buf_tail = nodes_buf_head;
  struct _RENode *nodes_buf_next = nodes_buf_tail->storage;

  //   states buffer
  struct _REStatesBuffer *states_buf_head = malloc(sizeof (struct _REStatesBuffer));
  assert(((void)"Get more memory, forehead", states_buf_head != NULL));
  states_buf_head->next = NULL;
  struct _REStatesBuffer *states_buf_tail = states_buf_head;
  struct _REState *states_buf_next = states_buf_tail->storage;

  // parse pattern
  struct _RENode *node_prev = NULL;
  struct _REState *state_prev = NULL;
  for (const char *pattern_char = pattern; *pattern_char != '\0'; ++pattern_char) {
    // assuming per specification that no invalid characters are possible within the pattern string
    if (*pattern_char == '*') {
      assert(state_prev != NULL);
      assert(node_prev != NULL);

      // modify previous node to transition to the same state
      node_prev->next = state_prev;

      // add zero-width matching node from previous state to current state
      // since this is only a basic implementation, there have been no other nodes added since the
      // previous state was parsed, so we can just append to the end of the nodes buffer and
      // increment the range sentinel on the state
      assert(((void)"Node buffer grow not implementd", nodes_buf_next - nodes_buf_tail->storage != _RE_NODES_BUFFER_CAPACITY)); // TODO now a function
      *nodes_buf_next = (struct _RENode){ '\0', states_buf_next }; // '\0' for zero-width match
      assert(nodes_buf_next - node_prev == 1); // for my own sanity
      state_prev->nodes_end = nodes_buf_next + 1;
      continue;
    }

    // we only need to check growth for the nodes buffer, since the next state needs to be valid
    // when it is incremented in the previous iteration, so will already by valid at this stage,
    // or, if there was no previous iteration, then the next state will still be valid since it
    // is the beginning of a freshly allocated buffer
    assert(((void)"Node buffer grow not implemented", nodes_buf_next - nodes_buf_tail->storage != _RE_NODES_BUFFER_CAPACITY)); // TODO

    // assign pattern character to matching node as the only node on a new state
    *nodes_buf_next = (struct _RENode){ *pattern_char, NULL };
    *states_buf_next = (struct _REState){ nodes_buf_next, nodes_buf_next + 1, false };

    // push state, growing states buffer if needed
    state_prev = states_buf_next++;
    assert(((void)"States buffer grow not implemented", states_buf_next - states_buf_tail->storage != _RE_STATES_BUFFER_CAPACITY)); // TODO

    // set matching node transition pointer to new next state
    nodes_buf_next->next = states_buf_next;
    node_prev = nodes_buf_next++;
  }

  // assign start state and success states
  struct _REState *state_start = states_buf_head->storage;
  *states_buf_next = (struct _REState){ NULL, NULL, true }; // no need to check state since it guaranteed to be valid
  return (struct RegularExpression){ nodes_buf_head, states_buf_head, state_start };
}

/*  re_destroy
 *  void re_destroy(struct RegularExpression * re_pattern)
 *    re_pattern : regular expression data structure
 *
 *  Free resources used by regular expression data structure
 */
void re_destroy(struct RegularExpression *re_pattern) {
  assert(re_pattern != NULL);
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

int main(void) {
  struct RegularExpression pattern = re_compile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab*");

  re_destroy(&pattern);
  return 0;
}
