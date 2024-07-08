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

// This is a very simple implementation that just uses a contiguous array of match atoms as though
// they were a state machine. Only the special characters . and * are
// implemented.

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define _RE_ATOM_BUFFER_INITIAL_CAPACITY 20
#define _RE_BACKTRACKING_INITIAL_CAPACITY 20

void _assert(bool condition, const char *msg, const char *file, int line) {
  if (condition) return;
  printf("%s:%d: Assertion failed: %s", file, line, msg);
  exit(EXIT_FAILURE);
}

#define assert(_condition, _msg) (_assert((_condition), (_msg), __FILE__, __LINE__))
#define cassert(_condition) (_assert((_condition), (#_condition), __FILE__, __LINE__))

struct _REAtom {
  char literal;
  enum _REAtomFlags {
    SINGLE, ANY, END
  } flags;
};

struct _RegularExpression {
  struct _REAtom *atoms;
  const char *pattern_str;
  size_t atoms_size;
};

typedef struct _RegularExpression Pattern_t;

Pattern_t re_compile(const char *pattern_str) {
  size_t atom_buffer_capacity = _RE_ATOM_BUFFER_INITIAL_CAPACITY, atom_buffer_size = 0;
  struct _REAtom *atom_buffer = malloc(atom_buffer_capacity * sizeof (struct _REAtom));
  assert(atom_buffer != NULL, "No more memory");

  struct _REAtom *previous = NULL;
  for (const char *pattern_char = pattern_str; *pattern_char != '\0'; ++pattern_char) {
    // * -> set ANY flag on previous atom
    if (*pattern_char == '*') {
      assert(previous != NULL, "Invalid pattern");
      previous->flags = ANY;
    }

    // literal -> assign value to new atom
    else {
      atom_buffer[atom_buffer_size] = (struct _REAtom){ *pattern_char, SINGLE };
      previous = atom_buffer + atom_buffer_size;
      ++atom_buffer_size;
    }

    // expand buffer if required
    if (atom_buffer_size == atom_buffer_capacity) {
      size_t new_capacity = atom_buffer_capacity * 2, idx_previous = previous - atom_buffer;
      struct _REAtom *new_buffer =  malloc(new_capacity * sizeof (struct _REAtom));
      cassert(new_buffer != NULL);
      memmove(new_buffer, atom_buffer, atom_buffer_size);
      free(atom_buffer);
      atom_buffer = new_buffer;
      atom_buffer_capacity = new_capacity;
      previous = atom_buffer + idx_previous;
    }
  }

  // add terminating atom which will be the success state
  atom_buffer[atom_buffer_size] = (struct _REAtom){ '\0', END };
  ++atom_buffer_size;
  return (Pattern_t){ atom_buffer, pattern_str, atom_buffer_size };
}

void re_destroy(Pattern_t *pattern) {
  if (pattern == NULL)
    return;

  if (pattern->atoms) {
    free(pattern->atoms);
    pattern->atoms = NULL;
    pattern->atoms_size = 0;
  }
}

bool re_match(Pattern_t *pattern, const char *search_str) {
  // backtrack buffer
  struct Context {
    struct _REAtom *state;
    const char *search_char;
  } *context_stack;

  size_t ctxs_capacity = _RE_BACKTRACKING_INITIAL_CAPACITY, ctxs_sz = 0;
  context_stack = malloc(ctxs_capacity * sizeof (struct Context));
  cassert(context_stack != NULL);

  // initial context
  bool state_machine_is_backtracking = false, does_pattern_match = false;
  const char *search_char = search_str;
  struct _REAtom *state = pattern->atoms;

  while (true) {
    bool is_match = false;

    // backtracking but stack is empty, pattern does not match search string
    if (state_machine_is_backtracking && ctxs_sz == 0) {
      break;
    }

    // backtrack by popping context stack and incrementing state
    // the ability to increment the state is encoded by the presence of the backtracking point,
    // since these points are only created when a repeating atom matches
    else if (state_machine_is_backtracking) {
      struct Context popped_context = context_stack[ctxs_sz--];
      state = popped_context.state;
      search_char = popped_context.search_char;
      ++state;

      // if the state after backtracking ends up being an END state, the algorithm should continue
      // backtracking
      state_machine_is_backtracking = state->flags == END;
    }

    // . (wildcard) match anything except null terminator
    else if (state->literal == '.') {
      is_match = *search_char != '\0';
    }

    // check match for literals
    else {
      is_match = *search_char == state->literal;
    }

    // match on non-repeating
    if (is_match && state->flags == SINGLE) {
      ++search_char;
      ++state;
    }

    // match on repeating, set backtracking point
    else if (is_match && state->flags == ANY) {
      // append to context stack and resize if necessary
      context_stack[ctxs_sz++] = (struct Context){ state, search_char };
      if (ctxs_sz == ctxs_capacity) {
        size_t new_capacity = ctxs_capacity * 2;
        struct Context *new_buffer =  malloc(new_capacity * sizeof (struct Context));
        cassert(new_buffer != NULL);
        memmove(new_buffer, context_stack, ctxs_sz);
        free(context_stack);
        context_stack = new_buffer;
        ctxs_capacity = new_capacity;
      }

      ++search_char;
      ++state;
    }

    // no match on non-repeating, begin backtracking
    else if (state->flags == SINGLE) {
      state_machine_is_backtracking = true;
    }

    // no match on repeating, skip
    else if (state->flags == ANY) {
      ++state;
    }

    // if search has reached the end of the string and the state is the terminating success state,
    // the pattern matches the search string
    if (*search_char == '\0' && state->flags == END) {
      does_pattern_match = true;
      break;
    }

    // if state is terminating but search is not, begin backtracking
    else if (state->flags == END) {
      state_machine_is_backtracking = true;
    }
  }

  // cleanup
  free(context_stack);
  return does_pattern_match;
}

int main(int argc, char *argv[]) {
  const char *pattern_str, *search_str;
  switch (argc) {
    case 0:
      return EXIT_FAILURE;
    case 1:
    case 2:
      printf("Usage: %s pattern search_string\n", argv[0]);
      return EXIT_FAILURE;
    case 3:
      pattern_str = argv[1];
      search_str = argv[2];
      break;
    default:
      printf("Too many arguments\n");
      return EXIT_FAILURE;
  }

  Pattern_t pattern = re_compile(pattern_str);
  bool does_match = re_match(&pattern, search_str);

  re_destroy(&pattern);

  return does_match;
}

