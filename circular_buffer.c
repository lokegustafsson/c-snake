#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint16_t* data;
    size_t    count;
    // Index of last occupied slot, or one less than head if empty
    size_t    head;
    // Index of first occupied slot, or one less than head if empty
    size_t    tail;
} cbuf_t;

cbuf_t* cbuf_init(size_t count) {
    cbuf_t* handle = malloc(sizeof(cbuf_t));
    handle->data = malloc(count * sizeof(uint16_t));
    handle->count = count;
    handle->head = count - 1;
    handle->tail = 0;
    return handle;
}

void cbuf_free(cbuf_t* cbuf) {
    free(cbuf->data);
    free(cbuf);
}

void cbuf_push(cbuf_t* cbuf, uint16_t item) {
    cbuf->head = (cbuf->head + 1) % cbuf->count;
    cbuf->data[cbuf->head] = item;
}

uint16_t cbuf_pop_tail(cbuf_t* cbuf) {
    uint16_t t = cbuf->data[cbuf->tail];
    cbuf->tail = (cbuf->tail + 1) % cbuf->count;
    return t;
}

uint16_t cbuf_head(cbuf_t* cbuf) {
    return cbuf->data[cbuf->head];
}

uint16_t cbuf_tail(cbuf_t* cbuf) {
    return cbuf->data[cbuf->tail];
}

size_t cbuf_count(cbuf_t* cbuf) {
    return (cbuf->count + cbuf->head - cbuf->tail + 1) % cbuf->count;
}
