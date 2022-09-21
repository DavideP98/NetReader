#ifndef PETRI_H
#define PETRI_H

#include<stdbool.h>
#include"uthash.h"

#define N_INIT 1000		// Numero massimo di posti e transizioni che la matrice può accogliere 
extern int **net;		// puntatore della matrice di incidenza
extern unsigned int net_limit;
extern unsigned int pl_index;		// contatore dei posti
extern unsigned int tr_index;		// contatore delle transizioni
extern unsigned int pl_limit;		// indice dell'ultimo posto delle rete originale (non estesa)
extern unsigned int tr_limit;		// indice dell'utlima transizione della rete originale (non estesa)
extern bool pre_cond;				// vera quando si sta effettuando il parsing dei pre-elementi
extern struct transition *tr_map;	// hash-map contenente le transizioni
extern struct place *pl_map;		// hash-map contenente i posti

struct transition{
	char *id;
	char *label;
	unsigned int net_tr_index;		// Indice nella matrice d'adiacenza
	UT_hash_handle hh;				// Per utilizzare la struct nella hash-map
	bool selected;
	unsigned int p_index;
};

struct place{
	char *id;
	char *label;
	unsigned int net_pl_index;		// Indice nella matrice d'adiacenza
	unsigned int tokens;
	UT_hash_handle hh;				// Per utilizzare la struct nella hash-map
	bool selected;
	unsigned int p_index;
};

/* Allocazione della matrice di incidenza */ 
void petri_init();

/* 
 * Aggiunge una transizione alla hash-map delle transizioni, se essa non è già
 * presente (tr_index NON viene incrementato )
 */
void add_transition(char *id, char *label, unsigned int index);

/* 
 * Aggiunge un posto alla hash-map delle transizioni
 * pl_index viene incrementato, se il posto non è già presente
 */
void add_place(char *id, char *label, unsigned int index, int weight, unsigned int tokens);

/* stampa la matrice di incidenza della rete */
void print_matrix(unsigned int height, unsigned int length);

/* seleziona i posti e le transizioni da duplicare */
void select(char *name);

/* 
 * effettua la copia dei posti e delle transizioni selezionate
 */
void copy_selection(unsigned int n);

/* genera il nuovo id per la copia di un posto o transizione */
char * get_new_id(char *old_id, unsigned int num);

int get_batch_index(char *ptr);

/* stampa la rete in formato .net */
void print_net();

/* stampa il modello Promela corrispondente alla rete */
void print_pml();

/* stampa del modello Promela con un altra strategia */
void print_pml2();


#endif
