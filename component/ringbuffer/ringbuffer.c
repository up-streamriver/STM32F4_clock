#include "ringbuffer.h"

struct ringbuffer
{
    uint32_t tail;
    uint32_t head;
    uint32_t length;
    uint8_t data[];
};

ringbuffer_t rb8_new(uint8_t *data,uint32_t length)
{
    ringbuffer_t rb = (ringbuffer_t)data;
    rb->length = length - sizeof(struct ringbuffer);
    return rb;
}

uint32_t next_tail(ringbuffer_t rb)
{
    return rb->tail + 1 < rb->length ? rb->tail + 1 : 0;
}

uint32_t next_head(ringbuffer_t rb)
{
    return rb->head + 1 < rb->length ? rb->head + 1 : 0;
}

bool rb8_empty(ringbuffer_t rb)
{
    return rb->head == rb->tail;
}

bool rb8_full(ringbuffer_t rb)
{
    return next_head(rb) == rb->tail;
}


bool rb8_put(ringbuffer_t rb,uint8_t data)
{
    if(rb8_full(rb))
    {
        return false;
    }
    else
    {
        rb->data[rb->head] = data;
        rb->head = next_head(rb);
    }
    return true;
}

bool rb8_get(ringbuffer_t rb,uint8_t *data)
{
    if(rb8_empty(rb))
    {
        return false;
    }
    else
    {
        *data = rb->data[rb->tail];
        rb->tail = next_tail(rb);
    }
    return true;
}

bool rb8_puts(ringbuffer_t rb,uint8_t* data,uint32_t length)
{
    bool result = true;
    for(uint32_t i=0;i<length && result;i++)
    {
        if(!rb8_put(rb,data[i]))
        {
            result = false;
        }
    }
    return result;
}

bool rb8_gets(ringbuffer_t rb,uint8_t* data,uint32_t length)
{
    bool result = true;
    for(uint32_t i=0;i<length && result;i++)
    {
        if(!rb8_get(rb,&data[i]))
        {
            result = false;
        }
    }
    return result;
}