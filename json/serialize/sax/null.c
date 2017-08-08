// =============================================================================
// <serialize/sax/null.c>
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
  t_jsax_write_delim (false);

#if T(PRETTY)
  const size_t key_space = 1u;
#else
  const size_t key_space = 0;
#endif

  if (size < (key_space + 4u))
  {
    st->need = (key_space + 4u) - size;
    return 1;
  }

#if T(PRETTY)
  *buf = ' ';
  buf += json_flags_obj (flags);
  size -= json_flags_obj (flags);
#endif

  buf_set4 (buf, 'n', 'u', 'l', 'l');

  buf += 4;
  size -= 4u;

  st->flags = (flags & json_flag_arr)
  | (!(flags & json_flag_arr) << json_flags_key_bit);

  st->buf = buf;
  st->size = size;

  return 0;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
#undef t_jsax_write_delim
