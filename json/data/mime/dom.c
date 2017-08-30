// =============================================================================
// <data/mime/dom.c>
//
// USON DOM API data string mime type handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the data string mime type
  u8* buf = (u8*)p;
  size_t len = j - p;

#if JSON(STREAM)
  // Buffer the MIME type
  u8* b = jsnp->buf;
  size_t sz = jsnp->used;

  json_buf_grow (&b, len + 1u);
  json_str_copy (b + sz, buf, len);

  buf = b;
  len += sz;
#endif

  // Save its length
  jsnp->st.data.len_mime = len;
}
