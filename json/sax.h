// =============================================================================
// <sax.h>
//
// JSON SAX API public definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_F2966EB7054B4C99B8E719FA00C91982
#define H_F2966EB7054B4C99B8E719FA00C91982

// -----------------------------------------------------------------------------

#if C(CXX)
  extern "C" {
#endif

// -----------------------------------------------------------------------------

#include "api/common.h"

// =============================================================================
// Constants
// -----------------------------------------------------------------------------

#if !JSON(DEPTH_SIZE)
  #define JSON_DEPTH_SIZE 2u
#endif

#if (JSON_DEPTH_SIZE < 2)
  #error "Invalid definition of `JSON_DEPTH_SIZE`"
#endif

// =============================================================================
// Types
// -----------------------------------------------------------------------------
// Number value
typedef union jsax_num_val_u
{
  json_num_str_t* str;
  json_num_big_t* big;
} jsax_num_val_t;

// -----------------------------------------------------------------------------
// Number
typedef struct jsax_num_s
{
  uintptr_t tag;
  jsax_num_val_t val;
} jsax_num_t;

// -----------------------------------------------------------------------------
// Serialization flags
// -----------------------------------------------------------------------------

enum
{
  json_flags_delim_bit = 8,
  json_flags_part_bit = 9,
  json_flags_done_bit = 10
};

typedef enum jsax_serialize_flags_e
{
  json_flag_delim = 1u << json_flags_delim_bit,
  json_flag_part = 1u << json_flags_part_bit,
  json_flag_done = 1u << json_flags_done_bit
} jsax_serialize_flags_t;

// -----------------------------------------------------------------------------
// Parser object
typedef struct jsax_s aligned(CPU_CACHE_LINE_SIZE) jsax_t;

// -----------------------------------------------------------------------------
// Serialization object
// -----------------------------------------------------------------------------

#define jsax_serialize_common_s\
  json_flags_t flags;\
                   \
  uint* stack;     \
  uint depth;      \
  uint level;      \
                   \
  u8* buf;         \
  size_t size;     \
  size_t need;     \
                   \
  u8* pos;         \
  size_t len;      \
                   \
  ushort flt_dig;  \
  ushort big_flt_dig

typedef struct jsax_serialize_s
{
  jsax_serialize_common_s;
} jsax_serialize_t;

typedef struct jsax_format_s
{
  jsax_serialize_common_s;

  uint indent_size;
  uint indent_width;
} jsax_format_t;

// -----------------------------------------------------------------------------
// Event handler callbacks
typedef bool (*jsax_key_fn)  (jsax_t*, jsax_key_t);
typedef bool (*jsax_str_fn)  (jsax_t*, jsax_str_t);
typedef bool (*jsax_num_fn)  (jsax_t*, jsax_num_t);
typedef bool (*jsax_int_fn)  (jsax_t*, intmax_t);
typedef bool (*jsax_uint_fn) (jsax_t*, uintmax_t);
typedef bool (*jsax_flt_fn)  (jsax_t*, double);
typedef bool (*jsax_bool_fn) (jsax_t*, bool);
typedef bool (*jsax_null_fn) (jsax_t*);
typedef bool (*jsax_mem_fn)  (const jsax_t*, u8**, size_t*, size_t);

// -----------------------------------------------------------------------------

#include "api/shared.h"

// =============================================================================
// Macros
// -----------------------------------------------------------------------------

#define jsax_skip_all(jsnp) do\
{                             \
  (jsnp)->chunked = true;     \
                              \
  (jsnp)->on_start = jsax_start_skip;\
  (jsnp)->on_key   = jsax_key_skip;\
  (jsnp)->on_str   = jsax_str_skip;\
  (jsnp)->on_num   = jsax_num_skip;\
  (jsnp)->on_int   = jsax_int_skip;\
  (jsnp)->on_uint  = jsax_uint_skip;\
  (jsnp)->on_flt   = jsax_flt_skip;\
  (jsnp)->on_bool  = jsax_bool_skip;\
  (jsnp)->on_null  = jsax_null_skip;\
  (jsnp)->on_end   = jsax_end_skip;\
} while (0)

// -----------------------------------------------------------------------------
// Depth stack
// -----------------------------------------------------------------------------

#define jsax_push_obj(stck, lvl) ((stck)[(lvl) / INT_BIT] &= ~(1u << ((lvl) % INT_BIT)))
#define jsax_push_arr(stck, lvl) ((stck)[(lvl) / INT_BIT] |= (1u << ((lvl) % INT_BIT)))

#define jsax_pop_coll(stck, lvl) (((stck)[(lvl) / INT_BIT] >> ((lvl) % INT_BIT)) & 1u)

// -----------------------------------------------------------------------------
// Serialization flags
// -----------------------------------------------------------------------------

#define json_flags_part(flags) (((flags) & json_flag_part) != 0)

#define json_flags_obj_val(flags) (((flags) & (json_flag_arr | json_flag_key)) == 0)
#define json_flags_key_undone(flags) (((flags) & (json_flag_key | json_flag_done)) == json_flag_key)
#define json_flags_val_undone(flags) (((flags) & (json_flag_key | json_flag_done)) == 0)

#define json_flags_undelim(flags) (((flags) & (json_flag_empty | json_flag_delim)) == 0)
#define json_flags_arr_undelim(flags) (((flags) & (json_flag_arr | json_flag_empty | json_flag_delim)) == json_flag_arr)

// -----------------------------------------------------------------------------
// Determine the extended number value type
// -----------------------------------------------------------------------------

#define jsax_is_num_str(num) json_is_num_tag_str (num.tag)
#define jsax_is_num_big(num) json_is_num_tag_big (num.tag)
#define jsax_is_num_big_flt(num) json_is_num_tag_big_flt (num.tag)
#define jsax_is_num_big_int_any(num) json_is_num_tag_big_int_any (num.tag)
#define jsax_is_num_big_int(num) json_is_num_tag_big_int (num.tag)
#define jsax_is_num_big_uint(num) json_is_num_tag_big_uint (num.tag)
#define jsax_is_num_special(num) json_is_num_tag_special (num.tag)
#define jsax_is_num_err jsax_is_num_special

// =============================================================================
// Functions
// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

extern void jsax_init (jsax_t* jsnp, u8* buf, size_t size);
extern void jsax_initi (jsax_t* jsnp, u8* buf, size_t size);

extern void jsax_init_stream (jsax_t* jsnp, u8* buf, size_t size);

// -----------------------------------------------------------------------------
// Parsing
// -----------------------------------------------------------------------------

extern bool jsax_parse (jsax_t* jsnp, u8* json, size_t size);
extern bool jsax_parsei (jsax_t* jsnp, u8* json);

extern bool jsax_parse_stream (jsax_t* jsnp, u8* json, size_t size, bool last);

// -----------------------------------------------------------------------------
// Serialization
// -----------------------------------------------------------------------------

extern void jsax_serialize_init (jsax_serialize_t* st, uint* stack, uint depth
, u8* buf, size_t size);

extern void jsax_serialize_init_stack (jsax_serialize_t* st, uint* stack, uint depth);
extern void jsax_serialize_init_buf (jsax_serialize_t* st, u8* buf, size_t size);

extern int jsax_write_start (jsax_serialize_t* st, bool obj);
extern int jsax_write_key (jsax_serialize_t* st, jsax_str_t key);
extern int jsax_write_str (jsax_serialize_t* st, jsax_str_t str);
extern int jsax_write_num (jsax_serialize_t* st, jsax_num_t num);
extern int jsax_write_int (jsax_serialize_t* st, intmax_t val);
extern int jsax_write_uint (jsax_serialize_t* st, uintmax_t val);
extern int jsax_write_flt (jsax_serialize_t* st, double val);
extern int jsax_write_bool (jsax_serialize_t* st, bool val);
extern int jsax_write_null (jsax_serialize_t* st);
extern int jsax_write_end (jsax_serialize_t* st);

// -----------------------------------------------------------------------------

extern void jsax_format_init (jsax_format_t* st, uint* stack, uint depth
, u8* buf, size_t size);

extern void jsax_format_init_stack (jsax_format_t* st, uint* stack, uint depth);
extern void jsax_format_init_buf (jsax_format_t* st, u8* buf, size_t size);

extern int jsax_pretty_start (jsax_format_t* st, bool obj);
extern int jsax_pretty_key (jsax_format_t* st, jsax_str_t key);
extern int jsax_pretty_str (jsax_format_t* st, jsax_str_t str);
extern int jsax_pretty_num (jsax_format_t* st, jsax_num_t num);
extern int jsax_pretty_int (jsax_format_t* st, intmax_t val);
extern int jsax_pretty_uint (jsax_format_t* st, uintmax_t val);
extern int jsax_pretty_flt (jsax_format_t* st, double val);
extern int jsax_pretty_bool (jsax_format_t* st, bool val);
extern int jsax_pretty_null (jsax_format_t* st);
extern int jsax_pretty_end (jsax_format_t* st);

// -----------------------------------------------------------------------------
// Predefined callbacks
// -----------------------------------------------------------------------------
// Trap callbacks: stop parsing immediately
extern bool jsax_start_trap (jsax_t* jsnp, bool obj);
extern bool jsax_key_trap (jsax_t* jsnp, jsax_key_t key);
extern bool jsax_str_trap (jsax_t* jsnp, jsax_str_t str);
extern bool jsax_num_trap (jsax_t* jsnp, jsax_num_t num);
extern bool jsax_int_trap (jsax_t* jsnp, intmax_t val);
extern bool jsax_uint_trap (jsax_t* jsnp, uintmax_t val);
extern bool jsax_flt_trap (jsax_t* jsnp, double val);
extern bool jsax_bool_trap (jsax_t* jsnp, bool val);
extern bool jsax_null_trap (jsax_t* jsnp);
extern bool jsax_end_trap (jsax_t* jsnp);
extern bool jsax_mem_trap (const jsax_t* jsnp, u8** buf, size_t* size, size_t need);

// -----------------------------------------------------------------------------
// Skip callbacks: skip the item and continue parsing
extern bool jsax_start_skip (jsax_t* jsnp, bool obj);
extern bool jsax_key_skip (jsax_t* jsnp, jsax_key_t key);
extern bool jsax_str_skip (jsax_t* jsnp, jsax_str_t str);
extern bool jsax_num_skip (jsax_t* jsnp, jsax_num_t num);
extern bool jsax_int_skip (jsax_t* jsnp, intmax_t val);
extern bool jsax_uint_skip (jsax_t* jsnp, uintmax_t val);
extern bool jsax_flt_skip (jsax_t* jsnp, double val);
extern bool jsax_bool_skip (jsax_t* jsnp, bool val);
extern bool jsax_null_skip (jsax_t* jsnp);
extern bool jsax_end_skip (jsax_t* jsnp);

// -----------------------------------------------------------------------------
// Extended number
// -----------------------------------------------------------------------------

static inline void jsax_set_num_big_flt (jsax_num_t* num, json_num_big_t* val)
{
  num->tag = json_num_flt | json_num_big;
  num->val.big = val;
}

static inline void jsax_set_num_big_int (jsax_num_t* num, json_num_big_t* val)
{
  num->tag = json_num_int | json_num_big;
  num->val.big = val;
}

static inline void jsax_set_num_big_uint (jsax_num_t* num, json_num_big_t* val)
{
  num->tag = json_num_uint | json_num_big;
  num->val.big = val;
}

// -----------------------------------------------------------------------------

static inline void jsax_set_num_str (jsax_num_t* num, json_num_str_t* str)
{
  num->tag = json_num_str;
  num->val.str = str;
}

// -----------------------------------------------------------------------------

#if C(CXX)
  }
#endif

// -----------------------------------------------------------------------------

#endif
