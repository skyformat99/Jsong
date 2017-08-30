// =============================================================================
// <serialize/sax/integer.c>
//
// SAX API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  #include "delimiter.c"

#if T(PRETTY)
  const size_t key_space = 1u;
#else
  const size_t key_space = 0;
#endif

  if (size < (key_space + 1u + INTMAX_DIG))
  {
    st->need = (key_space + 1u + INTMAX_DIG) - size;
    return 1;
  }

#if T(PRETTY)
  *buf = ' ';
  buf += json_flags_obj (flags);
  size -= json_flags_obj (flags);
#endif

  size_t l = int_smax_to_str (buf, val);

  buf += l;
  size -= l;

  st->flags = (flags & json_flag_arr)
  | (!(flags & json_flag_arr) << json_flags_key_bit);

  st->buf = buf;
  st->size = size;

  return 0;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
