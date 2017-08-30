// =============================================================================
// <identifier/dom.c>
//
// USON DOM API identifier handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the identifier string
  u8* buf = (u8*)p;
  size_t len = j - p;

#if !JSON(STREAM)
  if (unlikely (!json_flags_key (state)))
  {
    #include "primitives.c"
  }
#endif

  u8* b;

#if JSON(EXPLICIT)
  #if JSON(STREAM)

    json_istr_t* str;
    b = jsnp->buf;

    if (unlikely (b != null))
    {
      size_t sz = jsnp->used;

      size_t sz_fix = 0;

      if (sizeof (json_num_meta_t) < sizeof (json_istr_t))
      {
        if (!json_flags_key (state)) sz_fix = sizeof (json_istr_t) - sizeof (json_num_meta_t);
      }

      json_buf_grow (&b, sz_fix + len + 1u);
      json_str_copy (b + sz, buf, len);

      str = (json_istr_t*)b;

      if (likely (json_flags_key (state)))
      {
        str->len = sz - sizeof (json_istr_t) + len;
        str->buf[str->len] = '\0';

        goto ident_key;
      }

      len = sz - sizeof (json_num_meta_t) + len;

      if (sizeof (json_num_meta_t) > sizeof (json_istr_t))
      {
        json_str_move_left (b + sizeof (json_istr_t), b + sizeof (json_num_meta_t), len);
      }
      else if (sizeof (json_num_meta_t) < sizeof (json_istr_t))
      {
        json_str_move_right (b + sizeof (json_istr_t), b + sizeof (json_num_meta_t), len);
      }

      str->len = len;
      str->buf[str->len] = '\0';

      buf = str->buf;

      #define T_FREE

      #include "primitives.c"

      goto ident_value;
    }
    else

  #else // JSON(STREAM) ][

    // This is needed to obtain the terminating null
    // when the identifier happens to be a root element
    if (unlikely (json_flags_root (state)))

  #endif // !JSON(STREAM)
    {
  #if JSON(STREAM)
      if (unlikely (!json_flags_key (state)))
      {
        #include "primitives.c"
      }
  #endif

      b = mem_pool_alloc (jsnp->pool, sizeof (json_istr_t) + len + 1u);
      if (unlikely (b == null)) json_error (JSON_ERROR_MEMORY);

  #if JSON(STREAM)
      str = (json_istr_t*)b;
      json_str_copy (str->buf, buf, len);

      str->len = len;
      str->buf[str->len] = '\0';

      if (likely (json_flags_key (state)))
      {
ident_key:
    #if JSON(HASH_KEYS)
        if (unlikely (len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
    #endif

        json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);
        kv->ukey.ikey = (json_ikey_t*)str;
        kv->ukey.ikey->hash = json_hash_key (str->buf, str->len);
        kv->ukey.tag |= json_str_pack;

        state = flag_clr (state, json_flag_key);
        json_state (json_state_colon);

        goto case_json_state_colon;
      }
      else
      {
ident_value:
        json_elmnt_t* elmnt = jsnp->elmnt;
        elmnt->box.tag |= json_val_str;
        elmnt->val.istr = str;
        elmnt->val.tag |= json_str_pack;
      }
  #else // JSON(STREAM) ][

      goto ident_value;

  #endif // !JSON(STREAM)
    }

  #if !JSON(STREAM)
    else
  #endif

#elif !JSON(STREAM) // JSON(EXPLICIT) ][
    {
      if (unlikely (!json_flags_key (state)))
      {
        #include "primitives.c"
      }

  #if JSON(PACK_STRINGS)
      bool pack = (len < JSON_PSTR_SIZE);
  #else
      const bool pack = false;
  #endif

      u8* b = mem_pool_alloc (jsnp->pool, pack
      ? (sizeof (json_istr_t) + JSON_PSTR_SIZE)
      : sizeof (json_str_t));

      if (unlikely (b == null)) json_error (JSON_ERROR_MEMORY);

      if (likely (json_flags_key (state)))
      {
  #if JSON(HASH_KEYS)
        if (unlikely (len > JSON_KEY_LEN_MAX)) json_error (JSON_ERROR_MEMORY);
  #endif

        if (likely (pack))
        {
          json_ikey_t* key = (json_ikey_t*)b;
          json_str_copy (key->buf, buf, len);

          key->hash = json_hash_key (key->buf, len);
          key->buf[len] = '\0';

          json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);
          kv->ukey.ikey = key;
          kv->ukey.tag |= json_str_pack;
        }
        else
        {
          json_key_t* key = (json_key_t*)b;

          key->buf = buf;
          key->hash = json_hash_key (key->buf, len);
          key->buf[len] = '\0';

          json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);
          kv->ukey.key = key;
        }

        state = flag_clr (state, json_flag_key);
        json_state (json_state_colon);

        goto case_json_state_colon;
      }
      else
      {
        if (likely (pack))
        {
ident_value:;
          json_istr_t* str = (json_istr_t*)b;
          json_str_copy (str->buf, buf, len);

          str->len = len;
          str->buf[str->len] = '\0';

          json_elmnt_t* elmnt = jsnp->elmnt;
          elmnt->box.tag |= json_val_str;
          elmnt->val.ptr = str;
          elmnt->val.tag |= json_str_pack;
        }
        else
        {
          json_str_t* str = (json_str_t*)b;

          str->buf = buf;
          str->len = len;
          str->buf[str->len] = '\0';

          json_elmnt_t* elmnt = jsnp->elmnt;
          elmnt->box.tag |= json_val_str;
          elmnt->val.str = str;
        }
      }
    }
#endif // !JSON(STREAM)
}
