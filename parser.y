%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<stdbool.h> /* usata per la variabile pre_cond */
	#include"petri.h"
	#include<string.h>

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

void read_file(char *filename){
	FILE *file;
	file = fopen(filename, "r");
	if(!file){
		fprintf(stderr, "could not open %s\n", filename);
		exit(1);
	}
	yyin = file;
}

int main(int argc, char **argv){

	switch(argc){
		case 1:
			/* invocazione senza argomenti */
			printf("Error: NetReader needs at least a filename\n");
			break; 
		case 2:
			/* solo nome del file */
			read_file(argv[1]);	
			petri_init();
			yyparse();
			print_pml();
			break;
		case 3:
			read_file(argv[2]);
			petri_init();
			yyparse();
			if(!strcmp(argv[1],"-p"))
				print_pml();
			else if(!strcmp(argv[1], "-n"))
				print_net();
			else
				printf("Unknown argument\n");
	}
}

void yyerror (char *s) {
	fprintf(stderr, "%s\n", s);
}
