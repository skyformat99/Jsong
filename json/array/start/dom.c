// =============================================================================
// <array/start/dom.c>
//
// JSON DOM API array start handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Allocate the array and some of its items
  json_arr_t* arr = mem_pool_alloc_flex (jsnp->pool, sizeof (json_arr_t)
  , sizeof (json_item_t) * 2u);

  if (unlikely (arr == null)) json_error (JSON_ERROR_MEMORY);

  // Get the current element and set the array value
  json_elmnt_t* elmnt = jsnp->elmnt;
  elmnt->box.tag |= json_type_arr;
  elmnt->val.arr = arr;

  // Initialize the array
  json_item_t* item = (json_item_t*)(arr + 1);
  arr->items = item;
  arr->size = 2u;
  arr->num = 0;

  // Set the upper element and the upper collection type
  arr->node.elmnt = elmnt;
  arr->node.tag |= state & json_flag_arr;

  // Initialize the first array item
  item->box.arr = arr;
  item->val.arr = arr;

  // Set the current collection and the current element
  jsnp->coll = (json_coll_t*)arr;
  jsnp->elmnt = (json_elmnt_t*)item;
}
