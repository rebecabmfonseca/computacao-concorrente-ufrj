#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"timer.h"
#include<time.h>

int main(int argc, char* argv[]){
	FILE *texto;	
	char *busca,*pos,*str;
	long long int quantidade = 0;
    long long index = 0;
    long long ocorrencias = 0;
    long long limite = 0;
    long long linhas = 0;
    long long quantidadeLinhas = 0;
    double tempoTotal = 0.0;

	int caractere;
	double inicio,fim;
	GET_TIME(inicio);	
	if(argc < 2){printf("Digite: %s  <nomeArquivo.txt> <valor que quero encontrar> \n",argv[0]);return 2;} 
	
	texto = fopen(argv[1],"r"); 
	if(texto == NULL){printf("Erro ao abrir o arquivo.\n"); return 2;}	
	
	busca = argv[2];
	
	while(((caractere = fgetc(texto)) != EOF)){	
		quantidade++;
		if(caractere == '\n') quantidadeLinhas++;
	}				
	
	fclose(texto);
	
	limite = quantidade;
	
	str = (char *) malloc(sizeof(char) * limite);
	if(str == NULL){printf("Error no malloc -- str\n"); return 2;}
	
	texto = fopen(argv[1],"r"); 	
	if(texto == NULL){printf("erro ao abrir o arquivo.\n"); return 2;}	
	
    GET_TIME(fim);	
    tempoTotal = fim - inicio;
	printf("Tempo sequencial inicial: %.10f\n",fim - inicio);
	
    GET_TIME(inicio);
	while((fgets(str, limite, texto)) != NULL){
		index = 0;	
		linhas++;	
		while((pos = strstr(str + index,busca)) != NULL){
			if(linhas > quantidadeLinhas){				
				break;				
		        }			        
			index = (pos - str) + 1;
			ocorrencias++;			
		}
	}
	GET_TIME(fim);
    tempoTotal += fim - inicio; 
	fclose(texto);
	printf("Encontrei '%s'  %lld vezes no arquivo que tem %lld linhas \n",busca,ocorrencias,quantidadeLinhas);
	printf("Tempo sequencial da busca do arquivo:%.10f\n",fim - inicio);
    printf("Tempo total: %.10f", tempoTotal);
	free(str);
	return 0;
}