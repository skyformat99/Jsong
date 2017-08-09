// =============================================================================
// <serialize/sax.c>
//
// SAX API JSON serialization.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/integer.h>
#include <quantum/buffer.h>
#include <quantum/string.h>

// -----------------------------------------------------------------------------

#include "../serialize.h"
#include "../sax.h"
#include "../utils.h"
#include "../utils/misc.h"

// -----------------------------------------------------------------------------

void jsax_serialize_init (jsax_serialize_t* st, uint* stack, uint depth
, u8* buf, size_t size)
{
  st->flags = json_flag_empty;

  st->stack = stack;
  st->depth = depth;
  st->level = 0;

  st->buf = buf;
  st->size = size;

  st->flt_dig = 16u;

#if JSON(BIG_NUMBERS)
  #ifdef FLT128_MAX
    st->big_flt_dig = 35u;
  #elif (LDBL_BINARY == 80)
    st->big_flt_dig = 20u;
  #else
    st->big_flt_dig = 16u;
  #endif
#else
  st->big_flt_dig = 16u;
#endif
}

void jsax_serialize_init_stack (jsax_serialize_t* st, uint* stack, uint depth)
{
  st->stack = stack;
  st->depth = depth;
}

void jsax_serialize_init_buf (jsax_serialize_t* st, u8* buf, size_t size)
{
  st->buf = buf;
  st->size = size;
}

void jsax_format_init (jsax_format_t* st, uint* stack, uint depth
, u8* buf, size_t size)
{
  jsax_serialize_init ((jsax_serialize_t*)st, stack, depth, buf, size);

  st->indent_size = 2u;
  st->indent_width = 0;
}

void jsax_format_init_stack (jsax_format_t* st, uint* stack, uint depth)
{
  jsax_serialize_init_stack ((jsax_serialize_t*)st, stack, depth);
}

void jsax_format_init_buf (jsax_format_t* st, u8* buf, size_t size)
{
  jsax_serialize_init_buf ((jsax_serialize_t*)st, buf, size);
}

// -----------------------------------------------------------------------------

#define jsax_write_delim(dummy)\
  json_flags_t flags = st->flags;\
                               \
  if (!json_flags_val_undone (flags)) return -1;\
                               \
  u8* buf = st->buf;           \
  size_t size = st->size;      \
                               \
  if (json_flags_arr_undelim (flags))\
  {                          \
    if (size == 0)           \
    {                        \
      st->need = 1u;         \
      return 1;              \
    }                        \
                             \
    *buf++ = ',';            \
    size--;                  \
                             \
    flags |= json_flag_delim;\
  }

#define jsax_pretty_delim(coll)\
  json_flags_t flags = st->flags;\
                                 \
  if (!json_flags_val_undone (flags)) return -1;\
                                   \
  u8* buf = st->buf;               \
  size_t size = st->size;          \
                                   \
  uint isz = st->indent_size;      \
  uint iw = st->indent_width;      \
                                   \
  const size_t mul = coll ? 1u : 2u;\
                                   \
  if (json_flags_arr_undelim (flags))\
  {                                \
    if (size < (1u + cstrlen (t_eol) + iw))\
    {                              \
      st->need = (1u + cstrlen (t_eol) + iw) - size;\
      return 1;                    \
    }                              \
                                   \
    *buf++ = ',';                  \
    size--;                        \
                                   \
    if (!coll)                     \
    {                              \
      json_str_copy (buf, t_eol, cstrlen (t_eol));\
      buf += cstrlen (t_eol);      \
                                   \
      json_str_fill (buf, iw, ' ');\
      buf += iw;                   \
                                   \
      size -= cstrlen (t_eol) + iw;\
    }                              \
                                   \
    flags |= json_flag_delim;      \
  }                                \
                                   \
  if (json_flags_arr_empty (flags)/* && (iw != 0)*/)\
  {                                \
    if (size < (1u + (cstrlen (t_eol) * mul) + (iw * mul) - isz))\
    {                              \
      st->need = (1u + (cstrlen (t_eol) * mul) + (iw * mul) - isz) - size;\
      return 1;                    \
    }                              \
                                   \
    if (iw != isz)                 \
    {                              \
      json_str_copy (buf, t_eol, cstrlen (t_eol));\
      buf += cstrlen (t_eol);      \
                                   \
      json_str_fill (buf, iw - isz, ' ');\
      buf += iw - isz;             \
                                   \
      size -= cstrlen (t_eol) + (iw - isz);\
    }                              \
                                   \
    *buf++ = '[';                  \
    size--;                        \
                                   \
    if (!coll)                     \
    {                              \
      json_str_copy (buf, t_eol, cstrlen (t_eol));\
      buf += cstrlen (t_eol);      \
                                   \
      json_str_fill (buf, iw, ' ');\
      buf += iw;                   \
                                   \
      size -= cstrlen (t_eol) + iw;\
    }                              \
                                   \
    flags = flag_clr (flags, json_flag_empty);\
  }

// -----------------------------------------------------------------------------

int jsax_write_start (jsax_serialize_t* st, bool obj)
{
  #include "sax/start.c"
}

int jsax_pretty_start (jsax_format_t* st, bool obj)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/start.c"
}

// -----------------------------------------------------------------------------

int jsax_write_key (jsax_serialize_t* st, jsax_str_t key)
{
  #include "sax/key.c"
}

int jsax_pretty_key (jsax_format_t* st, jsax_str_t key)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/key.c"
}

// -----------------------------------------------------------------------------

int jsax_write_str (jsax_serialize_t* st, jsax_str_t str)
{
  #include "sax/string.c"
}

int jsax_pretty_str (jsax_format_t* st, jsax_str_t str)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/string.c"
}

// -----------------------------------------------------------------------------

int jsax_write_num (jsax_serialize_t* st, jsax_num_t num)
{
  #include "sax/number.c"
}

int jsax_pretty_num (jsax_format_t* st, jsax_num_t num)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/number.c"
}

// -----------------------------------------------------------------------------

int jsax_write_int (jsax_serialize_t* st, intmax_t val)
{
  #include "sax/integer.c"
}

int jsax_pretty_int (jsax_format_t* st, intmax_t val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/integer.c"
}

// -----------------------------------------------------------------------------

int jsax_write_flt (jsax_serialize_t* st, double val)
{
  #include "sax/float.c"
}

int jsax_pretty_flt (jsax_format_t* st, double val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/float.c"
}

// -----------------------------------------------------------------------------

int jsax_write_bool (jsax_serialize_t* st, bool val)
{
  #include "sax/boolean.c"
}

int jsax_pretty_bool (jsax_format_t* st, bool val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/boolean.c"
}

// -----------------------------------------------------------------------------

int jsax_write_null (jsax_serialize_t* st)
{
  #include "sax/null.c"
}

int jsax_pretty_null (jsax_format_t* st)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/null.c"
}

// -----------------------------------------------------------------------------

#undef jsax_write_delim
#undef jsax_pretty_delim

// -----------------------------------------------------------------------------

int jsax_write_end (jsax_serialize_t* st)
{
  #include "sax/end.c"
}

int jsax_pretty_end (jsax_format_t* st)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/end.c"
}
