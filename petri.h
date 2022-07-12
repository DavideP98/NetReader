#ifndef PETRI_H
#define PETRI_H

#include<stdbool.h>

struct arc{
	unsigned int weight;
	unsigned int place;		/* Indice nell'array dei posti */
};

struct transition{
	char *id;
	char *label;
	struct arc **pre;
	struct arc **post;
	bool user_tr;	/* Transizione controllata dall'utente nel gioco 
							   su reti di Petri */
};

struct place{
	char *id;
	char *label;
	unsigned int tokens;
	bool user_pl;	/* Posto osservabile dall'utente */
};

/* Dichiarazioni funzioni: */

void petri_init();

void add_transition();

void update_transition(const char *id, const char *label);

void add_place(const char *id, unsigned int weight);

void update_place(const char *id, const char *label, unsigned int tokens);

int find_place(const char *id);

void print_net();

#endif
