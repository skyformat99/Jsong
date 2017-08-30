// =============================================================================
// <serialize/dom/template.c>
//
// DOM API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  u8* p;
  size_t sz;

  u8* str_buf;
  size_t str_len;

#if ENABLED(USON)
  uson_str_verb_t* verb;
  uson_str_data_t* data;
#endif

  json_item_t* end;
  json_elmnt_t* stop = json_get_elmnt (st->root);

  json_node_t node = st->node;
  json_coll_type_t kind = node.tag & json_coll_type;
  json_elmnt_t* elmnt = json_get_elmnt (node);

  u8* b = st->buf + st->fill;
  const u8* e = st->buf + st->size;

#if T(PRETTY)
  uint isz = st->indent_size;
  uint iw = st->indent_width;
#endif

  // Restore the last item
  if ((node.ptr != st->root.ptr) && (kind == json_coll_arr))
  {
    json_item_t* item = json_as_item (elmnt);
    json_arr_t* arr = json_item_arr (item);

    end = arr->items + arr->num;
  }

  // Terminate the current element
  if (unlikely (st->term))
  {
    st->term = false;
    goto element_continue;
  }

  // Finish partial key or value string
  if (unlikely ((p = st->pos) != null))
  {
#if ENABLED(USON)
    if (st->verb != null)
    {
      verb = st->verb;

      str_buf = st->pos;
      str_len = st->len;

      goto verb_continue;
    }

    if (st->data)
    {
      str_buf = st->pos;
      str_len = st->len;

      goto data_continue;
    }
#endif

#if T(PRETTY)
    const size_t chrs_min = 3u;
#else
    const size_t chrs_min = 2u;
#endif

    if (unlikely ((size_t)(e - b) < chrs_min)) goto needspace;

    u8* buf = p;

    int ret = json_prefix (escape) (buf, st->len, b
    , (size_t)(e - b) - chrs_min, &p, &sz);

    b += sz;

    if (unlikely (ret != 0))
    {
      st->need = (size_t)ret + chrs_min;

      st->pos = p;
      st->len = st->len - (size_t)(p - buf);

      goto needspace;
    }

    st->pos = null;

    if (st->key)
    {
      st->key = false;

      b[0] = '"';
      b[1] = ':';

#if T(PRETTY)
      b[2] = ' ';
      b += !json_is_coll (elmnt);
#endif

      b += 2;

      goto element_serialize;
    }
    else
    {
      *b++ = '"';
      goto element_next;
    }
  }

  // Begin key string
  if (unlikely (st->key))
  {
    st->key = false;
    goto key_serialize;
  }

  // Serialize the JSON element
element_serialize:
  if (json_is_str (elmnt))
  {
#if ENABLED(USON)
    // Verbatim string
    if (uson_is_str_verb (elmnt))
    {
      verb = uson_get_str_verb (elmnt);

      const size_t chrs_min = 1u // '('
      + 1u // '!'
      + verb->id.len
      + cstrlen (t_eol)
      + cstrlen (t_eol)
      + 1u // '!'
      + verb->id.len
      + 1u // ')'
      ;

      if (unlikely ((size_t)(e - b) < chrs_min))
      {
        st->need = chrs_min - (size_t)(e - b);
        goto needspace;
      }

      b[0] = '(';
      b[1] = '!';
      b += 2;

      json_str_copy (b, verb->id.buf, verb->id.len);
      b += verb->id.len;

      json_str_copy (b, t_eol, cstrlen (t_eol));
      b += cstrlen (t_eol);

      if (verb->data.len == 0)
      {
        *b++ = '!';

        json_str_copy (b, verb->id.buf, verb->id.len);
        b += verb->id.len;

        *b++ = ')';
      }
      else
      {
        size_t max;

        str_buf = verb->data.buf;
        str_len = verb->data.len;

verb_continue:
        max = (size_t)(e - b) - (cstrlen (t_eol) + 2u + verb->id.len);
        sz = min2 (str_len, max);

        str_copy (b, str_buf, sz);

        b += sz;

        if (str_len > max)
        {
          st->verb = verb;
          st->need = (str_len - max) + (cstrlen (t_eol) + 2u + verb->id.len);

          st->pos = str_buf + sz;
          st->len = str_len - sz;

          goto needspace;
        }

        json_str_copy (b, t_eol, cstrlen (t_eol));
        b += cstrlen (t_eol);

        *b++ = '!';

        json_str_copy (b, verb->id.buf, verb->id.len);
        b += verb->id.len;

        *b++ = ')';
      }
    }

    // Encoded data string
    else if (uson_is_str_data (elmnt))
    {
      data = uson_get_str_data (elmnt);

      size_t chrs_min = 1u // '('
      + 1u // '?'
      + 1u // ':'
      + data->mime.len
      + 1u // '?'
      + 1u // ')'
      ;

  #if T(PRETTY)
      chrs_min = chrs_min
      + cstrlen (t_eol)
      - 1u // ')' (handled differently)
      ;
  #endif

      if (unlikely ((size_t)(e - b) < chrs_min))
      {
        st->need = chrs_min - (size_t)(e - b);
        goto needspace;
      }

      *b++ = '(';

      if (data->mime.len != 0)
      {
        b[0] = '?';
        b[1] = ':';
        b += 2;

        json_str_copy (b, data->mime.buf, data->mime.len);
        b += data->mime.len;

        *b++ = '?';
      }

  #if T(PRETTY)
      json_str_copy (b, t_eol, cstrlen (t_eol));
      b += cstrlen (t_eol);
  #endif

      str_buf = data->data.buf;
      str_len = data->data.len;

data_continue:
  #if T(PRETTY)
      // Break after every 80th character
      if (str_len > 60u)
      {
        if (unlikely ((size_t)(e - b) < (80u + cstrlen (t_eol))))
        {
          st->data = true;
          st->need = (80u + cstrlen (t_eol)) - (size_t)(e - b);

          st->pos = str_buf;
          st->len = str_len;

          goto needspace;
        }

        // Guaranteed to succeed
        uson_encode (str_buf, 60u, b, 80u, &p, &sz);

        b += 80;

        json_str_copy (b, t_eol, cstrlen (t_eol));
        b += cstrlen (t_eol);

        str_buf += 60;
        str_len -= 60u;

        goto data_continue;
      }

      // Last line
      else
      {
        if (unlikely ((size_t)(e - b) < ((80u + cstrlen (t_eol)) + iw + 1u)))
        {
          st->data = true;
          st->need = ((80u + cstrlen (t_eol)) + iw + 1u) - (size_t)(e - b);

          st->pos = str_buf;
          st->len = str_len;

          goto needspace;
        }

        // Guaranteed to succeed
        uson_encode (str_buf, str_len, b, 80u, &p, &sz);
        b += sz;

        json_str_copy (b, t_eol, cstrlen (t_eol));
        b += cstrlen (t_eol);

        json_str_fill (b, iw, ' ');
        b += iw;

        *b++ = ')';
      }
  #else // T(PRETTY) ][
      {
        int ret = uson_encode (str_buf, str_len, b
        , (size_t)(e - b) - 1u, &p, &sz);

        b += sz;

        if (unlikely (ret != 0))
        {
          st->data = true;
          st->need = (size_t)ret + 1u;

          st->pos = p;
          st->len = str_len - (size_t)(p - str_buf);

          goto needspace;
        }

        *b++ = ')';
      }
  #endif // !T(PRETTY)
    }

    // Regular JSON string
    else
#endif
    {
      if (unlikely ((size_t)(e - b) < 2u))
      {
        st->need = 2u - (size_t)(e - b);
        goto needspace;
      }

      json_str_t str = json_get_str (elmnt);

      *b++ = '"';

      int ret = json_prefix (escape) (str.buf, str.len, b
      , (size_t)(e - b) - 1u, &p, &sz);

      b += sz;

      if (unlikely (ret != 0))
      {
        st->need = (size_t)ret + 1u;

        st->pos = p;
        st->len = str.len - (size_t)(p - str.buf);

        goto needspace;
      }

      *b++ = '"';
    }
  }
  elif (json_is_int (elmnt))
  {
    if (unlikely ((size_t)(e - b) < (1u + INTMAX_DIG)))
    {
      st->need = (1u + INTMAX_DIG) - (size_t)(e - b);
      goto needspace;
    }

    b += int_smax_to_str (b, json_get_int (elmnt));
  }
  elif (json_is_uint (elmnt))
  {
    if (unlikely ((size_t)(e - b) < UINTMAX_DIG))
    {
      st->need = UINTMAX_DIG - (size_t)(e - b);
      goto needspace;
    }

    b += int_umax_to_str (b, json_get_uint (elmnt));
  }
  elif (json_is_flt (elmnt))
  {
    u8 buf[JSON_FLT_BUF_SIZE];

    size_t l = snprintf (buf, numof (buf), "%.*g"
    , (uint)(st->flt_dig), json_get_flt (elmnt));

    if (unlikely ((size_t)(e - b) < l))
    {
      st->need = l - (size_t)(e - b);
      goto needspace;
    }

    json_str_copy (b, buf, l);

    b += l;
  }
  elif (json_is_obj (elmnt))
  {
#if !T(PRETTY)
    if (unlikely ((size_t)(e - b) < 2u))
    {
      st->need = 2u - (size_t)(e - b);
      goto needspace;
    }

    *b++ = '{';
#endif

    json_obj_t* obj = json_get_obj (elmnt);

    // Fix the upper node link on-the-fly
    obj->node.elmnt = elmnt;
    obj->node.tag |= kind;

    json_kv_t* kv = obj->first;

    if (unlikely (kv == null))
    {
#if T(PRETTY)
      if (unlikely ((size_t)(e - b) < 3u))
      {
        st->need = 3u - (size_t)(e - b);
        goto needspace;
      }

      if ((kind == json_coll_obj) && (iw != 0))
      {

        b[0] = ' ';
        b[1] = '{';
        b[2] = '}';
        b += 3;
      }
      else
      {
        b[0] = '{';
        b[1] = '}';
        b += 2;
      }
#else
      *b++ = '}';
#endif
    }
    else
    {
#if T(PRETTY)
      if (unlikely ((size_t)(e - b) < (1u + (iw * 2u) + isz + (cstrlen (t_eol) * 2u))))
      {
        st->need = (1u + (iw * 2u) + isz + (cstrlen (t_eol) * 2u)) - (size_t)(e - b);
        goto needspace;
      }

      if ((kind == json_coll_obj) && (iw != 0))
      {
        json_str_copy (b, t_eol, cstrlen (t_eol));
        b += cstrlen (t_eol);

        json_str_fill (b, iw, ' ');
        b += iw;
      }

      *b++ = '{';
      iw += isz;

      json_str_copy (b, t_eol, cstrlen (t_eol));
      b += cstrlen (t_eol);

      json_str_fill (b, iw, ' ');
      b += iw;
#endif

      json_key_t key;

      kind = json_coll_obj;
      elmnt = json_as_elmnt (kv);

key_serialize:;
#if T(PRETTY)
      const size_t chrs_min = 4u;
#else
      const size_t chrs_min = 3u;
#endif

      if (unlikely ((size_t)(e - b) < chrs_min))
      {
        st->need = chrs_min - (size_t)(e - b);
        st->key = true;

        goto needspace;
      }

      *b++ = '"';

      key = json_get_key (json_as_kv (elmnt));

      int ret = json_prefix (escape) (key.buf, json_hash_len (key.hash)
      , b, (size_t)(e - b) - (chrs_min - 1u), &p, &sz);

      b += sz;

      if (unlikely (ret != 0))
      {
        st->need = (size_t)ret + (chrs_min - 1u);

        st->pos = p;
        st->len = json_hash_len (key.hash) - (size_t)(p - key.buf);
        st->key = true;

        goto needspace;
      }

      b[0] = '"';
      b[1] = ':';

#if T(PRETTY)
      b[2] = ' ';
      b += !json_is_coll (elmnt);
#endif

      b += 2;

      goto element_serialize;
    }
  }
  elif (json_is_arr (elmnt))
  {
#if !T(PRETTY)
    if (unlikely ((size_t)(e - b) < 2u))
    {
      st->need = 2u - (size_t)(e - b);
      goto needspace;
    }

    *b++ = '[';
#endif

    json_arr_t* arr = json_get_arr (elmnt);

    arr->node.elmnt = elmnt;
    arr->node.tag |= kind;

    if (unlikely (arr->num == 0))
    {
#if T(PRETTY)
      if (unlikely ((size_t)(e - b) < 3u))
      {
        st->need = 3u - (size_t)(e - b);
        goto needspace;
      }

      if ((kind == json_coll_obj) && (iw != 0))
      {
        b[0] = ' ';
        b[1] = '[';
        b[2] = ']';
        b += 3;
      }
      else
      {
        b[0] = '[';
        b[1] = ']';
        b += 2;
      }
#else
      *b++ = ']';
#endif
    }
    else
    {
#if T(PRETTY)
      if (unlikely ((size_t)(e - b) < (1u + (iw * 2u) + isz + (cstrlen (t_eol) * 2u))))
      {
        st->need = (1u + (iw * 2u) + isz + (cstrlen (t_eol) * 2u)) - (size_t)(e - b);
        goto needspace;
      }

      if ((kind == json_coll_obj) && (iw != 0))
      {
        json_str_copy (b, t_eol, cstrlen (t_eol));
        b += cstrlen (t_eol);

        json_str_fill (b, iw, ' ');
        b += iw;
      }

      *b++ = '[';
      iw += isz;

      json_str_copy (b, t_eol, cstrlen (t_eol));
      b += cstrlen (t_eol);

      json_str_fill (b, iw, ' ');
      b += iw;
#endif

      kind = json_coll_arr;
      elmnt = json_as_elmnt (arr->items);
      end = arr->items + arr->num;

      goto element_serialize;
    }
  }
  elif (json_is_bool (elmnt))
  {
    bool val = json_get_bool (elmnt);

    if (val)
    {
      if (unlikely ((size_t)(e - b) < 4u))
      {
        st->need = 4u - (size_t)(e - b);
        goto needspace;
      }

      buf_set4 (b, 't', 'r', 'u', 'e');

      b += 4;
    }
    else
    {
      if (unlikely ((size_t)(e - b) < 5u))
      {
        st->need = 5u - (size_t)(e - b);
        goto needspace;
      }

      buf_set5 (b, 'f', 'a', 'l', 's', 'e');

      b += 5;
    }
  }
  elif (json_is_null (elmnt))
  {
    if (unlikely ((size_t)(e - b) < 4u))
    {
      st->need = 4u - (size_t)(e - b);
      goto needspace;
    }

    buf_set4 (b, 'n', 'u', 'l', 'l');

    b += 4;
  }
  elif (json_is_num (elmnt))
  {
#if JSON(BIG_NUMBERS)
    if (json_is_num_big (elmnt))
    {
      if (json_is_num_big_int (elmnt))
      {
        if (unlikely ((size_t)(e - b) < (1u + INT128_DIG)))
        {
          st->need = (1u + INT128_DIG) - (size_t)(e - b);
          goto needspace;
        }

        b += int_s128_to_str (b, json_get_num_big_int (elmnt));
      }
      elif (json_is_num_big_uint (elmnt))
      {
        if (unlikely ((size_t)(e - b) < UINT128_DIG))
        {
          st->need = UINT128_DIG - (size_t)(e - b);
          goto needspace;
        }

        b += int_u128_to_str (b, json_get_num_big_uint (elmnt));
      }
      elif (json_is_num_big_flt (elmnt))
      {
        u8 buf[JSON_BIG_FLT_BUF_SIZE];

  #ifdef FLT128_MAX
        size_t l = quadmath_snprintf (buf, numof (buf), "%.*Qg"
        , (uint)(st->big_flt_dig), json_get_num_big_flt (elmnt));
  #else
    #if (LDBL_BINARY == 80)
      #if OS(WIN32)
        size_t l = __mingw_snprintf (buf, numof (buf), "%.*Lg"
        , (uint)(st->big_flt_dig), json_get_num_big_flt (elmnt));
      #else
        size_t l = snprintf (buf, numof (buf), "%.*Lg"
        , (uint)(st->big_flt_dig), json_get_num_big_flt (elmnt));
      #endif
    #else
        size_t l = snprintf (buf, numof (buf), "%.*Lg"
        , (uint)(st->big_flt_dig), json_get_num_big_flt (elmnt));
    #endif
  #endif

        if (unlikely ((size_t)(e - b) < l))
        {
          st->need = l - (size_t)(e - b);
          goto needspace;
        }

        json_str_copy (b, buf, l);

        b += l;
      }
    }
    else
#endif
    if (json_is_num_str (elmnt))
    {
      json_num_str_t str = json_get_num_str (elmnt);

      if (unlikely ((size_t)(e - b) < str.meta.len_number))
      {
        st->need = str.meta.len_number - (size_t)(e - b);
        goto needspace;
      }

      json_str_copy (b, str.buf, str.meta.len_number);

      b += str.meta.len_number;
    }
  }

element_next:
  // Finished?
  if (unlikely (elmnt == stop)) goto done;

element_continue:
  // Check if there's space for delimiter
#if T(PRETTY)
  if (unlikely ((size_t)(e - b) < (1u + cstrlen (t_eol) + iw)))
#else
  if (unlikely (b == e))
#endif
  {
#if T(PRETTY)
    st->need = (1u + cstrlen (t_eol) + iw) - (size_t)(e - b);
#else
    st->need = 1u;
#endif
    st->term = true;

    goto needspace;
  }

  // Next k/v pair
  if (kind == json_coll_obj)
  {
    json_kv_t* kv = json_as_kv (elmnt);
    elmnt = json_as_elmnt (json_kv_next (kv));

    // Object end
    if (unlikely (elmnt == null))
    {
#if T(PRETTY)
      json_str_copy (b, t_eol, cstrlen (t_eol));
      b += cstrlen (t_eol);

      iw -= isz;
      json_str_fill (b, iw, ' ');
      b += iw;
#endif

      *b++ = '}';

      json_obj_t* obj = json_kv_obj (kv);
      node = obj->node;

      goto element_upper;
    }

#if ENABLED(USON)
    *b++ = ';';
#else
    *b++ = ',';
#endif

#if T(PRETTY)
    json_str_copy (b, t_eol, cstrlen (t_eol));
    b += cstrlen (t_eol);

    json_str_fill (b, iw, ' ');
    b += iw;
#endif

    goto key_serialize;
  }

  // Next array item
  else
  {
    json_item_t* item = json_as_item (elmnt);

    // Array end
    if (unlikely ((item + 1) == end))
    {
#if T(PRETTY)
      json_str_copy (b, t_eol, cstrlen (t_eol));
      b += cstrlen (t_eol);

      iw -= isz;
      json_str_fill (b, iw, ' ');
      b += iw;
#endif

      *b++ = ']';

      json_arr_t* arr = json_item_arr (item);
      node = arr->node;

      goto element_upper;
    }

    elmnt = json_as_elmnt (item + 1);

#if ENABLED(USON)
  #if !T(PRETTY)
    *b++ = ' ';
  #endif
#else
    *b++ = ',';
#endif

#if T(PRETTY)
    json_str_copy (b, t_eol, cstrlen (t_eol));
    b += cstrlen (t_eol);

    json_str_fill (b, iw, ' ');
    b += iw;
#endif

    goto element_serialize;
  }

  // Upper collection
element_upper:
  kind = node.tag & json_coll_type;
  elmnt = json_get_elmnt (node);

  if (kind == json_coll_arr)
  {
    json_arr_t* arr = json_item_arr (json_as_item (elmnt));

    // Reached the root collection
    if (unlikely (arr == null)) goto done;

    end = arr->items + arr->num;
  }

  goto element_next;

  // Success
done:
  st->fill = (size_t)(b - st->buf);
  return true;

  // Not enough space in the buffer
needspace:
  node.elmnt = elmnt;
  node.tag |= kind;

  st->node = node;
  st->fill = (size_t)(b - st->buf);

#if T(PRETTY)
  st->indent_width = iw;
#endif

  return false;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
