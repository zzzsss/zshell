#ifndef _THE_SHELL_H
#define _THE_SHELL_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>


struct sh_token{
	char * text;
	int type; 
};
/* global context */
#define TOKEN_MAX_NUM 100
#define TOKEN_SIZE 200
extern struct sh_token * s_tokens;
extern int s_token_num;
extern char ** s_argv;
extern int s_argc;
extern int s_return_value;

/* debug */
/* #define SHELL_DEBUG */
void shell_debug_read_print();
void shell_debug_parse_print();


/* shell_read */
void shell_prompt(int);
int shell_read();

/* shell_parse */
#include "the_p_unit.h"
#define PARSE_OK	0
#define PARSE_NOCMD 10
#define PARSE_ERROR 11
int shell_parse();
extern struct the_p_unit* s_parse_top;

/* shell expand */
int shell_expand(int);

#define VAR_NAME_MAX 20
#define VAR_VALUE_MAX 50
struct sh_var_bind{
	char *name;
	char *value;
	struct sh_var_bind* next;
	struct sh_var_bind* before;
};
void init_var_binding();
void clear_var_binding();
char * get_var(char *);
void put_var(char *,char *,int);

/* exec */
/* the redirection unit
 * only support 0,1,2...
 * all after redirection...
 */
struct sh_redir{
	int valid;
	int which[3]; /* which token */
};
void shell_exec();

/* build_in */
#define S_BUILTIN_NUM 5
typedef int (*builtin_handle) (int,char**,struct sh_redir);
extern const char *s_builtins[];
extern builtin_handle s_builtin_handle[];
#define PATH_LENGTH_MAX 100

/* for pipe */
#define PIPE_PER_MAX 10 

/* others */
int is_assign(char*);
void assign_name(char *,int);

#endif
