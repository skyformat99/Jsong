// =============================================================================
// <serialize/sax/float.c>
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

  u8 temp[JSON_FLT_BUF_SIZE];

  size_t l = snprintf (temp, numof (temp), "%.*g"
  , (uint)(st->flt_dig), val);

  if (size < (key_space + l))
  {
    st->need = (key_space + l) - size;
    return 1;
  }

#if T(PRETTY)
  *buf = ' ';
  buf += json_flags_obj (flags);
  size -= json_flags_obj (flags);
#endif

  json_str_copy (buf, temp, l);

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
