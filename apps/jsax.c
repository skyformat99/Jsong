// =============================================================================
// JSON Formatter
//
// Reads the JSON input from the standard input
// and streams it back to the standard output
// with pretty formatting or minified.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/unicode.h>
#include <quantum/integer.h>
#include <quantum/buffer.h>
#include <quantum/string/implicit.h>
#include <quantum/io.h>

// -----------------------------------------------------------------------------

#include <json/sax.h>
#include <json/utils.h>
#include <json/utils/misc.h>
#include <json/utils/consts.h>
#include <json/serialize.h>

// =============================================================================
// Constants
// -----------------------------------------------------------------------------
// JSON data buffer
#if (IN_BUF_SIZE == 0)
  #define IN_BUF_SIZE 4096u
#endif

#if (IN_BUF_SIZE < JSON_BUF_SIZE_MIN)
  #error "Invalid definition of `IN_BUF_SIZE`"
#endif

// -----------------------------------------------------------------------------
// JSON data buffer
#if (OUT_BUF_SIZE == 0)
  #define OUT_BUF_SIZE 4096u
#endif

#if (OUT_BUF_SIZE < 256)
  #error "Invalid definition of `OUT_BUF_SIZE`"
#endif

// -----------------------------------------------------------------------------
// Intermediate JSON value buffer
#if (VAL_BUF_SIZE == 0)
  #define VAL_BUF_SIZE 4096u
#endif

#if (VAL_BUF_SIZE < 256)
  #error "Invalid definition of `VAL_BUF_SIZE`"
#endif

// -----------------------------------------------------------------------------
// Indentation size for pretty printing
#if (INDENT_SIZE == 0)
  #define INDENT_SIZE 2u
#endif

#if ((INDENT_SIZE < 2) || (INDENT_SIZE > 8))
  #error "Invalid definition of `INDENT_SIZE`"
#endif

// =============================================================================
// Types
// -----------------------------------------------------------------------------
// Formatter object wrapping the parser
typedef struct json_fmt_s
{
  jsax_t jsnp;
  jsax_serialize_t mini;
  jsax_format_t pretty;
} json_fmt_t;

// -----------------------------------------------------------------------------

#define flush(kind, fn) do\
{                         \
  if (ret != 0)           \
  {                       \
    if ((ret == -1) || (fmt->kind.need == 0)) return false;\
                          \
    fwrite (buf_out, 1u, OUT_BUF_SIZE - fmt->kind.size, stdout);\
    fn (&(fmt->kind), buf_out, numof (buf_out));\
               \
    goto retry;\
  }       \
} while (0)

// -----------------------------------------------------------------------------

static u8 buf_in[IN_BUF_SIZE];
static u8 buf_out[OUT_BUF_SIZE];

// =============================================================================
// Functions
// -----------------------------------------------------------------------------
// Pretty printing callback functions
// -----------------------------------------------------------------------------

static bint pretty_on_start (jsax_t* jsnp, bool obj);
static bint pretty_on_key (jsax_t* jsnp, jsax_key_t key);
static bint pretty_on_str (jsax_t* jsnp, jsax_str_t str);
static bint pretty_on_num (jsax_t* jsnp, jsax_num_t val);
static bint pretty_on_bool (jsax_t* jsnp, bool val);
static bint pretty_on_null (jsax_t* jsnp);
static bint pretty_on_end (jsax_t* jsnp);

// -----------------------------------------------------------------------------

static bint pretty_on_start (jsax_t* jsnp, bool obj)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_pretty_start (&(fmt->pretty), obj);

  flush (pretty, jsax_format_init_buf);

  return true;
}

static bint pretty_on_key (jsax_t* jsnp, jsax_key_t key)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_pretty_key (&(fmt->pretty)
  , json_str_make (key.buf, jsax_key_len (key)));

  flush (pretty, jsax_format_init_buf);

  return true;
}

static bint pretty_on_str (jsax_t* jsnp, jsax_str_t str)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_pretty_str (&(fmt->pretty), str);

  flush (pretty, jsax_format_init_buf);

  return true;
}

static bint pretty_on_num (jsax_t* jsnp, jsax_num_t val)
{
  if (!jsax_is_num_str (val)) return false;

  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_pretty_num (&(fmt->pretty), val);

  flush (pretty, jsax_format_init_buf);

  return true;
}

static bint pretty_on_bool (jsax_t* jsnp, bool val)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_pretty_bool (&(fmt->pretty), val);

  flush (pretty, jsax_format_init_buf);

  return true;
}

static bint pretty_on_null (jsax_t* jsnp)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_pretty_null (&(fmt->pretty));

  flush (pretty, jsax_format_init_buf);

  return true;
}

static bint pretty_on_end (jsax_t* jsnp)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_pretty_end (&(fmt->pretty));

  flush (pretty, jsax_format_init_buf);

  return true;
}

// -----------------------------------------------------------------------------
// Minifying callback functions
// -----------------------------------------------------------------------------

static bint mini_on_start (jsax_t* jsnp, bool obj);
static bint mini_on_key (jsax_t* jsnp, jsax_key_t key);
static bint mini_on_str (jsax_t* jsnp, jsax_str_t str);
static bint mini_on_num (jsax_t* jsnp, jsax_num_t val);
static bint mini_on_bool (jsax_t* jsnp, bool val);
static bint mini_on_null (jsax_t* jsnp);
static bint mini_on_end (jsax_t* jsnp);

// -----------------------------------------------------------------------------

static bint mini_on_start (jsax_t* jsnp, bool obj)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_write_start (&(fmt->mini), obj);

  flush (mini, jsax_serialize_init_buf);

  return true;
}

static bint mini_on_key (jsax_t* jsnp, jsax_key_t key)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_write_key (&(fmt->mini)
  , json_str_make (key.buf, jsax_key_len (key)));

  flush (mini, jsax_serialize_init_buf);

  return true;
}

static bint mini_on_str (jsax_t* jsnp, jsax_str_t str)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_write_str (&(fmt->mini), str);

  flush (mini, jsax_serialize_init_buf);

  return true;
}

static bint mini_on_num (jsax_t* jsnp, jsax_num_t val)
{
  if (!jsax_is_num_str (val)) return false;

  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_write_num (&(fmt->mini), val);

  flush (mini, jsax_serialize_init_buf);

  return true;
}

static bint mini_on_bool (jsax_t* jsnp, bool val)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_write_bool (&(fmt->mini), val);

  flush (mini, jsax_serialize_init_buf);

  return true;
}

static bint mini_on_null (jsax_t* jsnp)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_write_null (&(fmt->mini));

  flush (mini, jsax_serialize_init_buf);

  return true;
}

static bint mini_on_end (jsax_t* jsnp)
{
  json_fmt_t* fmt = (json_fmt_t*)jsnp;

retry:;
  int ret = jsax_write_end (&(fmt->mini));

  flush (mini, jsax_serialize_init_buf);

  return true;
}

// =============================================================================

int main (int argc, char** argv)
{
  // Parse command line arguments
  if (argc > 2)
  {
usage:
    printf ("Usage: %s [-mini] < in.json > out.json\n", argv[0]);
    return EXIT_FAILURE;
  }

  bint mini = false;

  if (argc == 2)
  {
    mini = true;
    if (!stri_iequal (argv[1], "-mini")) goto usage;
  }

#if OS(WIN32)
  // Put both streams into binary mode
  if (fflush (stdin) == EOF) goto error_io;
  if (setmode (STDIN, O_BINARY) == -1) goto error_io;

  if (fflush (stdout) == EOF) goto error_io;
  if (setmode (STDOUT, O_BINARY) == -1) goto error_io;
#endif

  // Initialize the SAX API parser and writers
  u8 buf_val[VAL_BUF_SIZE];
  uint stack[2];

  json_fmt_t fmt;

  jsax_init_stream (&(fmt.jsnp), buf_val, numof (buf_val));

  // Set up the SAX API event handlers
  if (mini)
  {
    fmt.jsnp.on_start = mini_on_start;
    fmt.jsnp.on_key = mini_on_key;
    fmt.jsnp.on_str = mini_on_str;
    fmt.jsnp.on_num = mini_on_num;
    fmt.jsnp.on_bool = mini_on_bool;
    fmt.jsnp.on_null = mini_on_null;
    fmt.jsnp.on_end = mini_on_end;

    jsax_serialize_init (&(fmt.mini), stack, numof (stack) * INT_BIT, buf_out, numof (buf_out));
  }
  else
  {
    fmt.jsnp.on_start = pretty_on_start;
    fmt.jsnp.on_key = pretty_on_key;
    fmt.jsnp.on_str = pretty_on_str;
    fmt.jsnp.on_num = pretty_on_num;
    fmt.jsnp.on_bool = pretty_on_bool;
    fmt.jsnp.on_null = pretty_on_null;
    fmt.jsnp.on_end = pretty_on_end;

    jsax_format_init (&(fmt.pretty), stack, numof (stack) * INT_BIT, buf_out, numof (buf_out));
  }

  // Parse the input JSON stream
  uint cstate = 0;

  size_t size = fread (buf_in, 1u, 3u, stdin);
  size_t off = 0;

  if (size != 3u)
  {
    if (!feof (stdin))
    {
error_io:
      fprintf (stderr, "%s.\n", "I/O error");
      return EXIT_FAILURE;
    }
  }
  else if (buf_equal (buf_in, utf8_bom, 3u)) goto skip_bom;

  json_uncomment_stream (buf_in, size, &cstate);
  if (jsax_parse_stream (&(fmt.jsnp), buf_in, size, feof (stdin))) goto json_done;

  off = size - fmt.jsnp.pos;
  json_str_move_left (buf_in, buf_in + fmt.jsnp.pos, off);

skip_bom:
  while (true)
  {
    size = fread (buf_in + off, 1u, numof (buf_in) - off, stdin);

    // Check for stream end
    if (size == 0)
    {
      if (!feof (stdin)) goto error_io;
      goto error_json;
    }

    // Parse the input chunk removing comments if they are present
    json_uncomment_stream (buf_in + off, size, &cstate);

    if (jsax_parse_stream (&(fmt.jsnp), buf_in, off + size, feof (stdin))) break;

    off = (off + size) - fmt.jsnp.pos;
    json_str_move_left (buf_in, buf_in + fmt.jsnp.pos, off);

    // Stop on fatal error
    if (fmt.jsnp.state == json_state_error) goto error_json;
  }

  // Add the trailing new line if not minifying
json_done:
  if (mini)
  {
    fwrite (buf_out, 1u, OUT_BUF_SIZE - fmt.mini.size, stdout);
  }
  else
  {
    fwrite (buf_out, 1u, OUT_BUF_SIZE - fmt.pretty.size, stdout);
    putchar ('\n');
  }

  return EXIT_SUCCESS;

  // Print the error message
error_json:
  fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[fmt.jsnp.err]
  , fmt.jsnp.line, fmt.jsnp.col, (uint)(fmt.jsnp.pos));

  return EXIT_FAILURE;
}
