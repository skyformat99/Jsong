// =============================================================================
// <identifier/primitives.c>
//
// See if an identifier value is a recognized special USON value token.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Infinity and not-a-number without an explicit sign
  if (len == 3)
  {
    // Infinity
    if (buf_equ3 (buf, 'i', 'n', 'f'))
    {
      uson_num_inf_evnt (false);

#if T(FREE)
ident_free:
      mem_pool_free (jsnp->pool, b, sz_fix + sz + len + 1u);
#endif

      goto ident_special;
    }

    // NaN
    if (buf_equ3 (buf, 'n', 'a', 'n'))
    {
      uson_num_nan_evnt (false);

#if T(FREE)
      goto ident_free;
#else
      goto ident_special;
#endif
    }
  }

  // Boolean true, null, infinity and nan with explicit signs
  if (len == 4)
  {
    // True
    if (buf_equ4 (buf, 't', 'r', 'u', 'e'))
    {
      json_bool_evnt (true);

#if T(FREE)
      goto ident_free;
#else
      goto ident_special;
#endif
    }

    // Null
    if (buf_equ4 (buf, 'n', 'u', 'l', 'l'))
    {
      json_null_evnt();

#if T(FREE)
      goto ident_free;
#else
      goto ident_special;
#endif
    }

    // Positive infinity and nan
    if (*buf == '+')
    {
      // Infinity
      if (buf_equ4 (buf, '+', 'i', 'n', 'f'))
      {
        uson_num_inf_evnt (false);

#if T(FREE)
        goto ident_free;
#else
        goto ident_special;
#endif
      }

      // NaN
      if (buf_equ4 (buf, '+', 'n', 'a', 'n'))
      {
        uson_num_nan_evnt (false);

#if T(FREE)
        goto ident_free;
#else
        goto ident_special;
#endif
      }
    }

    // Negative infinity and nan
    if (*buf == '-')
    {
      // Infinity
      if (buf_equ4 (buf, '-', 'i', 'n', 'f'))
      {
        uson_num_inf_evnt (true);

#if T(FREE)
        goto ident_free;
#else
        goto ident_special;
#endif
      }

      // NaN
      if (buf_equ4 (buf, '-', 'n', 'a', 'n'))
      {
        uson_num_nan_evnt (true);

#if T(FREE)
        goto ident_free;
#else
        goto ident_special;
#endif
      }
    }
  }

  // Boolean false
  if ((len == 5) && buf_equ5 (buf, 'f', 'a', 'l', 's', 'e'))
  {
    json_bool_evnt (false);

#if T(FREE)
      goto ident_free;
#else
      goto ident_special;
#endif
  }
}

// -----------------------------------------------------------------------------

#undef T_FREE
