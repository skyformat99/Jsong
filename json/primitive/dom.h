// =============================================================================
// <primitive/dom.h>
//
// JSON DOM API boolean and null value definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_F4025E8DFFD0476E8B592B965692959C
#define H_F4025E8DFFD0476E8B592B965692959C

// -----------------------------------------------------------------------------
// Boolean value handling
#define json_bool_evnt(v) do\
{                           \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                            \
  elmnt->box.tag |= json_val_tok;\
  elmnt->val.tag = json_bool_type | v;\
} while (0)

// -----------------------------------------------------------------------------
// Null value handling
#define json_null_evnt() do\
{                          \
  json_elmnt_t* elmnt = jsnp->elmnt;\
                           \
  elmnt->box.tag |= json_val_tok;\
  elmnt->val.tag = json_null;\
} while (0)

// -----------------------------------------------------------------------------

#endif
