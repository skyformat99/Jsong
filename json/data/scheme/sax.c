// =============================================================================
// <data/scheme/sax.c>
//
// USON SAX API data string scheme handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the data string scheme
  u8* buf = (u8*)p;
  size_t len = j - p;

#if JSON(STREAM)
  // Part of the data string scheme is already buffered
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

  // Check if specified encoding scheme is supported
  if (unlikely (len != 0))
  {
    if ((len != cstrlen ("base64uson"))
    || !json_str_equal (buf, "base64uson", cstrlen ("base64uson")))
    {
      json_error (JSON_ERROR_TOKEN);
    }
  }

  jsnp->st.data.scheme = 0;
}
