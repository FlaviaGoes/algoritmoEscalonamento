#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <unistd.h>

//Algoritmos de Escalonamento - Simulador

//ADS - 2o Semestre 2023
//Professor - Newton

//Flavia Moreira Goes
//Renan Yohanssen Martins
//Ruhan Garatini

#define QTDPROCESSOS 5 //Quantidade de processos

//Estrutura dos Processos
typedef struct {
	std::string id = ""; //identificador
	int criacao; //data de inicio
	int duracao; //tempo de duracao do processo
	int prioridade; //prioridade estatica
	int prioridadeDinamica; //prioridade dinamica do processo
	std::string status = "novo"; // estado inicial da tarefa
	int tempoExecucao = 0; //armazena o tempo que a funcao ja foi executada
	int contStatus = 0; //armazena a quatidade de mudancas de contexto
	int tempoTerminado = 0; //data de conclusao do processo
} processo;

//Funcao para continuar...
void continuar() {
	printf("\nPressione qualquer tecla para continuar...");
	getch();
}

//Funcao menu recursivo
void menu(int *op) {
	system("cls"); //Limpa o console
	printf("[1] FSCS (First Come, First Served)\n");
	printf("[2] Shortest Job First\n");
	printf("[3] Por prioridade, sem preempcao\n");
	printf("[4] Por prioridade, com preempcao por prioridade\n");
	printf("[5] Round-Robin com quantum = 2s, sem prioridade\n");
	printf("[6] Round-Robin com prioridade e envelhecimento (tq = 2, a = 1)\n");
	printf("[7] Nova entrada de processos\n");
	printf("[8] Sair\n");
	
	scanf("%d", op);
	if(*op < 0 || *op > 8) //Se o valor inserido for maior que 8 ou menor que 0
		menu(op); //A funcao menu e chamada novamente
}

//Funcao responsavel pela entrada dos processos, recebe o vetor proc
void entradaProcessos(processo proc[]) {
	system("cls"); //Limpa o console
	// criacao|duracao|prioridade
	printf("Entra de processos [criacao | duracao | prioridade]:\n");		
	for(int i = 0; i < QTDPROCESSOS; i++) {
		proc[i].id = ("P" + std::to_string(i+1)); //Definindo o id dos processos
		scanf("%d %d %d", &proc[i].criacao,  &proc[i].duracao,  &proc[i].prioridade);
	}
}

//Imprimir processo
void statusFila(processo fila[], int tempo) {
	std::string idProcura;
	int existe;
	
	printf("\n%d - %d\t", tempo, tempo+1); //Representacao do tempo de execucao
	for(int i=0; i < QTDPROCESSOS; i++){
		existe = 0;
		idProcura = ("P" + std::to_string(i+1));
		for(int j = 0; j < QTDPROCESSOS; j++){ // For para organizar a fila
			if(fila[j].id == idProcura){ 
				//Condicoes para cada contexto diferente
				existe = 1;
				if(fila[j].status == "Executando") 
					printf("##\t");
				else if(fila[j].status == "Pronto")
					printf("--\t");
				else if(fila[j].status == "Finalizado")
					printf("  \t");
				break;
			}	
		}
		if(!existe) //Processo fora da fila (ainda nao foi encontrado)
			printf("  \t");
	}	
}

// Imprimir Tempo Medio de Vida, Espera e Troca de Contexto
void tempoVida(processo fila[]){
	float vida = 0, espera = 0; //Variaveis para o armazenamento das informacoes de cada processo
	int troca = 0;
	for(int i = 0; i < QTDPROCESSOS; i++){ //For para percorrer todos os processos
		vida += (fila[i].tempoTerminado - fila[i].criacao); //Soma do tempo de Vida de todos os processos
		espera += abs(fila[i].tempoExecucao - (fila[i].tempoTerminado - fila[i].criacao)); //Soma do tempo de espera de todos os processos
		troca += fila[i].contStatus; //Soma da troca de contexto de cada processo
	}
	printf("\n\nTempo Medio de Vida: %2.1fs", vida/QTDPROCESSOS); //Dividir a soma do tempo de vida pela quantidade de processos
	printf("\nTempo Medio de Espera: %2.1fs", espera/QTDPROCESSOS); //Dividir a soma do tempo de espera pela quantidade de processos
	printf("\nNumero de Trocas de Contexto: %d", troca);
}

//Funcoes - Algoritmos de Escalonamento

//First Come, First Served
void fcfs(processo proc[]) {
	int topo = 0; // indica qual processo deve estar sendo executado
	int tempo = 0, procFinalizado = 0, contFila = 0; //tempo e contadores
	processo fila[QTDPROCESSOS]; //Novo vetor para os processos na fila de prontas
	
	system("cls"); //Limpa o console
	printf("Tempo\t"); //Imprime cabecalho do tempo
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1); //Imprime o cabecalho de cada tarefa
		fila[i].id = ""; //Evitar lixo de memoria
		fila[i].status = ""; //Evitar lixo de memoria
	}
	
	while(procFinalizado < QTDPROCESSOS) { //Enquanto a quantidade de processos finalizados for menor que a quantidade de processos
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) { //Se a criacao do processo for igual ao tempo
				fila[contFila] = proc[i]; //O processo entra na fila de tarefas com status Pronto
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++; //Conta a mudança de Contexto  do processo
				contFila++; //Incrementa o contador da fila
			}
		}
		
		//Se o processo estiver no contexto executando e seu tempo de execucao for igual sua duracao ele encerra
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado"; //muda o contexto do processo para finalizado
			fila[topo].tempoTerminado = tempo; //guarda o tempo que a tarefa foi encerrada
			fila[topo].contStatus++; //Incrementa a mudanca de contexto
			procFinalizado++; //Incrementa o numero de processos encerrados
			topo++; //Incrementa o topo indo para o proximo processo
		}
		
		//Caso o topo seja igual a quantidade de processos saimos da funcao
		if(topo == QTDPROCESSOS)
			break;
		
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++; //Conta a mudança de Contexto  do processo
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++; //Conta o tempo de execucao do processo
		}
		
		statusFila(fila, tempo); //Chamada para a funcao que imprime o contexto dos processos
		tempo++; //Incremento do tempo
	}

	tempoVida(fila); //chamada para exibir tempo medio de vida e espera do algoritmo alem das trocas de contexto
	continuar(); //funcao para continuar
}

//Shortest Job First
void SJF(processo proc[]) {
	int topo = 0; // indica qual processo deve estar sendo executado
	int tempo = 0, procFinalizado = 0, contFila = 0; //tempo e contadores
	processo fila[QTDPROCESSOS]; //Novo vetor para os processos na fila de prontas
	
	system("cls"); //Limpa o console
	printf("Tempo\t"); //Imprime cabecalho do tempo
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1); //Imprime o cabecalho de cada tarefa
		fila[i].id = ""; //Evitar lixo de memoria
		fila[i].status = ""; //Evitar lixo de memoria
	}
	
	while(procFinalizado < QTDPROCESSOS) { //Enquanto a quantidade de processos finalizados for menor que a quantidade de processos
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
	
		//Se o processo estiver no contexto executando e seu tempo de execucao for igual sua duracao ele encerra
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado"; //muda o contexto do processo para finalizado
			fila[topo].tempoTerminado = tempo; //guarda o tempo que a tarefa foi encerrada
			fila[topo].contStatus++; //Incrementa a mudanca de contexto
			procFinalizado++; //Incrementa o numero de processos encerrados
			topo++; //Incrementa o topo indo para o proximo processo
		}
		
		//Caso o topo seja igual a quantidade de processos saimos da funcao
		if(topo == QTDPROCESSOS)
			break;
		
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++; //Conta a mudança de Contexto  do processo
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++; //Conta o tempo de execucao do processo
		}
		
		statusFila(fila, tempo); //Chamada para a funcao que imprime o contexto dos processos
		tempo++; //Incremento do tempo
	}
	
	tempoVida(fila); //chamada para exibir tempo medio de vida e espera do algoritmo alem das trocas de contexto
	continuar(); //funcao para continuar
}

//Por Prioridade Sem Preempcao
void prioridade(processo proc[]) {
	int topo = 0, ver = 1, maior; //variavel para armazenar a maior prioridade
	int tempo = 0, procFinalizado = 0, contFila = 0; //tempo e contadores
	processo fila[QTDPROCESSOS]; //Novo vetor para os processos na fila de prontas
	
	system("cls"); //Limpa o console
	printf("Tempo\t"); //Imprime cabecalho do tempo
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1); //Imprime o cabecalho de cada tarefa
		fila[i].id = ""; //Evitar lixo de memoria
		fila[i].status = ""; //Evitar lixo de memoria
	}
	
	while(procFinalizado < QTDPROCESSOS) { //Enquanto a quantidade de processos finalizados for menor que a quantidade de processos
		maior = 0; // em cada interação maior e igual a 0
		
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) { //Se a criacao do processo for igual ao tempo
				fila[contFila] = proc[i]; //O processo entra na fila de tarefas com status Pronto
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++; //Conta a mudança de Contexto  do processo
				contFila++; //Incrementa o contador da fila
			}
		}
				
		//Se o processo estiver no contexto executando e seu tempo de execucao for igual sua duracao ele encerra
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado"; //muda o contexto do processo para finalizado
			fila[topo].tempoTerminado = tempo; //guarda o tempo que a tarefa foi encerrada
			fila[topo].contStatus++; //Incrementa a mudanca de contexto
			procFinalizado++; //Incrementa o numero de processos encerrados
			topo++; //Incrementa o topo indo para o proximo processo
		}
		
		//Caso o topo seja igual a quantidade de processos saimos da funcao
		if(procFinalizado == QTDPROCESSOS)
			break;
		
		for(int i = 0; i < QTDPROCESSOS; i++){ 
			if(topo != ver) // as duas variaveis precisam ser diferentes
				if(fila[i].status == "Pronto") // o processo precisar estar no contexto Pronto
					if(fila[i].prioridade > maior) { //Comparando a prioridade dos processos prontos
							maior = fila[i].prioridade; //atribui o valor para a variavel maior
					}		
		}
					
		for(int i = 0; i < QTDPROCESSOS; i++){
			if(topo != ver) 
				if(fila[i].status == "Pronto")
					if(fila[i].prioridade == maior){ //se a prioridade do processo for igual a maior prioridade
						topo = i; //atribui o processo para ser executado
						ver = i; //atribui para garantir que uma nova interacao ocorra somente quando o processo ser finalizado
					}	
		}
		
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++; //Conta a mudança de Contexto  do processo
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++; //Conta o tempo de execucao do processo
		}
		
		statusFila(fila, tempo); //Chamada para a funcao que imprime o contexto dos processos
		tempo++; //Incremento do tempo
	}
	
	tempoVida(fila); //chamada para exibir tempo medio de vida e espera do algoritmo alem das trocas de contexto
	continuar(); //funcao para continuar
}

//Por Prioridade Com Preempção 
void prioridadePreempcao(processo proc[]) {
	int topo = 0, maior = 0; //variavel para armazenar a maior prioridade
	int tempo = 0, procFinalizado = 0, contFila = 0; //tempo e contadores
	processo fila[QTDPROCESSOS]; //Novo vetor para os processos na fila de prontas
	
	system("cls"); //Limpa o console
	printf("Tempo\t"); //Imprime cabecalho do tempo
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1); //Imprime o cabecalho de cada tarefa
		fila[i].id = ""; //Evitar lixo de memoria
		fila[i].status = ""; //Evitar lixo de memoria
	}
	
	while(procFinalizado < QTDPROCESSOS) { //Enquanto a quantidade de processos finalizados for menor que a quantidade de processos	
		maior = 0; // em cada interação maior e igual a 0
		
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) { //Se a criacao do processo for igual ao tempo
				fila[contFila] = proc[i]; //O processo entra na fila de tarefas com status Pronto
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++; //Conta a mudança de Contexto  do processo
				contFila++; //Incrementa o contador da fila
			}
		}
				
		//Se o processo estiver no contexto executando e seu tempo de execucao for igual sua duracao ele encerra
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado"; //muda o contexto do processo para finalizado
			fila[topo].tempoTerminado = tempo; //guarda o tempo que a tarefa foi encerrada
			fila[topo].contStatus++; //Incrementa a mudanca de contexto
			procFinalizado++; //Incrementa o numero de processos encerrados
		}
		
		//Caso o topo seja igual a quantidade de processos saimos da funcao
		if(procFinalizado == QTDPROCESSOS)
			break;
		
		for(int i = 0; i < QTDPROCESSOS; i++){
			if(fila[i].status == "Pronto" || fila[i].status == "Executando") 
				if(fila[i].prioridade > maior) //Compara a prioridade de processos prontos e executando
					maior = fila[i].prioridade;
				
		}
								
		for(int i = 0; i < QTDPROCESSOS; i++){
			if(fila[i].status == "Pronto") //Se tiverem apenas processos prontos na interacao
				if(fila[i].prioridade == maior){
					topo = i;
				}	
		}
		
		for(int i = 0; i < QTDPROCESSOS; i++)
			for(int j = 0; j < QTDPROCESSOS; j++){ //Se tiverem processos prontos e executando
				if(fila[i].status == "Executando" && fila[j].status == "Pronto"){
					if(fila[i].prioridade == maior)
						topo = i;
					else if(fila[j].prioridade == maior){
							topo = j; //atribui j ao topo
							fila[i].status = "Pronto"; //Muda o contexto para pronto
							fila[i].contStatus++; //Incrementa as mudancas de contexto
					}				
				}	
			}
		
		// Mudanca de contexto para o primeiro elemento da fila
		if(fila[topo].status == "Pronto") {
			fila[topo].status = "Executando";
			fila[topo].contStatus++; //Conta a mudança de Contexto  do processo
		}
		
		if(fila[topo].status == "Executando") {
			fila[topo].tempoExecucao++; //Conta o tempo de execucao do processo
		}
		
		statusFila(fila, tempo); //Chamada para a funcao que imprime o contexto dos processos
		tempo++; //Incremento do tempo
	}
	
	tempoVida(fila); //chamada para exibir tempo medio de vida e espera do algoritmo alem das trocas de contexto
	continuar(); //funcao para continuar
}

//Round-Robin Com Quantum = 2s Sem Prioridade 
void roundRobin(processo proc[]) {
	int quantum = 0; //variavel que controla o tempo do processo no processador quando executado
	int tempo = 0, procFinalizado = 0, contFila = 0; //tempo e contadores
	processo fila[QTDPROCESSOS]; //Novo vetor para os processos na fila de prontas
	
	system("cls"); //Limpa o console
	printf("Tempo\t"); //Imprime cabecalho do tempo
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1); //Imprime o cabecalho de cada tarefa
		fila[i].id = ""; //Evitar lixo de memoria
		fila[i].status = ""; //Evitar lixo de memoria
	}
	
	while(procFinalizado < QTDPROCESSOS) { //Enquanto a quantidade de processos finalizados for menor que a quantidade de processos
		// Entrada dos processos na fila de Pronto
		for(int i = 0; i < QTDPROCESSOS; i++) {
			if(proc[i].criacao == tempo) { //Se a criacao do processo for igual ao tempo
				fila[contFila] = proc[i]; //O processo entra na fila de tarefas com status Pronto
				fila[contFila].status = "Pronto";
				fila[contFila].contStatus++; //Conta a mudança de Contexto  do processo
				contFila++; //Incrementa o contador da fila
			}
		}
		
		//Se o processo estiver no contexto executando e seu tempo de execucao for igual sua duracao ele encerra
		if(fila[0].status == "Executando" && fila[0].tempoExecucao == fila[0].duracao) {
			fila[0].status = "Finalizado"; //muda o contexto do processo para finalizado
			fila[0].tempoTerminado = tempo; //guarda o tempo que a tarefa foi encerrada
			fila[0].contStatus++; //Incrementa a mudanca de contexto
			procFinalizado++; //Incrementa o numero de processos encerrados
		}
		
		//Caso o topo seja igual a quantidade de processos saimos da funcao
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
			fila[0].contStatus++; //Conta a mudança de Contexto  do processo
		}
		
		if(fila[0].status == "Executando") {
			fila[0].tempoExecucao++; //Conta o tempo de execucao do processo
			quantum++; //Incrementa o quantum
		}
		
		statusFila(fila, tempo); //Chamada para a funcao que imprime o contexto dos processos
		tempo++; //Incremento do tempo
	}

	tempoVida(fila); //chamada para exibir tempo medio de vida e espera do algoritmo alem das trocas de contexto
	continuar(); //funcao para continuar
}

//Round-robin Com Prioridade e Envelhecimento (tq=2, a=1) 
void roundRobinPrioridade(processo proc[]) {
	int topo = 0, quantum = 0, maiorPrioridade = 0;
	int tempo = 0, procFinalizado = 0, contFila = 0; //tempo e contadores
	processo fila[QTDPROCESSOS]; //Novo vetor para os processos na fila de prontas
	
	system("cls"); //Limpa o console
	printf("Tempo\t"); //Imprime cabecalho do tempo
	for(int i = 0; i < QTDPROCESSOS; i++) {
		printf("P%d\t", i+1); //Imprime o cabecalho de cada tarefa
		fila[i].id = ""; //Evitar lixo de memoria
		fila[i].status = ""; //Evitar lixo de memoria
	}
	
	while(procFinalizado < QTDPROCESSOS) { //Enquanto a quantidade de processos finalizados for menor que a quantidade de processos	
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
		
		//Se o processo estiver no contexto executando e seu tempo de execucao for igual sua duracao ele encerra
		if(fila[topo].status == "Executando" && fila[topo].tempoExecucao == fila[topo].duracao) {
			fila[topo].status = "Finalizado"; //muda o contexto do processo para finalizado
			fila[topo].tempoTerminado = tempo; //guarda o tempo que a tarefa foi encerrada
			fila[topo].contStatus++; //Incrementa a mudanca de contexto
			procFinalizado++; //Incrementa o numero de processos encerrados
		}
		
		//Caso o topo seja igual a quantidade de processos saimos da funcao
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
		
		statusFila(fila, tempo); //Chamada para a funcao que imprime o contexto dos processos
		tempo++; //Incremento do tempo
	}

	tempoVida(fila); //chamada para exibir tempo medio de vida e espera do algoritmo alem das trocas de contexto
	continuar(); //funcao para continuar
}

int main(int argc, char** argv) {
	processo proc[QTDPROCESSOS]; //criacao de um vetor para os processos
	int op = 0;
	
	entradaProcessos(proc);
	while(op != 8) {
		menu(&op);
		
		switch(op) {
			case 1:
				fcfs(proc);
				break;
			case 2:
				SJF(proc);
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
