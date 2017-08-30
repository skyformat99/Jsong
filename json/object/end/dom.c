// =============================================================================
// <object/end/dom.c>
//
// JSON DOM API object end handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  json_node_t node;

  // Get the current object
  json_obj_t* obj = (json_obj_t*)(jsnp->coll);

  // Get the current object k/v pair
  json_kv_t* kv = (json_kv_t*)(jsnp->elmnt);

  // Empty object case handling
  if (unlikely (kv->val.obj == obj)) obj->first = null;
#if ENABLED(USON)
  else if (unlikely (kv->val.ptr == null))
  {
    mem_pool_free (jsnp->pool, kv->box.kv->next, sizeof (json_kv_t));
    kv->box.kv->next = null;
  }
#endif

  // Remove the object reference
  kv->next = null;

#if !USON(CONFIG)
collection_end:
#endif
  // Get the upper element node
  node = jsnp->coll->node;

  // Unbox the upper element
  json_elmnt_t* elmnt = json_unbox (json_elmnt_t*, node);

  // Unbox the upper collection
  json_coll_t* coll = json_unbox (json_coll_t*, elmnt->box);

  // Check for JSON end
  if (unlikely (coll == null))
  {
#if !USON(CONFIG)
  #if ENABLED(USON)
    if (uson_flags_config (state)) json_error (JSON_ERROR_TOKEN);
  #endif

    j++;
#endif

    jsnp->coll = null;
    jsnp->elmnt = elmnt;

    goto root_end;
  }

#if USON(CONFIG)
  json_error (JSON_ERROR_EXPECTED_MORE);
#else
  // See if the upper collection is an object or an array
  state = flag_clr (state, json_flag_arr)
  | ((node.tag & json_coll_arr) << json_flags_coll_bit);

  // Set the current collection
  if (likely (json_flags_obj (state)))
  {
    jsnp->coll = (json_coll_t*)(((json_kv_t*)(elmnt))->next);
  }
  else jsnp->coll = coll;

  // Set the current element
  jsnp->elmnt = elmnt;
#endif
}
