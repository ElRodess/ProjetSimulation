#ifndef __include_Var_h__
#include "stdio.h"
#include "stdlib.h"
#include <string.h>


#define __include_Var_h__
#define MAXEVENT 1000000	//taille max de l'echeancier
#define MAXTEMPS 1000
#define EPSILON 1e-3
#define Mu 60 // Loi de service MU fixé à 1h soit 60 minutes avec 1 minutes = 1 secondes en temps de simul
#define N_PC 10 // nombre d'Ordinateurs disponibles
int LAMBDA; // Loi d'arrivée LAMBDA à faire varier

typedef struct Event {
	int type; //0 pour arrive 1 pour sortie
	double date;
	int etat; //0 pour non traité 1 pour non traité
}event;

typedef struct Echeancier {
	event TAB[MAXEVENT];
	int taille;
}echeancier;
#endif 
