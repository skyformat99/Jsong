// =============================================================================
// <root/start/dom.c>
//
// JSON DOM API root element start handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Allocate the root element
  json_elmnt_t* elmnt = mem_pool_alloc (jsnp->pool, sizeof (json_elmnt_t));
  if (unlikely (elmnt == null)) json_error (JSON_ERROR_MEMORY);

  // Initialize the root element
  elmnt->box.coll = null;

  // Set the current element
  jsnp->elmnt = elmnt;
}
