#include "the_shell.h"

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


void p_unit_print_cur(struct the_p_unit *a,int cont,char *dest)
{
	int i;
	char tmp[MAX_LINE_LENGTH];
	if(a==0)
		return;
	if(a->cmd==0)
		for(i=0;i<a->text_num;i++){
			sprintf(tmp,"%s ",s_tokens[a->text_index[i]].text);
			STR_APP(dest,tmp);
		}
	else if(a->cmd == 'p')
		p_unit_print_cur(a->down,1,dest);
	else if(a->cmd == '{'){
		sprintf(tmp,"{ ");
		STR_APP(dest,tmp);
		p_unit_print_cur(a->down,1,dest);
		sprintf(tmp,"{ ");
		STR_APP(dest,tmp);
	}
	else if(a->cmd == '('){
		sprintf(tmp,"( ");
		STR_APP(dest,tmp);
		p_unit_print_cur(a->down,1,dest);
		sprintf(tmp,"( ");
		STR_APP(dest,tmp);
	}

	if(cont!=0){
		sprintf(tmp," %c ",a->link_next_type);
		p_unit_print_cur(a->next,cont,dest);
		STR_APP(dest,tmp);
	}
	return;
}


