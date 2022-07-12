%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<stdbool.h> /* usata per la variabile pre_cond */
	#include"petri.h"

	void yyerror(char *s);	/* chiamata da yyparse() in caso di errore */ 
	
	extern unsigned int tr_index;
	extern unsigned int pl_index;
	extern unsigned int pre_index;
	extern unsigned int post_index;
	extern bool pre_cond;	/* vera quando si sta effettuando il parsing di
	pre-condizioni */ 
%}

%union {
	unsigned int ival;	/* Per i token con valore numerico, cioè marcature e
						   pesi degli archi */
	char *text;			/* Per i token con valore testuale, es: nomi dei posti,
						   nomi di transizioni, etichette, ecc. */ 
}

%start net
%token TR PL AN NET ARROW	/* Token senza valori associati */ 
%token <text> NAME			/* Identificatori */ 
%token <text> TEXT			/* Per i contenuti delle etichette e delle
							   annotazioni, consiste di una sequenza di
							   caratteri circondata da parentesi graffe */
%token <ival> NUM 
%type <text> place
%type <text> label

%%

net			: trdesc net
			| pldesc net
			| andesc net
			| NET name_list		
				{ return 0; }	/* Parsing finito */ 
			;

trdesc		: TR NAME ':' label pl_list ARROW pl_list
				{	update_transition($2, $4);	
					pre_index = 0;
					post_index = 0;
				}
			| TR NAME pl_list ARROW pl_list
				{	update_transition($2, NULL);
					pre_index = 0;
					post_index = 0;
				}
			;

pldesc		: PL NAME ':' label '(' NUM ')'
				{ update_place($2, $4, $6); }
			| PL NAME ':' label
				{ update_place($2, $4, 0); }
			| PL NAME '(' NUM ')'
				{ update_place($2, NULL, $4); }
			;

andesc		: AN NAME NUM label
			;

label		: NAME
			| TEXT
			;

name_list	: NAME name_list
			| NAME
			;

pl_list		: place pl_list
			| /* epsilon */
				{ pre_cond = pre_cond ? false : true; }
			;

place		: NAME '*' NUM
				{	add_transition();
					add_place($1, $3); 
				}
			| NAME	
				{	add_transition();
					add_place($1, 1);
				}
			;
%%

extern FILE *yyin;

int main(int argc, char **argv){
	if (argc > 1){
		FILE *file;

		file = fopen(argv[1], "r");
		if(!file){
			fprintf(stderr, "could not open %s\n", argv[1]);
			exit(1);
		}
		yyin = file;
	}
	petri_init(); /* istanzia l'array delle transizioni e dei posti */ 
	yyparse();	  /* funzione generata da yacc */
	print_net();  /* stampa la rete nel formato .net di TINA */ 
}

void yyerror (char *s) {
	fprintf(stderr, "%s\n", s);
}
