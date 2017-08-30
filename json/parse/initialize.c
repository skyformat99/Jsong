// =============================================================================
// <parse/initialize.c>
//
// Initialization function template.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Reset everything
  *jsnp = (jsnp_t){0};

#if JSON(LINE_COL)
  // Reset the position
  jsnp->line = 1u;
  jsnp->col  = 1u;
#endif

#if JSON(SAX)
  // Reset the intermediate value buffer
  jsnp->buf  = buf;
  jsnp->size = size;

  // Set the default callbacks
  jsnp->on_start = jsax_prefix (start_trap);
  jsnp->on_key   = jsax_prefix (key_trap);
  jsnp->on_str   = jsax_prefix (str_trap);
  jsnp->on_num   = jsax_prefix (num_trap);
  jsnp->on_int   = jsax_prefix (int_trap);
  jsnp->on_flt   = jsax_prefix (flt_trap);
  jsnp->on_bool  = jsax_prefix (bool_trap);
  jsnp->on_null  = jsax_prefix (null_trap);
  jsnp->on_end   = jsax_prefix (end_trap);

  #if ENABLED(USON)
    // USON string metadata callbacks
    jsnp->on_id   = usax_id_trap;
    jsnp->on_mime = usax_mime_trap;
  #endif

  // Memory request callback
  jsnp->on_mem = jsax_prefix (mem_trap);
#else
  // Set the memory pool
  jsnp->pool = pool;
#endif
}
