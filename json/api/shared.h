// =============================================================================
// <api/shared.h>
//
// JSON API shared definitions.
//
// These definitions are shared between APIs, but with subtle differences.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#if !H(F2966EB7054B4C99B8E719FA00C91982) && !H(FC78D0A7AEB640E9BA16E56CC02715BE)
  #error "This header must not be included directly"
#endif

#if (H(F2966EB7054B4C99B8E719FA00C91982) && !H(A3263E8F162D478FAB413A8B50F7E7F2))\
|| (H(FC78D0A7AEB640E9BA16E56CC02715BE) && !H(DC0B83F6FA1442759F6240A101A608F3))

#if H(F2966EB7054B4C99B8E719FA00C91982)
  // SAX API definitions
  #define H_A3263E8F162D478FAB413A8B50F7E7F2
#elif H(FC78D0A7AEB640E9BA16E56CC02715BE)
  // DOM API definitions
  #define H_DC0B83F6FA1442759F6240A101A608F3
#endif

// =============================================================================
// Types
// -----------------------------------------------------------------------------
// Parser object
#if H(A3263E8F162D478FAB413A8B50F7E7F2)
  struct jsax_s
#else
  struct json_s
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

#if H(A3263E8F162D478FAB413A8B50F7E7F2)
  // Parse JSON strings in chunks
  bint chunked;
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
  } st;

#if H(A3263E8F162D478FAB413A8B50F7E7F2)
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
#endif

  // Position in the input buffer
  size_t pos;
  // Bytes needed to complete the current JSON sequence
  size_t need;
};

// =============================================================================
// Macros
// -----------------------------------------------------------------------------

#if H(A3263E8F162D478FAB413A8B50F7E7F2)
  // Access the SAX parser structure embedded in other object
  #define jsax_cast(obj) (jsax_t*)(obj)

  #define jsax_key_len(key) (json_hash_len (key.hash))
#else
  #define json_key_len(key) (json_hash_len ((key)->hash))
#endif

// -----------------------------------------------------------------------------

#endif
