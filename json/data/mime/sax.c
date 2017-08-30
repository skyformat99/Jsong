// =============================================================================
// <data/mime/sax.c>
//
// USON SAX API data string mime type handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the data string mime type
  u8* buf = (u8*)p;
  size_t len = j - p;

#if JSON(STREAM)
  // Part of the data string mime type is already buffered
  if (unlikely (jsnp->used != 0))
  {
    // Assemble the complete string
    u8* b = jsnp->buf;
    size_t sz = jsnp->used;

    json_buf_grow (&b, len + 1u);
    json_str_copy (b + sz, buf, len);
    jsnp->used = 0;

    buf = b;
    len += sz;
  }
#endif

  // Null-terminate the data string mime type
  buf[len] = '\0';

  // Set the data string mime type
  jsax_str_t str;

  str.len = len;
  str.buf = buf;

  // Execute the data string mime type event handler
  jsax_callback (jsnp->on_mime, jsnp, str);
}
