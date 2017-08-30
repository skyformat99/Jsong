// =============================================================================
// RapidJSON Benchmark
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifdef _WIN32
  #include <Windows.h>
#else
  #include <time.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdio.h>

// -----------------------------------------------------------------------------

#ifdef _WIN32
  #define stricmp _stricmp
#else
  #define stricmp strcasecmp
#endif

// -----------------------------------------------------------------------------

#ifdef RAPIDJSON_RECURSIVE
  // Recursive parsing (potentially overflowing the stack and crashing everything)
  #define RAPIDJSON_PARSE_DEFAULT_FLAGS (kParseStopWhenDoneFlag)
#else
  // Jsong never recurses and always stops parsing after the root element end
  #define RAPIDJSON_PARSE_DEFAULT_FLAGS (kParseIterativeFlag | kParseStopWhenDoneFlag)
#endif

#include <rapidjson/document.h>
#include <rapidjson/reader.h>

using namespace rapidjson;

// -----------------------------------------------------------------------------

#include "bench.h"

// -----------------------------------------------------------------------------
// DOM API test
static void rapiddom (char* fname)
{
  size_t size;
  char* json = readjson (fname, &size);

  Document doc;

  bench_start();

  doc.ParseInsitu (json);

  bench_end();

  free (json);

  if (doc.HasParseError())
  {
    printf ("Error: %u [%u]\n", doc.GetParseError()
    , (unsigned)(doc.GetErrorOffset()));
  }
}

// -----------------------------------------------------------------------------
// SAX API test
// -----------------------------------------------------------------------------
// The callbacks are inlined by the compiler
struct SAXHandler: public BaseReaderHandler<UTF8<>, SAXHandler>
{
  bool Null() {return true;}
  bool Bool (bool b) {return true;}
  bool Int (int i) {return true;}
  bool Uint (unsigned u) {return true;}
  bool Int64 (int64_t i) {return true;}
  bool Uint64 (uint64_t u) {return true;}
  bool Double (double d) {return true;}
  bool String (const char* str, SizeType length, bool copy) {return true;}
  bool StartObject() {return true;}
  bool Key(const char* str, SizeType length, bool copy) {return true;}
  bool EndObject(SizeType memberCount) {return true;}
  bool StartArray() {return true;}
  bool EndArray(SizeType elementCount) {return true;}
};

static void rapidsax (char* fname)
{
  size_t size;
  char* json = readjson (fname, &size);

  SAXHandler handler;
  Reader reader;

  InsituStringStream iss (json);

  bench_start();

  reader.Parse (iss, handler);

  bench_end();

  free (json);

  if (reader.HasParseError())
  {
    printf ("Error: %u [%u]\n", reader.GetParseErrorCode()
    , (unsigned)(reader.GetErrorOffset()));
  }
}

// =============================================================================

int main (int argc, char** argv)
{
  // Parse command line arguments
  if (argc != 2)
  {
usage:
    fprintf (stderr, "Usage: %s document.json\n", argv[0]);
    return EXIT_FAILURE;
  }

  char* fname = argv[1];

  // DOM benchmark
  rapiddom (fname);

  // SAX benchmark
  rapidsax (fname);

  return EXIT_SUCCESS;
}
