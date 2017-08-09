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
  size_t len = (size_t)(o - p);

#if JSON(STREAM)

  u8* b = jsnp->buf;

  // Part of the string is already buffered
  if (unlikely (b != null))
  {
    // Assemble the complete string
    size_t sz = jsnp->used;

    json_buf_grow (b, len + 1u);

    if (likely (json_flags_key (state)))
    {
      json_str_copy (b + sz, buf, len);

      // Get the actual string data
      buf = b + sizeof (json_istr_t);
      len += sz - sizeof (json_istr_t);

      goto string_key;
    }
    else
    {
      str_copy (b + sz, buf, len);

      buf = b + sizeof (json_istr_t);
      len += sz - sizeof (json_istr_t);

      goto string_value;
    }
  }
  else
  {
    // Allocate the string buffer with space reserved
    // in the beginning for the string object
    b = mem_pool_alloc (jsnp->pool, sizeof (json_istr_t) + len + 1u);
    if (unlikely (b == null)) json_error (JSON_ERROR_MEMORY);

    if (likely (json_flags_key (state)))
    {
      json_str_copy (b + sizeof (json_istr_t), buf, len);
      buf = b + sizeof (json_istr_t);

string_key:
      // Null-terminate the string
      buf[len] = '\0';

      // Check if the key string is too long to be hashed
  #if JSON(HASH_KEYS)
      if (unlikely (len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      // Set the key value
      json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);

      // Get the reserved key object
      kv->ukey.ikey = (json_ikey_t*)b;
      kv->ukey.ikey->hash = json_hash_key (buf, len);

      kv->ukey.tag |= json_str_pack;

      state = flag_clr (state, json_flag_key);

      json_break (json_state_colon, 0);
    }
    else
    {
      str_copy (b + sizeof (json_istr_t), buf, len);
      buf = b + sizeof (json_istr_t);

string_value:
      buf[len] = '\0';

      // Set the string value
      json_elmnt_t* elmnt = jsnp->elmnt;

      elmnt->box.tag |= json_val_str;

      // Get the reserved string object
      elmnt->val.istr = (json_istr_t*)b;
      elmnt->val.istr->len = len;
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
    // Set the packed key string
    if (likely (json_flags_key (state)))
    {
  #if JSON(HASH_KEYS)
      if (unlikely (len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      json_str_copy (b + sizeof (json_istr_t), buf, len);
      b[sizeof (json_istr_t) + len] = '\0';

      json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);

      kv->ukey.ikey = (json_ikey_t*)b;
      kv->ukey.ikey->hash = json_hash_key (buf, len);

      kv->ukey.tag |= json_str_pack;

      state = flag_clr (state, json_flag_key);

      json_break (json_state_colon, 0);
    }

    // Set the packed value string
    else
    {
      json_str_copy (b + sizeof (json_istr_t), buf, len);
      b[sizeof (json_istr_t) + len] = '\0';

      json_elmnt_t* elmnt = jsnp->elmnt;

      elmnt->box.tag |= json_val_str;

      elmnt->val.istr = (json_istr_t*)b;
      elmnt->val.istr->len = len;

      elmnt->val.tag |= json_str_pack;
    }
  }

  // String is too long to be packed
  else
  {
    // Null-terminate the string
    buf[len] = '\0';

    // Get the string object
    json_str_t* str = (json_str_t*)b;

    // Set the string buffer
    str->buf = buf;

    // String is a key
    if (likely (json_flags_key (state)))
    {
      // Check if the key string is too long to be hashed
  #if JSON(HASH_KEYS)
      if (unlikely (len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

      // Hash the key string
      json_key_t* key = (json_key_t*)str;
      key->hash = json_hash_key (buf, len);

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
      // Set the string value length
      str->len = len;

      // Set the string value
      json_elmnt_t* elmnt = jsnp->elmnt;

      elmnt->box.tag |= json_val_str;
      elmnt->val.str = str;
    }
  }

#endif // !JSON(STREAM)
}
