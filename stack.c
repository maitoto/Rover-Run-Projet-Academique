#include <stdlib.h>
#include <assert.h>
#include "stack.h"

t_stack createStack(int size)
{
    assert(size > 0);
    t_stack stack;
    stack.size = size;
    stack.nbElts = 0;
    stack.values = (int *)malloc(size * sizeof(int));
    return stack;
}

void push(t_stack *p_stack, int value)
{
    assert(p_stack->nbElts < p_stack->size);
    p_stack->values[p_stack->nbElts] = value;
    p_stack->nbElts++;
    return;
}

int pop(t_stack *p_stack)
{
    assert(p_stack->nbElts > 0);
    p_stack->nbElts--;
    return p_stack->values[p_stack->nbElts];
}

int top(t_stack stack)
{

    assert(stack.nbElts > 0);
    return stack.values[stack.nbElts - 1];
}
