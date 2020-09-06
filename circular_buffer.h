#include "circular_buffer.c"

cbuf_handle_t cbuf_init(size_t count);
void          cbuf_free(cbuf_handle_t cbuf);

void          cbuf_push(cbuf_handle_t cbuf, uint16_t item);
uint16_t      cbuf_pop_tail(cbuf_handle_t cbuf);

uint16_t      cbuf_head(cbuf_handle_t cbuf);
uint16_t      cbuf_tail(cbuf_handle_t cbuf);
size_t        cbuf_count(cbuf_handle_t cbuf);
