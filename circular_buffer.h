typedef struct cbuf_t cbuf_t;

cbuf_t*  cbuf_init(size_t count);
void     cbuf_free(cbuf_t* cbuf);

void     cbuf_push(cbuf_t* cbuf, uint16_t item);
uint16_t cbuf_pop_tail(cbuf_t* cbuf);

uint16_t cbuf_head(cbuf_t* cbuf);
uint16_t cbuf_tail(cbuf_t* cbuf);
size_t   cbuf_count(cbuf_t* cbuf);
