// =============================================================================
// <root/end/sax.c>
//
// JSON SAX API root element end handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
root_end:
  jsnp->state = json_state_error;
  jsnp->pos = (size_t)(j - json);
  jsnp->err = JSON_OK;

  return true;
}
