#include "the_shell.h"

int exec_common(struct the_p_unit* p,struct sh_redir d)
{
	/* the final unit of execution...
	 * if no cmd,just return 0
	 */
	if(p->text_num==0){
		int i=0;
		for(i=0;i<p->assign_num;i++)
			assign_name(s_tokens[p->assign_index[i]].text,0);
		return 0;
	}
	/* find the cmd build_in or $PATH */
	char *cmd = s_tokens[p->text_index[0]].text;
	int arg_n = p->text_num;
	char **args=NULL;
	args = malloc(sizeof(char*)*(p->text_num+1));
	int i=0;
	for(i=0;i<p->text_num;i++)
		args[i] = s_tokens[p->text_index[i]].text;
	args[i]=NULL;
	int answer=0;
	int find_it=0;

	/* if there is / in the cmd --- no search */
	if(has_slash(cmd)){
		struct stat buf;
		int k=stat(cmd,&buf);
		if(k==-1)
			goto FINAL;
		else{
			int pid_here;
			if((pid_here=fork())==0){
				int i;
				for(i=0;i<3;i++)
					if(d.which[i]>=3){
						close(i);
						dup(d.which[i]);
					}
				for(i=0;i<p->assign_num;i++)
					assign_name(s_tokens[p->assign_index[i]].text,1);
				execv(cmd,args);
				exit(1);/*error here*/
			}
			else{
				waitpid(pid_here,&answer,0);
				find_it=1;
				goto FINAL;
			}
		}
	}

	/* search for builtins(ignore assigns) */
	for(i=0;i<S_BUILTIN_NUM;i++)
	{
		if(strcmp(cmd,s_builtins[i])==0){
			find_it=1;
			answer=(s_builtin_handle[i])(arg_n,args,d);
			goto FINAL;
		}
	}
	/* search PATH */
	char *the_path = get_var("PATH");
	if(the_path != NULL){
		char *path_temp=malloc(PATH_LENGTH_MAX);
		int temp_l=0;
		int i=0;
		while(1){
			if(the_path[i]==':' || the_path[i]=='\0'){
				if(temp_l==0){
					/* current directory */
					path_temp[0]='.';
					path_temp[1]='\0';
				}
				path_temp[temp_l]='\0';
				DIR *the_dir = opendir(path_temp);
				if(the_dir != NULL){
					struct dirent* the_entry;
					while((the_entry=readdir(the_dir))!=NULL)
					{
						if(strcmp(cmd,the_entry->d_name)==0){
							find_it=1;
							if(path_temp[temp_l-1]!='/'){
								path_temp[temp_l]='/';
								path_temp[temp_l+1]='\0';
							}
							strcat(path_temp,cmd);
							break;
						}
					}
				}
				if(the_path[i]=='\0' || find_it==1)
					break;
				i++;
				temp_l=0;
			}
			else{
				path_temp[temp_l++]=the_path[i++];
			}
		}
		if(find_it!=0){
			int pid_here;
			if((pid_here=fork())==0){
				int i;
				for(i=0;i<3;i++)
					if(d.which[i]>=3){
						close(i);
						dup(d.which[i]);
					}
				for(i=0;i<p->assign_num;i++)
					assign_name(s_tokens[p->assign_index[i]].text,1);
				execv(path_temp,args);
				exit(1);/*error here*/
			}
			else{
				waitpid(pid_here,&answer,0);
			}
		}
		free(path_temp);
	}

FINAL:
	free(args);
	if(find_it==0){
		char here_buf[TOKEN_SIZE];
		sprintf(here_buf,"Command can't find: %s...\n",cmd);
		if(d.which[2]!=-1)
			write(d.which[2],here_buf,strlen(here_buf));
		else
			fprintf(stderr,here_buf);
		return 127;
	}
	return answer;
}

int exec_pipe(struct the_p_unit *p,struct sh_redir d)
{
	/* just return last pipe's result */
	int pids[PIPE_PER_MAX];
	int p_num=0;
	int pipe_fd[2]={-1,-1};
	int pipe_before=-1;
	p=p->down;
	while(1){
		int t=p->link_next_type;
		pipe(pipe_fd);
		int the_pid;
		if((the_pid=fork())==0){
			close(pipe_fd[0]);
			if(pipe_before!=-1)
				d.which[0]=pipe_before;
			if(t=='|' || t=='p'){
				d.which[1]=pipe_fd[1];
				if(t=='p')
					d.which[2]=pipe_fd[1];
			}
			else
				close(pipe_fd[1]);
			int answer=exec_one(p,d);
			exit(answer);
		}
		else{
			pids[p_num++]=the_pid;
			close(pipe_fd[1]);
			if(pipe_before!=-1)
				close(pipe_before);
			pipe_before=pipe_fd[0];

			p=p->next;
			if(p==NULL){
				close(pipe_before);
				break;
			}
		}
	}
	/* wait for them */
	int i=0;
	int final_answer;
	for(i=0;i<p_num;i++)
		waitpid(pids[i],&final_answer,0);
	return final_answer;
}
int exec_b1(struct the_p_unit *p,struct sh_redir d)
{
	return exec_top(p->down,d);
}
int exec_b2(struct the_p_unit *p,struct sh_redir d)
{
	int pid;
	if((pid=fork())==0){
		int answer=exec_top(p->down,d);
		exit(answer);
	}
	else{
		int answer=0;
		waitpid(pid,&answer,0);
		return answer;
	}
}
int exec_one(struct the_p_unit* p,struct sh_redir d)
{
	int i=0;
	int fd_open[2] = {-1,-1};
	for(i=0;i<p->redirec_num;i++){
		char *in_t = s_tokens[p->redirec_index[i]].text+1;	
		if(*in_t == '\0')
		{
			fprintf(stderr,"Error with redirection...\n");
			return 1;
		}
		if(p->redirec_type[i]==0){
			int in_fd=open(in_t,O_RDONLY);
			if(in_fd < -1){
				fprintf(stderr,"Error with redir %s...\n",in_t);
				perror("That is");
			}
			if(fd_open[0] != -1)
			 	close(fd_open[0]);
			fd_open[0]=in_fd;
		}
		else if(p->redirec_type[i]==1){
			int out_fd=open(in_t,O_WRONLY|O_CREAT|O_TRUNC,0664);
			if(out_fd < -1){
				fprintf(stderr,"Error with redir %s...\n",in_t);
				perror("That is");
			}
			if(fd_open[1] != -1)
				close(fd_open[1]);
			fd_open[1]=out_fd;
		}
		else{
			fprintf(stderr,"SYS_ERROR when redirection...\n");
			exit(1);
		}
	}
	if(fd_open[0] != -1)
		d.which[0] = fd_open[0];
	if(fd_open[1] != -1)
		d.which[1] = fd_open[1];

	int answer=0;
	if(p->cmd == 0)
		answer=exec_common(p,d);
	else if(p->cmd == 'p')
		answer=exec_pipe(p,d);
	else if(p->cmd == '{')
		answer=exec_b1(p,d);
	else if(p->cmd == '(')
		answer=exec_b2(p,d);

	close(fd_open[0]);
	close(fd_open[1]);
	return answer;
}

int exec_top(struct the_p_unit* p,struct sh_redir d)
{
	/* first exec this one */
	int answer=0;
	if(p->link_next_type==';' || p->link_next_type=='E'){
		answer=exec_one(p,d);
		s_return_value=answer;
		if(p->next != NULL)
			return exec_top(p->next,d);
	}
	else if(p->link_next_type=='&'){
		/* not waiting fot it */
		if(fork()==0){
			answer=exec_one(p,d);
			exit(answer);
		}
		else{
			s_return_value=0;
			if(p->next != NULL)
				return exec_top(p->next,d);
		}
	}
	else if(p->link_next_type=='a'){
		answer=exec_one(p,d);
		s_return_value=answer;
		if(answer!=0)
			/* do not need check,cause must have some when reading*/
			p=p->next;	
		if(p->next != NULL)
			return exec_top(p->next,d);
	}
	else if(p->link_next_type=='o'){
		answer=exec_one(p,d);
		if(answer==0)
			p=p->next;
		if(p->next != NULL)
			return exec_top(p->next,d);
	}
	else{
		/* error */
		fprintf(stderr,"SYS_ERROR when executing...\n");
		exit(1);
	}
	return s_return_value;
}
void shell_remove_quote()
{
	int i;
	for(i=0;i<s_token_num;i++)
		if(s_tokens[i].type==0)
		{
			char *buf=malloc(TOKEN_SIZE);
			int k=0;
			char *a = s_tokens[i].text;
			while(*a != '\0'){
				if(*a=='\\'){
					buf[k++]=*(++a);
					a++;
				}
				else if(*a=='\''){
					a++;
					while(*a!='\''){
						buf[k++]=*a;
						a++;
					}
					a++;
				}
				else if(*a=='\"'){
					a++;
					continue;
				}
				else
					buf[k++]=*a++;
			}
			buf[k]='\0';
			free(s_tokens[i].text);
			s_tokens[i].text=buf;
		}
}

void shell_exec()
{
	/* first remove the quote */
	shell_remove_quote();
#ifdef SHELL_DEBUG
	shell_debug_read_print();
#endif
	struct sh_redir d;
	d.which[0]=-1;
	d.which[1]=-1;
	d.which[2]=-1;
	if(s_parse_top->down != NULL)
		exec_top(s_parse_top->down,d);
}
