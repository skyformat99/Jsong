// =============================================================================
// <data/sse2.c>
//
// Process USON encoded data faster using SSE2.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

const xi128 cA = _mm_set1_epi8 ('A' - 1);
const xi128 cZ = _mm_set1_epi8 ('Z' + 1);
const xi128 vAZ = _mm_set1_epi8 (-65);

const xi128 ca = _mm_set1_epi8 ('a' - 1);
const xi128 cz = _mm_set1_epi8 ('z' + 1);
const xi128 vaz = _mm_set1_epi8 (-71);

const xi128 c0 = _mm_set1_epi8 ('0' - 1);
const xi128 c9 = _mm_set1_epi8 ('9' + 1);
const xi128 v09 = _mm_set1_epi8 (4);

const xi128 cmi = _mm_set1_epi8 ('-');
const xi128 vmi = _mm_set1_epi8 (17);

const xi128 cun = _mm_set1_epi8 ('_');
const xi128 vun = _mm_set1_epi8 (-32);

// Decode 16 bytes at a time
while ((size_t)(e - j) >= 16u)
{
  xi128 d1, d2, d3, d4;

  // Get the input chunk
  xi128 dj = _mm_loadu_si128 ((const xi128*)j);

  // Decode the [A-Z] range
  xi128 gtA = _mm_cmpgt_epi8 (dj, cA);
  xi128 ltZ = _mm_cmplt_epi8 (dj, cZ);
  xi128 dAZ = _mm_and_si128 (_mm_and_si128 (gtA, ltZ), vAZ);

  // Decode the [a-z] range
  xi128 gta = _mm_cmpgt_epi8 (dj, ca);
  xi128 ltz = _mm_cmplt_epi8 (dj, cz);
  xi128 daz = _mm_and_si128 (_mm_and_si128 (gta, ltz), vaz);

  // Decode the [0-9] range
  xi128 gt0 = _mm_cmpgt_epi8 (dj, c0);
  xi128 lt9 = _mm_cmplt_epi8 (dj, c9);
  xi128 d09 = _mm_and_si128 (_mm_and_si128 (gt0, lt9), v09);

  // Decode the '-' character
  xi128 dmi = _mm_and_si128 (_mm_cmpeq_epi8 (dj, cmi), vmi);

  // Decode the '_' character
  xi128 dun = _mm_and_si128 (_mm_cmpeq_epi8 (dj, cun), vun);

  // Create the result mask
  d1 = _mm_or_si128 (dAZ, daz);
  d2 = _mm_or_si128 (dmi, dun);

  xi128 dm = _mm_or_si128 (_mm_or_si128 (d1, d2), d09);

  // Convert the vector result mask to the integral mask
  // that will be used to check for input errors
  register uint m = _mm_movemask_epi8 (_mm_cmpeq_epi8 (dm, _mm_setzero_si128()));

  // Add the vector result mask on top of the input chunk
  // to get the 16 intermediate 6-bit result bytes
  xi128 dr = _mm_add_epi8 (dj, dm);

  // Shift them to construct the 12 final 8-bit result bytes
  d1 = _mm_and_si128 (dr, _mm_set1_epi32 (0x003F003F));
  d2 = _mm_srli_epi32 (_mm_and_si128 (dr, _mm_set1_epi32 (0x3F003F00)), 2);

#if 1
  dr = _mm_madd_epi16 (_mm_or_si128 (d1, d2), _mm_set1_epi32 (0x00011000));
#else
  d3 = _mm_or_si128 (d1, d2);

  d1 = _mm_and_si128 (d3, _mm_set1_epi32 (0x00000FFF));
  d2 = _mm_srli_epi32 (_mm_and_si128 (d3, _mm_set1_epi32 (0x0FFF0000)), 4);

  dr = _mm_or_si128 (d1, d2);
#endif

  // Finally, reshuffle the result vector for the store operation
  d1 = _mm_srli_si128 (_mm_slli_si128 (dr, 12), 12);
  d2 = _mm_srli_si128 (_mm_and_si128 (dr, _mm_set_epi32 (0, 0, 0x00FFFFFF, 0)), 1);
  d3 = _mm_srli_si128 (_mm_and_si128 (dr, _mm_set_epi32 (0, 0x00FFFFFF, 0, 0)), 2);
  d4 = _mm_slli_si128 (_mm_srli_si128 (dr, 12), 9);

  dr = _mm_or_si128 (_mm_or_si128 (d1, d2), _mm_or_si128 (d3, d4));

  // Store the result vector
  #include "sse_outro.c"
}
