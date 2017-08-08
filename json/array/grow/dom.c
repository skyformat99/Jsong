// =============================================================================
// <array/grow/dom.c>
//
// JSON DOM API array growing.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the current array
  json_arr_t* arr = (json_arr_t*)(jsnp->coll);

  // Get the current array items
  json_item_t* items = arr->items;

  // Grow the array length
  uint num = ++(arr->num);
  uint size = arr->size;

  // Grow the array buffer if necessary
  if (unlikely (num == size))
  {
#if 1
    uint sz = size * 2u;
#else
    uint sz = size + (size >> 1);
#endif

    items = arr->items = mem_pool_realloc (jsnp->pool, items
    , arr_size (items, size), arr_size (items, sz));

    if (unlikely (items == null)) json_error (JSON_ERROR_MEMORY);

    arr->size = sz;
  }

  // Get the next array item and initialize it
  json_item_t* item = items + num;
  item->box.arr = arr;
  item->val.arr = arr;

  // Set the current element
  jsnp->elmnt = (json_elmnt_t*)item;
}
