#ifndef _THE_STACK_H
#define _THE_STACK_H
#include "stdlib.h"

#define Z_STACK_L 10
typedef
struct z_stack{
	int *data;
	int num;
	int capacity;
}
z_stack;

void z_stack_push(z_stack*,int);
int z_stack_pop(z_stack*);
int z_stack_look(z_stack*);
void z_stack_init(z_stack*);
void z_stack_free(z_stack*);
int z_stack_empty(z_stack*);

#endif

