// =============================================================================
// JSON Escaper
//
// Escape JSON string passed in standard input.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/unicode.h>
#include <quantum/io.h>

// -----------------------------------------------------------------------------

#include <json/utils.h>

// -----------------------------------------------------------------------------

#if (BUF_SIZE == 0)
  #define BUF_SIZE 4096u
#endif

#if (BUF_SIZE < 256)
  #error "Invalid definition of `BUF_SIZE`"
#endif

// =============================================================================

int main (int argc, char** argv)
{
  // Parse command line arguments
  if (argc != 1)
  {
    fprintf (stderr, "Usage: %s < in.json > out.json\n", argv[0]);
    return EXIT_FAILURE;
  }

#if OS(WIN32)
  // Put both streams into binary mode
  if (fflush (stdin) == EOF) goto error_io;
  if (setmode (STDIN, O_BINARY) == -1) goto error_io;

  if (fflush (stdout) == EOF) goto error_io;
  if (setmode (STDOUT, O_BINARY) == -1) goto error_io;
#endif

  u8 in[BUF_SIZE];
  u8 out[BUF_SIZE];

  bint eof = false;

  size_t off = 0;
  size_t insz = fread (in, 1u, 3u, stdin);

  if (insz != 3u)
  {
    if (!feof (stdin))
    {
error_io:
      fprintf (stderr, "%s.\n", "I/O error");
      return EXIT_FAILURE;
    }

    eof = true;

    goto escape;
  }

  if (!buf_equal (in, utf8_bom, 3u)) off = 3u;

  while (true)
  {
    int ret;
    const u8* ptr;
    const u8* pos;
    size_t outsz;

    size_t size = numof (in) - off;
    insz = fread (in + off, 1u, size, stdin);

    if (insz != size)
    {
      if (!feof (stdin)) goto error_io;
      eof = true;
    }

    insz += off;
    off = 0;

escape:
    ptr = in;

    while (true)
    {
      ret = json_escape (ptr, insz, out, numof (out), (u8**)&pos, &outsz);
      size = fwrite (out, 1u, outsz, stdout);

      if (size != outsz) goto error_io;
      if (ret == 0) break;

      insz -= (size_t)(pos - ptr);
      ptr = pos;
    }

    if (eof) break;
  }

  return EXIT_SUCCESS;
}
