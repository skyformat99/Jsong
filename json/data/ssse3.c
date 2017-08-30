// =============================================================================
// <data/ssse3.c>
//
// Process USON encoded data faster using SSSE3.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

                                       // '-' '0-9' 'A-O' 'P-Z' 'a-o' 'p-z'                        '_'
const xi128 lutl = _mm_setr_epi8 (1, 1, 0x2D, 0x30, 0x41, 0x50, 0x61, 0x70, 1, 1, 1, 1, 1, 1, 1, 0x5F);
const xi128 lutu = _mm_setr_epi8 (0, 0, 0x2D, 0x39, 0x4F, 0x5A, 0x6F, 0x7A, 0, 0, 0, 0, 0, 0, 0, 0x5F);
const xi128 lutv = _mm_setr_epi8 (0, 0, 0x11, 0x04, 0xBF, 0xBF, 0xB9, 0xB9, 0, 0, 0, 0, 0, 0, 0, 0xE0);

const xi128 xshf = _mm_setr_epi8 (
   2,  1,  0,
   6,  5,  4,
  10,  9,  8,
  14, 13, 12,
  0xFF, 0xFF,
  0xFF, 0xFF
);

const xi128 f0 = _mm_set1_epi8 (0xF0);

// Decode 16 bytes at a time
while ((size_t)(e - j) >= 16u)
{
  // Get the input chunk
  const xi128 dj = _mm_loadu_si128 ((const xi128*)j);

  // Extract the high nibbles of all bytes for lookup procedure
  xi128 dh = _mm_and_si128 (dj, f0);

  // We have to handle the underscore '_' character case separately
  // because it shares its high nibble with the 'P-Z' character range
  // in the lookup tables above
  xi128 du = _mm_and_si128 (_mm_cmpeq_epi8 (dj, _mm_set1_epi8 (0x5F)), f0);

  // We take advantage of the fact that the `PSHUFB` instruction sets
  // the output vector byte to zero if the corresponding mask byte has
  // its highest bit `0x80` set.
  //
  // First, we merge the underscore character test with the extracted
  // high nibbles and shift the result to construct the shuffle mask.
  // It will now have all its high nibble bits set to zeros and
  // the underscore will be represented as `0x0F` in it.
  //
  // Then, we merge this mask with the original unshifted high nibbles.
  // Now for all character bytes outside the ASCII range their highest bit
  // will be set in the mask and they will be shuffled to zeros, which will
  // allow us to repurpose the high 8 bytes of lookup tables for something
  // else. And since the underscore character will point to the 15th byte
  // in the tables, we can conveniently store its information there.
  xi128 dl = _mm_or_si128 (_mm_srli_epi32 (_mm_or_si128 (dh, du), 4), dh);

  // Construct the range lookup vectors
  // and the result addition vector
  xi128 ll = _mm_shuffle_epi8 (lutl, dl);
  xi128 lu = _mm_shuffle_epi8 (lutu, dl);
  xi128 lv = _mm_shuffle_epi8 (lutv, dl);

  // Validate the input vector against the lookup ranges
  ll = _mm_cmplt_epi8 (dj, ll);
  lu = _mm_cmpgt_epi8 (dj, lu);

  xi128 dm = _mm_or_si128 (ll, lu);

  register uint m = _mm_movemask_epi8 (dm);

  // Sum the input vector with the addition vector
  // to get the 16 intermediate result bytes
  xi128 dr = _mm_add_epi8 (dj, lv);

  // Reshuffle them into the final 12 8-bit bytes
  dr = _mm_maddubs_epi16 (dr, _mm_set1_epi32 (0x01400140));
  dr = _mm_madd_epi16 (dr, _mm_set1_epi32 (0x00011000));

  dr = _mm_shuffle_epi8 (dr, xshf);

  // Store the result vector
  #include "sse_outro.c"
}
