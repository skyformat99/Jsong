// =============================================================================
// <sax.c>
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

// -----------------------------------------------------------------------------

#include "api/macros.h"
#include "api/sax.h"

// -----------------------------------------------------------------------------
// Trap event handlers
// -----------------------------------------------------------------------------

bool jsax_prefix (start_trap) (jsax_prefix (t)* jsnp, bool obj)
{
  return false;
}

bool jsax_prefix (key_trap) (jsax_prefix (t)* jsnp, jsax_key_t key)
{
  return false;
}

bool jsax_prefix (str_trap) (jsax_prefix (t)* jsnp, jsax_str_t str)
{
  return false;
}

bool jsax_prefix (num_trap) (jsax_prefix (t)* jsnp, jsax_num_t num)
{
  return false;
}

bool jsax_prefix (int_trap) (jsax_prefix (t)* jsnp, intmax_t val)
{
  return false;
}

bool jsax_prefix (uint_trap) (jsax_prefix (t)* jsnp, uintmax_t val)
{
  return false;
}

bool jsax_prefix (flt_trap) (jsax_prefix (t)* jsnp, double val)
{
  return false;
}

bool jsax_prefix (bool_trap) (jsax_prefix (t)* jsnp, bool val)
{
  return false;
}

bool jsax_prefix (null_trap) (jsax_prefix (t)* jsnp)
{
  return false;
}

bool jsax_prefix (end_trap) (jsax_prefix (t)* jsnp)
{
  return false;
}

bool jsax_prefix (id_trap) (jsax_prefix (t)* jsnp, jsax_str_t str)
{
  return false;
}

bool jsax_prefix (mime_trap) (jsax_prefix (t)* jsnp, jsax_str_t str)
{
  return false;
}

bool jsax_prefix (mem_trap) (const jsax_prefix (t)* jsnp, u8** buf, size_t* size, size_t need)
{
  return false;
}

// -----------------------------------------------------------------------------
// Skip event handlers
// -----------------------------------------------------------------------------

bool jsax_prefix (start_skip) (jsax_prefix (t)* jsnp, bool obj)
{
  return true;
}

bool jsax_prefix (key_skip) (jsax_prefix (t)* jsnp, jsax_key_t key)
{
  return true;
}

bool jsax_prefix (str_skip) (jsax_prefix (t)* jsnp, jsax_str_t str)
{
  return true;
}

bool jsax_prefix (num_skip) (jsax_prefix (t)* jsnp, jsax_num_t num)
{
  return true;
}

bool jsax_prefix (int_skip) (jsax_prefix (t)* jsnp, intmax_t val)
{
  return true;
}

bool jsax_prefix (uint_skip) (jsax_prefix (t)* jsnp, uintmax_t val)
{
  return true;
}

bool jsax_prefix (flt_skip) (jsax_prefix (t)* jsnp, double val)
{
  return true;
}

bool jsax_prefix (bool_skip) (jsax_prefix (t)* jsnp, bool val)
{
  return true;
}

bool jsax_prefix (null_skip) (jsax_prefix (t)* jsnp)
{
  return true;
}

bool jsax_prefix (end_skip) (jsax_prefix (t)* jsnp)
{
  return true;
}

bool jsax_prefix (id_skip) (jsax_prefix (t)* jsnp, jsax_str_t str)
{
  return true;
}

bool jsax_prefix (mime_skip) (jsax_prefix (t)* jsnp, jsax_str_t str)
{
  return true;
}
