// =============================================================================
// <object/end/sax.c>
//
// JSON SAX API object end handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
#if !USON(CONFIG)
  // Execute the object end event handler
  jsax_callback (jsnp->on_end, jsnp);
#endif

  // Reduce the stack depth
  uint depth = --(jsnp->depth);

  // Check for JSON end
  if (unlikely (depth == 0))
  {
#if !USON(CONFIG)
  #if ENABLED(USON)
    if (uson_flags_config (state)) json_error (JSON_ERROR_TOKEN);
  #endif

    j++;
#endif

    goto root_end;
  }

#if USON(CONFIG)
  json_error (JSON_ERROR_EXPECTED_MORE);
#else
  // Pop the array flag from the depth stack
  state = flag_clr (state, json_flag_arr)
  | (jsax_pop_coll (jsnp->stack, depth - 1u) << json_flags_coll_bit);
#endif
}
