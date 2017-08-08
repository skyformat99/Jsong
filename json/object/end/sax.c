// =============================================================================
// <object/end/sax.c>
//
// JSON SAX API object end handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Execute the object end event handler
  jsax_callback (jsnp->on_end, jsnp);

  // Reduce the stack depth
  uint depth = --(jsnp->depth);

  // Check for JSON end
  if (unlikely (depth == 0))
  {
    j++;
    goto root_end;
  }

  // Pop the array flag from the depth stack
  state = flag_clr (state, json_flag_arr)
  | (jsax_pop_coll (jsnp->stack, depth - 1u) << json_flags_coll_bit);
}
