#include"the_shell.h"
int do_cd(int arg_n,char **args,struct sh_redir d)
{
	char buf[TOKEN_SIZE];
	if(arg_n==1){
		/* change to $HOME */
		char *a = get_var("HOME");
		if(a==NULL)
			return 0;
		else{
			if(chdir(a)<0){
				sprintf(buf,"Can't change dir to %s...\n",a);
				if(d.which[2]!=-1)
					write(d.which[2],buf,strlen(buf));
				else
					write(2,buf,strlen(buf));
				return 1;
			}
			return 0;
		}
	}
	/* change to arg1 */
	char *a = args[1];
	if(chdir(a)<0){
		sprintf(buf,"Can't change dir to %s...\n",a);
		if(d.which[2]!=-1)
			write(d.which[2],buf,strlen(buf));
		else
			write(2,buf,strlen(buf));
		return 1;
	}
	return 0;
}
int do_export(int arg_n,char **args,struct sh_redir d)
{
	int i=1;
	for(i=1;i<arg_n;i++){
		if(is_assign(args[i]))
			assign_name(args[i],1);
		else
			put_var(args[i],"",1);
	}
	return 0;
}
int do_exit(int arg_n,char **args,struct sh_redir d)
{
	write(2,"Ok,exit...\n",11);
	unsigned char k=0;
	if(arg_n>1 && ((k=atoi(args[1]))>0))
		exit(k);
	exit(0);
}
int do_help(int arg_n,char **args,struct sh_redir d)
{
	char *buf = "HELP for the shell: no help...\n";
	if(d.which[2]!=-1)
		write(d.which[2],buf,strlen(buf));
	else
		write(2,buf,strlen(buf));
	return 0;
}
int do_set(int arg_n,char **args,struct sh_redir d)
{
	/* not support yet */
	return 0;
}

/* job control */
int do_jobs(int arg_n,char **args,struct sh_redir d)
{
	int pid;
	if((pid=fork())==0){
		int i=0;
		for(i=0;i<3;i++)
			if(d.which[i]>=3){
				close(i);
				dup(d.which[i]);
			}
		exit(jc_jobs());
	}
	else{
		int answer=0;
		waitpid(pid,&answer,0);
		return answer;
	}
}

/* fg,bg just take the first arg... */
int do_fgbg(int arg_n,char **args,struct sh_redir d)
{
	if(arg_n<=1){
		fprintf(stderr,"BGFG: need argu...\n");
		return 1;
	}
	int i=atoi(&args[1][1]);
	if(i>0){
		if(strcmp(args[0],"fg")==0)
			return jc_fgbg(i,JC_FORE_RUN);
		else
			return jc_fgbg(i,JC_BACK_RUN);
	}
	fprintf(stderr,"BGFG: bad argu %s...\n",args[1]);
	return 1;
}
/* format: kill -signum %[N]/[N] */
int do_kill(int arg_n,char ** args,struct sh_redir d)
{
	int sig,j_num;
	if(arg_n <= 2){ 
		fprintf(stderr,"BGFG: bad argu...\n");
		return 1;
	}
	if(args[1][0]!='-' || (sig=atoi(&args[1][1]))<=0){
		fprintf(stderr,"BGFG: bad signum...\n");
		return 1;
	}
	if(args[2][0]=='%'){
		if((j_num=atoi(&args[2][1]))<=0){
			fprintf(stderr,"BGFG: bad %% format...\n");
			return 1;
		}
		return jc_kill(j_num,sig);
	}
	else{
		if((j_num=atoi(args[2]))<=0){
			fprintf(stderr,"BGFG: argument not pid...\n");
			return 1;
		}
		return kill(j_num,sig);
	}
}

const char *s_builtins[]=
{"cd","export","exit","help","set","jobs","fg","bg","kill"};

builtin_handle s_builtin_handle[]=
{do_cd,do_export,do_exit,do_help,do_set,do_jobs,do_fgbg,do_fgbg,do_kill};


