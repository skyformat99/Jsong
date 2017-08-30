// =============================================================================
// <serialize/sax/number.c>
//
// SAX API JSON serialization template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  #include "delimiter.c"

#if T(PRETTY)
  const size_t key_space = 1u;
#else
  const size_t key_space = 0;
#endif

#if JSON(BIG_NUMBERS)
  if (jsax_is_num_big (num))
  {
    size_t l;

    if (jsax_is_num_big_int (num))
    {
      if (size < (key_space + 1u + INT128_DIG))
      {
        st->need = (key_space + 1u + INT128_DIG) - size;
        return 1;
      }

  #if T(PRETTY)
      *buf = ' ';
      buf += json_flags_obj (flags);
      size -= json_flags_obj (flags);
  #endif

      l = int_s128_to_str (buf, num.val.big->i);
    }
    else if (jsax_is_num_big_uint (num))
    {
      if (size < (key_space + UINT128_DIG))
      {
        st->need = (key_space + UINT128_DIG) - size;
        return 1;
      }

  #if T(PRETTY)
      *buf = ' ';
      buf += json_flags_obj (flags);
      size -= json_flags_obj (flags);
  #endif

      l = int_u128_to_str (buf, num.val.big->u);
    }
    else if (jsax_is_num_big_flt (num))
    {
      u8 temp[JSON_BIG_FLT_BUF_SIZE];

  #ifdef FLT128_MAX
      l = quadmath_snprintf (temp, numof (temp), "%.*Qg"
      , (uint)(st->big_flt_dig), num.val.big->f);
  #else
    #if (LDBL_BINARY == 80)
      #if OS(WIN32)
        l = __mingw_snprintf (temp, numof (temp), "%.*Lg"
        , (uint)(st->big_flt_dig), num.val.big->f);
      #else
        l = snprintf (temp, numof (temp), "%.*Lg"
        , (uint)(st->big_flt_dig), num.val.big->f);
      #endif
    #endif
  #endif

      if (size < (key_space + l))
      {
        st->need = (key_space + l) - size;
        return 1;
      }

  #if T(PRETTY)
      *buf = ' ';
      buf += json_flags_obj (flags);
      size -= json_flags_obj (flags);
  #endif

      json_str_copy (buf, temp, l);
    }

    buf += l;
    size -= l;
  }
  else
#endif
  if (jsax_is_num_str (num))
  {
    if (size < (key_space + num.val.str->meta.len_number))
    {
      st->need = (key_space + num.val.str->meta.len_number) - size;
      return 1;
    }

#if T(PRETTY)
    *buf = ' ';
    buf += json_flags_obj (flags);
    size -= json_flags_obj (flags);
#endif

    json_str_copy (buf, num.val.str->buf, num.val.str->meta.len_number);

    buf += num.val.str->meta.len_number;
    size -= num.val.str->meta.len_number;
  }

  st->flags = (flags & json_flag_arr)
  | (!(flags & json_flag_arr) << json_flags_key_bit);

  st->buf = buf;
  st->size = size;

  return 0;
}

// -----------------------------------------------------------------------------

#undef T_PRETTY
#undef t_eol
