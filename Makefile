all: the_shell.o the_shell.h the_debug.o the_read.o the_parse.o the_expand.o the_p_unit.o the_p_unit.h  the_analy_string.o the_stack.h the_stack.o the_execute.o the_builtin.o
	gcc -g the_shell.o the_shell.h the_debug.o the_read.o the_parse.o the_expand.o the_p_unit.o the_analy_string.o the_stack.o the_p_unit.h the_stack.h  the_execute.o the_builtin.o -o shell

the_shell.o: the_shell.c the_shell.h
	gcc -g -c the_shell.c the_shell.h

the_debug.o: the_debug.c the_shell.h 
	gcc -g -c the_debug.c the_shell.h

the_read.o: the_read.c the_stack.h the_shell.h
	gcc -g -c the_read.c the_shell.h the_stack.h 

the_p_unit.o: the_p_unit.c the_p_unit.h
	gcc -g -c the_p_unit.c the_p_unit.h

the_parse.o: the_parse.c the_shell.h 
	gcc -g -c the_shell.h the_parse.c 

the_analy_string.o: the_shell.h the_analy_string.c
	gcc -g -c the_shell.h the_analy_string.c

the_expand.o: the_shell.h the_expand.c
	gcc -g -c the_shell.h the_expand.c

the_execute.o: the_shell.h the_execute.c
	gcc -g -c the_shell.h the_execute.c

the_builtin.o : the_shell.h the_builtin.c
	gcc -g -c the_shell.h the_builtin.c

clean:
	rm *.o *.gch
