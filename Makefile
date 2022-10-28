netreader: y.tab.o lex.yy.o petri.o
	gcc -lm -o netreader petri.o y.tab.o lex.yy.o

y.tab.o: y.tab.c
	gcc -c y.tab.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

petri.o: petri.c
	gcc -c petri.c

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.c: lexer.l
	lex lexer.l

.PHONY: clean

clean:
	rm *.o

