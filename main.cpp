#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <unistd.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#define QTDPROCESSOS 5

typedef struct {
	std::string id = "";
	int criacao;
	int duracao;
	int prioridade;
	int prioridadeDinamica;
	std::string status = "novo"; // estado inicial da tarefa
	int tempoEspera = 0;
	int tempoExecucao = 0;
	int contStatus = 0;
	int tempoTerminado = 0;
} processo;

void continuar() {
	printf("\nPressione qualquer tecla para continuar...");
	getch();
}
void menu(int *op) {
	system("cls");
	printf("[1] FSCS (First Come, First Served)\n");
	printf("[2] Shortest Job First\n");
	printf("[3] Por prioridade, sem preempcao\n");
	printf("[4] Por prioridade, com preempcao por prioridade\n");
	printf("[5] Round-Robin com quantum = 2s, sem prioridade\n");
	printf("[6] Round-Robin com prioridade e envelhecimento (tq = 2, a = 1)\n");
	printf("[7] Nova entrada de processos\n");
	printf("[8] Sair\n");
	
	scanf("%d", op);
	if(*op < 0 || *op > 8)
		menu(op);
}

void entradaProcessos(processo proc[]) {
	system("cls");
	// criacao|duracao|prioridade
	printf("Entra de processos [criacao | duracao | prioridade]:\n");		
	for(int i = 0; i < QTDPROCESSOS; i++) {
		proc[i].id = ("P" + std::to_string(i+1));
		scanf("%d %d %d", &proc[i].criacao,  &proc[i].duracao,  &proc[i].prioridade);
	}
}

//Imprimir processo
void statusFila(processo fila[], int tempo) {
	std::string idProcura;
	int existe = 0;
	
	printf("\n%d - %d\t", tempo, tempo+1);
	for(int i=0; i < QTDPROCESSOS; i++){
		existe = 0;
		idProcura = ("P" + std::to_string(i+1));
		for(int j = 0; j < QTDPROCESSOS; j++){ // For para organizar a fila
			if(fila[j].id == idProcura){
				existe = 1;
				if(fila[j].status == "Executando")
					printf("##\t");
				else if(fila[j].status == "Pronto")
					printf("--\t");
				else if(fila[j].status == "Finalizado")
					printf("  \t");
				else
					printf("%s\t", fila[j].status.c_str());
					
				break;
			}
			
		}
		if(!existe) // Processo fora da fila (ainda nao foi criado)
			printf("  \t");
	}
	
}

// Imprimir Tempo Medio de Vida, Espera e Troca de Contexto
void tempoVida(processo fila[]){
	float soma = 0, espera = 0; 
	int troca = 0;
	for(int i = 0; i < QTDPROCESSOS; i++){
		soma += (fila[i].tempoTerminado - fila[i].criacao);
		fila[i].tempoEspera = abs(fila[i].tempoExecucao - (fila[i].tempoTerminado - fila[i].criacao) );
		espera += fila[i].tempoEspera;
		troca += fila[i].contStatus;
	}
	printf("\n\nTempo Medio de Vida: %2.1fs", soma/QTDPROCESSOS);
	printf("\nTempo Medio de Espera: %2.1fs", espera/QTDPROCESSOS);
	printf("\nNumero de Trocas de Contexto: %d", troca);
}

//Funcoes - Algoritmos de Escalonamento
void fscs(processo proc[]) {
	int topo = 0;
	int tempo = 0, procFinalizado = 0, contFila = 0;
	processo fila[QTDPROCESSOS];
	
	system("cls");
	printf("Tempo\t");
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1);
		fila[i].id = "";
		fila[i].status = "";
	}
	
	while(procFinalizado < QTDPROCESSOS) {		
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) {
				fila[contFila] = proc[i];
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++;
				contFila++;
			}
		}
		
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado";
			fila[topo].tempoTerminado = tempo;
			fila[topo].contStatus++;
			procFinalizado++;
			topo++;
		}
		
		if(topo == QTDPROCESSOS)
			break;
		
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++;
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++;
		}
		
		statusFila(fila, tempo);
		tempo++;
	}

	tempoVida(fila);
	
	continuar();
}

void shortJobFirst(processo proc[]) {
	int topo = 0, menor = 0;
	int tempo = 0, procFinalizado = 0, contFila = 0;
	processo fila[QTDPROCESSOS];
	
	system("cls");
	printf("Tempo\t");
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1);
		fila[i].id = "";
		fila[i].status = "";
	}
	
	while(procFinalizado < QTDPROCESSOS) {
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) {
				for(int j = 0; j <= QTDPROCESSOS; j++) {
					if((proc[i].duracao < fila[j].duracao && fila[j].status == "Pronto") || j == contFila) {
						if(j != contFila) {
							for(int c = contFila; c > j; c--) {
								fila[c] = fila[c-1];
							}
						}
						fila[j] = proc[i];
						fila[j].status = "Pronto";
						fila[j].contStatus++;
						contFila++;
						
						break;
					}
				}
			}
		}	
		
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado";
			fila[topo].tempoTerminado = tempo;
			fila[topo].contStatus++;
			procFinalizado++;
			topo++;
		}
		
		if(topo == QTDPROCESSOS)
			break;
		
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++;
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++;
		}
	
		statusFila(fila, tempo);
	
		tempo++;
	}
	
	tempoVida(fila);
	
	continuar();
}

void prioridade(processo proc[]) {
	int topo = 0, ver = 1, maior = 0;
	int tempo = 0, procFinalizado = 0, contFila = 0;
	processo fila[QTDPROCESSOS];
	
	system("cls");
	printf("Tempo\t");
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1);
		fila[i].id = "";
		fila[i].status = "";
	}
	
	while(procFinalizado < QTDPROCESSOS) {
		maior = 0;	
		
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) {
				fila[contFila] = proc[i];
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++;
				contFila++;
			}
		}
			
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado";
			fila[topo].tempoTerminado = tempo;
			fila[topo].contStatus++;
			procFinalizado++;
			topo++;
		}
		
		if(procFinalizado == QTDPROCESSOS)
			break;
		
		for(int i = 0; i < QTDPROCESSOS; i++){
			if(topo != ver)
				if(fila[i].status == "Pronto")
					if(fila[i].prioridade > maior) {
							maior = fila[i].prioridade;
					}		
		}
					
		for(int i = 0; i < QTDPROCESSOS; i++){
			if(topo != ver)
				if(fila[i].status == "Pronto")
					if(fila[i].prioridade == maior){
						topo = i;
						ver = i;
					}	
		}

		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++;
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++;
		}
	
		statusFila(fila, tempo);
		tempo++;
	}
	
	tempoVida(fila);
	
	continuar();
}

void prioridadePreempcao(processo proc[]) {
	int topo = 0, maior = 0;
	int tempo = 0, procFinalizado = 0, contFila = 0;
	processo fila[QTDPROCESSOS];
	
	system("cls");
	printf("Tempo\t");
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1);
		fila[i].id = "";
		fila[i].status = "";
	}
	
	while(procFinalizado < QTDPROCESSOS) {
		maior = 0;	
		
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) {
				fila[contFila] = proc[i];
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++;
				contFila++;
			}
		}
			
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado";
			fila[topo].tempoTerminado = tempo;
			fila[topo].contStatus++;
			procFinalizado++;
		}
		
		if(procFinalizado == QTDPROCESSOS)
			break;
		
		for(int i = 0; i < QTDPROCESSOS; i++){
			if(fila[i].status == "Pronto" || fila[i].status == "Executando")
				if(fila[i].prioridade > maior)
					maior = fila[i].prioridade;
				
		}
								
		for(int i = 0; i < QTDPROCESSOS; i++){
			if(fila[i].status == "Pronto")
				if(fila[i].prioridade == maior){
					topo = i;
				}	
		}
		
		for(int i = 0; i < QTDPROCESSOS; i++)
			for(int j = 0; j < QTDPROCESSOS; j++){
				if(fila[i].status == "Executando" && fila[j].status == "Pronto")
					if(fila[i].prioridade != maior){
						fila[i].status = "Pronto";
						fila[i].contStatus++;
					}
					if(fila[j].prioridade == maior)
						topo = j;
			}
	
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++;
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++;
		}
	
		statusFila(fila, tempo);
		tempo++;
	}
	
	tempoVida(fila);
	
	continuar();
}

void roundRobin(processo proc[]) {
	int topo = 0, quantum = 0;
	int tempo = 0, procFinalizado = 0, contFila = 0;
	processo fila[QTDPROCESSOS];
	
	system("cls");
	printf("Tempo\t");
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1);
		fila[i].id = "";
		fila[i].status = "";
	}
	
	while(procFinalizado < QTDPROCESSOS) {
			
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) {
				fila[contFila] = proc[i];
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++;
				contFila++;
			}
		}
		
		if(fila[0].status == "Executando" && fila[0].tempoExecucao == fila[0].duracao) {
			fila[0].status = "Finalizado";
			fila[0].tempoTerminado = tempo;
			fila[0].contStatus++;
			procFinalizado++;
		}
		
		if(procFinalizado == QTDPROCESSOS)
			break;
			
			
		for(int j = 0; j < QTDPROCESSOS; j++){
			if(quantum == 2 || fila[0].status == "Finalizado") {	
				quantum = 0;
				if(fila[0].status == "Executando"){
					fila[0].status = "Pronto";
					fila[0].contStatus++;
				}
				processo aux = fila[0];
				for(int i = 0; i < contFila-1; i++) {
					fila[i] = fila[i+1];
				}
				fila[contFila-1] = aux;	
			}
			else
				break;
		
		}
		
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[0].status == "Pronto") {
			fila[0].status = "Executando";
			fila[0].contStatus++;
		}
		
		if(fila[0].status == "Executando") {
			fila[0].tempoExecucao++;
			quantum++;
		}
		
		statusFila(fila, tempo);
		tempo++;
	}

	tempoVida(fila);
	
	continuar();
}


void roundRobinPrioridade(processo proc[]) {
	int topo = 0, quantum = 0, maiorPrioridade = 0;
	int tempo = 0, procFinalizado = 0, contFila = 0;
	processo fila[QTDPROCESSOS];
	
	system("cls");
	printf("Tempo\t");
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1);
		fila[i].id = "";
		fila[i].status = "";
		fila[i].prioridadeDinamica = 0;
	}
	
	while(procFinalizado < QTDPROCESSOS) {		
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) {
				fila[contFila] = proc[i];
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++;
				fila[contFila].prioridadeDinamica = fila[contFila].prioridade;
				contFila++;
			}
		}
		
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado";
			fila[topo].tempoTerminado = tempo;
			fila[topo].contStatus++;
			procFinalizado++;
		}
		
		if(procFinalizado == QTDPROCESSOS)
			break;
		
		if(quantum == 2 || fila[topo].status != "Executando") {
			quantum = 0;
			for(int i = 0; i < QTDPROCESSOS; i++) {
				if(fila[i].status == "Pronto") {
					maiorPrioridade = i;
					break;
				}
			}
			for(int i = 0; i < QTDPROCESSOS; i++) { //Aponta para o proximo processo a ser executado (maior prioridade)
				if(fila[i].status == "Pronto") {
					if(fila[i].prioridadeDinamica > fila[maiorPrioridade].prioridadeDinamica)
						maiorPrioridade = i;
				}
			}
		}
				
		if(maiorPrioridade != topo || fila[topo].status == "Pronto") {
			if(fila[topo].status == "Executando") {
				fila[topo].status = "Pronto";
				fila[topo].contStatus++;
			}
			topo = maiorPrioridade;
			fila[topo].status = "Executando";
			fila[topo].prioridadeDinamica = fila[topo].prioridade;
			fila[topo].contStatus++;
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++;
			quantum++;
			for(int i = 0; i < QTDPROCESSOS; i++) {
				if(fila[i].status == "Pronto")
					fila[i].prioridadeDinamica++;
			}
		}
		
		statusFila(fila, tempo);
		tempo++;
	}

	tempoVida(fila);
	
	continuar();
}

int main(int argc, char** argv) {
	processo proc[QTDPROCESSOS];
	int op = 0;
	
	entradaProcessos(proc);
	while(op != 8) {
		menu(&op);
		
		switch(op) {
			case 1:
				fscs(proc);
				break;
			case 2:
				shortJobFirst(proc);
				break;
			case 3:
				prioridade(proc);
				break;
			case 4:
				prioridadePreempcao(proc);
				break;
			case 5:
				roundRobin(proc);
				break;
			case 6:
				roundRobinPrioridade(proc);
				break;
			case 7:
				entradaProcessos(proc);
				break;
		}
	}
	
	return 0;
}
