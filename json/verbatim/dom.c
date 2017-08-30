// =============================================================================
// <verbatim/dom.c>
//
// USON DOM API verbatim string handling.
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
  u8* b = jsnp->buf;
  size_t sz = jsnp->used;

  json_buf_grow (&b, len + 1u);
  str_copy (b + sz, buf, len);

  buf = b;
  len += sz;
#else
  buf -= l;
  len += l;
#endif

  // Null-terminate both strings
  buf[l] = '\0';
  buf[len] = '\0';

  // Set the verbatim string value
  json_elmnt_t* elmnt = jsnp->elmnt;
  uson_str_verb_t* str = elmnt->val.verb;

  str->id.len = l;
  str->id.buf = buf;

  str->data.len = len - (l + 1u);
  str->data.buf = buf + (l + 1u);

#if JSON(STREAM)
  str->data.len--;
  str->data.buf++;
#endif

  elmnt->box.tag |= json_val_str;
  elmnt->val.tag |= uson_str_verb;
}
