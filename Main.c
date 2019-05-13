#include "stdio.h"
#include "stdlib.h"
#include "Scenario2.h"
#include "Var.h"

int Lectures_Lambdas(double* Lambdas)
{
	FILE* f = fopen("./lambda.txt","r");
	if(f != NULL)
	{
	int nombre = 0;
	int tmp = 0; int ok = 1;
	fscanf(f,"%d",&nombre);
	printf("%d \n",nombre);
	Lambdas = malloc(nombre * sizeof(int));
	for(int i = 0;i<nombre;i++)
	{
		fscanf(f,"%d",&tmp);
		printf("tmp : %d \n",tmp);
		Lambdas[i] = tmp;
		printf("Lambdas[%d] = %d \n",i,Lambdas[i]);
		//~ ok =  1+Lambdas[i];
		//~ printf("%d \n", ok);
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
	// formart ./main fichier1 fichier 2 fichier3 lambda
	/*
	if(argc != 5)
	{
		printf("too few arguments \n");
		return 0;	
	}
	*/
	double* Lambdas;
	int nombre = Lectures_Lambdas(Lambdas);
	if(nombre ==-1)
	{
		printf("ERREUR lambda.txt \n");
		return 0;
	}
	LAMBDA = 50;
	printf("lambda = %d \n",LAMBDA);
	
	// on libere la memoire
	free(Lambdas);
	return 0;
}

