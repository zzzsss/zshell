#include "the_shell.h"

int is_assign(char * a)
{
	/* check when parsing:
	 * whether the token is assign
	 */
	if(!isalpha(*a))
		return 0;
	a++;
	while(isalnum(*a)||(*a)=='_')
		a++;
	if((*a)!='=')
		return 0;
	else
		return 1;
}

void assign_name(char *a,int export)
{
	char name[VAR_NAME_MAX];
	char value[VAR_VALUE_MAX];
	int i;
	for(i=0;a[i]!='='&&a[i]!='\0';i++)
		name[i]=a[i];
	name[i]='\0';
	if(a[i]=='=')
		strncpy(value,&a[i+1],VAR_NAME_MAX);
	else{
		/* error */
		fprintf(stderr,"SYS_ERROR: not assign token --- %s...\n",a);
		return;
	}
	put_var(name,value,export);
}

int has_slash(char *t)
{
	int l=strlen(t);
	int i;
	for(i=0;i<l;i++)
		if(t[i]=='/')
			return 1;
	return 0;
}
