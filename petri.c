#include"petri.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct transition **tr_array; 

struct place **pl_array;

unsigned int tr_index = 0;
unsigned int pl_index = 0; 
unsigned int pre_index = 0;	
unsigned int post_index = 0;
bool pre_cond = true;

/*
 * Istanzia l'array dei posti e l'array delle transizioni
 */
void petri_init(){
	tr_array = (struct transition **) malloc(20 * sizeof(struct transition *));
	pl_array = (struct place **) malloc(20 * sizeof(struct place *));
}

/*
 * Alloca lo spazio per una transitione e inserisce il puntatore verso tale
 * struttura nell'array di puntatori alle transizioni.
 */
void add_transition(){
	if(tr_array[tr_index] == NULL){ /* transizione non ancora istanziata */
		tr_array[tr_index] = (struct transition *) 
			malloc(sizeof (struct transition));
		tr_array[tr_index]->pre = (struct arc **)
			malloc(10 * sizeof(struct arc *));
		tr_array[tr_index]->post = (struct arc **)
			malloc(10 * sizeof(struct arc *));
	}
}

void update_transition(const char *id, const char *label){
	struct transition *tr = tr_array[tr_index];
	tr->id = (char *) malloc(sizeof(strlen(id) + 1));
	strcpy(tr->id, id);
	if(label != NULL){ /* L'etichetta non è obbligatoria */
		tr->label = (char *) malloc(sizeof(strlen(label) + 1));
		strcpy(tr->label, label);
	}
	tr_index++;	/* il parsing della transizione è terminato, si può incrementare
				   il contatore */ 
}

/*
 * Aggiunge un posto all'array dei posti (se non è già presente) e aggiorna le
 * informazioni sulle pre/post condizioni della transizione di indice tr_index
 */
void add_place(const char *id, unsigned int weight){
	int temp_index = find_place(id);
	if(temp_index == -1){
	    /* Aggiungi posto */
		struct place *pl = (struct place *) malloc(sizeof(struct place));
		pl->id = (char *) malloc(strlen(id) + 1);
		strcpy(pl->id, id);
		pl_array[pl_index] = pl;
		temp_index = pl_index;
		pl_index++;
	}
	struct transition *tr = tr_array[tr_index];
	struct arc *a = (struct arc *) malloc(sizeof (struct arc));
	a->place = temp_index;
	a->weight = weight;
	if(pre_cond){
		/* il posto attuale è una pre-condizione di tr */
		tr->pre[pre_index]  = a;
		pre_index++;
	}else{
		tr->post[post_index] = a;
		post_index++;
	}
}

/*
 * Inserimento dell'informazione riguardante il numero di token di un posto
 */
void update_place(const char *id, const char *label, unsigned int tokens){
	int temp_index = find_place(id);
	struct place *pl = pl_array[temp_index];
	pl->tokens = tokens;
	if(label != NULL){
		pl->label = (char *) malloc(sizeof(strlen(label) + 1));
		strcpy(pl->label, label);
	}
}

/* 
 * Compie una ricerca lineare nell'array dei posti per determinare se il posto
 * di nome id è presente. Se è presente ritorna il suo indice, altrimenti 
 * ritorna -1
 */
int find_place(const char *id){
	for(int i = 0; i < pl_index; i++)
		if(strcmp(id, pl_array[i]->id) == 0){
			return i;
		}
	return -1;
}

void print_net(){
	/* stampa delle transizioni */
	for(int i = 0; i < tr_index; i++){
		struct transition *tr = tr_array[i];
		if(tr->label != NULL)
			printf("tr %s : %s [0,w[ ", tr->id, tr->label);
		else
			printf("tr %s [0,w[ ", tr->id);
		/* stampa pre-condizioni */
		struct arc *a = tr->pre[0];
		int j = 1;
		while(a != NULL){
			if(a->weight > 1)
				printf("%s*%d ", pl_array[a->place]->id, a->weight);
			else
				printf("%s ", pl_array[a->place]->id);
			a = tr->pre[j];
			j++;
		}
		printf("-> ");
		a = tr->post[0];
		j = 1;
		while(a != NULL){
			if(a->weight > 1)
				printf("%s*%d ", pl_array[a->place]->id, a->weight);
			else
				printf("%s ", pl_array[a->place]->id);
			a = tr->post[j];
			j++;
		}
		printf("\n");
	}
	/* Stampa dei posti */
	for(int i = 0; i < pl_index; i++){
		struct place *pl = pl_array[i];
		if(pl->tokens > 0){
			if(pl->label != NULL)
				printf("pl %s : %s (%d)", pl->id, pl->label, pl->tokens);
			else
				printf("pl %s (%d)", pl->id, pl->tokens);
			printf("\n");
		}
	}
}

void print_pml(){
	printf("\n");
	/* Dichiarazione dei posti (canali) */
	struct place *pl;
	for(int i = 0; i < pl_index; i++){
		pl = pl_array[i];	
		printf("chan %s = [1] of {bool};", pl->id);
		if(pl->label != NULL)
			printf("\t//%s", pl->label );
		printf("\n");
	}
	printf("\n");
	/* Transizioni */
	struct transition *tr;
	struct arc *a;
	int j;
	for(int i = 0; i < tr_index; i++){
		tr = tr_array[i];
		printf("active proctype %s(){\n", tr->id);
		printf("\tdo\n");
		printf("\t:: atomic{\n");
		printf("\t    ( ");
		a = tr->pre[0];
		j = 1;	
		while(a != NULL){
			if(j > 1) printf("&& ");
			printf("nempty(%s) ", pl_array[a->place]->id);
			a = tr->pre[j];
			j++;
		}
		a = tr->post[0];
		j = 1;
		printf("&& ");
		while(a != NULL){
			if(j > 1) printf("&& ");
			printf("empty(%s) ", pl_array[a->place]->id);
			a = tr->post[j];
			j++;
		}
		printf(") ->\n");
		a = tr->pre[0];
		j = 1;
		while(a != NULL){
			printf("\t\t%s ? _;\n", pl_array[a->place]->id);
			a = tr->pre[j];
			j++;
		}
		a = tr->post[0];
		j = 1;
		while(a != NULL){
			printf("\t\t%s ! true;\n", pl_array[a->place]->id);
			a = tr->post[j];
			j++;
		}
		printf("\t\tprintf(\"%s\\n\");\n", tr->id );
		printf("\t   }\n");
		printf("\tod\n");
		printf("}\n\n");

	}
	
	/* Marcatura iniziale (processo di init) */
	printf("init{\n");
	for(int i = 0; i < pl_index; i++){
		pl = pl_array[i];
		if(pl->tokens > 0)
			printf("\t%s ! true;\n", pl->id);		
	}
	printf("}\n");
}
