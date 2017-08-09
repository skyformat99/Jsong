// =============================================================================
// JSON Unescaper
//
// Unescape a JSON string from the standard input to the standard output.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/unicode.h>
#include <quantum/buffer.h>
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

  u8 buf[BUF_SIZE];
  bool eof = false;

  size_t off = 0;
  size_t bufsz = fread (buf, 1u, 3u, stdin);

  if (bufsz != 3u)
  {
    if (!feof (stdin))
    {
error_io:
      fprintf (stderr, "%s.\n", "I/O error");
      return EXIT_FAILURE;
    }

    eof = true;

    goto unescape;
  }

  if (!buf_equal (buf, utf8_bom, 3u)) off = 3u;

  while (true)
  {
    int ret;
    const u8* pos;
    size_t outsz;

    size_t size = numof (buf) - off;
    bufsz = fread (buf + off, 1u, size, stdin);

    if (bufsz != size)
    {
      if (!feof (stdin)) goto error_io;
      eof = true;
    }

    bufsz += off;
    off = 0;

unescape:
    ret = json_unescape_inplace (buf, bufsz, (u8**)&pos, &outsz);
    size = fwrite (buf, 1u, outsz, stdout);

    if (size != outsz) goto error_io;

    if (ret < 0)
    {
      if (ret == INT_MIN)
      {
error_input:
        fprintf (stderr, "%s.\n", "Invalid input");
        return EXIT_FAILURE;
      }

      off = bufsz - (size_t)(pos - buf);
    }

    if (eof)
    {
      if (off != 0) goto error_input;
      break;
    }

    buf_move (buf, buf + bufsz - off, off);
  }

  return EXIT_SUCCESS;
}
