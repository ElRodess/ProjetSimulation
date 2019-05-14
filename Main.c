#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "Scenario2.h"
#include "Var.h"

int* Lambdas;
int Lectures_Lambdas()
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
		fscanf(f,"%d",&Lambdas[i]);
		printf("Lambdas[%d] = %d \n",i,Lambdas[i]);
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
	
	int nombre = Lectures_Lambdas();
	if(nombre ==-1)
	{
		printf("ERREUR lambda.txt \n");
		return 0;
	}
	char S2[32] = "MM1_";
	char* data =".data";
	for(int i = 0;i<nombre;i++)
	{
		sprintf(S2,"%s%d",S2,i);
		sprintf(S2,"%s%s",S2,data);
		printf("%s \n",S2);
		LAMBDA = Lambdas[i];
		MM1(S2);
		sprintf(S2,"MM1_");
	}
	
	// on libere la memoire
	free(Lambdas);
	return 0;
}

