// =============================================================================
// JSON Console
//
// Interactive JSON console.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

#include <quantum/build.h>
#include <quantum/core.h>

#include <quantum/integer.h>
#include <quantum/string/implicit.h>
#include <quantum/io.h>

// -----------------------------------------------------------------------------

#include <json/dom.h>
#include <json/utils.h>
#include <json/utils/consts.h>
#include <json/serialize.h>

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
  if (argc != 2)
  {
    printf ("Usage: %s document.json\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Read the JSON document
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
    fprintf (stderr, "%s.\n", "I/O error");

    return EXIT_FAILURE;
  }

  fclose (f);

  // Preprocess the JSON to remove comments
  json_uncommenti (json);

  // Create the memory pool
  bool err = false;

  mem_pool_t* pool = json_pool_create();

  if (pool == null)
  {
    err = true;
    goto done;
  }

  // Initialize the DOM API JSON parser
  json_t jsnp;
  json_initi (&jsnp, pool);

  // Parse the JSON document
  json_node_t root = json_parsei (&jsnp, json);

  if (!json_is_elmnt (root))
  {
    fprintf (stderr, "%s [%u:%u] [%u]\n", json_errors[jsnp.err]
    , jsnp.line, jsnp.col, (uint)(jsnp.pos));

    err = true;

    goto done;
  }

  json_node_t node = root;

  // Start the JSON console
  printf ("JSON Console\n\n", 0);

  while (!feof (stdin))
  {
    size_t len;
    u8 query[1024];
    json_key_t keys[16];

    putchar ('>');
    putchar (' ');

    // Get the query string
    if ((len = io_line_read (query, sizeof (query))) == IO_SIZE_ERROR)
    {
      fprintf (stderr, "%s.\n", "I/O error");
      err = true;

      goto done;
    }

    // Check if shutdown is requested
    if (stri_equal (query, "exit") || stri_equal (query, "quit")) break;

    // Serialize the selected node
    if (stri_equal (query, "serialize") || stri_equal (query, "format"))
    {
      if (!json_is_elmnt (node))
      {
        puts ("Undefined");
        continue;
      }

      json_format_t st;
      u8 buf[BUF_SIZE];

      json_format_init (&st, node, buf, numof (buf), 0);

      while (true)
      {
        bool ret;

        if (stri_equal (query, "format")) ret = json_format (&st);
        else ret = json_serialize ((json_serialize_t*)&st);

        printf ("%.*s", (uint)st.fill, buf);

        if (ret) break;
        assert (st.need <= numof (buf));

        json_format_init_buf (&st, buf, numof (buf), 0);
      }

      putchar ('\n');

      continue;
    }

    // Parse the JSON Pointer
    uint num = json_parse_pointer (query, len, query
    , keys, numof (keys));

    if (num == UINT_MAX)
    {
      puts ("Invalid JSON Pointer syntax");
      continue;
    }

    // Query the DOM with the JSON Pointer
    node = json_pointer (root, keys, num);

    if (!json_is_elmnt (node))
    {
      puts ("Undefined");
      continue;
    }

    // Print the value
    json_elmnt_t* elmnt = json_get_elmnt (node);
    json_type_t type = json_get_type (elmnt);

    switch (type)
    {
      case json_val_obj:
      {
        puts ("Object");
        break;
      }
      case json_val_arr:
      {
        puts ("Array");
        break;
      }
      case json_val_str:
      {
        json_str_t str = json_get_str (elmnt);
        printf ("String: %.*s\n", str.len, str.buf);

        break;
      }
      case json_val_int:
      {
        printf ("Number: %" PRIiMAX "\n", json_get_int (elmnt));
        break;
      }
      case json_val_uint:
      {
        printf ("Number: %" PRIuMAX "\n", json_get_uint (elmnt));
        break;
      }
      case json_val_flt:
      {
        printf ("Number: %.16g\n", json_get_flt (elmnt));
        break;
      }
      case json_val_tok:
      {
        if (json_is_bool (elmnt))
        {
          printf ("Boolean: %s\n", json_get_bool (elmnt)
          ? json_str_true : json_str_false);
        }
        else puts ("Null");

        break;
      }
      case json_val_num:
      {
        if (json_is_num_str (elmnt))
        {
          json_num_str_t num = json_get_num_str (elmnt);
          printf ("Number: %.*s\n", num.meta.len_number, num.buf);
        }
#if JSON(BIG_NUMBERS)
        else if (json_is_num_big_int (elmnt))
        {
          u8 buf[1 + INT128_DIG + 1];

          buf[int_s128_to_str (buf, json_get_num_big_int (elmnt))] = '\0';
          printf ("Number: %s\n", buf);
        }
        else if (json_is_num_big_uint (elmnt))
        {
          u8 buf[UINT128_DIG + 1];

          buf[int_s128_to_str (buf, json_get_num_big_int (elmnt))] = '\0';
          printf ("Number: %s\n", buf);
        }
        else if (json_is_num_big_flt (elmnt))
        {
  #ifdef FLT128_MAX
          u8 buf[JSON_BIG_FLT_BUF_SIZE];

          quadmath_snprintf (buf, numof (buf), "%.35Qg"
          , json_get_num_big_flt (elmnt));

          printf ("Number: %s\n", buf);
  #else
    #if (LDBL_BINARY == 80)
      #if OS(WIN32)
          __mingw_printf ("Number: %.20Lg\n", json_get_num_big_flt (elmnt));
      #else
          printf ("Number: %.20Lg\n", json_get_num_big_flt (elmnt));
      #endif
    #else
          printf ("Number: %.16Lg\n", json_get_num_big_flt (elmnt));
    #endif
  #endif
        }
#endif
        else if (json_is_num_special (elmnt))
        {
          printf ("Number range error [%X]\n", elmnt->val.tag);
        }
        else puts ("Number");

        break;
      }
    }
  }

done:
  if (pool != null) json_pool_destroy (pool);
  if (json != null) free (json);

  return err ? EXIT_FAILURE : EXIT_SUCCESS;
}
