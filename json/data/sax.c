// =============================================================================
// <data/sax.c>
//
// USON SAX API data string handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the data string
  u8* buf = (u8*)p;
  size_t len = o - p;

#if JSON(STREAM)
  // Part of the data string is already buffered
  if (unlikely (jsnp->used != 0))
  {
    // Assemble the complete string
    u8* b = jsnp->buf;
    size_t sz = jsnp->used;

    json_buf_grow (&b, len + 1u);
    str_copy (b + sz, buf, len);
    jsnp->used = 0;

    buf = b;
    len += sz;
  }
#endif

  // Null-terminate the data string
  buf[len] = '\0';

  // Set the data string value
  jsax_str_t str;

  str.len = len;
  str.buf = buf;

  // Execute the string value event handler
  jsax_callback (jsnp->on_str, jsnp, str);
}
