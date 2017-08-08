// =============================================================================
// Jsong Benchmark
//
// Measure the performance of Jsong JSON parser
// with various JSON inputs using both DOM and SAX APIs.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>

#if OS(WIN32)
  #include <Windows.h>
#else
  #include <quantum/time.h>
#endif

#include <quantum/core.h>

#include <quantum/string/implicit.h>
#include <quantum/io.h>

// -----------------------------------------------------------------------------

#include <json/dom.h>
#include <json/sax.h>
#include <json/utils.h>

// -----------------------------------------------------------------------------

#include "bench.h"

// =============================================================================
// Constants
// -----------------------------------------------------------------------------

#if (SAX_BUF_SIZE == 0)
  #define SAX_BUF_SIZE 4096u
#endif

#if (SAX_BUF_SIZE < 256)
  #error "Invalid definition of `SAX_BUF_SIZE`"
#endif

// =============================================================================
// Functions
// -----------------------------------------------------------------------------
// Read the JSON data from file and get its size.
// Optionally, null-terminate the read buffer.
static u8* readjson (const u8* fname, size_t* out, bint zero)
{
  u8* json;

  io_file_t* f = fopen (fname, "rb");

  if (f == null)
  {
    perror ("fopen");
    return null;
  }

  if (!io_file_consume (f, &json, out, zero | io_consume_utf8))
  {
    fclose (f);
    return null;
  }

  fclose (f);

  return json;
}

// =============================================================================
// DOM API benchmarks
// -----------------------------------------------------------------------------

static bint dombenchz (const u8* fname)
{
  size_t size;
  u8* json = readjson (fname, &size, true);

  if (json == null) return false;

  mem_pool_t* pool = json_pool_create();

  if (pool == null)
  {
    fprintf (stderr, "%s.\n", json_errors[JSON_ERROR_MEMORY]);
    return false;
  }

  json_t jsnp;
  json_initi (&jsnp, pool);

  bench_start();

  json_node_t root = json_parsei (&jsnp, json);

  if (json_is_elmnt (root))
  {
    bench_end();
  }
  else
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));
  }

  json_pool_destroy (pool);
  free (json);

  return true;
}

// -----------------------------------------------------------------------------

static bint dombenchb (const u8* fname)
{
  size_t size;
  u8* json = readjson (fname, &size, false);

  if (json == null) return false;

  mem_pool_t* pool = json_pool_create();

  if (pool == null)
  {
    fprintf (stderr, "%s\n", json_errors[JSON_ERROR_MEMORY]);
    return false;
  }

  json_t jsnp;
  json_init (&jsnp, pool);

  bench_start();

  json_node_t root = json_parse (&jsnp, json, size);

  if (json_is_elmnt (root))
  {
    bench_end();
  }
  else
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));
  }

  json_pool_destroy (pool);
  free (json);

  return true;
}

// -----------------------------------------------------------------------------

static bint dombenchs (const u8* fname)
{
  size_t size;
  u8* json = readjson (fname, &size, false);

  if (json == null) return false;

  mem_pool_t* pool = json_pool_create();

  if (pool == null)
  {
    fprintf (stderr, "%s\n", json_errors[JSON_ERROR_MEMORY]);
    return false;
  }

  json_t jsnp;
  json_init_stream (&jsnp, pool);

  bench_start();

  json_node_t root = json_parse_stream (&jsnp, json, size, true);

  if (json_is_elmnt (root))
  {
    bench_end();
  }
  else
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));
  }

  json_pool_destroy (pool);
  free (json);

  return true;
}

// =============================================================================
// SAX API benchmarks
// -----------------------------------------------------------------------------

static bint saxbenchz (const u8* fname)
{
  u8 tmp[SAX_BUF_SIZE];

  size_t size;
  u8* json = readjson (fname, &size, true);

  if (json == null) return false;

  jsax_t jsnp;
  jsax_initi (&jsnp, tmp, numof (tmp));
  jsax_verify (&jsnp);

  bench_start();

  bint rc = jsax_parsei (&jsnp, json);

  if (rc)
  {
    bench_end();
  }
  else
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));
  }

  free (json);

  return true;
}

// -----------------------------------------------------------------------------

static bint saxbenchb (const u8* fname)
{
  u8 tmp[SAX_BUF_SIZE];

  size_t size;
  u8* json = readjson (fname, &size, false);

  if (json == null) return false;

  jsax_t jsnp;
  jsax_initi (&jsnp, tmp, numof (tmp));
  jsax_verify (&jsnp);

  bench_start();

  bint rc = jsax_parse (&jsnp, json, size);

  if (rc)
  {
    bench_end();
  }
  else
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));
  }

  free (json);

  return true;
}

// -----------------------------------------------------------------------------

static bint saxbenchs (const u8* fname)
{
  u8 tmp[SAX_BUF_SIZE];

  size_t size;
  u8* json = readjson (fname, &size, false);

  if (json == null) return false;

  jsax_t jsnp;
  jsax_init_stream (&jsnp, tmp, numof (tmp));
  jsax_verify (&jsnp);

  bench_start();

  bint rc = jsax_parse_stream (&jsnp, json, size, true);

  if (rc)
  {
    bench_end();
  }
  else
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));
  }

  free (json);

  return true;
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

  // DOM API benchmarks
  if (!dombenchz (fname)) return EXIT_FAILURE;
  if (!dombenchb (fname)) return EXIT_FAILURE;
  if (!dombenchs (fname)) return EXIT_FAILURE;

  putchar ('\n');

  // SAX API benchmarks
  if (!saxbenchz (fname)) return EXIT_FAILURE;
  if (!saxbenchb (fname)) return EXIT_FAILURE;
  if (!saxbenchs (fname)) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
