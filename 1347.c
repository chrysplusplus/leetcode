/* File: 1347.c
 * Minumum Number of Steps ot Make Two Strings Anagram
 *
 * You are given two strings of the same length, `s` and `t`. In one step you
 * can choose any character of `t` and replace is with another character.
 *
 * Return the minimum number of steps to make `t` an anagram of `s`
 *
 * Example 1:
 *   Input: s = "bab", t = "aba"
 *   Output: 1
 *     Replace the first a in t with b, t = "bba" which is anagram of s
 *
 * Example 2:
 *   Input: s = "leetcode", t = "practice"
 *   Output: 5
 *     Replace p, r, a, i, c
 *
 * Example 3:
 *   Input: s = "anagram", t = "mangaar"
 *   Output: 0
 *     They are already anagrams
 */

#include <stdio.h>
#include <assert.h>

// technically this should be 50'000 to match leetcode specifications
#define BUFFER_CAPACITY 16

void init_int_buffer(int *p_buffer, size_t capacity) {
  int *end = p_buffer + capacity;
  for ( ; p_buffer != end; ++p_buffer)
    *p_buffer = 0;
}

int main(void) {
  const char *s_array = "leetcode";
  const char *t_array = "practice";

  // create bitfield for characters in s to be consumed in searching procedure
  int s_consumed_chars[BUFFER_CAPACITY];
  init_int_buffer(s_consumed_chars, BUFFER_CAPACITY);

  int score = 0, length = 0;
  for (const char *search = t_array; *search != '\0'; ++search) {
    ++length;
    //printf("%c\n", *search);
    for (size_t anagram_idx = 0; s_array[anagram_idx] != '\0'; ++anagram_idx) {
      //printf("\t%d,%c\n", anagram_idx, s_array[anagram_idx]);
      assert(anagram_idx < 16);
      // skip if char already consumed
      if (s_consumed_chars[anagram_idx])
        continue;
      else if (*search == s_array[anagram_idx]) {
        s_consumed_chars[anagram_idx] = 1;
        score += 1;
        // break loop since we only want to consume one character from the anagram
        break;
      }
    }
  }

  // subtracting the "score" of matching characters from the length of the input string
  // will give us the fewest characters need to change to make an anagram - though not
  // specifics of which characters to change
  int steps = length - score;
  printf("Minimum steps: %d", steps);
}
