// =============================================================================
// <number/compute.c>
//
// JSON number computation.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  u8* buf;
  size_t len;

  // Get the total number of decimal places
  d = lc + lf;

  // Check if mantissa doesn't overflow for sure
  if (likely (d < JSON_NUM_UINT_DIG))
  {
number_from_int:
    // Floating point number
    if (unlikely (meta.floating))
    {
      mf = mi;

number_from_flt:
      // Exponent handling
      if (unlikely (meta.exponent))
      {
        // Compute the signed exponent value
        ep = (int)int_negif (ex, meta.exponent_sign) - lf;

        // Check for zero mantissa
        ep = (mf == 0) ? 0 : ep;

        // Positive exponent
        if (likely (ep > 0))
        {
#if JSON(BIG_NUMBERS)
          if (unlikely ((ep + (lc - 1u) + lf) >= JSON_NUM_EXP10_MAX))
          {
            bf = mi;
            goto number_big_exp_pos;
          }
#endif

          // Check for exponent overflow
          if (unlikely (ep > JSON_NUM_EXP10_MAX))
          {
#if JSON(ERROR_STRING_NUMBERS)
            void* ptr;

  #if !JSON(STREAM)
            bool pack;
  #endif

number_as_str:
  #if JSON(EXPLICIT)
    #if JSON(STREAM) && !JSON(SAX)
            if (likely (jsnp->buf == null))
    #else
            // This is needed to obtain the terminating null
            // when the number happens to be a root element
            if (unlikely (json_flags_root (state)
      #if JSON(STREAM) && JSON(SAX)
            && (jsnp->used == 0)
      #endif
            ))
    #endif
            {
    #if JSON(SAX)
              // Allocate the number string buffer
              u8* b = jsnp->buf;

              json_buf_grow (&b, len + 1u);
              json_str_copy (b, buf, len);

              buf = b;
    #else
              // Allocate the number string object
              json_num_istr_t* str = mem_pool_alloc (jsnp->pool, obj_size (str) + len + 1u);
              if (unlikely (str == null)) json_error (JSON_ERROR_MEMORY);

              json_str_copy (str->buf, buf, len);

              buf = str->buf;
              ptr = str;

      #if !JSON(STREAM)
              pack = true;
      #endif
    #endif
            }
            else
  #endif
            {
  #if JSON(SAX)
    #if JSON(STREAM)
              // Get the number string buffer
              buf = jsnp->buf;
              jsnp->used = 0;
    #endif
  #else
    #if JSON(STREAM)
              // Get the number string object
              json_num_istr_t* str = (json_num_istr_t*)(jsnp->buf);

              buf = str->buf;
              ptr = str;
    #else
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
    #endif
  #endif
            }

            // Null-terminate
            buf[len] = '\0';

            // Set the number lengths
            meta.len_cardinal = lc;
            meta.len_fraction = lf;
            meta.len_number = len;

            // Restore the fraction dot
            if (unlikely (meta.fraction))
            {
  #if ENABLED(USON)
              bint s = chr_is_sign (*buf);
  #else
              bint s = meta.sign;
  #endif

              json_str_move_left (buf + s, buf + s + 1, lc);
              buf[s + lc] = '.';
            }

  #if JSON(SAX)
            // Set the number string value
            jsax_num_t val;
            json_num_str_t str;

            str.buf = buf;
            str.meta = meta;

            val.num.str = &str;
            val.tag = json_num_str;

            // Execute the number event handler
            jsax_callback (jsnp->on_num, jsnp, val);
  #else
            // Set the number type
            json_elmnt_t* elmnt = jsnp->elmnt;
            elmnt->box.tag |= json_val_num;

            // Set the number string value
            elmnt->val.ptr = ptr;
            ((json_num_istr_t*)ptr)->meta = meta;

            // Set the number string type
    #if JSON(STREAM)
            elmnt->val.tag |= json_str_pack | json_num_str;
    #else
            elmnt->val.tag |= pack | json_num_str;
    #endif
  #endif

            goto number_computed;
#else // JSON(ERROR_STRING_NUMBERS) ][
number_oflow:
            json_num_oflow_evnt (meta);
            goto number_erange;
#endif
          }
          else
          {
            // See if extra exponent powers can be transferred into mantissa
            // for the fast path floating point conversion case
            if (unlikely (ep > JSON_NUM_FLINT_POW10))
            {
              int x = ep - JSON_NUM_FLINT_POW10;
              int m = JSON_NUM_FLINT_DIG10 - d;

              if (likely (m >= x))
              {
                ep -= x;
                mf *= json_pow (10, x);
              }
            }

            mf *= json_pow (10, ep);

            // Check for exponent overflow
            if (unlikely (mf == JSON_NUM_HUGE_VAL))
            {
#if JSON(ERROR_STRING_NUMBERS)
              goto number_as_str;
#else
              goto number_oflow;
#endif
            }
          }
        }

        // Negative exponent
        else if (unlikely (ep < 0))
        {
          ep = -ep;

#if JSON(BIG_NUMBERS)
          if (unlikely ((ep + (lc - 1u) - lf) >= (-(JSON_NUM_EXP10_MIN) + 1)))
          {
            bf = mi;
            goto number_big_exp_neg;
          }
#endif

          if (unlikely (ep > -(JSON_NUM_EXP10_MIN)))
          {
            ep -= -(JSON_NUM_EXP10_MIN);

            if (likely (ep < -(JSON_NUM_EXP10_MIN)))
            {
              mf /= json_pow (10, -(JSON_NUM_EXP10_MIN));
              mf /= json_pow (10, ep);
            }
            else
            {
#if JSON(ERROR_STRING_NUMBERS)
              goto number_as_str;
#else
number_uflow:
              json_num_uflow_evnt (meta);
              goto number_erange;
#endif
            }
          }
          else mf /= json_pow (10, ep);

          // Check for exponent underflow
          if (unlikely (mf == 0))
          {
#if JSON(ERROR_STRING_NUMBERS)
            goto number_as_str;
#else
            goto number_uflow;
#endif
          }
        }
      }

      // No exponent
      else mf /= json_pow (10, lf);

      // Adjust the sign
      mf = flt_negif (mf, meta.sign);

number_flt:
      json_num_flt_evnt (mf);
    }

    // Integral number
    else
    {
  #if CPU(TWOS_COMPLEMENT)
      if (likely (mi > ((uintmax_t)JSON_NUM_INT_MAX + meta.sign)))
  #else
      if (likely (mi > (uintmax_t)JSON_NUM_INT_MAX))
  #endif
      {
        if (unlikely (meta.sign)) goto number_from_flt;
        else json_num_uint_evnt (mi);
      }

      // Adjust the sign
      mi = int_negif (mi, meta.sign);

      json_num_int_evnt (mi);
    }

number_erange:;
  }

  // Big mantissa --------------------------------------------------------------

  else
  {
    // Check if mantissa overflows for sure
    if (unlikely (d > JSON_NUM_DIG_MAX))
    {
#if JSON(ERROR_STRING_NUMBERS)
      goto number_as_str;
#else
      goto number_oflow;
#endif
    }

    // Save the length
    len = (size_t)(j - p);

#if JSON(STREAM)
    // Assemble the complete number string
  #if JSON(SAX)
    if (unlikely (jsnp->used != 0))
  #else
    if (unlikely (jsnp->buf != null))
  #endif
    {
      u8* b = jsnp->buf;

      // Make space for the terminating null also,
      // in case if number ends up as a string
      json_buf_grow (&b, len + 1u);
      json_str_copy (b + jsnp->used, p, len);

      p = b;

    #if !JSON(SAX)
      // Skip the header
      p += sizeof (json_num_istr_t);

      // Adjust for deallocation
      jsnp->used += len + 1u;
    #endif
    }
    else
#endif
    {
      // Save the pointer
      buf = (u8*)p;
    }

    // Get rid of the sign
#if ENABLED(USON)
    p += chr_is_sign (*p);
#else
    p += meta.sign;
#endif

    // Get rid of the fraction dot
    if (unlikely (meta.fraction))
    {
      json_str_move_right ((u8*)(p + 1), p, lc);
      p++;
    }

#if JSON(BIG_NUMBERS)
    // Experimental big number support
    if (likely (d == JSON_NUM_UINT_DIG))
    {
      uint l = chr_dig_to_int (p[JSON_NUM_UINT_DIG - 1]);

      mi = int_umax_from_str (p, JSON_NUM_UINT_DIG - 1);

      if (likely (!int_can_oflow_dig (mi, JSON_NUM_UINT_MAX)
      || !int_will_oflow_dig (mi, JSON_NUM_UINT_MAX, l)))
      {
        mi = (mi * 10u) + l;
        goto number_from_int;
      }

  #if (JSON_NUM_BIG_UINT_DIG > JSON_NUM_UINT_DIG)
      bi = ((json_big_uint_t)mi * 10u) + l;
      goto number_from_big_int;
  #else
      mf = ((double)mi * 10) + l;
      goto number_from_flt;
  #endif
    }
    else
    {
  #if (JSON_NUM_BIG_UINT_DIG > JSON_NUM_UINT_DIG)
      bi = int_u128_from_str (p, (d >= JSON_NUM_BIG_UINT_DIG)
      ? (JSON_NUM_BIG_UINT_DIG - 1) : d);

      if (likely (d < JSON_NUM_BIG_UINT_DIG))
  #else
      if (false)
  #endif
      {
number_from_big_int:
  #if (JSON_NUM_BIG_UINT_DIG > JSON_NUM_UINT_DIG)
        if (unlikely (meta.floating))
  #else
        if (true)
  #endif
        {
  #if (JSON_NUM_BIG_UINT_DIG > JSON_NUM_UINT_DIG)
          bf = bi;
  #endif

number_from_big_flt:
          if (unlikely (meta.exponent))
          {
            ep = (int)int_negif (ex, meta.exponent_sign) - lf;
            ep = (bf == 0) ? 0 : ep;

            if (likely (ep > 0))
            {
number_big_exp_pos:
              if (unlikely (ep > JSON_NUM_BIG_FLINT_POW10))
              {
                int x = ep - JSON_NUM_BIG_FLINT_POW10;
                int m = JSON_NUM_BIG_FLINT_DIG10 - d;

                if (likely (m >= x))
                {
                  ep -= x;
                  bf *= json_big_pow (10, x);
                }
              }

              bf *= json_big_pow (10, ep);

              if (unlikely (bf == JSON_NUM_BIG_HUGE_VAL))
              {
  #if JSON(ERROR_STRING_NUMBERS)
                goto number_as_str;
  #else
                goto number_oflow;
  #endif
              }
            }
            else if (unlikely (ep < 0))
            {
              ep = -ep;

number_big_exp_neg:
              if (unlikely (ep > -(JSON_NUM_BIG_EXP10_MIN)))
              {
                ep -= -(JSON_NUM_BIG_EXP10_MIN);

                if (likely (ep < -(JSON_NUM_BIG_EXP10_MIN)))
                {
                  bf /= json_big_pow (10, -(JSON_NUM_BIG_EXP10_MIN));
                  bf /= json_big_pow (10, ep);
                }
                else
                {
  #if JSON(ERROR_STRING_NUMBERS)
                  goto number_as_str;
  #else
                  goto number_uflow;
  #endif
                }
              }
              else bf /= json_big_pow (10, ep);

              if (unlikely (bf == 0))
              {
  #if JSON(ERROR_STRING_NUMBERS)
                goto number_as_str;
  #else
                goto number_uflow;
  #endif
              }
            }
          }
          else bf /= json_big_pow (10, lf);

          bf = meta.sign ? -bf : bf;

number_big_flt:
          json_num_big_flt_evnt (bf);
        }
  #if (JSON_NUM_BIG_UINT_DIG > JSON_NUM_UINT_DIG)
        else
        {
  #if CPU(TWOS_COMPLEMENT)
          if (likely (bi > ((json_big_uint_t)JSON_NUM_BIG_INT_MAX + meta.sign)))
  #else
          if (likely (bi > (json_big_uint_t)JSON_NUM_BIG_INT_MAX))
  #endif
          {
            if (unlikely (meta.sign)) goto number_from_big_flt;
            else json_num_big_uint_evnt (bi);
          }

          bi = int_negif (bi, meta.sign);

          json_num_big_int_evnt (bi);
        }
  #endif
      }
      else
      {
  #if (JSON_NUM_BIG_UINT_DIG > JSON_NUM_UINT_DIG)
        if (likely (d == JSON_NUM_BIG_UINT_DIG))
        {
          uint l = chr_dig_to_int (p[JSON_NUM_BIG_UINT_DIG - 1]);

          if (likely (!int_can_oflow_dig (bi, JSON_NUM_BIG_UINT_MAX)
          || !int_will_oflow_dig (bi, JSON_NUM_BIG_UINT_MAX, l)))
          {
            bi = (bi * 10u) + l;
            goto number_from_big_int;
          }
        }

        bf = bi;

        const u8* z = p + d;

        r = p + ((d > (JSON_NUM_BIG_FLINT_DIG10 + 2)) ? (JSON_NUM_BIG_FLINT_DIG10 + 2) : d);
        p += JSON_NUM_BIG_UINT_DIG - 1;
  #else
        bf = 0;

        const u8* z = p + d;

        r = p + ((d > (JSON_NUM_BIG_FLINT_DIG10 + 2)) ? (JSON_NUM_BIG_FLINT_DIG10 + 2) : d);
  #endif

        while (p < r)
        {
          bf = (bf * 10) + chr_dig_to_int (*p);
          p++;
        }

        bf *= json_big_pow (10, (size_t)(z - p));

        if (unlikely (bf == JSON_NUM_BIG_HUGE_VAL))
        {
  #if JSON(ERROR_STRING_NUMBERS)
          goto number_as_str;
  #else
          goto number_oflow;
  #endif
        }

        meta.floating = true;

        goto number_from_big_flt;
      }
    }
#else
    // Parse as much as possible as an integer
    mi = int_umax_from_str (p, JSON_NUM_UINT_DIG - 1);

    // Check for integral overflow in the last decimal place
    if (likely (d == JSON_NUM_UINT_DIG))
    {
      uint l = chr_dig_to_int (p[JSON_NUM_UINT_DIG - 1]);

      if (likely (!int_will_oflow_dig (mi, JSON_NUM_UINT_MAX, l)))
      {
        mi = (mi * 10u) + l;
        goto number_from_int;
      }
    }

    // Continue parsing as a floating point number
    mf = mi;

    // Parse until adding individual decimal digits would be
    // meaningless due to complete loss of significance
    const u8* z = p + d;

    r = p + ((d > (JSON_NUM_FLINT_DIG10 + 2)) ? (JSON_NUM_FLINT_DIG10 + 2) : d);
    p += JSON_NUM_UINT_DIG - 1;

    while (p < r)
    {
      mf = (mf * 10) + chr_dig_to_int (*p);
      p++;
    }

    // Raise to power equivalent to the number of remaining decimal digits
    mf *= json_pow (10, (size_t)(z - p));

    // Check for floating point overflow
    if (unlikely (mf == JSON_NUM_HUGE_VAL))
    {
  #if JSON(ERROR_STRING_NUMBERS)
      goto number_as_str;
  #else
      goto number_oflow;
  #endif
    }

    meta.floating = true;

    goto number_from_flt;
#endif
  }

#if JSON(STREAM)
  // Free the buffer after big number computation
  #if JSON(SAX)
    jsnp->used = 0;
  #else
    if (unlikely (jsnp->buf != null))
    {
      mem_pool_free (jsnp->pool, jsnp->buf, jsnp->used);
    }
  #endif
#endif

number_computed:;
}
