// =============================================================================
// Jsong
//
// This unit is here merely to test compatibility of Jsong headers with C++.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>

// Needs to be included first in order to resolve
// the `noreturn` keyword conflict with <quantum/core.h>
#include <stdlib.h>

#include <quantum/core.h>

#include <json/dom.h>
#include <json/sax.h>
#include <json/utils.h>

// -----------------------------------------------------------------------------

int main (int argc, char** argv)
{
  return EXIT_SUCCESS;
}
