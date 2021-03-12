#Makefile
#Robert Hu
#ryhu
#CMPS 111
#4/20/2019
#assgn-1

all: myshell.o
		cc -o myshell myshell.o lex.yy.c -lfl

myshell.o: lex.yy.c
		cc -c myshell.c

lex.yy.c: lex.l
		flex lex.l

clean:
		rm myshell myshell.o lex.yy.c myshell.core