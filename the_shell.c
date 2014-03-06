#include "the_shell.h"

struct sh_token s_tokens_all[TOKEN_MAX_NUM];
struct sh_token *s_tokens;
int s_token_num=0;
int s_argc;
char **s_argv;
int s_return_value=0;

void shell_token_clear();

void deal_chld(int a)
{
	int answer;
	int pid=waitpid(-1,&answer,0);
	if(pid!=-1)
		fprintf(stderr,"DONE: the job of pid %d...\n",pid);
	return;
}
void deal_int(int a)
{
	printf("hello\n");
	fflush(0);
	return;
}

int main(int argc,char *argv[])
{
	/* initiial */
	init_var_binding();
	s_argv = argv;
	s_argc = argc;
	/* do not deal with SIGCHLD, may cause zombies with & */
	/*signal(SIGCHLD,deal_chld);*/
	/*signal(SIGINT,deal_int);*/

	s_tokens = s_tokens_all;
	int i=0;
	for(i=0;i<TOKEN_MAX_NUM;i++)
		s_tokens[i].text = NULL;

	/* currently no options supporting */
	while(1){
		/* print prompt */
		shell_prompt(1);

		/* shell_read */
		int it;
		if((it = shell_read())==1){ /*EOF*/
			fprintf(stderr,"Ok, quit the shell\n");
			break;
		}
		else if(it == 2){ /*reading error*/
			fprintf(stderr,"Oops, reading error...\n");
		}
		else{
#ifdef SHELL_DEBUG
			shell_debug_read_print();
#endif
		
			it=shell_parse();
			if(it == PARSE_NOCMD);
			else if(it == PARSE_ERROR){
				fprintf(stderr,"Oops, parsing error...\n");
			}
			else{
#ifdef SHELL_DEBUG
				shell_debug_parse_print();
				shell_debug_read_print();
#endif
			
				shell_exec();
			}
			
		}
		shell_token_clear();
		if(s_parse_top)
			p_unit_free_all(&s_parse_top);
	}

	/* out here */
	clear_var_binding();
	return 0;
}
void shell_token_clear()
{
	int i;
	for(i=0;i<s_token_num;i++)
	{
		free(s_tokens[i].text);
		s_tokens[i].text=NULL;
	}
	s_token_num=0;
}
