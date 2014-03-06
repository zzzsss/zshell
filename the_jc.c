/* implement the job control 
 * the jc rely on the handler of the signal
 * but not using tcsetpgrp
 */

#include "the_shell.h"

struct job_t shell_jc_list[MAX_JOB];
int job_num=0;
struct job_t current_f_job;

/* 	for builtins 	*/
int jc_jobs()
{
	int i=0;
	struct job_t* tmp = &shell_jc_list[0];
	for(i=0;i<job_num;i++,tmp++){
		if(tmp->r_flag==JC_NONE)
			continue;
		jc_print_job(tmp);
	}
	return 0;
}

int jc_fgbg(int j_num,int flag)
{
	if(j_num>0 && j_num<=job_num){
		struct job_t* tmp = &shell_jc_list[j_num-1];
		if(tmp->r_flag != JC_NONE){
			if(tmp->r_flag==JC_STOP){
				kill(-1*tmp->id,SIGCONT);
			}
			if(flag==JC_FORE_RUN){	//fg
				jc_addjob(tmp->cmd,JC_FORE_RUN,tmp->id,0);
				tmp->r_flag=JC_NONE;

				printf("%s",tmp->cmd);
				printf("\n");
			}
			else{
				tmp->r_flag=flag;
				jc_print_job(tmp);
			}
			int answer=0;
			if(flag==JC_FORE_RUN){	//fg
				while(1){
					int i=waitpid(-1*tmp->id,&answer,WUNTRACED);
					if(i==0 || WIFSTOPPED(answer))
						break;
				}
			}
			return answer;
		}
	}
	fprintf(stderr,"JOB_CONTROL: fg_bg: No such job");
	return 1;
}

int jc_kill(int j_num,int sig)
{
	if(j_num>0 && j_num<=job_num){
		struct job_t* tmp = &shell_jc_list[j_num-1];
		if(tmp->r_flag != JC_NONE){
			kill(-1*tmp->id,sig);
		}
		return 0;
	}
	fprintf(stderr,"JOB_CONTROL: kill : No such job");
	return 1;
}


/* for signals */
void jc_stop_cur() //SIGTSTP
{
	if(current_f_job.r_flag!=JC_NONE){
		fprintf(stderr,"STOPPED: ");
		jc_addjob(current_f_job.cmd,JC_STOP,current_f_job.id,1);
		kill(-1*current_f_job.id,SIGTSTP);
		//jc_clear_fore_job(0);
	}
}

void jc_kill_cur(int sig) //SIGINT;SIGQUIT
{
	if(current_f_job.r_flag!=JC_NONE){
		kill(-1*current_f_job.id,sig);
	}
}

/* others */
void jc_addjob(char *cmd,int flag,int id,int try_find)
{
	if(flag==JC_FORE_RUN){
		strncpy(current_f_job.cmd,cmd,MAX_LINE_LENGTH);
		current_f_job.r_flag=JC_FORE_RUN;
		current_f_job.id=id;
	}
	else if(job_num>=MAX_JOB){
		fprintf(stderr,"JOB_CONTROL: Job list is full...\n");
	}
	else{
		if(try_find){
			int i=0;
			struct job_t* tm = &shell_jc_list[0];
			for(i=0;i<job_num;i++,tm++){
				if(tm->id==id){
					tm->r_flag=flag;
					fprintf(stderr,"[%d]: %d\n",i+1,id);
					return;
				}
			}
		}
		fprintf(stderr,"[%d]: %d\n",job_num+1,id);
		struct job_t* tmp = &shell_jc_list[job_num++];
		tmp->r_flag = flag;
		tmp->id = id;
		tmp->cmd=malloc(MAX_LINE_LENGTH);
		tmp->cmd[0]='\0';
		strncpy(tmp->cmd,cmd,MAX_LINE_LENGTH);
	}
}

void jc_clear_fore_job()
{
	current_f_job.r_flag = JC_NONE;
	current_f_job.cmd[0]='\0';
}

void jc_print_job(struct job_t *j)
{
	printf("[%d] %s\t\t",(int)((j-shell_jc_list)+1),
			((j->r_flag==JC_STOP)?"STOPPED":"RUNNING"));
	printf("%s",j->cmd);
	printf("\n");
}

void jc_chld()
{
	int answer=0;
	int i=0;
	struct job_t* tmp = &shell_jc_list[0];
	for(i=0;i<job_num;i++,tmp++){
		if(tmp->r_flag==JC_NONE)
			continue;
		if(waitpid(-1*tmp->id,&answer,WCONTINUED|WNOHANG|WUNTRACED)!=0){
			if(WIFEXITED(answer) | WIFSIGNALED(answer))
				tmp->r_flag=JC_DONE;
		}
	}
}

void jc_clear_done()
{
	int i=0;
	struct job_t* tmp = &shell_jc_list[0];
	for(i=0;i<job_num;i++,tmp++){
		if(tmp->r_flag==JC_DONE){
			printf("[%d] %s\t\t",(int)((tmp-shell_jc_list)+1),"DONE");
			printf("%s",tmp->cmd);
			tmp->r_flag=JC_NONE;
		}
	}
}




