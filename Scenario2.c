#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "Scenario2.h"
#include "Var.h"

double Lambda = 0; // L < M 

	//cond d'arret

double temps = 0;
int n = 0;		//nb de clients dans la file a l'instant temps
int compteur = 0;	//cond d'arret 2
double cumule = 0;

echeancier Ech;

double Exponnentielle(int lbda) {
	double r = (double)random()/RAND_MAX;	//entre 0 et 1
	
	while(r==0 || r==1) {	//tant que x vaut 0 ou 1 on refait un random
		r = (double)random()/RAND_MAX;
	}
	return -log(r)/(lbda*1.0); // - log(u)/lamda, avec U = unif(0,1)
}

void Ajouter_Ech(event e) {
	if(Ech.taille < MAXEVENT) {
		Ech.TAB[Ech.taille] = e;
		Ech.taille++;
		//printf("Taille = %d\n", Ech.taille);
	}
	else{}
		//printf("echeancier PLEIN");
}

void Init_Ech(){
	event e;
	e.type = 0;
	e.date = 0;
	e.etat = 0;
	Ech.taille = 0;
	Ajouter_Ech(e);
}

void Arrive_Event(event e) {
	//printf("execution Arrivé Client\n");
	n++;
	
	event e1;
	e1.type = 0;
	e1.date = e.date + Exponnentielle(Lambda);
	e1.etat = 0;
	
	Ajouter_Ech(e1);
	
	if (n == 1) {
		event e2;
		e2.type = 1;
		e2.date = e.date + Exponnentielle(Mu);
		e2.etat = 0;
		
		Ajouter_Ech(e2);
	}
	
	temps = e.date;
	
}

void Service_Event(event e)	//service = Mu
{
	if (n>0) {
		n--;
		if( n>0)
		{
			event e1;
			e1.type = 1;
			e1.date = e.date + Exponnentielle(Mu);
			e1.etat = 0;
			Ajouter_Ech(e1);
		}	
		temps = e.date;	
	}
	
}

event Extraire(){
	int i,imin;
	event min;
	
	for (i = 0; i < Ech.taille; i++) {
		if(Ech.TAB[i].etat == 0) {
			min = Ech.TAB[i];
			imin = i;
			break;
		}
	}
	for (i = 0; i < Ech.taille; i++) {
		if(min.date > Ech.TAB[i].date && Ech.TAB[i].etat == 0) {
			min = Ech.TAB[i];
			imin = i;
		}
	}
	
	Ech.TAB[imin].etat = 1;
	return min;
}

void affiche_echeancier()
{
	event e;
	
	printf("--> temps %f et N = %ld taille : %d [ ",temps,n,Ech.taille);
	for( int i=0;i<Ech.taille ; i++)
	{
		e = Ech.TAB[i];
		
		if(e.type == 0){
			printf(" (AC,%f,%d),",e.date,e.etat);
	}
		if(e.type == 1){
			printf(" (FS,%lf,%d),",e.date,e.etat);
	}
		}
		printf("] \n \n ");
}

int condition_arret(long double old, long double new)
{
	if( fabs(old-new) < EPSILON && temps > 500)
	{
		compteur++;
		if(compteur > 1e3)
			return 1;
	}
	return 0;
}

void Simulateur(FILE* f1)
{
	
	double OldNmoyen = 0;
	double Nmoyen =0 ;
	event e ;
	
	Init_Ech();
	
	while(condition_arret(OldNmoyen,Nmoyen) == 0 )//temps < MAXTEMPS)//   //arret > 0
	{
		e = Extraire();
		cumule += (e.date-temps)*n;
		
		OldNmoyen = Nmoyen;
		Nmoyen = cumule/temps; //moyenne 
		if(temps == 0)
		{
			printf("temps = 0 et N = 0 et Nmoyen = 0 \n");
			fprintf(f1,"0    0 \n");
		}
		else
		{
			printf("temps = %f et N = %d et Nmoyen = %lf\n",temps,n,Nmoyen);
			fprintf(f1,"%f      %lf \n",temps, Nmoyen);
		}
		
		if(e.type == 0)
			Arrive_Event(e);
		if(e.type == 1)
			Service_Event(e);
	}

}

int MM1(char* fichier)
{
	temps = 0;
	n = 0;	
	compteur = 0;	
	cumule = 0;
	Lambda = LAMBDA/10;
	printf("Lambda = %f \n",Lambda);
	FILE *f1 = fopen(fichier,"w");
	printf("flag1 \n");
	srandom(time(NULL)+getpid());
	Simulateur(f1);
	fclose(f1);
	return 0;

}
