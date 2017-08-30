// =============================================================================
// <serialize/sax/string.c>
//
// SAX API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  #include "delimiter.c"

  u8* pos;
  size_t len;

  if (json_flags_part (flags))
  {
    if (size == 0)
    {
      st->need = 1u;
      goto fail;
    }

    pos = st->pos;
    len = st->len;

    goto partial;
  }

#if T(PRETTY)
  const size_t key_space = 1u;
#else
  const size_t key_space = 0;
#endif

  if (size < (key_space + 2u))
  {
    st->need = (key_space + 2u) - size;
    goto fail;
  }

  int ret;

  u8* end;
  size_t sz;

  pos = str.buf;
  len = str.len;

#if T(PRETTY)
  *buf = ' ';
  buf += json_flags_obj (flags);
  size -= json_flags_obj (flags);
#endif

  *buf++ = '"';
  size--;

partial:
  ret = json_prefix (escape) (pos, len, buf, size - 1u, &end, &sz);

  buf += sz;
  size -= sz;

  if (ret != 0)
  {
    flags |= json_flag_part;

    st->need = (size_t)ret + 1u;

    st->pos = end;
    st->len = len - (size_t)(end - pos);

    goto fail;
  }

  *buf++ = '"';
  size--;

  st->flags = (flags & json_flag_arr)
  | (!(flags & json_flag_arr) << json_flags_key_bit);

  st->buf = buf;
  st->size = size;

  return 0;

fail:
  st->flags = flags;

  st->buf = buf;
  st->size = size;

  return 1;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
