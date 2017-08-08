// =============================================================================
// <buffer/sax.h>
//
// JSON SAX API value buffer definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_B008F19F464D43EFA51D461CC34BE386
#define H_B008F19F464D43EFA51D461CC34BE386

// -----------------------------------------------------------------------------
// SAX API value buffer doesn't require initialization
#define json_buf_init(a_sz)

// -----------------------------------------------------------------------------
// Check if there's enough space to store the combined parsed value
// in the supplied intermediate value buffer.
//
// If there isn't, request it via the provided memory allocation callback.
#define json_buf_grow(a_buf, a_sz) do\
{                                    \
  if (unlikely ((a_sz) > (jsnp->size - jsnp->used)))\
  {                                  \
    if (!(jsnp->on_mem (jsnp, &(jsnp->buf), &(jsnp->size), (a_sz))))\
    {                                \
      json_error (JSON_ERROR_MEMORY);\
    }                 \
                      \
    a_buf = jsnp->buf;\
  }       \
} while (0)

// -----------------------------------------------------------------------------

#endif
