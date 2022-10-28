%{
	#include"petri.h"
	#include<stdio.h>
	#include<stdlib.h>
	#include<stdbool.h>
	#include<string.h>
	int yylex();
	void yyerror(char *s);	/* chiamata da yyparse() in caso di errore */ 
	char *temp[5];
	int cnt;
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
			| NET NAME	
				{ return 0; }	/* Parsing finito */ 
			;

trdesc		: TR NAME ':' label pl_list ARROW pl_list
				{	
					add_transition($2, $4, tr_index);
					tr_index++;
				}
			| TR NAME pl_list ARROW pl_list
				{	
					add_transition($2, NULL, tr_index);
					tr_index++;
				}
			;

pldesc		: PL NAME ':' label '(' NUM ')'
				{
					add_place($2, $4, pl_index, 0, $6);
				}
			| PL NAME ':' label
				{
				}
			| PL NAME '(' NUM ')'
				{
					add_place($2, NULL, pl_index, 0, $4);
				}
			;

andesc		: AN NAME NUM '{' exp '}'
			;

label		: NAME
			| TEXT
			;

exp			: NUM '(' name_list ')' exp /* sintassi espansione rete */
				{
					pl_limit = pl_index - 1;
					tr_limit = tr_index - 1;
					copy_selection($1);	 
				}
			/* sintassi per la strategia del gioco su rete di petri */
			| strategy exp 
			| /* epsilon */
			;	

strategy	: name_list_p ':' NAME ';'
				{
					struct transition *tr;
					HASH_FIND_STR(tr_map, $3, tr);
					for(int i = 0; i < 5; i++){
						tr->strategy[i] = temp[i];
					}
					cnt = 0;
				}
			;

name_list_p : NAME ',' name_list_p
				{
					temp[cnt] = $1;	
					cnt++;
				}
			| NAME
				{
					temp[cnt] = $1;
					cnt++;
				}
			;

name_list	: NAME ',' name_list
				{
					_select($1);
				}
			| NAME
				{
					_select($1);
				}
			;

pl_list		: place pl_list
			| /* epsilon */
				{ pre_cond = pre_cond ? false : true; }
			;

place		: NAME '*' NUM
				{	
					add_place($1, NULL, pl_index, $3, 0);
				}
			| NAME	
				{
					add_place($1, NULL, pl_index, 1, 0);
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
			printf("Error: missing file name argument\n");
			break;
		case 2:
			// l'output di default è il modello Promela (metodo veloce)
			read_file(argv[1]);
			petri_init();
			yyparse();
			print_pml();
			break;
		case 3:
			read_file(argv[2]);
			petri_init();
			yyparse();
			if(!strcmp(argv[1], "-net"))
				print_net();
			else if(!strcmp(argv[1], "-pml"))
				print_pml();
			else if(!strcmp(argv[1], "-pml2"))
				print_pml2();
			else
				printf("Unknown argument: possible arguments are -net -pml -pml2\n");
			break;
		case 4:
			printf("Error: too many arguments\n");
			break;
	}
	/*
	struct transition *tr;
	for(tr = tr_map; tr != NULL; tr = tr->hh.next){
		if(tr->strategy[0] != NULL){
			printf("%s scatta quando: ", tr->id);
			for(int i = 0; i < 5; i++){
				printf("%s ", tr->strategy[i]);
			}
			printf("sono marcati\n");
		}
	}
	*/
}

void yyerror (char *s) {
	fprintf(stderr, "%s\n", s);
}
