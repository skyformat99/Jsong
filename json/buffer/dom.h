// =============================================================================
// <buffer/dom.h>
//
// JSON DOM API value buffer definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_B6A382756E6E4C4790CB942BB71B379E
#define H_B6A382756E6E4C4790CB942BB71B379E

// -----------------------------------------------------------------------------
// Reset the buffer and (optionally) reserve some space for the DOM value object
#define json_buf_init(a_sz) do\
{                     \
  jsnp->buf = null;   \
  jsnp->size = (a_sz);\
  jsnp->used = (a_sz);\
} while (0)

// -----------------------------------------------------------------------------
// Grow the buffer to store the parsed DOM value
#define json_buf_grow(a_buf, a_sz) do\
{                                    \
  size_t sz = jsnp->size + (a_sz);   \
                                     \
  *(a_buf) = mem_pool_realloc (jsnp->pool, *(a_buf), jsnp->size, (sz));\
                                     \
  if (unlikely (*(a_buf) == null)) json_error (JSON_ERROR_MEMORY);\
                       \
  jsnp->buf = *(a_buf);\
  jsnp->size = sz;\
} while (0)

// -----------------------------------------------------------------------------

#endif
