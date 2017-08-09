// =============================================================================
// <api/common.h>
//
// JSON API common definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_6B58350F01524715AEE7F79CAA7F2C37
#define H_6B58350F01524715AEE7F79CAA7F2C37

// -----------------------------------------------------------------------------

#include "../utils/support.h"

// =============================================================================
// Constants
// -----------------------------------------------------------------------------
// Minimum parsing buffer size (length of the surrogate Unicode sequence)
#define JSON_SEQ_LEN_MAX 12u
#define JSON_BUF_SIZE_MIN JSON_SEQ_LEN_MAX

// -----------------------------------------------------------------------------
// Limit the maximum property key string length to 65535 characters
// and reserve the remaining bits for the hash value
#if JSON(HASH_KEYS)
  #define JSON_KEY_LEN_MAX 0xFFFFu
#else
  #define JSON_KEY_LEN_MAX UINT_MAX
#endif

// =============================================================================
// Types
// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------
// Value string
// -----------------------------------------------------------------------------

typedef struct json_str_s
{
  // String length excluding the terminating '\0'
  uint len;
  // Null-terminated unescaped UTF-8 string
  u8* buf;
} json_str_t, jsax_str_t;

#define json_str_make(buf, len) (json_str_t){(len), (buf)}

#define json_str_from_stri(str) (json_str_t){(uint)stri_length (str), (str)}
#define json_str_from_cstr(str) (json_str_t){(uint)cstrlen (str), (str)}

// -----------------------------------------------------------------------------
// Key string (structure layout must be identical to that of value string)
typedef struct json_key_s
{
  uint hash;
  u8* buf;
} json_key_t, jsax_key_t;

// -----------------------------------------------------------------------------
// Number
// -----------------------------------------------------------------------------
// Number string metadata
typedef struct json_num_meta_s
{
  // Flags
  u8 sign: 1,     // Is a negative number
     floating: 1, // Is a floating point number
     fraction: 1, // Has fractional part
     exponent: 1, // Has exponential part
     exponent_sign: 1, // Has exponent sign
     reserved: 1;

  // Length of the cardinal part excluding sign
  u8 len_cardinal;

  // Length of the fractional part excluding dot
  u8 len_fraction;

  // Length of the exponential part excluding [Ee] and its sign
  // is derived as such: `len - sign - len_cardinal - fraction - len_fraction
  // - exponent - exponent_sign`

  // Total number length
  u8 len_number;
} json_num_meta_t;

// -----------------------------------------------------------------------------
// Number string
typedef struct json_num_str_s
{
  // Parsed number metadata
  json_num_meta_t meta;
  // Null-terminated ASCII string
  u8* buf;
} json_num_str_t;

// -----------------------------------------------------------------------------
// Big number value
#if JSON(BIG_NUMBERS)
  // Determine the big integral type
  #if HAVE(INT128)
    // GCC and Clang can provide 128-bit integral type as an extension
    typedef s128 json_big_int_t;
    typedef u128 json_big_uint_t;
  #else
    // Fallback to `intmax_t`
    typedef intmax_t json_big_int_t;
    typedef uintmax_t json_big_uint_t;
  #endif

  // Determine the big floating point type
  #if defined(FLT128_MAX)
    // GCC and Clang can provide 128-bit floating point type as an extension
    // with 113 bits of significand precision
    typedef f128 json_big_flt_t;
  #else
    // GCC and Clang can make extended precision floating point type
    // available with 64 bits of significand precision for x87 ISA.
    //
    // For MSC `long double` is actually just `double`
    // and has 53 bits of significand precision.
    typedef long double json_big_flt_t;
  #endif

  typedef union json_num_big_u
  {
    json_big_flt_t f;
    json_big_int_t i;
    json_big_uint_t u;
  } json_num_big_t;
#else
  typedef void json_num_big_t;
#endif

// -----------------------------------------------------------------------------
// Number type
typedef enum json_num_type_e
{
  json_num_special,
  json_num_flt,
  json_num_int,
  json_num_uint
} json_num_type_t;

// -----------------------------------------------------------------------------
// Number kind
// -----------------------------------------------------------------------------

enum
{
  json_num_type_bit = 0,
  json_num_kind_bit = 2
};

typedef enum json_num_kind_e
{
  // High magnitude number
  json_num_big = 0 << json_num_kind_bit,
  // Number parsed as string
  json_num_str = 1 << json_num_kind_bit
} json_num_kind_t;

// -----------------------------------------------------------------------------
// Special Number kind
// -----------------------------------------------------------------------------

enum
{
  json_num_special_kind_bit = 3
};

typedef enum json_num_special_kind_e
{
  // Range error
  json_num_err = 0 << json_num_special_kind_bit,
  // Infinity and zero (distinguished from the above as not an error)
  json_num_inf = 1 << json_num_special_kind_bit,
  json_num_zero = json_num_inf,
  // NaN (not a number)
  json_num_nan = 2 << json_num_special_kind_bit
} json_num_special_kind_t;

// -----------------------------------------------------------------------------
// Special number flags
// -----------------------------------------------------------------------------

enum
{
  json_num_special_sign_bit = 8,
  json_num_special_range_bit = 9
};

typedef enum json_num_special_sign_e
{
  json_num_special_pos = 0 << json_num_special_sign_bit,
  json_num_special_neg = 1 << json_num_special_sign_bit
} json_num_special_sign_t;

typedef enum json_num_special_range_e
{
  json_num_special_oflow = 0 << json_num_special_range_bit,
  json_num_special_uflow = 1 << json_num_special_range_bit
} json_num_special_range_t;

// -----------------------------------------------------------------------------
// Metadata bits
enum
{
  // Number type bits
  json_num_type = 3u << json_num_type_bit,
  // Number kind bit
  json_num_kind = 1u << json_num_kind_bit,
  // Special number kind bits
  json_num_special_kind = 31u << json_num_special_kind_bit,
  // Special number sign bit
  json_num_special_sign = 1u << json_num_special_sign_bit,
  // Special number range
  json_num_special_range = 1u << json_num_special_range_bit
};

// -----------------------------------------------------------------------------
// Parser types
// -----------------------------------------------------------------------------
// Parser state
typedef enum json_state_e
{
  json_state_root,  // Expecting root element

  json_state_key,   // Expecting object property key
  json_state_value, // Expecting object or array value

  json_state_colon, // Expecting colon after property key
  json_state_comma, // Expecting comma after value

  json_state_string, // On string
  json_state_number, // On number

  json_state_done,
  json_state_error = json_state_done
} json_state_t;

// -----------------------------------------------------------------------------
// Parser flags
// -----------------------------------------------------------------------------

enum
{
  json_flags_coll_bit = 0,
  json_flags_empty_bit = 2,
  json_flags_key_bit = 3,
  json_flags_last_bit = 4
};

typedef enum json_flags_e
{
  // Whether the current collection is an object, an array, or a root collection
  json_flag_obj = 0 << json_flags_coll_bit,
  json_flag_arr = 1 << json_flags_coll_bit,
  json_flag_root = 2 << json_flags_coll_bit,
  // Whether the current collection is empty
  json_flag_empty = 1 << json_flags_empty_bit,
  // Whether the current string is an object property key or a value
  json_flag_key = 1 << json_flags_key_bit,
  json_flag_val = 0 << json_flags_key_bit,
  // Whether the current JSON chunk is the last one in a stream
  json_flag_last = 1 << json_flags_last_bit
} json_flags_t;

// -----------------------------------------------------------------------------
// Metadata bits
enum
{
  json_flags_coll = 3u << json_flags_coll_bit
};

// -----------------------------------------------------------------------------
// Value state
typedef struct json_val_st_s
{
  size_t pos;
  size_t len;
} json_val_st_t;

// -----------------------------------------------------------------------------
// String state
typedef struct json_str_st_s
{
  // Beginning of the string in JSON data
  size_t pos;
  // Calculated length of the unescaped string
  size_t len;
} json_str_st_t;

// -----------------------------------------------------------------------------
// Number state
typedef struct json_num_st_s
{
  // Beginning of the number string in JSON data
  size_t pos;
  // Its determined length
  size_t len;
  // Metadata collected so far
  json_num_meta_t meta;
  // Absolute exponent value
  uint exponent;
  // Absolute mantissa value
  uintmax_t mantissa;
} json_num_st_t;

// =============================================================================
// Macros
// -----------------------------------------------------------------------------
// Key hash length
#if JSON(HASH_KEYS)
  #define json_hash_len(hash) ((hash) & JSON_KEY_LEN_MAX)
#else
  #define json_hash_len(hash) (hash)
#endif

// -----------------------------------------------------------------------------
// Extended number type
// -----------------------------------------------------------------------------

#define json_is_num_tag_str(tag) (((tag) & json_num_kind) == json_num_str)
#define json_is_num_tag_big(tag) (((tag) & json_num_kind) == json_num_big)
#define json_is_num_tag_big_flt(tag) (((tag) & (json_num_type | json_num_kind)) == (json_num_flt | json_num_big))
#define json_is_num_tag_big_int_any(tag) (((tag) & (json_num_int | json_num_big)) == (json_num_int | json_num_big))
#define json_is_num_tag_big_int(tag) (((tag) & (json_num_type | json_num_kind)) == (json_num_int | json_num_big))
#define json_is_num_tag_big_uint(tag) (((tag) & (json_num_type | json_num_kind)) == (json_num_uint | json_num_big))
#define json_is_num_tag_special(tag) (((tag) & json_num_type) == json_num_special)

// -----------------------------------------------------------------------------
// Parser flags
// -----------------------------------------------------------------------------

#define json_flags_root(flags) (((flags) & json_flag_root) != 0)
#define json_flags_empty(flags) (((flags) & json_flag_empty) != 0)

#define json_flags_obj(flags) (((flags) & json_flags_coll) == 0)
#define json_flags_obj_empty(flags) (((flags) & (json_flags_coll | json_flag_empty)) == json_flag_empty)

#define json_flags_arr(flags) (((flags) & json_flags_coll) == json_flag_arr)
#define json_flags_arr_empty(flags) (((flags) & (json_flags_coll | json_flag_empty)) == (json_flag_arr | json_flag_empty))

#define json_flags_key(flags) (((flags) & json_flag_key) != 0)
#define json_flags_last(flags) (((flags) & json_flag_last) != 0)

// -----------------------------------------------------------------------------
// Error condition
// -----------------------------------------------------------------------------

#define json_error_internal(jsnp) ((jsnp)->err <= JSON_ERROR_DEPTH)
#define json_error_value(jsnp) (((jsnp)->err >= JSON_ERROR_CHARACTER) && ((jsnp)->err <= JSON_ERROR_NUMBER))
#define json_error_string(jsnp) (((jsnp)->err >= JSON_ERROR_CHARACTER) && ((jsnp)->err <= JSON_ERROR_UNICODE))
#define json_error_token(jsnp) ((jsnp)->err >= JSON_ERROR_TOKEN)

#define json_error_msg(jsnp) json_errors[(jsnp)->err]

// -----------------------------------------------------------------------------

#endif
