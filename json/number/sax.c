// =============================================================================
// <number/sax.c>
//
// JSON SAX API number string handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the number string
  u8* buf = (u8*)p;
  size_t len = (size_t)(j - p);

#if JSON(EXPLICIT)
  // This is needed to obtain the terminating null
  // when the number happens to be a root element
  if (unlikely (json_flags_root (state)
  #if JSON(STREAM)
    || (jsnp->used != 0)
  #endif
  ))
  {
    // Assemble the complete number string
    u8* b = jsnp->buf;
    size_t sz = jsnp->used;

    json_buf_grow (&b, len + 1u);
    json_str_copy (b + sz, buf, len);
    jsnp->used = 0;

    buf = b;
    len += sz;
  }
#endif

  // Null-terminate
  buf[len] = '\0';

  // Set the total number length
  meta.len_number = len;

  // Set the number string value
  jsax_num_t num;
  json_num_str_t str;

  str.buf = buf;
  str.meta = meta;

  num.val.str = &str;
  num.tag = json_num_str;

  // Execute the number event handler
  jsax_callback (jsnp->on_num, jsnp, num);
}
