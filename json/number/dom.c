// =============================================================================
// <number/dom.c>
//
// JSON DOM API number string handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  void* ptr;

#if !JSON(STREAM)
  bint pack;
#endif

  // Get the number string
  u8* buf = (u8*)p;
  size_t len = (size_t)(j - p);

#if JSON(EXPLICIT)
  #if JSON(STREAM)
    u8* b = jsnp->buf;

    // Part of the number string is already buffered
    if (unlikely (b != null))
    {
      // Assemble the complete number string
      size_t sz = jsnp->used;

      json_buf_grow (b, len + 1u);
      json_str_copy (b + sz, buf, len);

      buf = b + sizeof (json_num_istr_t);
      len += sz - sizeof (json_num_istr_t);

      ptr = b;
    }
    else
  #else
    // This is needed to obtain the terminating null
    // when the number happens to be a root element
    if (unlikely (json_flags_root (state)))
  #endif
    {
      // Allocate the number string object
      json_num_istr_t* str = mem_pool_alloc (jsnp->pool, sizeof (json_num_istr_t) + len + 1u);
      if (unlikely (str == null)) json_error (JSON_ERROR_MEMORY);

      json_str_copy (str->buf, buf, len);

      buf = str->buf;
      ptr = str;

  #if !JSON(STREAM)
      pack = true;
  #endif
    }
  #if !JSON(STREAM)
    else
  #endif
#elif !JSON(STREAM)
    {
      // Check if the string can be packed
  #if JSON(PACK_STRINGS)
      pack = (len < JSON_PSTR_SIZE);
  #else
      pack = false;
  #endif

      // Allocate the number string object
      ptr = mem_pool_alloc (jsnp->pool, pack
      ? (sizeof (json_istr_t) + JSON_PSTR_SIZE)
      : sizeof (json_str_t));

      if (unlikely (ptr == null)) json_error (JSON_ERROR_MEMORY);

      if (likely (pack))
      {
        json_num_istr_t* str = ptr;
        json_str_copy (str->buf, buf, len);
        buf = str->buf;
      }
      else
      {
        json_num_str_t* str = ptr;
        str->buf = (u8*)buf;
      }
    }
#endif

  // Null-terminate
  buf[len] = '\0';

  // Set the total number length
  meta.len_number = len;

  // Set the number type
  json_elmnt_t* elmnt = jsnp->elmnt;
  elmnt->box.tag |= json_type_num;

  // Set the number string value
  elmnt->val.ptr = ptr;
  ((json_num_istr_t*)ptr)->meta = meta;

  // Set the number string type
#if JSON(STREAM)
  elmnt->val.tag |= json_str_pack | json_num_str;
#else
  elmnt->val.tag |= pack | json_num_str;
#endif
}
