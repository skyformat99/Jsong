// =============================================================================
// <serialize/sax/boolean.c>
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

  if (val)
  {
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

    buf_set4 (buf, 't', 'r', 'u', 'e');

    buf += 4;
    size -= 4u;
  }
  else
  {
    if (size < (key_space + 5u))
    {
      st->need = (key_space + 5u) - size;
      return 1;
    }

#if T(PRETTY)
    *buf = ' ';
    buf += json_flags_obj (flags);
    size -= json_flags_obj (flags);
#endif

    buf_set5 (buf, 'f', 'a', 'l', 's', 'e');

    buf += 5;
    size -= 5u;
  }

  st->flags = (flags & json_flag_arr)
  | (!(flags & json_flag_arr) << json_flags_key_bit);

  st->buf = buf;
  st->size = size;

  return 0;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
