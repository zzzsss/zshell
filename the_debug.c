#include "the_shell.h"

void shell_debug_read_print()
{
	int i=0;
	for(i=0;i<s_token_num;i++)
	{
		if(s_tokens[i].text==NULL)
			fprintf(stderr,"%d:%c\n",i,s_tokens[i].type);
		else
			fprintf(stderr,"%d:%s\n",i,s_tokens[i].text);
	}
}

void the_p_print(struct the_p_unit* a)
{
	int i;
	fprintf(stderr,"%c---",a->cmd);
	fprintf(stderr,"Assign:");
	for(i=0;i<a->assign_num;i++)
		fprintf(stderr,"%d ",a->assign_index[i]);
	fprintf(stderr,"Redirec:");
	for(i=0;i<a->redirec_num;i++)
		fprintf(stderr,"%d ",a->redirec_index[i]);
	fprintf(stderr,"Command:");
	for(i=0;i<a->text_num;i++)
		fprintf(stderr,"%d ",a->text_index[i]);
	fprintf(stderr,"Link--%c...\n",a->link_next_type);
}
void shell_debug_parse_print()
{
	p_unit_map_all(s_parse_top,the_p_print);
}

