#ifndef _THE_P_UNIT
#define _THE_P_UNIT

#define MAX_INDEX 10
#define MAX_ARGU 20

/* text flags */
#define T_ASSIGN 0
#define T_RED_ALONE 1
#define T_RED_WITH 2
#define T_COMMON 3

/* use the index of the sh_tokens... */
struct the_p_unit{
	struct the_p_unit* next; /* next unit */
	struct the_p_unit* left;
	struct the_p_unit* down; /* next level unit */
	struct the_p_unit* up;
	int assign_num;
	int *assign_index;
	int redirec_num;
	int *redirec_type; /* 0 < ; 1 > */
	int *redirec_index;
	int link_next_type; /* | |&(p) &&(a) ||(o) & ; end(E)*/
	int cmd;	/* 0 { ( p */
	int text_num;
	int *text_index; /* pids of each one when not 0((,{,p) */
};

typedef void (*map_f) (struct the_p_unit*);
int p_unit_init(struct the_p_unit**);
void p_unit_free_all(struct the_p_unit**);
void p_unit_map_all(struct the_p_unit*,map_f);

#endif

