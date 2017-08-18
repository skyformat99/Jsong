// =============================================================================
// <string/dom.c>
//
// JSON DOM API string handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the unescaped string
  u8* buf = (u8*)p;
  size_t len = o - p;

#if JSON(STREAM)

  json_istr_t* str;
  u8* b = jsnp->buf;

  // Part of the string is already buffered
  if (unlikely (b != null))
  {
    // Assemble the complete string
    size_t sz = jsnp->used;

    json_buf_grow (b, len + 1u);
    str = (json_istr_t*)b;

    // Get the actual string buffer pointer & length
    if (likely (json_flags_key (state)))
    {
      json_str_copy (b + sz, buf, len);
      str->len = sz - sizeof (json_istr_t) + len;

      goto string_key;
    }
    else
    {
      str_copy (b + sz, buf, len);
      str->len = sz - sizeof (json_istr_t) + len;

      goto string_value;
    }
  }
  else
  {
    // Allocate the string buffer with space reserved
    // in the beginning for the string object
    b = mem_pool_alloc (jsnp->pool, sizeof (json_istr_t) + len + 1u);
    if (unlikely (b == null)) json_error (JSON_ERROR_MEMORY);

    str = (json_istr_t*)b;
    str->len = len;

    if (likely (json_flags_key (state)))
    {
      json_str_copy (str->buf, buf, len);

string_key:
      // Null-terminate the string
      str->buf[str->len] = '\0';

  #if JSON(HASH_KEYS)
      // Check if the key string is too long to be hashed
      if (unlikely (str->len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      // Set the key value
      json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);

      kv->ukey.ikey = (json_ikey_t*)str;
      kv->ukey.ikey->hash = json_hash_key (str->buf, str->len);

      kv->ukey.tag |= json_str_pack;

      state = flag_clr (state, json_flag_key);

      json_break (json_state_colon, 0);
    }
    else
    {
      str_copy (str->buf, buf, len);

string_value:
      str->buf[str->len] = '\0';

      // Set the string value
      json_elmnt_t* elmnt = jsnp->elmnt;

      elmnt->box.tag |= json_val_str;

      elmnt->val.istr = str;
      elmnt->val.tag |= json_str_pack;
    }
  }

#else // JSON(STREAM) ][

  // Check if the string can be packed
#if JSON(PACK_STRINGS)
  bool pack = (len < JSON_PSTR_SIZE);
#else
  const bool pack = false;
#endif

  // Allocate the string object
  u8* b = mem_pool_alloc (jsnp->pool, pack
  ? (sizeof (json_istr_t) + JSON_PSTR_SIZE)
  : sizeof (json_str_t));

  if (unlikely (b == null)) json_error (JSON_ERROR_MEMORY);

  // String can be packed
  if (likely (pack))
  {
    json_istr_t* str = (json_istr_t*)b;

    str->len = len;
    json_str_copy (str->buf, buf, len);
    str->buf[str->len] = '\0';

    // Set the packed key string
    if (likely (json_flags_key (state)))
    {
  #if JSON(HASH_KEYS)
      if (unlikely (str->len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);

      kv->ukey.ikey = (json_ikey_t*)str;
      kv->ukey.ikey->hash = json_hash_key (str->buf, str->len);

      kv->ukey.tag |= json_str_pack;

      state = flag_clr (state, json_flag_key);

      json_break (json_state_colon, 0);
    }

    // Set the packed value string
    else
    {
      json_elmnt_t* elmnt = jsnp->elmnt;

      elmnt->box.tag |= json_val_str;

      elmnt->val.istr = str;
      elmnt->val.tag |= json_str_pack;
    }
  }

  // String is too long to be packed
  else
  {
    // Set the string buffer
    json_str_t* str = (json_str_t*)b;

    str->buf = buf;
    str->len = len;

    str->buf[str->len] = '\0';

    // String is a key
    if (likely (json_flags_key (state)))
    {
  #if JSON(HASH_KEYS)
      // Hash the key string
      if (unlikely (str->len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      json_key_t* key = (json_key_t*)str;
      key->hash = json_hash_key (str->buf, str->len);

      // Set up the property object
      json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);
      kv->ukey.key = key;

      // Unset the key flag
      state = flag_clr (state, json_flag_key);

      json_break (json_state_colon, 0);
    }

    // String is a value
    else
    {
      json_elmnt_t* elmnt = jsnp->elmnt;

      elmnt->box.tag |= json_val_str;
      elmnt->val.str = str;
    }
  }

#endif // !JSON(STREAM)
}
