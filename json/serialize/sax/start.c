// =============================================================================
// <serialize/sax/start.c>
//
// SAX API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if T(PRETTY)
  #define t_jsax_write_delim jsax_pretty_delim
#else
  #define t_jsax_write_delim jsax_write_delim
#endif

// -----------------------------------------------------------------------------

if (true)
{
  uint* stack = st->stack;
  uint depth = st->depth;
  uint level = st->level;

  if ((level + 1u) == depth)
  {
    st->need = 0;
    return 1;
  }

  t_jsax_write_delim (true);

#if !T(PRETTY)
  if (size == 0)
  {
    st->need = 1u;
    return 1;
  }
#endif

  if (obj) jsax_push_obj (stack, level);
  else jsax_push_arr (stack, level);

  level++;

#if !T(PRETTY)
  *buf++ = '[' | (obj << 5);
  size--;
#endif

  st->flags = (!obj << json_flags_coll_bit) | (obj << json_flags_key_bit) | json_flag_empty;

  st->level = level;

  st->buf = buf;
  st->size = size;

#if T(PRETTY)
  st->indent_width = iw + isz;
#endif

  return 0;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
#undef t_jsax_write_delim
