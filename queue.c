

#include <assert.h>
#include <stdlib.h>
#include "queue.h"

t_queue createQueue(int size)
{

    assert(size > 0);
    t_queue queue;
    queue.size = size;
    queue.first = 0;
    queue.last = 0;
    queue.values = (t_position *)malloc(size * sizeof(t_position));
    return queue;
}

void enqueue(t_queue *p_queue, t_position pos)
{
    assert((p_queue->last - p_queue->first) < p_queue->size);
    p_queue->values[(p_queue->last) % p_queue->size] = pos;
    p_queue->last++;
    return;
}

t_position dequeue(t_queue *p_queue)
{
    assert(p_queue->last != p_queue->first);
    p_queue->first++;
    return p_queue->values[(p_queue->first - 1) % p_queue->size];
}