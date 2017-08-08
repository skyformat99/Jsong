// =============================================================================
// <array/start/sax.c>
//
// JSON SAX API array start handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Check if the maximum allowed stack depth has been reached
  uint depth = jsnp->depth++;
  if (unlikely (depth == JSON_DEPTH_MAX - 1u)) json_error (JSON_ERROR_DEPTH);

  // Push the array flag to the depth stack
  jsax_push_arr (jsnp->stack, depth);

  // Execute the array start event handler
  jsax_callback (jsnp->on_start, jsnp, false);
}
