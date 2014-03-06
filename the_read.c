#include "the_shell.h"
#include "the_stack.h"

/* the prompt of shell */
void shell_prompt(int n)
{
	char *prompt;
	if(n==1){
		if((prompt=get_var("PS1"))==NULL)
			fprintf(stderr,"$$ ");
		else{
			fprintf(stderr,prompt);
			printf(" ");
		}
	}
	else if(n==2){
		if((prompt=get_var("PS2"))==NULL)
			fprintf(stderr,"> ");
		else{
			fprintf(stderr,prompt);
			printf(" ");
		}
	}
}

/* the read of shell */
/* the char buf */
int buf;
int whether;
int get_one()
{
	if(!whether)
		return getchar();
	else{
		whether=0;
		return buf;
	}
}
void put_one(int c)
{
	buf=c;
	whether=1;
}
/* read stack */
static z_stack * sh_stack;
static z_stack z_the_shell_stack;
int the_pending=0;
/* skip quote */
int skip_quote(char kind,char *buf,int *count)
{
	int c;
	while((c=getchar()) != EOF)
	{
		if(c==kind){
			buf[(*count)++]=c;
			return 0;
		}
		else if(c=='\n'){
			buf[(*count)++]=c;
			shell_prompt(2);
		}
		else if(c=='\\' && kind != '\'')
		{
			buf[(*count)++]=c;
			buf[(*count)++]=get_one();
		}
		else
			buf[(*count)++]=c;
	}
	fprintf(stderr,"Unexpected EOF for string...\n");
	return -1;
}
int get_token()
{

	char * buffer = malloc(TOKEN_SIZE);
	int c;
	int count=0;
	c=get_one();

	while(c==' ' || c=='\t' || c=='\n'&&(!z_stack_empty(sh_stack)||the_pending)){
		if(c=='\n')
			shell_prompt(2);
		c=get_one();
	}

	if(c==EOF){
		if(z_stack_empty(sh_stack) && the_pending==0)
			return 1;
		else{
			fprintf(stderr,"Unxpected EOF with unmatching separator...\n");
			return 2;
		}
	}
	else if(c=='\n')
		return 0;
	else if(c=='{'){
		z_stack_push(sh_stack,c);
		s_tokens[s_token_num].type='{';
	}
	else if(c=='}'){
		if(z_stack_look(sh_stack) != '{')
		{
			fprintf(stderr,"Error near unexpected token %c...\n",c);
			return 2;
		}
		z_stack_pop(sh_stack);
		s_tokens[s_token_num].type='}';
	}
	else if(c=='('){
		z_stack_push(sh_stack,c);
		s_tokens[s_token_num].type='(';
	}
	else if(c==')'){
		if(z_stack_look(sh_stack) != '(')
		{
			fprintf(stderr,"Error near unexpected token %c...\n",c);
			return 2;
		}
		z_stack_pop(sh_stack);
		s_tokens[s_token_num].type=')';
	}
	else if(c==';') /* do not need push... */
		s_tokens[s_token_num].type=';';
	else if(c=='&'){
		int c2=get_one();
		if(c2=='&'){
			the_pending = 1;
			s_tokens[s_token_num].type='a';/*and*/
		}
		else{
			put_one(c2);
			s_tokens[s_token_num].type='&';
		}
	}
	else if(c=='|'){
		int c2=get_one();
		if(c2=='|')
			s_tokens[s_token_num].type='o';/*or*/
		else if(c2=='&')
			s_tokens[s_token_num].type='p';/*another pipe*/
		else{
			put_one(c2);
			s_tokens[s_token_num].type='|';
		}
		the_pending=1;
	}
	else{
		/*ordinary*/
		s_tokens[s_token_num].type=0;
		int pending=0;

		if(c=='<' || c=='>')
		{
			buffer[count++] = c;
			while((c=get_one())==' ' || c=='\t');
			if(c=='\n' || c=='{' || c=='}' || c=='(' || c==')' || c=='&' || c=='|' || c==';'){
				fprintf(stderr,"Error near unexpected token %c...\n",c);
				return 2;
			}
		}

		while(1){
			if(c==' ' || c=='\t' || c=='\n' || c=='{' || c=='}'&&!pending || c=='(' ||c==')'&&!pending || c=='&' || c=='|' || c==EOF || c==';'){
				put_one(c);
				break;
			}
			else if(c=='\\'){	/*escape next char*/
				buffer[count++] = c;
				buffer[count++] = get_one();
			}
			else if(c=='$'){	/* $() ${} */
				buffer[count++] = c;
				int c2=get_one();
				if(c2=='{' || c2=='('){
				   	pending=1;	
					buffer[count++]=c2;
				}
				else
					put_one(c2);
			}
			else if(c==')' || c=='}'){
				buffer[count++] = c;
				pending=0;
			}
			else if(c=='\'' || c=='\"'){
				buffer[count++]=c;
				if(skip_quote(c,buffer,&count)==-1)
					return 2;
			}
			else
				buffer[count++]=c;

			c = get_one();
		}

		buffer[count] = '\0';
		s_tokens[s_token_num].text = malloc(count+1);
		strcpy(s_tokens[s_token_num].text,buffer);
		the_pending=0;
	}
	free(buffer);
	s_token_num++;
	return 0xFF;
}
int shell_read()
{
	sh_stack = &z_the_shell_stack;
	z_stack_init(sh_stack);
	int c;
	while((c = get_token()) == 0xFF);
	return c;
	z_stack_free(sh_stack);
}

