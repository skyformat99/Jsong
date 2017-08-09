// =============================================================================
// <dom.c>
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/integer.h>

// -----------------------------------------------------------------------------

#define MEM_POOL_STATIC

#include "api/dom.h"
#include "utils/misc.h"

// -----------------------------------------------------------------------------
// DOM tree traversal
// -----------------------------------------------------------------------------
// Compare key strings for equality if only length is known
#define json_key_cmp_name(a_kv, a_str, a_len) do\
{                                               \
  bool pack = a_kv->ukey.tag & json_str_pack;   \
  json_ukey_t ukey = {.ptr = json_unpack (void*, a_kv->ukey)};\
                                                \
  if (unlikely (json_key_len (ukey.ikey) == a_len))\
  {                                             \
    const u8* buf;                              \
                                                \
    if (likely (pack)) buf = ukey.ikey->buf;    \
    else buf = ukey.key->buf;                   \
                                                \
    if (unlikely (json_str_equal (buf, a_str, a_len))) return (json_kv_t*)a_kv;\
  }       \
} while (0)

// -----------------------------------------------------------------------------
// Compare key strings for equality if hash is known
#define json_key_cmp_hash(a_kv, a_str, a_hash) do\
{                                                \
  bool pack = a_kv->ukey.tag & json_str_pack;    \
  json_ukey_t ukey = {.ptr = json_unpack (void*, a_kv->ukey)};\
                                                 \
  if (unlikely (ukey.ikey->hash == a_hash))      \
  {                                              \
    const u8* buf;                               \
                                                 \
    if (likely (pack)) buf = ukey.ikey->buf;     \
    else buf = ukey.key->buf;                    \
                                                 \
    if (unlikely (json_str_equal (buf, a_str, len))) return (json_kv_t*)a_kv;\
  }       \
} while (0)

// -----------------------------------------------------------------------------
// Lookup object property by name
inline json_kv_t* json_kv_find (const json_kv_t* kv, const u8* str, uint len)
{
  while (kv != null)
  {
    json_key_cmp_name (kv, str, len);
    kv = kv->next;
  }

  return null;
}

// -----------------------------------------------------------------------------
// Lookup object property by name in reverse order (backwards)
inline json_kv_t* json_kv_rfind (const json_kv_t* kv, const u8* str, uint len)
{
  json_kv_t* prev;

  if (unlikely (kv == null)) return null;

  while (true)
  {
    json_key_cmp_name (kv, str, len);

    prev = json_unbox (json_kv_t*, kv->box);

    // Previous property value cannot be a property.
    //
    // If pointers are the same, then it means we have reached
    // the beginning of an upper object and `prev->box.ptr` is
    // actually the `obj->first` field of an upper object,
    // and `prev` itself is an object.
    if (unlikely (prev->box.ptr == kv)) return null;

    kv = prev;
  }
}

// -----------------------------------------------------------------------------

#if JSON(HASH_KEYS)

// -----------------------------------------------------------------------------
// Same as above, except do the lookup using the provided key hash value
inline json_kv_t* json_kv_find_hash (const json_kv_t* kv, const u8* str, uint hash)
{
  uint len = json_hash_len (hash);

  while (kv != null)
  {
    json_key_cmp_hash (kv, str, hash);
    kv = kv->next;
  }

  return null;
}

// -----------------------------------------------------------------------------
// Same as above, but backwards
inline json_kv_t* json_kv_rfind_hash (const json_kv_t* kv, const u8* str, uint hash)
{
  json_kv_t* prev;

  if (unlikely (kv == null)) return null;

  uint len = json_hash_len (hash);

  while (true)
  {
    json_key_cmp_hash (kv, str, hash);

    prev = json_unbox (json_kv_t*, kv->box);

    if (unlikely (prev->box.ptr == kv)) return null;

    kv = prev;
  }
}

// -----------------------------------------------------------------------------

#endif // JSON(HASH_KEYS)

// -----------------------------------------------------------------------------

#undef json_key_cmp_name
#undef json_key_cmp_hash

// -----------------------------------------------------------------------------
// RFC 6901 JSON Pointer implementation
// -----------------------------------------------------------------------------
// Parse the JSON Pointer string. It cannot be null-terminated,
// so its length must be provided explicitly by the caller.
uint json_parse_pointer (const u8* str, uint len, u8* buf, json_key_t* keys, uint max)
{
  if (unlikely (len == 0)) return 0;

  uint num = 0;

  const u8* s = str;
  const u8* e = str + len;

  const u8* p = buf;
  u8* b = buf;

  register uint c = *s++;

  // URI-style syntax handling
  if (unlikely (c == '#'))
  {
    // Empty location (root element)
    if (unlikely (s == e)) return 0;

    // Next character must begin the pointer element
    if (unlikely ((c = *s++) != '/')) return UINT_MAX;

    while (s != e)
    {
      c = *s++;

      // JSON Pointer character sequence parsing template
      #define json_parse_pointer_seq(u)\
      /* Next pointer element start */ \
      if (unlikely (c == '/'))         \
      {                                \
        if (unlikely (num == max)) return UINT_MAX;\
                                       \
        keys[num].buf = (u8*)p;        \
        keys[num].hash = json_hash_key (p, (size_t)(b - p));\
        num++;                         \
                                       \
        *b++ = '\0';                   \
        p = b;                         \
                                       \
        continue;                      \
      }                                \
      /* Pointer escape sequence */    \
      if (unlikely (c == '~'))         \
      {                                \
        if (unlikely (s == e)) return UINT_MAX;\
                                       \
        c = *s++;                      \
                                       \
        if (unlikely ((c | 1u) != '1')) return UINT_MAX;\
                                       \
        c = (c == '0') ? '~' : '/';    \
      }                                \
      /* URI percent-encoded sequence */\
      else if (u && (c == '%'))        \
      {                                \
        if (unlikely ((size_t)(e - s) < 2u)) return UINT_MAX;\
                                       \
        uint xh = chr_xdig_to_int (s[0]);\
        uint xl = chr_xdig_to_int (s[1]);\
                                       \
        if (unlikely ((xh | xl) > 0xFu)) return UINT_MAX;\
                           \
        c = (xh << 4) | xl;\
        s += 2;\
      }      \
             \
      *b++ = c

      // Parse and handle URI percent-encoded sequences
      json_parse_pointer_seq (true);
    }
  }

  // Default pointer syntax
  else
  {
    if (unlikely (c != '/')) return UINT_MAX;

    while (s != e)
    {
      c = *s++;

      // Parse without URI percent-encoded sequences recognition
      json_parse_pointer_seq (false);
    }
  }

  // Handle the last pointer element
  if (unlikely (num == max)) return UINT_MAX;

  *b = '\0';

  keys[num].buf = (u8*)p;
  keys[num].hash = json_hash_key (p, (size_t)(b - p));

  return num + 1u;

  #undef json_parse_pointer_seq
}

// -----------------------------------------------------------------------------
// Query the DOM tree using the parsed JSON Pointer
json_node_t json_pointer (json_node_t node, const json_key_t* keys, uint num)
{
  // Number of elements to traverse
  const json_key_t* end = keys + num;

  // Get the collection type from the element node
  json_coll_type_t coll = node.tag & json_coll_type;

  // Get the element itself
  json_elmnt_t* elmnt = json_unbox (json_elmnt_t*, node);

  while (keys < end)
  {
    // Get the element value.
    // It must be an object or an array.
    json_type_t type = json_get_type (elmnt);

    // Lookup the property by hash if it's an object
    if (likely (type == json_val_obj))
    {
#if JSON(HASH_KEYS)
      elmnt = (json_elmnt_t*)json_kv_find_hash (elmnt->val.obj->first
      , keys->buf, keys->hash);
#else
      elmnt = (json_elmnt_t*)json_kv_find (elmnt->val.obj->first
      , keys->buf, keys->hash);
#endif

      if (unlikely (elmnt == null)) return json_node_null;

      coll = json_coll_obj;
    }

    // Lookup the array item by its index
    else if (likely (type == json_val_arr))
    {
      int_parse_t parse;

      uint idx = int_uint_parse (keys->buf, json_key_len (keys), &parse);

      if (unlikely (parse.err != int_parse_ok))
      {
        // `-` reference (next item after the last array element)
        // is not handled
        return json_node_null;
      }

      elmnt = (json_elmnt_t*)json_arr_get (elmnt->val.arr, idx);

      if (unlikely (elmnt == null)) return json_node_null;

      coll = json_coll_arr;
    }

    // The element node value is neither an object nor an array.
    // There is nothing left to look up.
    else return json_node_null;

    keys++;
  }

  // Pack whatever element node we have reached and return it
  node.ptr = elmnt;
  node.tag |= coll;

  return node;
}

// -----------------------------------------------------------------------------
// Query the DOM tree using the simple JSON Query syntax.
// JSON Query doesn't require any pre-parsing to use it.
json_node_t json_query (json_node_t node, const u8* str, uint len)
{
  if (unlikely (len == 0)) return node;

  json_coll_type_t coll = node.tag & json_coll_type;
  json_elmnt_t* elmnt = json_unbox (json_elmnt_t*, node);

  const u8* s = str;
  const u8* e = str + len;

  register uint c = *s++;

  if (unlikely (c != '/')) return json_node_null;

  const u8* p = s;

  // Get the key strings and query the DOM on the fly
  while (s != e)
  {
    if (unlikely (*s++ == '/'))
    {
      #define json_query_elmnt()\
      json_type_t type = json_get_type (elmnt);\
                                \
      if (likely (type == json_val_obj))\
      {                         \
        elmnt = (json_elmnt_t*)json_kv_find (elmnt->val.obj->first\
        , p, (size_t)(s - p));  \
                                \
        if (unlikely (elmnt == null)) return json_node_null;\
                                \
        coll = json_coll_obj;   \
      }                         \
      else if (likely (type == json_val_arr))\
      {                         \
        elmnt = (json_elmnt_t*)json_arr_get (elmnt->val.arr\
        , int_uint_from_str (p, (size_t)(s - p)));\
                                \
        if (unlikely (elmnt == null)) return json_node_null;\
                               \
        coll = json_coll_arr;  \
      }                        \
      else return json_node_null

      json_query_elmnt();

      p = s;
    }
  }

  // Query the last key
  json_query_elmnt();

  node.ptr = elmnt;
  node.tag |= coll;

  return node;
}

// -----------------------------------------------------------------------------
// Factory
// -----------------------------------------------------------------------------

#if (MEM_POOL_ALIGNMENT == 0) && (INTPTR_BIT <= 64)
  #define MEM_POOL_ALIGNMENT 8u
#endif

#include <quantum/memory/pool.c>

// -----------------------------------------------------------------------------

mem_pool_t* json_pool_create (void)
{
  return mem_pool_create (JSON_POOL_SIZE, JSON_POOL_MAX, sizeof (json_kv_t));
}

void json_pool_destroy (mem_pool_t* pool)
{
  mem_pool_destroy (pool);
}

// -----------------------------------------------------------------------------

json_elmnt_t* json_new_root (mem_pool_t* pool)
{
  json_elmnt_t* elmnt = mem_pool_alloc (pool, sizeof (json_elmnt_t));

  if (unlikely (elmnt == null)) return null;

  elmnt->box.ptr = null;
  elmnt->val.ptr = null;

  return elmnt;
}

// -----------------------------------------------------------------------------

json_obj_t* json_new_obj (mem_pool_t* pool)
{
  json_obj_t* obj = mem_pool_alloc (pool, sizeof (json_obj_t*));

  if (unlikely (obj == null)) return null;

  obj->first = null;

  return obj;
}

// -----------------------------------------------------------------------------

json_arr_t* json_new_arr (mem_pool_t* pool)
{
  json_arr_t* arr = mem_pool_alloc (pool, sizeof (json_arr_t*));

  if (unlikely (arr == null)) return null;

  arr->items = (json_item_t*)(arr + 1);
  arr->size = 0;
  arr->num = 0;

  return arr;
}

// -----------------------------------------------------------------------------

json_kv_t* json_new_kv (mem_pool_t* pool)
{
  json_kv_t* kv = mem_pool_alloc (pool, sizeof (json_kv_t));

  if (unlikely (kv == null)) return null;

  kv->val.ptr = null;

  return kv;
}

// -----------------------------------------------------------------------------

json_key_t* json_new_key (const u8* str, uint len, mem_pool_t* pool)
{
  json_key_t* key = mem_pool_alloc (pool, sizeof (json_key_t));

  if (unlikely (key == null)) return null;

  key->buf = (u8*)str;
  key->hash = json_hash_key (str, len);

  return key;
}

json_ikey_t* json_new_ikey (const u8* str, uint len, mem_pool_t* pool)
{
  json_ikey_t* ikey = mem_pool_alloc (pool, sizeof (json_ikey_t) + len);

  if (unlikely (ikey == null)) return null;

  ikey->hash = json_hash_key (str, len);
  json_str_copy (ikey->buf, str, len);

  return ikey;
}

// -----------------------------------------------------------------------------

json_str_t* json_new_str (const u8* buf, uint len, mem_pool_t* pool)
{
  json_str_t* str = mem_pool_alloc (pool, sizeof (json_str_t));

  if (unlikely (str == null)) return null;

  str->buf = (u8*)buf;
  str->len = len;

  return str;
}

json_istr_t* json_new_istr (const u8* buf, uint len, mem_pool_t* pool)
{
  json_istr_t* str = mem_pool_alloc (pool, sizeof (json_istr_t) + len);

  if (unlikely (str == null)) return null;

  str->len = len;
  str_copy (str->buf, buf, len);

  return str;
}

// -----------------------------------------------------------------------------

json_num_str_t* json_new_num_str (const u8* buf, uint len, mem_pool_t* pool)
{
  json_num_str_t* str = mem_pool_alloc (pool, sizeof (json_num_str_t));

  if (unlikely (str == null)) return null;

  obj_zero (str);

  str->meta.len_number = len;
  str->buf = (u8*)buf;

  return str;
}

json_num_istr_t* json_new_num_istr (const u8* buf, uint len, mem_pool_t* pool)
{
  json_num_istr_t* str = mem_pool_alloc (pool, sizeof (json_num_istr_t) + len);

  if (unlikely (str == null)) return null;

  str->meta.len_number = len;
  str_copy (str->buf, buf, len);

  return str;
}

// -----------------------------------------------------------------------------

json_num_big_t* json_new_num_big (mem_pool_t* pool)
{
  json_num_big_t* big = mem_pool_alloc (pool, sizeof (json_num_big_t));

  if (unlikely (big == null)) return null;

  obj_zero (big);

  return big;
}

// -----------------------------------------------------------------------------
// DOM tree manipulation
// -----------------------------------------------------------------------------
// Array manipulation
// -----------------------------------------------------------------------------

json_item_t* json_arr_set (json_arr_t* arr, uint idx, mem_pool_t* pool)
{
  uint num = arr->num;

  if (likely (idx < num)) return arr->items + idx;

  uint size = arr->size;
  uint isz = (idx + 1u) - num;

  if (likely (idx < size))
  {
    arr->num = idx + 1u;
    json_arr_init (arr, num, isz);

    return arr->items + idx;
  }

  uint nsz = (idx + 1u) + (size >> 1);

  json_item_t* items = mem_pool_realloc (pool, arr->items
  , arr_size (items, size), arr_size (items, nsz));

  if (unlikely (items == null)) return null;

  arr->items = items;
  arr->size = nsz;
  arr->num = idx + 1u;

  json_arr_init (arr, num, isz);

  return items + idx;
}

// -----------------------------------------------------------------------------

json_item_t* json_arr_add (json_arr_t* arr, uint num, mem_pool_t* pool)
{
  uint size = arr->size;
  uint n = arr->num;

  if (likely ((n + num) <= size))
  {
    arr->num = n + num;
    json_arr_init (arr, n, num);

    return arr->items + n;
  }

  uint nsz = (n + num) + (size >> 1);

  json_item_t* items = mem_pool_realloc (pool, arr->items
  , arr_size (items, size), arr_size (items, nsz));

  if (unlikely (items == null)) return null;

  arr->items = items;
  arr->size = nsz;
  arr->num = n + num;

  json_arr_init (arr, n, num);

  return items + n;
}

// -----------------------------------------------------------------------------

bool json_arr_resize (json_arr_t* arr, uint num, bool soft, mem_pool_t* pool)
{
  uint n = arr->num;

  // Shrunk
  if (unlikely (num < n))
  {
    if (unlikely (!soft))
    {
      json_item_t* items = mem_pool_realloc (pool, arr->items
      , arr_size (items, arr->size), arr_size (items, num));

      if (unlikely (items == null)) return false;

      arr->items = items;
    }

    arr->num = num;

    return true;
  }

  // Grow
  else if (likely (num > n))
  {
    uint size = arr->size;

    if (likely (num <= size))
    {
      arr->num = num;
      json_arr_init (arr, n, num - n);

      return true;
    }

    json_item_t* items = mem_pool_realloc (pool, arr->items
    , arr_size (items, size), arr_size (items, num));

    if (unlikely (items == null)) return false;

    arr->items = items;
    arr->size = num;
    arr->num = num;

    json_arr_init (arr, n, num - n);
  }

  return true;
}

// -----------------------------------------------------------------------------

void json_arr_remove (json_arr_t* arr, uint idx, uint num, bool soft)
{
  uint n = arr->num;

  if (likely ((idx + num) >= n))
  {
    arr->num = (idx < n) ? idx : n;
    return;
  }

  if (likely (soft))
  {
    json_arr_init (arr, idx, num);
  }
  else
  {
    arr->num -= num;

    mem_move (arr->items + idx, arr->items + idx + num
    , arr_size (arr->items, n - (idx + num)));
  }
}
