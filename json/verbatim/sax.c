// =============================================================================
// <verbatim/sax.c>
//
// USON SAX API verbatim string handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the verbatim string
  u8* buf = (u8*)p;
  size_t len = j - p;

#if JSON(STREAM)
  // Delimiting identifier is already buffered
  if (jsnp->used != (l + 1u))
  {
    u8* b = jsnp->buf;
    size_t sz = jsnp->used;

    json_buf_grow (&b, len + 1u);
    str_copy (b + sz, buf, len);
    jsnp->used = 0;

    buf = b + (l + 1);
    len += sz - (l + 1u);
  }
#endif

  buf++;
  len--;

  // Null-terminate the verbatim string
  buf[len] = '\0';

  // Set the verbatim string value
  jsax_str_t str;

  str.len = len;
  str.buf = buf;

  // Execute the string value event handler
  jsax_callback (jsnp->on_str, jsnp, str);
}
