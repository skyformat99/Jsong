// =============================================================================
// <verbatim/id/dom.c>
//
// USON DOM API verbatim string identifier handling.
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

  // Save it
  jsnp->st.verb.id = buf;
  jsnp->st.verb.len_id = len;
}
