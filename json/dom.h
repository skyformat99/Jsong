// =============================================================================
// <dom.h>
//
// JSON DOM API public definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_FC78D0A7AEB640E9BA16E56CC02715BE
#define H_FC78D0A7AEB640E9BA16E56CC02715BE

// -----------------------------------------------------------------------------

#if C(CXX)
  extern "C" {
#endif

// -----------------------------------------------------------------------------

#include <quantum/memory.h>
#include <quantum/memory/pool.h>

// -----------------------------------------------------------------------------

#include "api/common.h"

// =============================================================================
// Types
// -----------------------------------------------------------------------------
// Value type
typedef enum json_type_e
{
  json_type_obj,
  json_type_arr,
  json_type_primitive,
  json_type_str,
  json_type_num,
  json_type_flt,
  json_type_int,
  json_type_uint
} json_type_t;

// -----------------------------------------------------------------------------
// Primitive value
typedef enum json_primitive_e
{
  json_undefined,
  json_null,
  json_bool_false,
  json_bool_true
} json_primitive_t;

// -----------------------------------------------------------------------------
// String type
typedef enum json_str_type_e
{
  json_str_norm,
  json_str_pack
} json_str_type_t;

// -----------------------------------------------------------------------------
// Collection type
typedef enum json_coll_type_e
{
  json_coll_obj,
  json_coll_arr
} json_coll_type_t;

// -----------------------------------------------------------------------------
// Metadata bits
enum
{
  json_null_type = 1u << 0,
  json_bool_type = 1u << 1,
  json_str_type = json_null_type,
  json_coll_type = json_null_type
};

// -----------------------------------------------------------------------------
// Immutable key, value, and number strings
// -----------------------------------------------------------------------------

typedef struct json_istr_s
{
  uint len;
#if C(CXX)
  u8 buf[1];
#else
  u8 buf[];
#endif
} json_istr_t;

typedef struct json_ikey_s
{
  uint hash;
#if C(CXX)
  u8 buf[1];
#else
  u8 buf[];
#endif
} json_ikey_t;

typedef struct json_num_istr_s
{
  json_num_meta_t meta;
#if C(CXX)
  u8 buf[1];
#else
  u8 buf[];
#endif
} json_num_istr_t;

// -----------------------------------------------------------------------------
// DOM tree structure types
// -----------------------------------------------------------------------------

typedef struct json_coll_s json_coll_t;
typedef struct json_elmnt_s json_elmnt_t;

typedef struct json_arr_s json_arr_t;
typedef struct json_item_s json_item_t;

typedef struct json_obj_s json_obj_t;
typedef struct json_kv_s json_kv_t;

// -----------------------------------------------------------------------------
// Boxed DOM collection node
typedef union json_box_u
{
  void* ptr;     // Upper collection
  uintptr_t tag; // JSON value type (object, array, string, number, etc.)

  json_coll_t* coll;
  json_arr_t* arr;
  json_obj_t* obj; // Upper object if this is the first property
  json_kv_t* kv;   // Otherwise, previous property
} json_box_t;

// -----------------------------------------------------------------------------
// Boxed DOM element node
typedef union json_node_u
{
  void* ptr;     // Upper element
  uintptr_t tag; // JSON upper collection type (object or array)

  json_elmnt_t* elmnt;
  json_item_t* item; // Only valid during parsing due to array reallocation
  json_kv_t* kv;
} json_node_t;

// -----------------------------------------------------------------------------
// Object property key
typedef union json_key_u
{
  void* ptr;     // Key string
  uintptr_t tag; // Key string type (packed or not)

  json_key_t* key;
  json_ikey_t* ikey;
} json_ukey_t;

// -----------------------------------------------------------------------------
// Extended number
typedef union json_num_u
{
  json_num_str_t* str;
  json_num_istr_t* istr;
  json_num_big_t* big;
} json_num_t;

// -----------------------------------------------------------------------------
// Boxed JSON value
typedef union json_value_u
{
  void* ptr;     // Value data
  uintptr_t tag; // Additional value metadata (if applicable)

  json_obj_t* obj;
  json_arr_t* arr;

  json_str_t* str;
  json_istr_t* istr;
  json_num_t num;

  double f;
  intmax_t i;
  uintmax_t u;
} json_value_t;

// -----------------------------------------------------------------------------
// Generic collection
struct json_coll_s
{
  json_elmnt_t* elmnt;
  json_node_t node; // Upper node that a collection is attached to
};

// -----------------------------------------------------------------------------
// Generic element
struct json_elmnt_s
{
  json_box_t box; // Collection that an element belongs to
  json_value_t val;
};

// -----------------------------------------------------------------------------
// Array collection
struct json_arr_s
{
  json_item_t* items;
  json_node_t node;
  uint size;
  uint num;
};

// -----------------------------------------------------------------------------
// Array item
struct json_item_s
{
  json_box_t box;
  json_value_t val;
};

// -----------------------------------------------------------------------------
// Object collection
struct json_obj_s
{
  json_kv_t* first;
  json_node_t node;
};

// -----------------------------------------------------------------------------
// Object key / value pair
struct json_kv_s
{
  json_box_t box; // Stores either previous k/v pair or upper object
  json_value_t val;
  json_ukey_t ukey;
  json_kv_t* next;
};

// -----------------------------------------------------------------------------
// Serialization object
// -----------------------------------------------------------------------------

#define json_serialize_common_s\
  json_node_t root;\
  json_node_t node;\
              \
  u8* buf;    \
  size_t fill;\
  size_t size;\
  size_t need;\
              \
  u8* pos;    \
  size_t len; \
              \
  bool key;   \
  bool term;  \
              \
  ushort flt_dig;\
  ushort big_flt_dig

typedef struct json_serialize_s
{
  json_serialize_common_s;
} json_serialize_t;

typedef struct json_format_s
{
  json_serialize_common_s;

  uint indent_size;
  uint indent_width;
} json_format_t;

// -----------------------------------------------------------------------------
// Parser object
typedef struct json_s aligned(CPU_CACHE_LINE_SIZE) json_t;

// -----------------------------------------------------------------------------

#include "api/shared.h"

// =============================================================================
// Macros
// -----------------------------------------------------------------------------
// DOM null node
#define json_node_null (json_node_t){null}
// DOM root node
#define json_node_root(elmnt) (json_node_t){elmnt}

// -----------------------------------------------------------------------------
// Untagging / unboxing / unpacking
// -----------------------------------------------------------------------------

#define JSON_META_BITS 0x7u
#define JSON_TYPE_BITS JSON_META_BITS
#define JSON_PACK_BITS 0x1u

#define json_untag(tag, bits) ((tag) & ~(uintptr_t)(bits))
#define json_unbox(type, box) ((type)json_untag (box.tag, JSON_TYPE_BITS))
#define json_unpack(type, box) ((type)json_untag (box.tag, JSON_PACK_BITS))

// -----------------------------------------------------------------------------
// Determine the element node kind
#define json_is_elmnt(node) (node.ptr != null)
#define json_is_item(node) ((node.tag & json_coll_type) != json_coll_obj)
#define json_is_kv(node) ((node.tag & json_coll_type) == json_coll_obj)

// -----------------------------------------------------------------------------
// Cast the element type according to its kind
#define json_as_elmnt(elmnt) ((json_elmnt_t*)(elmnt))
#define json_as_item(elmnt) ((json_item_t*)(elmnt))
#define json_as_kv(elmnt) ((json_kv_t*)(elmnt))

// -----------------------------------------------------------------------------
// Unpack the element from its node according to its kind
#define json_get_elmnt(node) json_unpack (json_elmnt_t*, node)
#define json_get_item(node) json_unpack (json_item_t*, node)
#define json_get_kv(node) json_unpack (json_kv_t*, node)

// -----------------------------------------------------------------------------
// Get the element value type
#define json_get_type(elmnt) ((elmnt)->box.tag & JSON_TYPE_BITS)
#define json_get_meta(elmnt) ((elmnt)->val.tag & JSON_META_BITS)

// -----------------------------------------------------------------------------
// Determine the element value type
#define json_is_coll(elmnt) (json_get_type (elmnt) <= json_type_arr)
#define json_is_obj(elmnt) (json_get_type (elmnt) == json_type_obj)
#define json_is_arr(elmnt) (json_get_type (elmnt) == json_type_arr)
#define json_is_null(elmnt) ((json_get_type (elmnt) == json_type_primitive) && (((elmnt)->val.tag & json_bool_type) == 0))
#define json_is_bool(elmnt) ((json_get_type (elmnt) == json_type_primitive) && (((elmnt)->val.tag & json_bool_type) != 0))
#define json_is_str(elmnt) (json_get_type (elmnt) == json_type_str)
#define json_is_num_any(elmnt) (json_get_type (elmnt) >= json_type_num)
#define json_is_num(elmnt) (json_get_type (elmnt) == json_type_num)
#define json_is_int_any(elmnt) (json_get_type (elmnt) >= json_type_int)
#define json_is_int(elmnt) (json_get_type (elmnt) == json_type_int)
#define json_is_uint(elmnt) (json_get_type (elmnt) == json_type_uint)
#define json_is_flt(elmnt) (json_get_type (elmnt) == json_type_flt)

// -----------------------------------------------------------------------------
// Determine the extended number value type
#define json_is_num_str(elmnt) (((elmnt)->val.tag & json_num_kind) == json_num_str)
#define json_is_num_big(elmnt) (((elmnt)->val.tag & json_num_kind) == json_num_big)
#define json_is_num_big_int(elmnt) (((elmnt)->val.tag & (json_num_type | json_num_kind)) == (json_num_int | json_num_big))
#define json_is_num_big_uint(elmnt) (((elmnt)->val.tag & (json_num_type | json_num_kind)) == (json_num_uint | json_num_big))
#define json_is_num_big_flt(elmnt) (((elmnt)->val.tag & (json_num_type | json_num_kind)) == (json_num_flt | json_num_big))
#define json_is_num_err(elmnt) (((elmnt)->val.tag & json_num_type) == json_num_err)

// -----------------------------------------------------------------------------
// Determine if the key string or value string is packed
#define json_is_packed(ent) ((ent.tag & JSON_PACK_BITS) != 0)

// -----------------------------------------------------------------------------
// Access the element value according to its type
// -----------------------------------------------------------------------------
// Object / array
#define json_get_obj(elmnt) ((elmnt)->val.obj)
#define json_get_arr(elmnt) ((elmnt)->val.arr)

// -----------------------------------------------------------------------------
// Key string
static inline json_key_t json_get_key (const json_kv_t* kv)
{
  if (likely (json_is_packed (kv->ukey)))
  {
    json_ikey_t* ikey = json_unpack (json_ikey_t*, kv->ukey);

    json_key_t key =
    {
      .hash = ikey->hash,
      .buf = ikey->buf
    };

    return key;
  }

  return *(kv->ukey.key);
}

// -----------------------------------------------------------------------------
// Value string
static inline json_str_t json_get_str (const void* ptr)
{
  json_elmnt_t* elmnt = json_as_elmnt (ptr);

  if (likely (json_is_packed (elmnt->val)))
  {
    json_istr_t* istr = json_unpack (json_istr_t*, elmnt->val);

    json_str_t str =
    {
      .len = istr->len,
      .buf = istr->buf
    };

    return str;
  }

  return *(elmnt->val.str);
}

// -----------------------------------------------------------------------------
// Number string
static inline json_num_str_t json_get_num_str (const void* ptr)
{
  json_elmnt_t* elmnt = json_as_elmnt (ptr);

  if (likely (json_is_packed (elmnt->val)))
  {
    json_num_istr_t* istr = json_unbox (json_num_istr_t*, elmnt->val);

    json_num_str_t str =
    {
      .meta = istr->meta,
      .buf = istr->buf
    };

    return str;
  }

  return *json_unbox (json_num_str_t*, elmnt->val);
}

// -----------------------------------------------------------------------------
// Number (integer, float)
#define json_get_int(elmnt) ((elmnt)->val.i)
#define json_get_uint(elmnt) ((elmnt)->val.u)
#define json_get_flt(elmnt) ((elmnt)->val.f)

// -----------------------------------------------------------------------------
// Extended number (big integer or big float)
#define json_get_num_big(elmnt) (*(json_unbox (json_num_big_t*, (elmnt)->val)))
#define json_get_num_big_int(elmnt) (json_unbox (json_num_big_t*, (elmnt)->val)->i)
#define json_get_num_big_uint(elmnt) (json_unbox (json_num_big_t*, (elmnt)->val)->u)
#define json_get_num_big_flt(elmnt) (json_unbox (json_num_big_t*, (elmnt)->val)->f)

// -----------------------------------------------------------------------------
// Primitive
#define json_get_bool(elmnt) ((elmnt)->val.tag & 1u)
#define json_get_null(elmnt) null

// =============================================================================
// Functions
// -----------------------------------------------------------------------------
// Pool-based memory management
// -----------------------------------------------------------------------------

extern mem_pool_t* json_pool_create (void);
extern void json_pool_destroy (mem_pool_t* pool);

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

extern void json_init (json_t* jsnp, mem_pool_t* pool);
extern void json_initi (json_t* jsnp, mem_pool_t* pool);

extern void json_init_stream (json_t* jsnp, mem_pool_t* pool);

// -----------------------------------------------------------------------------
// Parsing
// -----------------------------------------------------------------------------

extern json_node_t json_parse (json_t* jsnp, u8* json, size_t size);
extern json_node_t json_parsei (json_t* jsnp, u8* json);

extern json_node_t json_parse_stream (json_t* jsnp, u8* json, size_t size, bool last);

// -----------------------------------------------------------------------------
// Serialization
// -----------------------------------------------------------------------------

extern void json_serialize_init (json_serialize_t* st, json_node_t root
, u8* buf, size_t size, size_t fill);

extern void json_serialize_init_buf (json_serialize_t* st
, u8* buf, size_t size, size_t fill);

extern bint json_serialize (json_serialize_t* st);

// -----------------------------------------------------------------------------

extern void json_format_init (json_format_t* st, json_node_t root
, u8* buf, size_t size, size_t fill);

extern void json_format_init_buf (json_format_t* st
, u8* buf, size_t size, size_t fill);

extern bint json_format (json_format_t* st);

// -----------------------------------------------------------------------------
// DOM tree traversal
// -----------------------------------------------------------------------------
// Get an object property by its key
// -----------------------------------------------------------------------------

extern json_kv_t* json_kv_find_name (const json_kv_t* kv, const u8* str, uint len);
extern json_kv_t* json_kv_rfind_name (const json_kv_t* kv, const u8* str, uint len);

extern json_kv_t* json_kv_find_hash (const json_kv_t* kv, const u8* str, uint hash);
extern json_kv_t* json_kv_rfind_hash (const json_kv_t* kv, const u8* str, uint hash);

// -----------------------------------------------------------------------------

static inline json_kv_t* json_obj_find_name (const json_obj_t* obj, const u8* str, uint len)
{
  return json_kv_find_name (obj->first, str, len);
}

static inline json_kv_t* json_obj_find_hash (const json_obj_t* obj, const u8* str, uint hash)
{
  return json_kv_find_hash (obj->first, str, hash);
}

// -----------------------------------------------------------------------------
// Get an array item by its index
// -----------------------------------------------------------------------------
// Get an array item at the specified index, growing array if necessary
extern json_item_t* json_arr_item (json_arr_t* arr, uint idx, mem_pool_t* pool);

// -----------------------------------------------------------------------------
// Get an array item at the specified index, only if it actually exists
static inline json_item_t* json_arr_idx (const json_arr_t* arr, uint idx)
{
  if (likely (idx < arr->num)) return arr->items + idx;
  return null;
}

// -----------------------------------------------------------------------------
// Object key / value pairs navigation
// -----------------------------------------------------------------------------

static inline json_kv_t* json_kv_next (const json_kv_t* kv)
{
  return kv->next;
}

static inline json_kv_t* json_kv_prev (const json_kv_t* kv)
{
  json_kv_t* prev = json_unbox (json_kv_t*, kv->box);

  // The previous key is actually an upper object
  if (unlikely (prev->box.ptr == kv)) return null;

  return prev;
}

// -----------------------------------------------------------------------------
// Traverse backwards until the object that a k/v pair belongs to is reached
static inline json_obj_t* json_kv_obj (const json_kv_t* kv)
{
  while (true)
  {
    json_kv_t* prev = json_unbox (json_kv_t*, kv->box);

    if (unlikely (prev->box.ptr == kv)) return (json_obj_t*)prev;

    kv = prev;
  }
}

// -----------------------------------------------------------------------------
// Get the array that an item belongs to
static inline json_arr_t* json_item_arr (const json_item_t* item)
{
  return json_unbox (json_arr_t*, item->box);
}

// -----------------------------------------------------------------------------
// DOM tree manipulation
// -----------------------------------------------------------------------------

extern void json_kv_add (json_kv_t* kv, json_kv_t* link, mem_pool_t* pool);
extern void json_kv_remove (json_kv_t* kv, mem_pool_t* pool);

static inline void json_obj_add (json_obj_t* obj, json_kv_t* link, mem_pool_t* pool)
{
  json_kv_add (obj->first, link, pool);
}

// -----------------------------------------------------------------------------

extern json_item_t* json_arr_add (json_arr_t* arr, uint num, mem_pool_t* pool);
extern void json_arr_remove (json_arr_t* arr, uint idx, uint num, bint move, mem_pool_t* pool);

extern bint json_arr_resize (json_arr_t* arr, uint num, bint shrink, mem_pool_t* pool);

// -----------------------------------------------------------------------------
// Allocation
// -----------------------------------------------------------------------------

extern json_obj_t* json_new_obj (mem_pool_t* pool);
extern json_arr_t* json_new_arr (mem_pool_t* pool);

extern json_elmnt_t* json_new_root (mem_pool_t* pool);
extern json_kv_t* json_new_kv (mem_pool_t* pool);

extern json_key_t* json_new_key (const u8* str, uint len, mem_pool_t* pool);
extern json_ikey_t* json_new_ikey (const u8* str, uint len, mem_pool_t* pool);

extern json_str_t* json_new_str (const u8* buf, uint len, mem_pool_t* pool);
extern json_istr_t* json_new_istr (const u8* buf, uint len, mem_pool_t* pool);

extern json_num_str_t* json_new_num_str (const u8* buf, uint len, mem_pool_t* pool);
extern json_num_big_t* json_new_num_big (mem_pool_t* pool);

// -----------------------------------------------------------------------------
// Assignment
// -----------------------------------------------------------------------------

static inline void json_elmnt_set_obj (json_elmnt_t* elmnt, json_obj_t* obj)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_obj;
  elmnt->val.obj = obj;
}

static inline void json_elmnt_set_arr (json_elmnt_t* elmnt, json_arr_t* arr)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_arr;
  elmnt->val.arr = arr;
}

// -----------------------------------------------------------------------------

static inline void json_elmnt_set_str (json_elmnt_t* elmnt, json_str_t* str)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_str;
  elmnt->val.str = str;
}

static inline void json_elmnt_set_istr (json_elmnt_t* elmnt, json_istr_t* str)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_str;

  elmnt->val.istr = str;
  elmnt->val.tag |= json_str_pack;
}

// -----------------------------------------------------------------------------

static inline void json_elmnt_set_num_big_int (json_elmnt_t* elmnt, json_num_big_t* big)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_num;

  elmnt->val.num.big = big;
  elmnt->val.tag |= json_num_int | json_num_big;
}

static inline void json_elmnt_set_num_big_flt (json_elmnt_t* elmnt, json_num_big_t* big)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_num;

  elmnt->val.num.big = big;
  elmnt->val.tag |= json_num_flt | json_num_big;
}

// -----------------------------------------------------------------------------

static inline void json_elmnt_set_num_str (json_elmnt_t* elmnt, json_num_str_t* str)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_num;

  elmnt->val.num.str = str;
  elmnt->val.tag |= json_num_str;
}

// -----------------------------------------------------------------------------

static inline void json_elmnt_set_int (json_elmnt_t* elmnt, intmax_t val)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_int;
  elmnt->val.i = val;
}

static inline void json_elmnt_set_uint (json_elmnt_t* elmnt, uintmax_t val)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_uint;
  elmnt->val.u = val;
}

static inline void json_elmnt_set_flt (json_elmnt_t* elmnt, double val)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_flt;
  elmnt->val.f = val;
}

// -----------------------------------------------------------------------------

static inline void json_elmnt_set_bool (json_elmnt_t* elmnt, bool val)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_primitive;
  elmnt->val.tag = 2u | val;
}

static inline void json_elmnt_set_null (json_elmnt_t* elmnt)
{
  elmnt->box.tag = (elmnt->box.tag & ~JSON_TYPE_BITS) | json_type_primitive;
  elmnt->val.tag = 1u;
}

// -----------------------------------------------------------------------------

static inline void json_kv_set_key (json_kv_t* kv, json_key_t* key)
{
  kv->ukey.key = key;
}

static inline void json_kv_set_ikey (json_kv_t* kv, json_ikey_t* key)
{
  kv->ukey.ikey = key;
  kv->ukey.tag |= json_str_pack;
}

// -----------------------------------------------------------------------------

static inline void json_kv_set_obj (json_kv_t* kv, json_obj_t* obj)
{
  obj->node.kv = kv;
  obj->node.tag |= json_coll_obj;

  json_elmnt_set_obj (json_as_elmnt (kv), obj);
}

static inline void json_kv_set_arr (json_kv_t* kv, json_arr_t* arr)
{
  arr->node.kv = kv;
  arr->node.tag |= json_coll_obj;

  json_elmnt_set_arr (json_as_elmnt (kv), arr);
}

// -----------------------------------------------------------------------------

static inline void json_item_set_obj (json_item_t* item, json_obj_t* obj)
{
  obj->node.item = item;
  obj->node.tag |= json_coll_arr;

  json_elmnt_set_obj (json_as_elmnt (item), obj);
}

static inline void json_item_set_arr (json_item_t* item, json_arr_t* arr)
{
  arr->node.item = item;
  arr->node.tag |= json_coll_arr;

  json_elmnt_set_arr (json_as_elmnt (item), arr);
}

// -----------------------------------------------------------------------------
// JSON Pointer
// -----------------------------------------------------------------------------

extern uint json_parse_pointer (const u8* str, uint len, u8* buf, json_key_t* keys, uint max);
extern json_node_t json_pointer (json_node_t node, const json_key_t* keys, uint num);

// -----------------------------------------------------------------------------
// JSON Query
// -----------------------------------------------------------------------------

extern json_node_t json_query (json_node_t node, const u8* str, uint len);

// -----------------------------------------------------------------------------

#if C(CXX)
  }
#endif

// -----------------------------------------------------------------------------

#endif
