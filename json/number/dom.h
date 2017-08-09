// =============================================================================
// <number/dom.h>
//
// JSON DOM API number definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_E2814E4400FE4C089B4762994D395F81
#define H_E2814E4400FE4C089B4762994D395F81

// -----------------------------------------------------------------------------

#define json_num_int_evnt(a) do\
{                              \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                               \
  elmnt->box.tag |= json_val_int;\
  elmnt->val.i = (a);\
} while (0)

#define json_num_uint_evnt(a) do\
{                               \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                                \
  elmnt->box.tag |= json_val_uint;\
  elmnt->val.u = (a);\
} while (0)

#define json_num_flt_evnt(a) do\
{                              \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                               \
  elmnt->box.tag |= json_val_flt;\
  elmnt->val.f = (a);\
} while (0)

// -----------------------------------------------------------------------------

#if JSON(BIG_NUMBERS)
#define json_num_big_int_evnt(a) do\
{                                  \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                                   \
  elmnt->box.tag |= json_val_num; \
  json_val_init (elmnt, num.big);  \
                                   \
  elmnt->val.num.big->i = (a);     \
  elmnt->val.tag |= json_num_int | json_num_big;\
} while (0)

#define json_num_big_uint_evnt(a) do\
{                                   \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                                    \
  elmnt->box.tag |= json_val_num;  \
  json_val_init (elmnt, num.big);   \
                                    \
  elmnt->val.num.big->u = (a);      \
  elmnt->val.tag |= json_num_uint | json_num_big;\
} while (0)

#define json_num_big_flt_evnt(a) do\
{                                  \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                                   \
  elmnt->box.tag |= json_val_num; \
  json_val_init (elmnt, num.big);  \
                                   \
  elmnt->val.num.big->f = (a);     \
  elmnt->val.tag |= json_num_flt | json_num_big;\
} while (0)
#endif // JSON(BIG_NUMBERS)

// -----------------------------------------------------------------------------

#define json_num_oflow_evnt(meta) do\
{                                   \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                                    \
  elmnt->box.tag |= json_val_num;  \
  elmnt->val.tag = json_num_special \
  | json_num_err | json_num_special_oflow\
  | (meta.sign << json_num_special_sign_bit);\
} while (0)

#define json_num_uflow_evnt(meta) do\
{                                   \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                                    \
  elmnt->box.tag |= json_val_num;  \
  elmnt->val.tag = json_num_special \
  | json_num_err | json_num_special_uflow\
  | (meta.sign << json_num_special_sign_bit);\
} while (0)

// -----------------------------------------------------------------------------

#endif
