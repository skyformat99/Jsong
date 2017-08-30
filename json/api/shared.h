// =============================================================================
// <api/shared.h>
//
// JSON API shared definitions. These definitions are shared between APIs,
// but with subtle differences.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if !H(F2966EB7054B4C99B8E719FA00C91982) && !H(FC78D0A7AEB640E9BA16E56CC02715BE)
  #error "This header must not be included directly"
#endif

// -----------------------------------------------------------------------------
// This header can be included four times maximum
#if (H(JSON_SAX) && !ENABLED(JSON_SAX)) || (H(JSON_DOM) && !ENABLED(JSON_DOM))\
 || (H(USON_SAX) && !ENABLED(USON_SAX)) || (H(USON_DOM) && !ENABLED(USON_DOM))

// -----------------------------------------------------------------------------
// SAX API definitions
#if H(JSON_SAX)
  #define ENABLE_JSON_SAX
#elif H(USON_SAX)
  #define ENABLE_USON_SAX
// DOM API definitions
#elif H(JSON_DOM)
  #define ENABLE_JSON_DOM
#elif H(USON_DOM)
  #define ENABLE_USON_DOM
#endif

// =============================================================================
// Types
// -----------------------------------------------------------------------------
// Parser object
#if H(JSON_SAX)
  struct jsax_s
#elif H(USON_SAX)
  struct usax_s
#elif H(JSON_DOM)
  struct json_s
#elif H(USON_DOM)
  struct uson_s
#endif
{
  // Parser state
  uint state;

  // Current line number
  uint line;
  // Character number on the current line
  uint col;

  // Error code
  uint err;

#if H(JSON_SAX) || H(USON_SAX)
  // Parse JSON strings in chunks
  bool chunked;
  // Zero-based stack depth
  uint depth;
  // Nested collections state.
  // Each bit represents an object (0) or an array (1).
  // A depth stack of 8 bytes is capable of holding 64 nested collections.
  uint stack[JSON_DEPTH_SIZE];
#else
  // Pool for memory allocations
  mem_pool_t* pool;
  // Current collection
  json_coll_t* coll;
  // Current element
  json_elmnt_t* elmnt;
#endif

  // Intermediate buffer used to parse values split across buffers.
  // In case of SAX API buffer is managed by the caller.
  u8* buf;
  size_t size;
  size_t used;

  // Parsing state of the current value
  union
  {
    json_val_st_t val;
    json_str_st_t str;
    json_num_st_t num;

#if H(USON_SAX) || H(USON_DOM)
    uson_verb_st_t verb;
    uson_data_st_t data;
#endif
  } st;

#if H(JSON_SAX)
  // Callback event handlers
  jsax_bool_fn on_start;
  jsax_key_fn  on_key;
  jsax_str_fn  on_str;
  jsax_num_fn  on_num;
  jsax_flt_fn  on_flt;
  jsax_int_fn  on_int;
  jsax_uint_fn on_uint;
  jsax_bool_fn on_bool;
  jsax_null_fn on_null;
  jsax_null_fn on_end;
  jsax_mem_fn  on_mem;
#elif H(USON_SAX)
  usax_bool_fn on_start;
  usax_key_fn  on_key;
  usax_str_fn  on_str;
  usax_num_fn  on_num;
  usax_flt_fn  on_flt;
  usax_int_fn  on_int;
  usax_uint_fn on_uint;
  usax_bool_fn on_bool;
  usax_null_fn on_null;
  usax_null_fn on_end;
  usax_str_fn  on_id;
  usax_str_fn  on_mime;
  usax_mem_fn  on_mem;
#endif

  // Position in the input buffer
  size_t pos;
  // Bytes needed to complete the current JSON sequence
  size_t need;
};

// -----------------------------------------------------------------------------
// SAX serialization object
// -----------------------------------------------------------------------------

#if H(JSON_SAX) || H(USON_SAX)

#ifndef jsax_serialize_common_s
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
#endif

#if H(JSON_SAX)
  struct jsax_serialize_s
#elif H(USON_SAX)
  struct usax_serialize_s
#endif
{
  jsax_serialize_common_s;
};

#if H(JSON_SAX)
  struct jsax_format_s
#elif H(USON_SAX)
  struct usax_format_s
#endif
{
  jsax_serialize_common_s;

  uint indent_size;
  uint indent_width;
};

#undef jsax_serialize_common_s

#endif // H(JSON_SAX) || H(USON_SAX)

// -----------------------------------------------------------------------------

#if H(JSON_DOM) || H(USON_DOM)

#ifndef json_serialize_common_s
  #define json_serialize_common_s\
    json_node_t root;\
    json_node_t node;\
                     \
    u8* buf;         \
    size_t fill;     \
    size_t size;     \
    size_t need;     \
                     \
    u8* pos;         \
    size_t len;      \
                     \
    bool key;        \
    bool term;       \
                     \
    ushort flt_dig;  \
    ushort big_flt_dig
#endif

#ifndef uson_serialize_common_s
  #define uson_serialize_common_s\
    uson_str_verb_t* verb;\
    bool data;\
    bool config
#endif

#if H(JSON_DOM)
  struct json_serialize_s
#elif H(USON_DOM)
  struct uson_serialize_s
#endif
{
  json_serialize_common_s;

#if H(USON_DOM)
  uson_serialize_common_s;
#endif
};

#if H(JSON_DOM)
  struct json_format_s
#elif H(USON_DOM)
  struct uson_format_s
#endif
{
  json_serialize_common_s;

#if H(USON_DOM)
  uson_serialize_common_s;
#endif

  uint indent_size;
  uint indent_width;
};

#undef json_serialize_common_s

#endif // H(JSON_DOM) || H(USON_DOM)

// -----------------------------------------------------------------------------

#undef H_JSON_SAX
#undef H_JSON_DOM

#undef H_USON_SAX
#undef H_USON_DOM

// -----------------------------------------------------------------------------

#endif
