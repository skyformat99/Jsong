# Jsong

**Jsong** is an ultra-fast and feature-rich JSON parser, writer,
and processing toolkit written in portable C99 using the
[QUANTUM](https://github.com/garnetius/quantum) library.

Before **Jsong** there wasn’t any decent JSON parser available
for C comparable to [RapidJSON](https://github.com/miloyip/rapidjson)
(from C++ world) in terms of raw speed and features, in my opinion.

Designed from ground up to be resource-efficient and as easy to use
as possible, **Jsong** finally fills the gap between C and JSON
like nothing else before it.

## Features

Highlights of the library are:

  * Full [RFC 7159](https://tools.ietf.org/html/rfc7159) conformance.

  * Comes with both **DOM** & **SAX** APIs and trivial usage.

  * Both APIs support parsing from **implicit** (null-terminated)
    and **explicit** strings.

  * **Streaming**: the parser can be fully restartable, allowing it to handle
    non-contiguous streams of JSON data in chunks.

    Great for network applications where the incoming JSON data can be parsed
    on-the-fly from a small buffer, reading it from a non-blocking socket.

  * Scales from **microcontrollers** to high end **servers**.

  * Constant **stack usage** complexity with no possibility of overflow,
    even with huge documents.

  * **Objects** are represented as doubly-linked lists of key / value pairs.

    **Arrays** are represented as (surprise) arrays of values: just as you
    would expect them to be for fast O(1) access to their values.

  * **SSE2** and **SSE4.2** acceleration for long string
    and whitespace processing.

  * Optional object property key **hashing** for faster lookups.

  * **RFC 6901** conformant [JSON Pointer](https://tools.ietf.org/html/rfc6901)
    implementation for information retrieval.

  * Support for true signed and unsigned **64-bit** integers.

  * Experimental support for **128-bit** integral and floating point types
    provided by recent Clang & GCC for your scientific calculations.
    (If above is just not *quite enough*.)

  * Ability to represent numbers as **number strings** in case if you want
    to parse them yourself utilizing arbitrary-precision arithmetic (for example).

    The key phrase here is *“number strings”*: the underlying data type
    is still a number (distinguished from regular JSON strings) with
    useful metadata attached to it.

  * JSON writers in both DOM & SAX style APIs with **minified**
    or **pretty** (indented and formatted) output.

  * Fully commented and meticulously organized code base with minimal
    dependency on memory allocator. The SAX version of parser
    has *no dependencies at all*.

    See [json.h](https://github.com/garnetius/jsong/blob/master/json/json.h)
    for the rest of minor tweaks you can play with.

## Limitations

No support for UTF encodings other than **UTF-8**.

Seriously, UTF-16 and UTF-32 are so common compared to UTF-8
that it’s just embarrassing, right?

Use [Ultraviolet](https://github.com/garnetius/ultraviolet)
to transcode your inputs to UTF-8 if you need that.

You can also use **Ultraviolet** for UTF-8 validation.

The streaming mode of **Jsong** allows to transcode (or validate)
and parse large documents in small parts.

## Performance

**Jsong** was benchmarked against **RapidJSON** – a well-known
and very fast JSON parser written in C++ where it consistently
outperformed the latter in each test.

Interested in raw numbers? Of course you are: [here](https://github.com/garnetius/jsong/blob/master/apps/README.md).

## Notes on DOM API

**Jsong** employs a couple of tricks to optimize its DOM memory usage.

  * Tagged pointers are used to store additional metadata, such as value type
    or node kind (whether a node belongs to an object or to an array)
    encoded in three lower bits. This approach works and is fully portable
    because the necessary pointer alignment is enforced with the help
    of pool-based memory allocations.

  * The first property of object stores the link to its object instead
    of previous k/v pair (since there’s no previous k/v pair anyway).
    This allows to get to the upper object starting from any k/v pair
    while preserving the doubly-linked list object structure at no
    additional cost. In case of arrays each array element simply stores
    the link to its array.

The design decision to keep previous and upper links in DOM leads to
higher memory usage but I am nevertheless very comfortable with
the chosen model.

Use SAX API parser and your own data structures if extreme
memory savings are your priority.

## Utilities

**Jsong** comes with several programs written to test it,
benchmark its speed and to demonstrate how to use use its APIs.

###### Benchmark

Self-explanatory: used to measure the performance of all parser variants.

###### JSON Console

JSON console program that parses an input document into DOM
and allows to query its contents using JSON Pointer syntax.

###### JSAX Filter

A program written to demonstrate the usage of **Jsong** SAX API.

Reads the JSON data from the standard input and outputs it
to the standard output minified or prettified.

###### Escaper

Reads the data from standard input and encodes it as a valid JSON string.

###### Unescaper

Does the opposite of the above.

## Usage Example for DOM API

Parse the JSON document, get the root node, and print what kind
of element it is and its value.

```c
#include <quantum/build.h>
#include <quantum/core.h>
#include <quantum/io.h>

#include <json/dom.h>
#include <json/utils/consts.h>

int main (int argc, char** argv)
{
  // Parse command line arguments
  if (argc != 2)
  {
    printf ("Usage: %s document.json\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Read the JSON document into the implicit string
  u8* json;
  size_t size;

  io_file_t* f = fopen (argv[1], "rb");

  if (f == null)
  {
    perror ("fopen");
    return EXIT_FAILURE;
  }

  if (!io_file_consume (f, &json, &size, io_consume_text | io_consume_utf8))
  {
    fclose (f);
    return EXIT_FAILURE;
  }

  fclose (f);

  // Create the memory pool
  mem_pool_t* pool = json_pool_create();

  if (pool == null)
  {
    free (json);
    return EXIT_FAILURE;
  }

  // Initialize the DOM API JSON parser for implicit string
  json_t jsnp;
  json_initi (&jsnp, pool);

  // Parse the JSON document from the implicit string
  json_node_t root = json_parsei (&jsnp, json);

  if (!json_is_elmnt (root))
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));

    json_pool_destroy (pool);
    free (json);

    return EXIT_FAILURE;
  }

  // Print the JSON value
  json_elmnt_t* elmnt = json_get_elmnt (root);
  json_type_t type = json_get_type (elmnt);

  switch (type)
  {
    case json_type_obj:
    {
      puts ("Object");
      break;
    }
    case json_type_arr:
    {
      puts ("Array");
      break;
    }
    case json_type_str:
    {
      json_str_t str = json_get_str (elmnt);
      printf ("String: %.*s\n", str.len, str.buf);

      break;
    }
    case json_type_int:
    {
      printf ("Number: %" PRIiMAX "\n", json_get_int (elmnt));
      break;
    }
    case json_type_uint:
    {
      printf ("Number: %" PRIuMAX "\n", json_get_uint (elmnt));
      break;
    }
    case json_type_flt:
    {
      printf ("Number: %.16g\n", json_get_flt (elmnt));
      break;
    }
    case json_type_num:
    {
      puts ("Number");
      break;
    }
    case json_type_primitive:
    {
      if (json_is_bool (elmnt))
      {
        printf ("Boolean: %s\n", json_get_bool (elmnt)
        ? json_str_true : json_str_false);
      }
      else puts ("Null");

      break;
    }
    default:
    {
      json_pool_destroy (pool);
      free (json);

      return EXIT_FAILURE;
    }
  }

  json_pool_destroy (pool);
  free (json);

  return EXIT_SUCCESS;
}
```

## Building

[QUANTUM](https://github.com/garnetius/quantum) and
[Ultraviolet](https://github.com/garnetius/ultraviolet) are required
as build (but not runtime) dependencies.

Building with Microsoft C compiler is not supported and attempting to do so
even with newest Visual Studio will probably leave a smoking hole in the ground.

Use Clang/C2 or [MinGW64](https://mingw-w64.org/) toolchain
(*not* the [old](http://www.mingw.org/) one) to build on Windows.
