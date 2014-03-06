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
	DIR *current = opendir(".");
	struct dirent *entry;
	while((entry = readdir(current))!=NULL)
	{
		if(strcmp(a,entry->d_name)==0){
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
	sprintf(buf,"Can't find dir to %s...\n",a);
	if(d.which[2]!=-1)
		write(d.which[2],buf,strlen(buf));
	else
		write(2,buf,strlen(buf));
	return 1;
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

const char *s_builtins[]=
{"cd","export","exit","help","set"};

builtin_handle s_builtin_handle[]=
{do_cd,do_export,do_exit,do_help,do_set};


