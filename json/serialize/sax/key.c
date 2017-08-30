// =============================================================================
// <serialize/sax/key.c>
//
// SAX API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  json_flags_t flags = st->flags;

  if (!json_flags_key_undone (flags)) return -1;

  u8* buf = st->buf;
  size_t size = st->size;

#if T(PRETTY)
  uint iw = st->indent_width;
#endif

  if (json_flags_undelim (flags))
  {
#if T(PRETTY)
    if (size < (1u + cstrlen (t_eol) + iw))
    {
      st->need = (1u + cstrlen (t_eol) + iw) - size;
      return 1;
    }

  #if ENABLED(USON)
    *buf++ = ';';
  #else
    *buf++ = ',';
  #endif

    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);

    json_str_fill (buf, iw, ' ');
    buf += iw;

    size -= 1u + cstrlen (t_eol) + iw;
#else
    if (size == 0)
    {
      st->need = 1u;
      return 1;
    }

  #if ENABLED(USON)
    *buf++ = ';';
  #else
    *buf++ = ',';
  #endif

    size--;
#endif

    flags |= json_flag_delim;
  }

#if T(PRETTY)
  if (json_flags_empty (flags))
  {
    uint isz = st->indent_size;

    if (size < (1u + (cstrlen (t_eol) * 2u) + (iw * 2u) - isz))
    {
      st->need = (1u + (cstrlen (t_eol) * 2u) + (iw * 2u) - isz) - size;
      return 1;
    }

    if (iw != isz)
    {
      json_str_copy (buf, t_eol, cstrlen (t_eol));
      buf += cstrlen (t_eol);

      json_str_fill (buf, iw - isz, ' ');
      buf += iw - isz;

      size -= cstrlen (t_eol) + (iw - isz);
    }

    *buf++ = '{';
    size--;

    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);

    json_str_fill (buf, iw, ' ');
    buf += iw;

    size -= cstrlen (t_eol) + iw;

    flags = flag_clr (flags, json_flag_empty);
  }
#endif

  u8* pos;
  size_t len;

  if (json_flags_part (flags))
  {
    if (size < 2u)
    {
      st->need = 2u - size;
      goto fail;
    }

    pos = st->pos;
    len = st->len;

    goto partial;
  }

  if (size < 3u)
  {
    st->need = 3u - size;
    goto fail;
  }

  int ret;

  u8* end;
  size_t sz;

  pos = key.buf;
  len = key.len;

  *buf++ = '"';
  size--;

partial:
  ret = json_prefix (escape) (pos, len, buf, size - 2u, &end, &sz);

  buf += sz;
  size -= sz;

  if (ret != 0)
  {
    flags |= json_flag_part;

    st->need = (size_t)ret + 2u;

    st->pos = end;
    st->len = len - (size_t)(end - pos);

    goto fail;
  }

  buf[0] = '"';
  buf[1] = ':';

  buf += 2;
  size -= 2u;

  st->flags = 0;

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
