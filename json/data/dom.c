// =============================================================================
// <data/dom.c>
//
// USON DOM API data string handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the data string
  u8* buf = (u8*)p;
  size_t len = o - p;

  // Get the saved MIME type length
  size_t len_mime = jsnp->st.data.len_mime;

#if JSON(STREAM)
  // MIME type is already buffered
  u8* b = jsnp->buf;
  size_t sz = jsnp->used;

  json_buf_grow (&b, len + 1u);
  str_copy (b + sz, buf, len);

  buf = b;
  len += sz;
#else
  buf -= len_mime + 1u;
  len += len_mime + 1u;
#endif

  // Null-terminate both strings
  buf[len_mime] = '\0';
  buf[len] = '\0';

  // Set the data string value
  json_elmnt_t* elmnt = jsnp->elmnt;
  uson_str_data_t* str = elmnt->val.data;

  str->scheme = jsnp->st.data.scheme;

  str->mime.len = len_mime;
  str->mime.buf = buf;

  str->data.len = len - (len_mime + 1u);
  str->data.buf = buf + (len_mime + 1u);

  elmnt->box.tag |= json_val_str;
  elmnt->val.tag |= uson_str_data;
}
