// =============================================================================
// <serialize/sax/verbatim.c>
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
    if (size < (cstrlen (t_eol) + 2u + val->id.len))
    {
      st->need = cstrlen (t_eol) + 2u + val->id.len;
      goto fail;
    }

    pos = st->pos;
    len = st->len;

    goto partial;
  }

  size_t chrs_min = 1u // '('
  + 1u // '!'
  + val->id.len
  + cstrlen (t_eol)
  + cstrlen (t_eol)
  + 1u // '!'
  + val->id.len
  + 1u // ')'
  ;

#if T(PRETTY)
  chrs_min++; // ' '
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

  buf[0] = '(';
  buf[1] = '!';
  buf += 2;
  size -= 2;

  json_str_copy (buf, val->id.buf, val->id.len);
  buf += val->id.len;
  size -= val->id.len;

  json_str_copy (buf, t_eol, cstrlen (t_eol));
  buf += cstrlen (t_eol);
  size -= cstrlen (t_eol);

  if (val->data.len == 0)
  {
    *buf++ = '!';
    size--;

    json_str_copy (buf, val->id.buf, val->id.len);
    buf += val->id.len;
    size -= val->id.len;

    *buf++ = ')';
    size--;
  }
  else
  {
    size_t max;

partial:
    max = size - (cstrlen (t_eol) + 2u + val->id.len);
    sz = min2 (len, max);

    str_copy (buf, pos, sz);
    buf += sz;
    size -= sz;

    if (len > max)
    {
      flags |= json_flag_part;

      st->need = (len - max) + (cstrlen (t_eol) + 2u + val->id.len);

      st->pos = buf + sz;
      st->len = len - sz;

      goto fail;
    }

    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);
    size -= cstrlen (t_eol);

    *buf++ = '!';
    size--;

    json_str_copy (buf, val->id.buf, val->id.len);
    buf += val->id.len;
    size -= val->id.len;

    *buf++ = ')';
    size--;
  }

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
