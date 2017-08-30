// =============================================================================
// <identifier/sax.c>
//
// USON SAX API identifier handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the identifier string
  u8* buf = (u8*)p;
  size_t len = j - p;

#if JSON(EXPLICIT)
  // This is needed to obtain the terminating null
  // when the number happens to be a root element
  if (unlikely (json_flags_root (state)
  #if JSON(STREAM)
    || (jsnp->used != 0)
  #endif
  ))
  {
    u8* b = jsnp->buf;
    size_t sz = jsnp->used;

    json_buf_grow (&b, len + 1u);
    json_str_copy (b + sz, buf, len);
    jsnp->used = 0;

    buf = b;
    len += sz;
  }
#endif

  buf[len] = '\0';

  if (json_flags_key (state))
  {
#if JSON(HASH_KEYS)
    if (unlikely (len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
#endif

    jsax_key_t key;

    key.buf = buf;
    key.hash = json_hash_key (buf, len);

    jsax_callback (jsnp->on_key, jsnp, key);

    state = flag_clr (state, json_flag_key);
    json_state (json_state_colon);

    goto case_json_state_colon;
  }
  else
  {
    #include "primitives.c"

    jsax_str_t str;

    str.len = len;
    str.buf = buf;

    jsax_callback (jsnp->on_str, jsnp, str);
  }
}
