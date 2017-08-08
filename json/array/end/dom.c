// =============================================================================
// <array/end/dom.c>
//
// JSON DOM API array end handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  // Get the current array
  json_arr_t* arr = (json_arr_t*)(jsnp->coll);

  // Get the current array item
  json_item_t* item = (json_item_t*)(jsnp->elmnt);

  // Empty array case handling
  if (likely (item->val.arr != arr)) arr->num++;

  goto collection_end;
}
