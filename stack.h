

#ifndef UNTITLED1_STACK_H
#define UNTITLED1_STACK_H


typedef struct s_stack
{
    int *values;
    int size;
    int nbElts;
} t_stack;


t_stack createStack(int);


void push(t_stack *, int);


int pop(t_stack *);


int top(t_stack);


#endif 