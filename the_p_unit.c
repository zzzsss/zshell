#include "the_p_unit.h"

int p_unit_init(struct the_p_unit** a)
{
	*a = malloc(sizeof(struct the_p_unit));
	struct the_p_unit *temp = *a;
	temp->next = 0;
	temp->down = 0;
	temp->up = 0;
	temp->left = 0;
	temp->assign_num=0;
	temp->assign_index = malloc(sizeof(int) * MAX_INDEX);
	temp->redirec_num=0;
	temp->redirec_type = malloc(sizeof(int) * MAX_INDEX);
	temp->redirec_index = malloc(sizeof(int) * MAX_INDEX);
	temp->link_next_type = 0;
	temp->cmd = 0;
	temp->text_num = 0;
	temp->text_index = malloc(sizeof(int) * MAX_ARGU);
	return 0;
}

void p_unit_free_all(struct the_p_unit **a)
{
	struct the_p_unit *t = *a;
	if(t != 0){
		if(t->next)
			p_unit_free_all(&t->next);
		if(t->down)
			p_unit_free_all(&t->down);
		free(t);
		*a = 0; 
	}
}

void p_unit_map_all(struct the_p_unit *a,map_f f)
{
	if(a){
	if(a->down)
		p_unit_map_all(a->down,f);
	f(a);
	if(a->next)
		p_unit_map_all(a->next,f);
	}
}
