// =============================================================================
// <string/sax.c>
//
// JSON SAX API string handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the unescaped string
  u8* buf = (u8*)p;
  size_t len = (size_t)(o - p);

#if JSON(STREAM)
  if (unlikely (jsnp->used != 0))
  {
    // Assemble the complete string
    u8* b = jsnp->buf;
    size_t sz = jsnp->used;

    json_buf_grow (b, len + 1u);
    str_copy (b + sz, buf, len);
    jsnp->used = 0;

    buf = b;
    len += sz;
  }
#endif

  // Null-terminate
  buf[len] = '\0';

  // String is a property key
  if (json_flags_key (state))
  {
#if JSON(HASH_KEYS)
    // Check if the key string is too long to be hashed
    if (unlikely (len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
#endif

    // Set the key string
    jsax_key_t key;

    key.buf = buf;
    key.hash = json_hash_key (buf, len);

    // Execute the key string event handler
    jsax_callback (jsnp->on_key, jsnp, key);

    // Unset the key flag
    state = flag_clr (state, json_flag_key);

    json_break (json_state_colon, 0);
  }

  // String is a value
  else
  {
    // Set the value string
    jsax_str_t str;

    str.buf = buf;
    str.len = len;

    // Execute the value string event handler
    jsax_callback (jsnp->on_str, jsnp, str);
  }
}
