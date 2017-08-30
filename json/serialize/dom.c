// =============================================================================
// <serialize/dom.c>
//
// DOM API JSON serialization.
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

#include "../dom.h"
#include "../utils.h"
#include "../utils/misc.h"

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

void json_prefix (serialize_init) (json_prefix (serialize_t)* st
, json_node_t root, u8* buf, size_t size, size_t fill)
{
  st->root = root;
  st->node = root;

  json_prefix (serialize_init_buf) (st, buf, size, fill);

  st->buf = buf;
  st->size = size;
  st->fill = fill;

  st->pos = null;
  st->key = false;
  st->term = false;

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

#if ENABLED(USON)
  st->verb = null;
  st->data = false;
#endif
}

void json_prefix (serialize_init_buf) (json_prefix (serialize_t)* st
, u8* buf, size_t size, size_t fill)
{
  st->buf = buf;
  st->size = size;
  st->fill = fill;
}

// -----------------------------------------------------------------------------

void json_prefix (format_init) (json_prefix (format_t)* st, json_node_t root
, u8* buf, size_t size, size_t fill)
{
  json_prefix (serialize_init) ((json_prefix (serialize_t)*)st
  , root, buf, size, fill);

  st->indent_size = 2u;
  st->indent_width = 0;
}

void json_prefix (format_init_buf) (json_prefix (format_t)* st
, u8* buf, size_t size, size_t fill)
{
  json_prefix (serialize_init_buf) ((json_prefix (serialize_t)*)st
  , buf, size, fill);
}

// -----------------------------------------------------------------------------
// Serialization
// -----------------------------------------------------------------------------

bool json_prefix (serialize) (json_prefix (serialize_t)* st)
{
  #define t_eol OS_EOL

  #include "dom/template.c"
}

bool json_prefix (format) (json_prefix (format_t)* st)
{
  #define T_PRETTY
  #define t_eol OS_EOL

  #include "dom/template.c"
}
