// =============================================================================
// <serialize/sax/data.c>
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
#if !T(PRETTY)
    if (size == 0)
    {
      st->need = 1u;
      goto fail;
    }
#endif

    pos = st->pos;
    len = st->len;

    goto partial;
  }

  size_t chrs_min = 1u // '('
  + 1u // '?'
  + 1u // ':'
  + val->mime.len
  + 1u // '?'
  + 1u // ')'
  ;

#if T(PRETTY)
  chrs_min = chrs_min
  + cstrlen (t_eol)
  - 1u // ')' (handled differently)
  + 1u // ' '
  ;
#endif

  if (size < chrs_min)
  {
    st->need = chrs_min - size;
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

  *buf++ = '(';
  size--;

  if (val->mime.len)
  {
    buf[0] = '?';
    buf[1] = ':';
    buf += 2;
    size -= 2;

    json_str_copy (buf, val->mime.buf, val->mime.len);
    buf += val->mime.len;
    size -= val->mime.len;
  }

#if T(PRETTY)
  json_str_copy (buf, t_eol, cstrlen (t_eol));
  buf += cstrlen (t_eol);
  size -= cstrlen (t_eol);
#endif

partial:
#if T(PRETTY)
  // Break after every 80th character
  if (len > 60u)
  {
    if (unlikely (size < (80u + cstrlen (t_eol))))
    {
      st->need = (80u + cstrlen (t_eol)) - size;

      st->pos = pos;
      st->len = len;

      goto fail;
    }

    // Guaranteed to succeed
    json_encode (pos, 60u, buf, 80u, &end, &sz);
    buf += 80;
    size -= 80u;

    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);
    size -= cstrlen (t_eol);

    pos += 60;
    len -= 60u;

    goto partial;
  }

  // Last line
  else
  {
    if (unlikely (size < ((80u + cstrlen (t_eol)) + iw + 1u)))
    {
      st->need = ((80u + cstrlen (t_eol)) + iw + 1u) - size;

      st->pos = pos;
      st->len = len;

      goto fail;
    }

    // Guaranteed to succeed
    json_encode (pos, len, buf, 80u, &end, &sz);
    buf += sz;
    size -= sz;

    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);
    size -= cstrlen (t_eol);

    json_str_fill (buf, iw, ' ');
    buf += iw;
    size -= iw;

    *buf++ = ')';
    size--;
  }
#else // T(PRETTY) ][
  {
    int ret = json_encode (pos, len, buf
    , size - 1u, &end, &sz);

    buf += sz;
    size -= sz;

    if (unlikely (ret != 0))
    {
      flags |= json_flag_part;

      st->need = (size_t)ret + 1u;

      st->pos = end;
      st->len = len - (size_t)(end - pos);

      goto fail;
    }

    *buf++ = ')';
    size--;
  }
#endif // !T(PRETTY)

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
