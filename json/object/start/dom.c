// =============================================================================
// <object/start/dom.c>
//
// JSON DOM API object start handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Allocate the object and its first property key
  json_obj_t* obj = mem_pool_alloc (jsnp->pool, sizeof (json_obj_t) + sizeof (json_kv_t));
  if (unlikely (obj == null)) json_error (JSON_ERROR_MEMORY);

  // Get the current element and set the object value
  json_elmnt_t* elmnt = jsnp->elmnt;
  elmnt->box.tag |= json_type_obj;
  elmnt->val.obj = obj;

  // Initialize the object
  json_kv_t* kv = (json_kv_t*)(obj + 1);
  obj->first = kv;

  // Set the upper element and the upper collection type
  obj->node.elmnt = elmnt;
  obj->node.tag |= state & json_flag_arr;

  // Initialize the first property key
  kv->box.obj = obj;
  kv->val.obj = obj;
  kv->next = (json_kv_t*)obj;

  // Set the current collection and the current element
  jsnp->coll = (json_coll_t*)obj;
  jsnp->elmnt = (json_elmnt_t*)kv;
}
