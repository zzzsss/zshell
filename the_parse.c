/*
 * parse the read_in
 * common simple pattern: x=... cmd arg1 arg2 >...
 */

#include "the_shell.h"
struct the_p_unit * s_parse_top;

static int now_token=0;

int check_before(int a,int name)
{
	int before = (now_token==0) ? -100 : s_tokens[now_token-1].type;
	if(a==1){
		if(now_token==0||before=='|'||before=='&'||before=='a'||before=='o'||before=='p'||before==';')
			return PARSE_OK;
		else{
			fprintf(stderr,"Syntax error found near %c...\n",name);
			return PARSE_ERROR;
		}
	}
	else if(a==2){
		if(before != 0 && before!='}' && before!=')'){
			fprintf(stderr,"Syntax error found near %c...\n",name);
			return PARSE_ERROR;
		}
		else
			return PARSE_OK;
	}
}
int check_after_for_sepa(int name)
{
	int after = (now_token == (s_token_num-1)) ? -100 : s_tokens[now_token+1].type;
	if((name=='&'||name==';') && after==-100)
		return PARSE_OK;
	if(after!=0 && after!='{' && after!='('){
		fprintf(stderr,"Syntax error found near %c...\n",name);
		return PARSE_ERROR;
	}
	else
		return PARSE_OK;
}

int parse_start(struct the_p_unit* top)
{
	/* 1.1since {} () has been matched when reading so do not need to check.
	 * 1.2since && || | |& also has check appending token...
	 * 2.some syntax problem must be checked(; | |& && ||)
	 * 3.also check () {} for before
	 * 4.get redirection and assign information
	 * 5.also expand the shell var
	 */
	int a;
	int maybe_assign=1;	/* at the beginning of a unit*/
	while(now_token < s_token_num){
		a = s_tokens[now_token].type;
		switch(a){
			case '{': case '(':
				maybe_assign = 0;
				/* check before */
				if(check_before(1,a)) /* 1 class for () {} */ 
					return PARSE_ERROR;
				top->cmd = (a=='{') ? '{' : '(';
				now_token++;
				p_unit_init(&top->down);
				top->down->up = top;
				int temp = parse_start(top->down);
				if(temp != 0)
					return temp;
				break;
			case '}': case ')':
				maybe_assign = 0;
				top->link_next_type = 'E';
				return PARSE_OK;
				break;
			case '|': case '&': case 'a': case 'o': case 'p': case ';':
				maybe_assign=1;
				if(check_before(2,a))
					return PARSE_ERROR;
				if(check_after_for_sepa(a))
					return PARSE_ERROR;
				top->link_next_type = a;
				p_unit_init(&top->next);
				top = top->next;	/* transfer to next */
				break;
			default:
				if(shell_expand(now_token))
					return PARSE_ERROR;
				if(s_tokens[now_token].text[0]=='<'){
					top->redirec_index[top->redirec_num] = now_token;
					top->redirec_type[top->redirec_num++] = 0;
				}
				else if(s_tokens[now_token].text[0]=='>'){
					top->redirec_index[top->redirec_num] = now_token;
					top->redirec_type[top->redirec_num++] = 1;
				}
				else if(top->cmd ==0){
					top->cmd = 0;	/* no level down */
					if(maybe_assign && is_assign(s_tokens[now_token].text))
						top->assign_index[top->assign_num++] = now_token;
					else{
						maybe_assign = 0;
						top->text_index[top->text_num++] = now_token;
					}
				}
				else{
					fprintf(stderr,"Syntax error near %s...",s_tokens[now_token].text);
					return PARSE_ERROR;
				}
				break;
		}
		now_token++;
	}
	top->link_next_type = 'E'; /* the end of all tokens */
	return 0;
}

void combine_pipe(struct the_p_unit* top)
{
	struct the_p_unit *temp;
	p_unit_init(&temp);
	temp->cmd = 'p';
	if((top)->down != NULL)
		combine_pipe(((top)->down));
	int k=(top)->link_next_type;
	if(k=='|' || k=='p'){ /* find the first one of pipe */
		if((top)->left != NULL){
			(top)->left->next = temp;
			temp->left = (top)->left;
			(top)->left = NULL;
		}
		if((top)->up != NULL){
			(top)->up->down = temp;
			temp->up = (top)->up;
			(top)->up = temp;
			temp->down = top;
		}
		while(1){
			top = ((top)->next);
			if(((top)->down) != NULL)
				combine_pipe((top)->down);
			k=(top)->link_next_type;
			if(k=='|' || k=='p')
				continue;
			break;
		}
		temp->next = (top)->next;
		temp->link_next_type = (top)->link_next_type;
		if(temp->next != NULL)
			temp->next->left = temp;
		(top)->next = NULL;
		(top)->link_next_type = 'E';

		if(temp->next != NULL)
			combine_pipe((temp->next));
	}
	else{
		p_unit_free_all(&temp); /* no related nodes */
		if((top)->next != NULL)
			combine_pipe(((top)->next));
	}
}

int shell_parse()
{
	if(s_token_num == 0)
		return PARSE_NOCMD;

	/* inital global parse_unit */
	p_unit_init(&s_parse_top);
	s_parse_top->cmd = 'A'; /* the header */
	p_unit_init(&(s_parse_top->down));
	s_parse_top->down->up = s_parse_top;
	now_token = 0;

	/* start the parse */
	int t=parse_start(s_parse_top->down);
	if(t!=0) /* parse error */
		return t;

	combine_pipe(s_parse_top->down);
	return 0;
}

