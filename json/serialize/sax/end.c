// =============================================================================
// <serialize/sax/end.c>
//
// SAX API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  uint* stack = st->stack;
  uint depth = st->depth;
  uint level = st->level;

  // Everything is closed already
  if (level == 0) return -1;

  json_flags_t flags = st->flags;

  // Cannot close object if its current key is without value
  if (json_flags_obj_val (flags)) return -1;

  u8* buf = st->buf;
  size_t size = st->size;

  // See if the current collection is an object
  bint obj = json_flags_obj (flags);

  // See if the upper collection is an object.
  // Would produce wrong result when closing the root collection,
  // but it doesn't matter at this point.
  level--;
  bint obj_upper = !jsax_pop_coll (stack, level - clamp (level));

#if T(PRETTY)
  // This (rather complex) logic is necessary to put
  // the opening and closing braces at the right place.
  // There's no way around it without changing
  // the style of indentation.
  uint isz = st->indent_size;
  uint iw = st->indent_width;

  // Print an empty collection.
  // Tn this case we need to print both the opening
  // and closing braces.
  if (json_flags_empty (flags))
  {
    // Empty collection must be placed on the same line
    // together with its key in an upper object
    if (obj_upper | !level)
    {
      if (size < 3u)
      {
        st->need = 3u - size;
        return 1;
      }

      // Prepend space only if not a root object (there's no key).
      // (At the time of root object indentation is zero.)
      *buf = ' ';
      buf += (iw != isz);
      size -= (iw != isz);

      *buf++ = '[' | (obj << 5);
      size--;

      iw -= isz;
    }
    // Always placed on it's own line in an array
    else
    {
      iw -= isz;

      if (size < (2u + cstrlen (t_eol) + iw))
      {
        st->need = (2u + cstrlen (t_eol) + iw) - size;
        return 1;
      }

      json_str_copy (buf, t_eol, cstrlen (t_eol));
      buf += cstrlen (t_eol);

      json_str_fill (buf, iw, ' ');
      buf += iw;

      *buf++ = '[' | (obj << 5);

      size -= 1u + cstrlen (t_eol) + iw;
    }
  }
  // Print a non-empty collection.
  // The opening brace is already printed.
  else
  {
    iw -= isz;

    if (size < (1u + cstrlen (t_eol) + iw))
    {
      st->need = (1u + cstrlen (t_eol) + iw) - size;
      return 1;
    }

    json_str_copy (buf, t_eol, cstrlen (t_eol));
    buf += cstrlen (t_eol);

    json_str_fill (buf, iw, ' ');
    buf += iw;

    size -= cstrlen (t_eol) + iw;
  }

  st->indent_width = iw;
#else
  if (size == 0)
  {
    st->need = 1u;
    return 1;
  }
#endif

  *buf++ = ']' | (obj << 5);
  size--;

  st->flags = (!obj_upper << json_flags_coll_bit)
  | (obj_upper << json_flags_key_bit)
  | (!level << json_flags_done_bit);

  st->level = level;

  st->buf = buf;
  st->size = size;

  return 0;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
