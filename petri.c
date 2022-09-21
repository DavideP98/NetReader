#include"petri.h"
#include<math.h>
#include"uthash.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>


int **net = NULL;
unsigned int pl_limit;
unsigned int tr_limit;
bool pre_cond = true;
unsigned int pl_index = 0;
unsigned int tr_index = 0;
struct place *pl_map = NULL;
struct transition *tr_map = NULL;

void petri_init(){
	/* Allocazione della matrice di incidenza */
	net = (int **) calloc(N_INIT, sizeof(int*));
	for(unsigned int i = 0; i < N_INIT; i++){
		net[i] = (int *) calloc(N_INIT, sizeof(int));
	}
}

void add_place(char *name, char *label, unsigned int index, int weight, unsigned int tokens){
	struct place *temp;
	HASH_FIND_STR(pl_map, name, temp);
	if(temp == NULL){	/* Il posto non è presente nella mappa*/
		temp = malloc(sizeof *temp);
		temp->id = (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(temp->id, name);
		if(label != NULL){
			temp->label = (char *) malloc(sizeof(char) * strlen(label) + 1);
			strcpy(temp->label, label);
		}
		temp->tokens = tokens;
		temp->net_pl_index = index;
		temp->selected = false;
		HASH_ADD_KEYPTR(hh, pl_map, temp->id, strlen(temp->id), temp);
		/* Aggiorna matrice d'incidenza */
		net[pl_index][tr_index] = pre_cond ? -weight : weight;
		pl_index++;	
	}else{	/* Il posto è già nella mappa, pl_index non viene incrementato*/
		temp->tokens = tokens;
		temp->selected = false;
		if(label != NULL){
			temp->label = (char *) malloc(sizeof(char) * strlen(label) + 1);
			strcpy(temp->label, label);
		}
		if(tokens == 0){
			/* aggiornare la matrice d'incidenza */
			net[temp->net_pl_index][tr_index] = pre_cond ? -weight : weight;
		}
	}
}


void add_transition(char *name, char *label, unsigned int index){
	struct transition *temp;
	// Controllo che la transizione non sia già presente nella mappa
	// Nella struttura dei file .net le transizioni non vengono ripeture
	HASH_FIND_STR(tr_map, name, temp);
	if(temp == NULL){
		temp = malloc(sizeof *temp); 
		temp->id	= (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(temp->id, name);
		if(label != NULL){
			temp->label = (char *) malloc(sizeof(char) * strlen(label) + 1);
			strcpy(temp->label, label);
		}
		temp->net_tr_index = index;
		temp->selected = false;
		HASH_ADD_KEYPTR(hh, tr_map, temp->id, strlen(temp->id), temp);
		
	}
}

void select(char *name){

	struct transition *tr;
	HASH_FIND_STR(tr_map, name, tr);

	if(tr == NULL){
		struct place *pl;
		HASH_FIND_STR(pl_map, name, pl);
		if(pl != NULL){
			pl->selected = true;
		}
	}else{
		tr->selected = true;
	}
}

void copy_selection(unsigned int n){
	struct place *pl;
	for(pl = pl_map; pl != NULL; pl = pl->hh.next){
		if(pl->selected){
			for(int i = 0; i < n; i++){
				/* Aggiungo una copia con un nuovo nome */
				add_place(get_new_id(pl->id, i), pl->label, pl_index,
					0, pl->tokens);	
				struct place *tmp_pl;
				HASH_FIND_STR(pl_map, get_new_id(pl->id, i), tmp_pl);
				tmp_pl->p_index = pl->net_pl_index;
			}
		}
	}
	struct transition *tr;
	for(tr = tr_map; tr != NULL; tr = tr->hh.next){
		if(tr->selected){
			for(int i = 0; i < n; i++){
				add_transition(get_new_id(tr->id, i), tr->label, tr_index); 
				tr_index++;
				struct transition *tmp_tr;
				HASH_FIND_STR(tr_map, get_new_id(tr->id, i), tmp_tr);
				tmp_tr->p_index = tr->net_tr_index;
			}
		}
	}
	unsigned int i = 0, j = 0;
	for(pl = pl_map; pl != NULL; pl = pl->hh.next){
		i = pl->net_pl_index;	
		for(tr = tr_map; tr != NULL; tr = tr->hh.next){
			j = tr->net_tr_index;
			if(i > pl_limit){
				// il posto i è una copia
				if(j > tr_limit){
					// il posto i e la transitione j sono copie
					// controlla se appartengono allo stesso batch di copie
					if(get_batch_index(pl->id) == get_batch_index(tr->id)){
						net[i][j] = net[pl->p_index][tr->p_index];
					}
				}else{
					if(net[pl->p_index][j] != 0 && !tr->selected)
						net[i][j] = net[pl->p_index][j]; 
				}
			}else{
				if(j > tr_limit)
					// la transizione j è una copia
					if(net[i][tr->p_index] != 0 && !pl->selected)
						net[i][j] = net[i][tr->p_index];
				
			}
		}
	}
}

char * get_new_id(char *old_id, unsigned int num){
	/* numero di cifre che compone il numero */
	unsigned int str_size = ceil(log10(num) + 1);
	char *new_id = (char *)
		malloc((strlen(old_id) + str_size + 1) * sizeof(char));
	strcpy(new_id, old_id);
	/* conversione in stringa del numero */
	char temp[str_size];
	sprintf(temp, "_%d", num);

	/* generazione del nuovo id */
	strcat(new_id, temp);

	return new_id;
}

int get_batch_index(char *ptr){
	unsigned int i = 0;
	bool parse_num = false;
	char tmp[5] = "";
	for(; *ptr; ptr++){
		if(*ptr == '_') parse_num = true; 
		if(parse_num && *ptr >= '0' && *ptr <= '9'){
			tmp[i] = *ptr;
			i++;
		}
	}
	if(!strcmp(tmp, ""))
		return -1;
	else
		return atoi(tmp);
}

void print_matrix(unsigned int height, unsigned int length){
	
	for(int i = 0; i < height; i++){
		for(int j = 0; j < length; j++){
			if(net[i][j] < 0)
				printf("%d ", net[i][j]);
			else /* lascio uno spazio in più per compensare l'assenza del meno */
				printf(" %d ", net[i][j]);
		}
		printf("\n");
	}
}

void print_net(){
	struct transition *tr;
	struct place *pl;
	int weight = 0;
	for(tr = tr_map; tr != NULL; tr = tr->hh.next){
		printf("tr %s [0,w[ ", tr->id);
		for(pl = pl_map; pl != NULL; pl = pl->hh.next){
			weight = net[pl->net_pl_index][tr->net_tr_index];
			if(weight < -1){
				printf("%s*%d ", pl->id, abs(weight));	
			}else if(weight < 0){
				printf("%s ", pl->id);
			}
		}
		printf("-> ");
		for(pl = pl_map; pl != NULL; pl = pl->hh.next){
			weight = net[pl->net_pl_index][tr->net_tr_index];
			if(weight > 1){
				printf("%s*%d ", pl->id, weight);	
			}else if(weight > 0){
				printf("%s ", pl->id);
			}
		}
		printf("\n");
	}
	for(pl = pl_map; pl != NULL; pl = pl->hh.next){
		if(pl->tokens > 0){
			printf("pl %s (%d) \n", pl->id, pl->tokens);	
		}
	}
	printf("net test\n");
}

void print_pml(){
	// definizione dei posti
	struct place *pl;
	for(pl = pl_map; pl != NULL; pl = pl->hh.next){
		printf("byte %s = 0;\n", pl->id);
	}
	struct transition *tr;
	bool first = true;
	for(tr = tr_map; tr != NULL; tr = tr->hh.next){
		printf("active proctype %s(){\n", tr->id);
		printf("\tdo\n");
		printf("\t:: atomic{\n\t\t");
		for(pl = pl_map; pl != NULL; pl = pl->hh.next){
			if(net[pl->net_pl_index][tr->net_tr_index] < 0){
				if(first){
					printf("(%s > 0 ", pl->id);
					first = false;
				}else{
					printf("&& %s > 0 ", pl->id);
				}
			}
		}	
		first = true;
		printf(") -> \n");
		// Modifica la marcatura
		for(pl = pl_map; pl != NULL; pl = pl->hh.next){
			if(net[pl->net_pl_index][tr->net_tr_index] < 0)
				printf("\t\t%s--;\n", pl->id);
			else if(net[pl->net_pl_index][tr->net_tr_index] > 0)
				printf("\t\t%s++;\n", pl->id);
		}
		printf("\t}\n");
		printf("\tod\n");
		printf("}\n");
	}
	printf("init{\n");
	for(pl = pl_map; pl != NULL; pl = pl->hh.next){
		if(pl->tokens > 0)
			printf("\t%s = %d;\n", pl->id, pl->tokens);
	}
	printf("}\n");

}

/*
void print_pml2(){
	// definizione macro
	printf("#define inp(x) (x > 0) -> x = x - 1\n");
	printf("#define out(x) x = x + 1\n");
	// definizione dei posti
	struct place *pl;
	for(pl = pl_map; pl != NULL; pl = pl->hh.next){
		printf("byte %s;\n", pl->id);
	}
	printf("\n");
	printf("init{\n");
	// marcatura iniziale
	for(pl = pl_map; pl != NULL; pl = pl->hh.next){
		if(pl->tokens > 0)
			printf("\t%s = %d;\n", pl->id, pl->tokens);
	}
	printf("\tdo\n");
	struct transition *tr;
	for(tr = tr_map; tr != NULL; tr = tr->hh.next){
		printf("\t// %s\n", tr->id); 
		printf("\t:: atomic{");
		bool first = true;
		for(pl = pl_map; pl != NULL; pl = pl->hh.next){
			if(net[pl->net_pl_index][tr->net_tr_index] < 0){
				if(first){
					printf("inp(%s)", pl->id);
					first = false;
				}else{
					printf(" && inp(%s)", pl->id);
				}
			}
		}
		printf(" -> ");
		first = true;
		for(pl = pl_map; pl != NULL; pl = pl->hh.next){
			if(net[pl->net_pl_index][tr->net_tr_index] > 0){
				if(first){
					printf("out(%s)", pl->id);
					first = false;
				}else{
					printf("; out(%s)", pl->id);
				}
			}
		}
		printf("}\n");
	}
	printf("\tod\n");
	printf("}\n");
}
*/

