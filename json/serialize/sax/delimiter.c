// =============================================================================
// <serialize/sax/delimiter.c>
//
// SAX API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

  json_flags_t flags = st->flags;

  if (!json_flags_val_undone (flags)) return -1;

  u8* buf = st->buf;
  size_t size = st->size;

#if T(PRETTY)
  uint isz = st->indent_size;
  uint iw = st->indent_width;
#endif

  if (json_flags_arr_undelim (flags))
  {
#if T(PRETTY)
    if (size < (1u + cstrlen (t_eol) + iw))
    {
      st->need = (1u + cstrlen (t_eol) + iw) - size;
      return 1;
    }

  #if !ENABLED(USON)
    *buf++ = ',';
    size--;
  #endif

  #if !T(COLL)
    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);
    size -= cstrlen (t_eol);

    json_str_fill (buf, iw, ' ');
    buf += iw;
    size -= iw;
  #endif

#else // T(PRETTY) ][

    if (size == 0)
    {
      st->need = 1u;
      return 1;
    }

  #if ENABLED(USON)
    *buf++ = ' ';
  #else
    *buf++ = ',';
  #endif

    size--;
#endif // T(PRETTY)

    flags |= json_flag_delim;
  }

#if ENABLED(USON)
  if (json_flags_arr_empty (flags)/* && (iw != 0)*/)
  {
  #if T(COLL)
    const size_t mul = 1u;
  #else
    const size_t mul = 2u;
  #endif

    if (size < (1u + (cstrlen (t_eol) * mul) + (iw * mul) - isz))
    {
      st->need = (1u + (cstrlen (t_eol) * mul) + (iw * mul) - isz) - size;
      return 1;
    }

    if (iw != isz)
    {
      json_str_copy (buf, t_eol, cstrlen (t_eol));
      buf += cstrlen (t_eol);
      size -= cstrlen (t_eol);

      json_str_fill (buf, iw - isz, ' ');
      buf += iw - isz;
      size -= iw - isz;
    }

    *buf++ = '[';
    size--;

  #if !T(COLL)
    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);
    size -= cstrlen (t_eol);

    json_str_fill (buf, iw, ' ');
    buf += iw;
    size -= iw;
  #endif

    flags = flag_clr (flags, json_flag_empty);
  }
#endif // USON

// -----------------------------------------------------------------------------

#undef T_COLL
