// =============================================================================
// <sax.c>
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

// -----------------------------------------------------------------------------

#include "api/sax.h"

// -----------------------------------------------------------------------------
// Trap event handlers
// -----------------------------------------------------------------------------

bool jsax_start_trap (jsax_t* jsnp, bool obj)
{
  return false;
}

bool jsax_key_trap (jsax_t* jsnp, jsax_key_t key)
{
  return false;
}

bool jsax_str_trap (jsax_t* jsnp, jsax_str_t str)
{
  return false;
}

bool jsax_num_trap (jsax_t* jsnp, jsax_num_t num)
{
  return false;
}

bool jsax_int_trap (jsax_t* jsnp, intmax_t val)
{
  return false;
}

bool jsax_uint_trap (jsax_t* jsnp, uintmax_t val)
{
  return false;
}

bool jsax_flt_trap (jsax_t* jsnp, double val)
{
  return false;
}

bool jsax_bool_trap (jsax_t* jsnp, bool val)
{
  return false;
}

bool jsax_null_trap (jsax_t* jsnp)
{
  return false;
}

bool jsax_end_trap (jsax_t* jsnp)
{
  return false;
}

bool jsax_mem_trap (const jsax_t* jsnp, u8** buf, size_t* size, size_t need)
{
  return false;
}

// -----------------------------------------------------------------------------
// Skip event handlers
// -----------------------------------------------------------------------------

bool jsax_start_skip (jsax_t* jsnp, bool obj)
{
  return true;
}

bool jsax_key_skip (jsax_t* jsnp, jsax_key_t key)
{
  return true;
}

bool jsax_str_skip (jsax_t* jsnp, jsax_str_t str)
{
  return true;
}

bool jsax_num_skip (jsax_t* jsnp, jsax_num_t num)
{
  return true;
}

bool jsax_int_skip (jsax_t* jsnp, intmax_t val)
{
  return true;
}

bool jsax_uint_skip (jsax_t* jsnp, uintmax_t val)
{
  return true;
}

bool jsax_flt_skip (jsax_t* jsnp, double val)
{
  return true;
}

bool jsax_bool_skip (jsax_t* jsnp, bool val)
{
  return true;
}

bool jsax_null_skip (jsax_t* jsnp)
{
  return true;
}

bool jsax_end_skip (jsax_t* jsnp)
{
  return true;
}
