// =============================================================================
// Jsong Benchmark
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#ifndef BENCH_H
#define BENCH_H

// -----------------------------------------------------------------------------
// Macros
#ifdef _WIN32
  #define bench_start()\
    unsigned long long q, b, a, d;\
                                  \
    QueryPerformanceFrequency ((LARGE_INTEGER*)&q);\
    QueryPerformanceCounter ((LARGE_INTEGER*)&b)

  #define bench_end()\
    QueryPerformanceCounter ((LARGE_INTEGER*)&a);\
    d = a - b;       \
                     \
    printf ("%s: %fus [%I64u]\n", __func__, (double)d / q * 1000000, d)
#else
  #define bench_start() clock_t t = clock()
  #define bench_end() printf ("%s: %u ticks\n", __func__, (unsigned)(clock() - t))
#endif

// -----------------------------------------------------------------------------
// Read JSON from file
#ifdef __cplusplus
static char* readjson (const char* fname, size_t* out)
{
  FILE* f = fopen (fname, "rb");
  assert (f != NULL);

  int rc = fseek (f, 0, SEEK_END);
  assert (rc == 0);

  off_t size = ftell (f);
  assert (size != -1);

  rc = fseek (f, 0, SEEK_SET);
  assert (rc == 0);

  char* json = (char*)calloc (size, 1u);
  assert (json != NULL);

  off_t read = fread (json, 1u, size, f);
  assert (read == size);

  fclose (f);

  *out = size;

  return json;
}
#endif // __cplusplus

// -----------------------------------------------------------------------------

#endif
