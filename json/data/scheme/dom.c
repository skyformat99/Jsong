// =============================================================================
// <data/scheme/dom.c>
//
// USON DOM API data string scheme handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the data string scheme
  u8* buf = (u8*)p;
  size_t len = j - p;

#if JSON(STREAM)
  u8* b = jsnp->buf;

  // Part of the data string scheme is already buffered
  if (unlikely (b != null))
  {
    // Assemble the complete string
    size_t sz = jsnp->used;

    json_buf_grow (&b, len + 1u);
    json_str_copy (b + sz, buf, len);

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
