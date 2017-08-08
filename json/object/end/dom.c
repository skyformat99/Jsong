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

  // Remove the object reference
  kv->next = null;

collection_end:
  // Get the upper element node
  node = jsnp->coll->node;

  // See if the upper collection is an object or an array
  state = flag_clr (state, json_flag_arr)
  | ((node.tag & json_coll_arr) << json_flags_coll_bit);

  // Unbox the upper element
  json_elmnt_t* elmnt = json_unbox (json_elmnt_t*, node);

  // Unbox the upper collection
  json_coll_t* coll = json_unbox (json_coll_t*, elmnt->box);

  // Check for JSON end
  if (unlikely (coll == null))
  {
    j++;

    jsnp->coll = null;
    jsnp->elmnt = elmnt;

    goto root_end;
  }

  // Set the current collection
  if (likely (json_flags_obj (state)))
  {
    jsnp->coll = (json_coll_t*)(((json_kv_t*)(elmnt))->next);
  }
  else jsnp->coll = coll;

  // Set the current element
  jsnp->elmnt = elmnt;
}
