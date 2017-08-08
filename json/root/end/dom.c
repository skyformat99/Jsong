// =============================================================================
// <root/end/dom.c>
//
// JSON DOM API root element end handling.
//
// Copyright Kristian Garnét.
// -----------------------------------------------------------------------------

if (true)
{
  json_node_t node;

root_end:
  node.elmnt = jsnp->elmnt;

  jsnp->state = json_state_error;
  jsnp->pos = (size_t)(j - json);
  jsnp->err = JSON_OK;

  return node;
}
