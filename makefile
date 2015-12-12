CFLAGS = -std=c99

repl: repl.o mpc.o
	cc $(CFLAGS) -Wall repl.c mpc.c -ledit -lm -o repl
