#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

double Buffer[99999];
#define MAXEVENT 1000000	//taille max de l'echeancier
#define MAXTEMPS 1000
#define EPSILON 1e-3
#define Mu 1 // Loi de service MU fixé à 1
#define N_PC 10 // nombre d'Ordinateurs disponibles

double* Lambdas;
double Lambda; // L < M 

int NbVal;
double temps = 0;
int n = 0;
int Nentrees = 0;		//nb de clients dans la file a l'instant temps
int compteur = 0;	//cond d'arret 2
double cumule = 0;
int File[10];

typedef struct Event {
	int type; //0 pour arrive 1 pour S1 µ... 10 pour S10
	double date;
	int etat; //0 pour non traité 1 pour non traité
}event;

typedef struct Echeancier {
	event TAB[MAXEVENT];
	int taille;
}echeancier;

echeancier Ech;

double Exponnentielle(double lbda) {
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

int Sum_Attente_file()
{
	int sum = 0;
	for(int i =0;i<10;i++)
	{
		if(File[i] >1)
			sum += File[i] -1;
	}
	return sum;
}

void Arrive_Event(event e) {
	//printf("execution Arrivé Client\n");
	event e1;
	e1.type = 0;
	e1.date = e.date + Exponnentielle(Lambda);
	e1.etat = 0;	
	Ajouter_Ech(e1);
	int alea = rand()%10;
	Nentrees++;	
	File[alea]++;
	if( File[alea] == 1)
	{
		event e2;
		e2.type = alea + 1; // indice de 0 à 10 mais type de 1 à 11 
		e2.date = e.date + Exponnentielle(Mu);
		e2.etat = 0;		
		Ajouter_Ech(e2);
	}		
	temps = e.date;
}

void Service_Event(event e)	//service = Mu
{
	int ind = e.type -1;
	if(File[ind] > 0)
	{
		File[ind]--;
		if(File[ind] >0)
		{
			event e1;
			e1.type = e.type;
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
			printf("(FS,%lf,%d),",e.date,e.etat);
	}
		}
		printf("] \n \n ");
}

int condition_arret(long double old, long double new)
{
	if( fabs(old-new) < EPSILON && temps > 1e4)
	{
		compteur++;
		if(compteur > 1e3)
			return 1;
	}
	return 0;
}

void init_buffer()
{
	for(int i = 0; i<99999;i++)
	{
		Buffer[i] = 0;
	}
}

double Get_90()
{
	double Vals[NbVal];
	int i,j;
	
	//printf("Nbval : %d \n",NbVal);
	for(i =0;i< NbVal;i++)
	{
		//printf("Buffer[%d] = %f \n",i,Buffer[i]);
		Vals[i] = Buffer[i];
		//printf("Vals[%d] = %f \n",i,Vals[i]);
	}
	int min = 0;
	double tmp = 0;
	// tri
	 for (i = 0; i < NbVal; i++) {
		 min = i;
       for (j =i+1 ; j< NbVal;j++){
		   if(Vals[j] < Vals[min])
				min = j;
	   }
	   if(min != i)
	   {
		   tmp = Vals[i];
		   Vals[i] = Vals[min];
		   Vals[min] = tmp;
	   }
	}
 
   int indice = 0.9 * NbVal;
   //printf("indice = %d \n",indice);
   double per90 = Vals[indice]; 
   //printf("per90 : %f \n", per90);
   return per90;
}

void Simulateur(FILE* f1)
{
	double OldNmoyen = 0;
	double Tmoyen =0 ;
	event e ;
	int k = 0;
	for(int i =0;i<10;i++)
	{
		File[i] =0;
	}
	Init_Ech();
	int Serv = 0;
	while((condition_arret(OldNmoyen,Tmoyen) == 0 && k <10000))//   //arret > 0
	{
		//printf("flag 1 \n");	
		e = Extraire();	
		n= Sum_Attente_file();
		//printf("n = %d \n",n);
		if(n>0)
		{
			cumule += (e.date-temps)*(n);
			Buffer[k] = (e.date-temps)*(n);
			OldNmoyen = Tmoyen;
			Tmoyen = cumule/Nentrees;
			Tmoyen = Tmoyen; //moyenne 
			//printf("Tmoyen = %f \n",Tmoyen);
			//printf("Buffer[%d] = %f \n",k,Buffer[k]);
			NbVal++; k++;
		}
		if(temps == 0)
		{
		//	printf("temps = 0 et N = 0 et Nmoyen = 0 \n");
			//fprintf(f1,"0    0 \n");
		}
		else
		{
		//	printf("temps = %f et N = %d et Tmoyen = %lf\n",temps,n,Tmoyen);
			//fprintf(f1,"%f      %lf \n",temps, Nmoyen);
		}
		
		if(e.type == 0)
			Arrive_Event(e);
		if(e.type >= 1)
			Service_Event(e);
	}
	//printf("flag1 \n");
	double get90 = Get_90();
	printf("get90 : %f \n",get90);
	//printf("Ouverture...");
	//printf("OK \n");
	//printf("ecriture...");
	fprintf(f1,"%lf %lf %lf\n",Lambda,Tmoyen,get90);
	//printf("OK \n");
	
}

int MMsplit10(char* fichier,double LAMBDA)
{
	init_buffer();
	NbVal = 0;
	temps = 0;
	n = 0;	
	Nentrees = 0;		//
	compteur = 0;	
	cumule = 0;
	//printf(" NPC = %d \n",N_PC);
	//printf("Lambda = %lf \n",LAMBDA);
	Lambda = LAMBDA;
	printf("Lambda = %lf \n",Lambda);
	
	//printf("flag1 \n");
	srandom(time(NULL)+getpid());
	FILE *f1 = fopen(fichier,"a");
	Simulateur(f1);
	fclose(f1);
	
	
	return 0;
}

int Lectures_Lambdas(char* fichier)
{
	FILE* f = fopen(fichier,"r");
	if(f != NULL)
	{
	int nombre = 0;
	double tmp = 0;
	fscanf(f,"%d",&nombre);
	printf("%d \n",nombre);
	Lambdas = malloc(nombre * sizeof(double));
	for(int i = 0;i<nombre;i++)
	{
		fscanf(f,"%lf",&Lambdas[i]);
		printf("Lambdas[%d] = %lf \n",i,Lambdas[i]);
	}
	fclose(f);
	return nombre;
	}
else
{
	return -1;
}
}

int main(int argc, char **argv)
{
	int nombre = Lectures_Lambdas(argv[1]);
	printf("OUVERTURE %s \n",argv[1]);
	if(nombre ==-1)
	{
		printf("ERREUR lambda.txt \n");
		return 0;
	}
	char* F2 = "Scenario2.data";
	remove(F2);
	for(int i = 0;i<nombre;i++)
	{
		MMsplit10(F2,Lambdas[i]);
	}
	// on libere la memoire
	free(Lambdas);
	return 0;
}
