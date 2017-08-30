// =============================================================================
// <parse/macros.h>
//
// Auxiliary macros.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Default return value on stream end or error
#if JSON(SAX)
  #define json_return() return false
#else
  #define json_return() return json_node_null
#endif

// -----------------------------------------------------------------------------
// State manipulation
#if JSON(EXPLICIT)
  // The actual state is kept in higher eight bits (to conserve registers)
  #define json_state_get() (state >> 8)
  // The lower eight bits hold the flags
  #define json_state_set(new) state = ((state & 0xFFu) | ((new) << 8))
#endif

// -----------------------------------------------------------------------------
// Line & character number tracking
#if JSON(LINE_COL)
  #define json_track(num) jsnp->col += (size_t)(num)
#else
  #define json_track(num)
#endif

// -----------------------------------------------------------------------------
// Terminate the parsing due to an unrecoverable error
#define json_error(code) do\
{                    \
  jsnp->err = (code);\
  goto error;\
} while (0)

// -----------------------------------------------------------------------------
// Control flow
// -----------------------------------------------------------------------------

#if JSON(EXPLICIT)
  // Check if there's still data available in the input
  #define json_access() likely (j != e)
  // Check if the input has been parsed completely
  #define json_done() likely (j == e)
  // No terminating null character
  #define json_zero() false

  // Check if the pointer would overflow the input
  #define json_over(ptr, num) unlikely (((ptr) + (num)) >= e)
  // Check if the end of the input has been reached
  #define json_end() unlikely (j == e)

  // Check if the specified number of characters is present in the input
  #define json_peek(num) likely ((size_t)(e - j) >= (num))
  // Check if the input lacks the specified number of characters (sequence is broken)
  #define json_broken(num) unlikely ((size_t)(e - j) < (num))

  #if JSON(STREAM)
    // Set the number of characters the current sequence is supposed to be.
    // The actual number of needed characters is adjusted later.
    #define json_seq(num) jsnp->need = (num)
    // Check if this is the last input chunk
    #define json_last() unlikely (json_flags_last (state))
    // Request more JSON data
    #define json_more() goto stream_end
  #else
    // Unused
    #define json_seq(num)
    // Always the last chunk
    #define json_last() true
    // No more data to request
    #define json_more() json_error (JSON_ERROR_EXPECTED_MORE)
  #endif

  // Set the current parser state
  #define json_state(state) json_state_set (state)

  // Advance the specified number of characters and repeat the current state
  #define json_continue(state, num)\
    json_track (num);\
    j += (num);\
    break

  // Advance the specified number of characters and break to a different state
  #define json_break(state, num)\
    json_state (state);\
    json_track (num);\
    j += (num);\
    break

#else // JSON(EXPLICIT) ][

  // The input data is available until the terminating null is reached
  #define json_access() true
  // Check if the terminating null character has been reached
  #define json_done() likely (c == '\0')
  // Check for terminating null character
  #define json_zero() unlikely (c == '\0')

  // Nothing to overflow, except the null character
  #define json_over(ptr, num) false
  // The input end isn't explicitly known
  #define json_end() false

  // Characters are available until the null character
  #define json_peek(num) true
  // The sequence isn't broken, unless it has the null character in it
  #define json_broken(num) false

  // Unused
  #define json_seq(num)
  // Only one chunk to parse
  #define json_last() true
  // Unreachable
  #define json_more()

  // No state to set: jump directly
  #define json_state(state)

  // Read the next character after the specified number of characters
  // and repeat the current state
  #define json_continue(state, num)\
    json_track (num);\
    c = j[num];\
    j += (num);\
    goto case_##state

  // Read the next character after the specified number of characters
  // and advance to the next state
  #define json_break(state, num)\
    json_track (num);\
    c = j[num];\
    j += (num);\
    goto case_##state
#endif // !JSON(EXPLICIT)
