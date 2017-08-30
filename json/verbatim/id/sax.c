// =============================================================================
// <verbatim/id/sax.c>
//
// USON SAX API verbatim string identifier handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the verbatim string identifier
  u8* buf = (u8*)p;
  size_t len = j - p;

#if JSON(STREAM)
  // Always buffer identifier string for comparison
  u8* b = jsnp->buf;
  size_t sz = jsnp->used;

  json_buf_grow (&b, len + 1u);
  json_str_copy (b + sz, buf, len);

  buf = b;
  len += sz;
#endif

  // Null-terminate the verbatim string identifier
  buf[len] = '\0';

  // Set the verbatim string identifier
  jsax_str_t str;

  str.len = len;
  str.buf = buf;

  // Execute the verbatim string identifier event handler
  jsax_callback (jsnp->on_id, jsnp, str);

#if !JSON(STREAM)
  // Restore the new line character
  buf[len] = '\n';
#endif

  // Save it
  jsnp->st.verb.id = buf;
  jsnp->st.verb.len_id = len;
}
