// =============================================================================
// <number/sax.h>
//
// JSON SAX API number definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_CE982CBBE3B24594B6D83DBA9B6C6FFF
#define H_CE982CBBE3B24594B6D83DBA9B6C6FFF

// -----------------------------------------------------------------------------

#define json_num_int_evnt(a) do\
{                              \
  jsax_callback (jsnp->on_int, jsnp, (a));\
} while (0)

#define json_num_uint_evnt(a) do\
{                               \
  jsax_callback (jsnp->on_uint, jsnp, (a));\
} while (0)

#define json_num_flt_evnt(a) do\
{                              \
  jsax_callback (jsnp->on_flt, jsnp, (a));\
} while (0)

// -----------------------------------------------------------------------------

#if JSON(BIG_NUMBERS)
#define json_num_big_int_evnt(a) do\
{                                  \
  jsax_num_t num;                  \
  json_num_big_t big;              \
                                   \
  big.i = (a);                     \
  num.val.big = &big;              \
  num.tag = json_num_int | json_num_big;\
                                   \
  jsax_callback (jsnp->on_num, jsnp, num);\
} while (0)

#define json_num_big_uint_evnt(a) do\
{                                   \
  jsax_num_t num;                   \
  json_num_big_t big;               \
                                    \
  big.u = (a);                      \
  num.val.big = &big;               \
  num.tag = json_num_uint | json_num_big;\
                                    \
  jsax_callback (jsnp->on_num, jsnp, num);\
} while (0)

#define json_num_big_flt_evnt(a) do\
{                                  \
  jsax_num_t num;                  \
  json_num_big_t big;              \
                                   \
  big.f = (a);                     \
  num.val.big = &big;              \
  num.tag = json_num_flt | json_num_big;\
                                   \
  jsax_callback (jsnp->on_num, jsnp, num);\
} while (0)
#endif

// -----------------------------------------------------------------------------

#define json_num_oflow_evnt(meta) do\
{                                   \
  jsax_num_t num;                   \
                                    \
  num.tag = json_num_special        \
  | json_num_err | json_num_special_oflow\
  | (meta.sign << json_num_special_sign_bit);\
                                    \
  jsax_callback (jsnp->on_num, jsnp, num);\
} while (0)

#define json_num_uflow_evnt(meta) do\
{                                   \
  jsax_num_t num;                   \
                                    \
  num.tag = json_num_special        \
  | json_num_err | json_num_special_uflow\
  | (meta.sign << json_num_special_sign_bit);\
                                    \
  jsax_callback (jsnp->on_num, jsnp, num);\
} while (0)

// -----------------------------------------------------------------------------

#endif
