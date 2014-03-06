/* expand the word ($...)
 * currently only support ${} $...
 * the codes also maintain the shell environment
 */
#include "the_shell.h"

/* the bindings */
static struct sh_var_bind s_bindings[27];
void init_var_binding()
{
	int i;
	for(i=0;i<27;i++){
		s_bindings[i].name=NULL;
		s_bindings[i].next=NULL;
		s_bindings[i].before=NULL;
	}
}
void clear_var_binding()
{
	int i;
	struct sh_var_bind* p;
	for(i=0;i<27;i++){
		p=s_bindings[i].next;
		while(p!=NULL){
			free(p->name);
			free(p->value);
			p=p->next;
			if(p != NULL)
				free(p->before);
		}
	}
}
int which_group(char a)
{
	if(a<='z' && a>='a')
		return a-'a';
	else if(a<='Z' && a>='A')
		return a-'A';
	else
		return 26;
}
char *get_var(char *name)
{
	char *a;
	/* oops,may be memory leak,but just ignored here*/
	char *the_temp=malloc(10);
	if(strcmp(name,"#")==0){
		sprintf(the_temp,"%d",s_argc);
		return(the_temp);
	}
	else if(strcmp(name,"?")==0){
		sprintf(the_temp,"%d",s_return_value);
		return(the_temp);
	}
	/* here is some problem,but we assume
	 * no strange string from user*/
	else if(isdigit(name[0]))
	{
		int what=atoi(name);
		if(what >= s_argc)
			return NULL;
		else
			return s_argv[what];
	}
	else if((a=getenv(name))==NULL)
	{
		int i = which_group(name[0]);
		struct sh_var_bind* temp=s_bindings[i].next;
		while(temp!=NULL){
			if(strcmp(temp->name,name)==0)
				return temp->value;
			temp=temp->next;
		}
		return NULL;
	}
	return a;
}
/* put a var(must alloc new mem) */
void put_var(char *name,char *value,int export)
{
	if(getenv(name) != 0)
	{
		setenv(name,value,1);
		return;
	}

	int i=which_group(name[0]);
	struct sh_var_bind* temp=s_bindings[i].next;
	while(temp!=NULL){
		if(strcmp(temp->name,name)==0)
			break;
		temp=temp->next;
	}
	if(export==1){
		setenv(name,value,1);
		/* delete it in the local table if found*/
		if(temp!=NULL){
			temp->before->next = temp->next;
			if(temp->next != NULL)
				temp->next->before = temp->before;
			free(temp->name);
			free(temp->value);
			free(temp);
		}
	}
	else{
		if(temp!=NULL)
			strcpy(temp->value,value);
		else{
			struct sh_var_bind *new_one = malloc(sizeof(struct sh_var_bind));
			new_one->name = malloc(VAR_NAME_MAX);
			strncpy(new_one->name,name,VAR_NAME_MAX);
			new_one->value = malloc(VAR_VALUE_MAX);
			strncpy(new_one->value,value,VAR_VALUE_MAX);
			/* just put it at front */
			temp = &s_bindings[i];
			new_one->next = temp->next;
			temp->next = new_one;
			new_one->before = temp;
		}
	}
}



int shell_expand(int now)
{
	/* must inside the range
	 * also must be type 0(here not checked)
	 */
	if(now<0 || now>=s_token_num)
		return PARSE_ERROR;
	char *a = s_tokens[now].text;
	char *other = malloc(TOKEN_SIZE);

	int i1=0;
	int i2=0;
	while(1){
		if(a[i1]=='\0'){
			other[i2]='\0';
			break;
		}
		if(a[i1]=='\\'){
			other[i2++]=a[i1++];
			other[i2++]=a[i1++];
			continue;
		}
		if(a[i1]=='\''){
			other[i2++] = a[i1++];
			while((other[i2++]=a[i1++]) != '\'');
			other[i2++] = a[i1++];
			continue;
		}
		if(a[i1]=='$'){
			if(a[i1+1]=='\0')	/*end*/
				other[i2++]=a[i1++];
			else if(a[i1+1]=='(')/* command_sub:not support now */
				other[i2++]=a[i1++];
			else if(a[i1+1]=='{'){/* ${ ... } */
				int before=i1;
				i1 += 2;
				char *temp = malloc(TOKEN_SIZE);
				int i_temp=0;
				while(a[i1]!='}'){
					if(a[i1]=='$' || a[i1]=='\0' || a[i1]=='{')
					{
						fprintf(stderr,"Error,bad substitution %s...\n",a);
						return PARSE_ERROR;
					}
					temp[i_temp++] = a[i1++];
				}
				i1++;
				temp[i_temp]='\0';
				/* passed ${} */
				/* get the env string(can't modify*/
				const char *the_env = get_var(temp);
				free(temp);
				/*append the environment*/
				int i;
				if(the_env != NULL)
					for(i=0;the_env[i] != '\0';i++)
						other[i2++] = the_env[i];
			}
			else{
				i1++;
				char *temp = malloc(TOKEN_SIZE);
				int i_temp=0;
				for(;a[i1]!='$'&&a[i1]!='\0';){
					temp[i_temp++] = a[i1];
					i1++;
				}
				temp[i_temp++]='\0';
				const char *the_env = get_var(temp);
				free(temp);
				int i;
				if(the_env != NULL)
					for(i=0;the_env[i]!='\0';i++)
						other[i2++] = the_env[i];
			}
		}
		else
			other[i2++] = a[i1++];
	}
	free(a);
	s_tokens[now].text=other;
	return PARSE_OK;
}

