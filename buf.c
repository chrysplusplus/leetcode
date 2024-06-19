#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

struct Buf {
  void *head;
  size_t elem_size, buf_size, capacity;
};

/* buf_init
 * Initialise a buffer with element size and initiali capacity
 */
bool buf_init(struct Buf *buf, size_t init_capacity, size_t elem_size) {
  buf->head = malloc(elem_size * init_capacity);
  if (buf->head == NULL) return false;
  buf->elem_size = elem_size;
  buf->buf_size = 0;
  buf->capacity = init_capacity;
  return true;
}

/* buf_grow
 * Expand buffer by reallocating with larger capacity
 */
bool buf_grow(struct Buf *buf) {
  size_t new_capacity = buf->capacity * 2;
  char *new_head = malloc(buf->elem_size * new_capacity);
  if (new_head == NULL) return false;
  memcpy(new_head, buf->head, buf->elem_size * buf->buf_size);
  free(buf->head);
  buf->head = new_head;
  buf->capacity = new_capacity;
  return true;
}

/* buf_get
 * Get value at index of buffer and store in result
 * Undefined behavour if result type does not have the same size as buffer elements
 */
void buf_get(const struct Buf *buf, size_t index, void *result) {
  assert(index < buf->buf_size);
  char *head = buf->head;
  char *pointer = head + index * buf->elem_size;
  memcpy(result, pointer, buf->elem_size);
}

/* buf_set
 * Set value at index of buffer to value
 * Undefined behavour if value type does not have the same size as buffer element
 */
void buf_set(struct Buf *buf, size_t index, const void *value) {
  assert(index < buf->buf_size);
  char *head = buf->head;
  char *pointer = head + index * buf->elem_size;
  memcpy(pointer, value, buf->elem_size);
}

/* buf_append
 * Append value to the end of buffer, growing the buffer if necessary
 * Return false if buffer could not be expanded, i.e. no more memory
 */
bool buf_append(struct Buf *buf, const void *value) {
  if (buf->buf_size == buf->capacity) {
    if (!buf_grow(buf)) return false;
  }

  char *head = buf->head;
  char *new_elem = head + buf->buf_size * buf->elem_size;
  memcpy(new_elem, value, buf->elem_size);
  buf->buf_size++;
  return true;
}

/* buf_destroy
 * Free memory from a buffer and set to empty state
 */
void buf_destroy(struct Buf *buf) {
  free(buf->head);
  buf->head = NULL;
  buf->buf_size = 0;
  buf->capacity = 0;
}

int main(void) {
  struct Buf hello_world_buffer;
  buf_init(&hello_world_buffer, 13, sizeof (char));

  // copy string into buffer
  const char hello_world_string[] = "Hello World\n";
  for (size_t idx = 0; idx < sizeof hello_world_string; ++idx) {
    buf_append(&hello_world_buffer, hello_world_string + idx);
  }

  // print from buffer
  char ch;
  for (size_t idx = 0; idx < hello_world_buffer.buf_size; ++idx) {
    buf_get(&hello_world_buffer, idx, &ch);
    putchar(ch);
  }
  fflush(stdout);

  buf_destroy(&hello_world_buffer);
  return 0;
}

