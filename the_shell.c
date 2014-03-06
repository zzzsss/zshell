#include "the_shell.h"

struct sh_token s_tokens_all[TOKEN_MAX_NUM];
struct sh_token *s_tokens;
int s_token_num=0;
int s_argc;
char **s_argv;
int s_return_value=0;
sigjmp_buf the_start_point;

void shell_token_clear();
extern struct job_t current_f_job;

void deal_chld(int a)
{
	int answer;
	jc_chld();
	return;
}
void deal_int(int a)
{
	jc_kill_cur(SIGINT);
	fprintf(stderr,"\n");
	if(current_f_job.r_flag==JC_NONE)
		siglongjmp(the_start_point,1);
}
void deal_stop(int a)
{
	jc_stop_cur();
	fprintf(stderr,"\n");
	if(current_f_job.r_flag==JC_NONE)
		siglongjmp(the_start_point,1);
}

int main(int argc,char *argv[])
{
	/* initiial */
	init_var_binding();
	s_argv = argv;
	s_argc = argc;
	/* deal with signal */
	struct sigaction sa1,sa2;
	sa1.sa_flags=0;
	sa1.sa_flags |= SA_RESTART;
	sigemptyset(&sa1.sa_mask);

	sa1.sa_handler=deal_int;
	sigaction(SIGINT,&sa1,&sa2);
	sa1.sa_handler=deal_stop;
	sigaction(SIGTSTP,&sa1,&sa2);
	sa1.sa_handler=deal_chld;
	sigaction(SIGCHLD,&sa1,&sa2);

	/*tokens initial*/
	s_tokens = s_tokens_all;
	int i=0;
	for(i=0;i<TOKEN_MAX_NUM;i++)
		s_tokens[i].text = NULL;

	current_f_job.cmd=malloc(MAX_LINE_LENGTH);
	current_f_job.cmd[0]='\0';

	/* currently no options supporting */
	while(1){
		/* the jmp_buf */
		sigsetjmp(the_start_point,1);
		
		/* clear */
		shell_token_clear();
		if(s_parse_top)
			p_unit_free_all(&s_parse_top);
	
		jc_clear_done();
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
