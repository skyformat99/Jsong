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

#include "../api/core.h"
#include "../api/macros.h"
#include "../api/serialize.h"

#include "../sax.h"
#include "../utils.h"
#include "../utils/misc.h"

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

void jsax_prefix (serialize_init) (jsax_prefix (serialize_t)* st
, uint* stack, uint depth, u8* buf, size_t size)
{
  st->flags = json_flag_empty;

  jsax_prefix (serialize_init_stack) (st, stack, depth);
  st->level = 0;

  jsax_prefix (serialize_init_buf) (st, buf, size);

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

void jsax_prefix (serialize_init_stack) (jsax_prefix (serialize_t)* st
, uint* stack, uint depth)
{
  st->stack = stack;
  st->depth = depth;
}

void jsax_prefix (serialize_init_buf) (jsax_prefix (serialize_t)* st
, u8* buf, size_t size)
{
  st->buf = buf;
  st->size = size;
}

// -----------------------------------------------------------------------------

void jsax_prefix (format_init) (jsax_prefix (format_t)* st
, uint* stack, uint depth, u8* buf, size_t size)
{
  jsax_prefix (serialize_init) ((jsax_prefix (serialize_t)*)st
  , stack, depth, buf, size);

  st->indent_size = 2u;
  st->indent_width = 0;
}

void jsax_prefix (format_init_stack) (jsax_prefix (format_t)* st
, uint* stack, uint depth)
{
  jsax_prefix (serialize_init_stack) ((jsax_prefix (serialize_t)*)st
  , stack, depth);
}

void jsax_prefix (format_init_buf) (jsax_prefix (format_t)* st
, u8* buf, size_t size)
{
  jsax_prefix (serialize_init_buf) ((jsax_prefix (serialize_t)*)st
  , buf, size);
}

// -----------------------------------------------------------------------------
// Serialization
// -----------------------------------------------------------------------------

int jsax_prefix (write_start) (jsax_prefix (serialize_t)* st, bool obj)
{
  #include "sax/start.c"
}

int jsax_prefix (pretty_start) (jsax_prefix (format_t)* st, bool obj)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/start.c"
}

// -----------------------------------------------------------------------------

int jsax_prefix (write_key) (jsax_prefix (serialize_t)* st, jsax_str_t key)
{
  #include "sax/key.c"
}

int jsax_prefix (pretty_key) (jsax_prefix (format_t)* st, jsax_str_t key)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/key.c"
}

// -----------------------------------------------------------------------------

int jsax_prefix (write_str) (jsax_prefix (serialize_t)* st, jsax_str_t str)
{
  #include "sax/string.c"
}

int jsax_prefix (pretty_str) (jsax_prefix (format_t)* st, jsax_str_t str)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/string.c"
}

// -----------------------------------------------------------------------------

int jsax_prefix (write_num) (jsax_prefix (serialize_t)* st, jsax_num_t num)
{
  #include "sax/number.c"
}

int jsax_prefix (pretty_num) (jsax_prefix (format_t)* st, jsax_num_t num)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/number.c"
}

// -----------------------------------------------------------------------------

int jsax_prefix (write_int) (jsax_prefix (serialize_t)* st, intmax_t val)
{
  #include "sax/integer.c"
}

int jsax_prefix (pretty_int) (jsax_prefix (format_t)* st, intmax_t val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/integer.c"
}

// -----------------------------------------------------------------------------

int jsax_prefix (write_flt) (jsax_prefix (serialize_t)* st, double val)
{
  #include "sax/float.c"
}

int jsax_prefix (pretty_flt) (jsax_prefix (format_t)* st, double val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/float.c"
}

// -----------------------------------------------------------------------------

int jsax_prefix (write_bool) (jsax_prefix (serialize_t)* st, bool val)
{
  #include "sax/boolean.c"
}

int jsax_prefix (pretty_bool) (jsax_prefix (format_t)* st, bool val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/boolean.c"
}

// -----------------------------------------------------------------------------

int jsax_prefix (write_null) (jsax_prefix (serialize_t)* st)
{
  #include "sax/null.c"
}

int jsax_prefix (pretty_null) (jsax_prefix (format_t)* st)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/null.c"
}

// -----------------------------------------------------------------------------

#if ENABLED(USON)

// -----------------------------------------------------------------------------

int usax_write_verb (usax_serialize_t* st, usax_str_verb_t* val)
{
  #include "sax/verbatim.c"
}

int usax_pretty_verb (usax_format_t* st, usax_str_verb_t* val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/verbatim.c"
}

// -----------------------------------------------------------------------------

int usax_write_data (usax_serialize_t* st, usax_str_data_t* val)
{
  #include "sax/data.c"
}

int usax_pretty_data (usax_format_t* st, usax_str_data_t* val)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/data.c"
}

// -----------------------------------------------------------------------------

#endif // USON

// -----------------------------------------------------------------------------

int jsax_prefix (write_end) (jsax_prefix (serialize_t)* st)
{
  #include "sax/end.c"
}

int jsax_prefix (pretty_end) (jsax_prefix (format_t)* st)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "sax/end.c"
}
