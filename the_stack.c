#include "the_stack.h"

void z_stack_free(z_stack *it)
{
	free(it->data);
}

void z_stack_init(z_stack *it)
{
	it->data = malloc(Z_STACK_L*sizeof(int));
	it->capacity = Z_STACK_L;
	it->num = 0;
}

void z_stack_push(z_stack *it,int a)
{
	if(it->num == it->capacity)
	{
		int *temp = malloc((2*it->capacity+1)*sizeof(int));
		int i;
		for(i=0;i<it->num;i++)
			temp[i] = it->data[i];
		free(it->data);
		it->data=temp;
		it->capacity = 2*it->capacity+1;
	}
	it->data[it->num++] = a;
}

int z_stack_look(z_stack* it)
{
	if(it->num == 0)
		return -1;

	return it->data[it->num-1];
}

int z_stack_pop(z_stack* it)
{
	if(it->num == 0)
		return -1;

	return it->data[--it->num];
}

int z_stack_empty(z_stack* it)
{
	return it->num==0;
}
