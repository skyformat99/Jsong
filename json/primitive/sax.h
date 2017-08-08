// =============================================================================
// <primitive/sax.h>
//
// JSON SAX API boolean and null value definitions.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef H_C433DB09408840C2B0F5E917E2DA6C15
#define H_C433DB09408840C2B0F5E917E2DA6C15

// -----------------------------------------------------------------------------

#define json_bool_evnt(val) jsax_callback (jsnp->on_bool, jsnp, val)
#define json_null_evnt() jsax_callback (jsnp->on_null, jsnp)

// -----------------------------------------------------------------------------

#endif
