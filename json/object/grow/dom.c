// =============================================================================
// <object/grow/dom.c>
//
// JSON DOM API object growing.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Allocate the next property key
  json_kv_t* link = mem_pool_alloc (jsnp->pool, sizeof (json_kv_t));
  if (unlikely (link == null)) json_error (JSON_ERROR_MEMORY);

  // Get the current property key and link it with the new key
  json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);
  kv->next = link;

  // Initialize the next property key
  link->box.kv = kv;
  link->val.ptr = null;
  link->next = (json_kv_t*)(jsnp->coll);

  // Set the current element
  jsnp->elmnt = (json_elmnt_t*)link;
}
