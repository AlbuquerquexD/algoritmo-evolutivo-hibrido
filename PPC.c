#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define TABULEIRO 400 // Número de casas do tabuleiro (NxN)
typedef struct{
	int fitness; // Tamanho do maior percurso válido
	int tour[TABULEIRO]; // Percurso do cavalo
	double sigma; // Parâmetro de estratégia (sigma) das ES, Cada indivíduo agora carrega sua própria "taxa de mutação", que evolui junto com ele.
}INDIVIDUO;

#define ELITISMO 10
#define GERACOES 1000
#define POPULACAO 1000
#define TORNEIO 3
#define MUTACAO 15 // vai da lugar para o sigma do tuor
#define RESETE n // Vai dizer quanto em cada geração nosso sigma vai adotar ... vai ser implementado ainda.

/* Função: escreveRelatorio ... */
void escreveRelatorio(double tempo, int fitness, int geracao){
	FILE *arq, *binario;
	arq = fopen("resultados.txt", "a");
	binario = fopen("metricasPPC.in", "ab");

	if(arq == NULL || binario == NULL){
		printf("Problemas na criação do arquivo\n");
		return;
	}

	fwrite (&geracao, sizeof(int), 1, binario);
 	fwrite (&tempo, sizeof(double), 1, binario);
 	fwrite (&fitness, sizeof(int), 1, binario);
	
	int result = fprintf(arq, "%d&%.3lf&%d\n",geracao, tempo, fitness);
	if(result == EOF)
		printf("Erro na gravação\n");
	fclose(arq);
	fclose(binario);
}


/* Função: gerarNumAleatorio ...*/
int gerarNumAleatorio(int N)
{
	int r = (int)(N * rand() / RAND_MAX);
	return r;
}

/* Funções de coordenadas, validação e movimento ...*/
void coordenadas(int numeroCasa, int *X, int *Y){
	int N = sqrt(TABULEIRO);
	*X = (numeroCasa-1)/N+1;
	*Y = (numeroCasa-1)%N+1;
}

bool posicaoValida(int X, int Y){
	int N = sqrt(TABULEIRO);
	return (X >= 1 && X <= N && Y >= 1 && Y <= N);
}

bool vizinhoValido(int atual, int proximo){
	int X = 0, Y = 0, proximoX = 0, proximoY = 0, validoX = 0, validoY = 0;
	coordenadas(atual, &X, &Y);
	coordenadas(proximo, &proximoX, &proximoY);

	validoX = abs(proximoX - X);
	validoY = abs(proximoY - Y);

	return (validoX == 1 && validoY == 2) || (validoX == 2 && validoY == 1);
}

int numeroCasa(int X, int Y){
	int N = sqrt(TABULEIRO);
	int numeroCasa = (Y - 1) * N + X;
	return numeroCasa;
}

int movimentosPossiveis(int X, int Y) {
	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	int contador = 0;

	for (int i = 0; i < 8; i++) {
		int proximoX = X + eixoX[i];
		int proximoY = Y + eixoY[i];
		if (posicaoValida(proximoX, proximoY))
			contador++;
	}

	return contador;
}

/* Função: regraWarnsdorff

   	Dado um número de casa o seu vizinho válido - com MENOR 
	número de movimentos possíveis - é calculado.

	Parâmetros:
		casa - número de casa.
		visitadas - vetor booleano com número das casas visitadas.
		
	Retorno: 
		Vizinho válido com menor número de movimentos possíveis.
*/
int regraWarnsdorff(int casa, bool visitadas[TABULEIRO+1]){
	int X = 0, Y = 0;
	coordenadas(casa, &X, &Y);
	// Define os movimentos possíveis do cavalo
	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	// Inicializado com um valor acima do possível (8).
	int minMovimentos = 9, valor = 0, valorF = 0; 

	for(int i = 0; i < 8; i++) {
		int auxX = X + eixoX[i];
		int auxY = Y + eixoY[i];
		int aux = (auxX - 1) * sqrt(TABULEIRO) + auxY;
		if (posicaoValida(auxX, auxY) && !visitadas[aux] && vizinhoValido(casa,aux)){
			valor = aux;
			int movimentos = movimentosPossiveis(auxX, auxY);
			if(movimentos < minMovimentos){
				minMovimentos = movimentos;
				valorF = valor;
			}
		}
	}

	if(valorF == 0) valorF = valor;

	return valorF;
}

int proximoMovimento(int casa, bool visitadas[TABULEIRO+1]) {
	int X = 0, Y = 0;
	coordenadas(casa, &X, &Y);
	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	int minMovimentos = 9, valor = 0;

	for(int i = 0; i < 8; i++) {
		int auxX = X + eixoX[i];
		int auxY = Y + eixoY[i];
		int aux = (auxX - 1) * sqrt(TABULEIRO) + auxY;
		if (posicaoValida(auxX, auxY) && !visitadas[aux] && vizinhoValido(casa,aux)){
			valor = aux;
			break;
		}
	}
	return valor;
}

/* Função: fitness ...*/
INDIVIDUO fitness(INDIVIDUO copia){
	INDIVIDUO adaptado = copia;
	bool visitadas[TABULEIRO + 1] = {false};

	int casa = adaptado.tour[0], contador = 0;
	visitadas[casa] = true;

	for(int i = 0; i < TABULEIRO-1; i++){
		casa = adaptado.tour[i];
		int proximo = adaptado.tour[i+1];

		if(vizinhoValido(casa, proximo) && !visitadas[proximo]){
			visitadas[proximo] = true;
			contador++;
		}
		else{
			proximo = regraWarnsdorff(casa, visitadas);
			if(proximo == 0)
				break;

			adaptado.tour[i+1] = proximo;
			visitadas[proximo] = true;
			contador++;
		}
	}
	
	adaptado.fitness = contador;

	return adaptado;
}

/* Função: vetorMovimentos ...*/
int vetorMovimentos(int casa) {
	int X = 0, Y = 0;
	coordenadas(casa, &X, &Y);

	int eixoX[] = {2, 1, -1, -2, -2, -1, 1, 2};
	int eixoY[] = {1, 2, 2, 1, -1, -2, -2, -1};
	int minMovimentos = 9, movimentos[9] = {0}, j = 0;

	for(int i = 0; i < 8; i++) {
		int auxX = X + eixoX[i];
		int auxY = Y + eixoY[i];
		int aux = (auxX - 1) * sqrt(TABULEIRO) + auxY;
		if (posicaoValida(auxX, auxY) && vizinhoValido(casa,aux)){
			movimentos[j] = aux;
			j++;
		}
	}
	
	int valor =	movimentos[gerarNumAleatorio(j)];

	return valor;
}

/* Função: inicializaCentro ...*/
void inicializaCentro(INDIVIDUO *populacao)
{
	int meio = 0, N = sqrt(TABULEIRO);
	meio = (pow(N,2) + 1)/2;

	for(int i = 0; i < POPULACAO; i++){
		populacao[i].tour[0] = meio;
		for(int j = 1; j < TABULEIRO; j++)
			populacao[i].tour[j] = j;
	}
	
	for(int i = 0; i < POPULACAO; i++){
		for(int j = 1; j < TABULEIRO; j++){
			int pos = (rand() % (TABULEIRO-1) + 1);
			int temp = populacao[i].tour[j];
			populacao[i].tour[j] = populacao[i].tour[pos];
			populacao[i].tour[pos] = temp;
		}
		//...Inicializa o parâmetro de estratégia sigma para cada indivíduo
		populacao[i].sigma = 1.0; // Valor inicial padrão para a estratégia de mutação
		populacao[i] = fitness(populacao[i]);	
	}
}

/*
* Implementação das Estratégias Evolucionárias (ES)
* Gera um número aleatório seguindo uma distribuição normal (Gaussiana)
* com média 0 e desvio padrão 1, Método polar de Marsaglia
* Link  referência: https://en.wikipedia.org/wiki/Marsaglia_polar_method
*/
double gerarGaussiano(double media, double desvio_padrao) {
    static double spare;
    static bool hasSpare = false;

    if (hasSpare) {
        hasSpare = false;
        return spare * desvio_padrao + media;
    } else {
        double u, v, s;
        do {
            u = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
            v = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
            s = u * u + v * v;
        } while (s >= 1.0 || s == 0.0);
        s = sqrt(-2.0 * log(s) / s);
        spare = v * s;
        hasSpare = true;
        return media + desvio_padrao * u * s;
    }
}

/*
* Implementação das Estratégias Evolucionárias (ES)
* Aplica a mutação auto-adaptável das Estratégias Evolucionárias.
* O processo ocorre em duas etapas:
* 1. Muta-se o parâmetro de estratégia 'sigma', que controla a intensidade dos passos da mutação.
* 2. Usa-se o novo 'sigma' para decidir se a mutação no percurso ocorrerá.
* Isto permite que o algoritmo aprenda autonomamente a melhor forma de mutar.
*
* Parâmetros:
* filho - O indivíduo a ser mutado.
*
* Retorno:
* O indivíduo após a tentativa de mutação.
*/
INDIVIDUO mutacaoES(INDIVIDUO filho){
    INDIVIDUO individuo_mutado = filho;

    // valor de τ (tau) fixo de 1/ raiz de n
    double tau = 1.0 / sqrt(TABULEIRO);
    double epsilon = 0.0001; // Valor mínimo para o sigma, para evitar que a evolução pare

    // MUTAÇÃO DA ESTRATÉGIA: Muta o valor de sigma
    // A fórmula σ' = σ ⋅ exp(τ ⋅ N(0,1)) faz com que o sigma evolua.
    individuo_mutado.sigma = individuo_mutado.sigma * exp(tau * gerarGaussiano(0,1));

    // Garante que o sigma não se torne pequeno demais a ponto de parar a busca
    if (individuo_mutado.sigma < epsilon) {
        individuo_mutado.sigma = epsilon;
    }

    // Diz o passo da mutação, queremos um valor positivo
    int passo = 1 + floor(fabs(individuo_mutado.sigma));
    for (size_t i = 0; i < passo; i++)
    {   int aux = 0;
        do {
            aux = gerarNumAleatorio(TABULEIRO);
        } while (aux == 0 || aux == TABULEIRO - 1);
        // Usa a mesma lógica da 'mutacaoVizinhos' original aplicando n vezes do passo obtido
        individuo_mutado.tour[aux + 1] = vetorMovimentos(individuo_mutado.tour[aux]);
    }

    return individuo_mutado;
        

/* Função: mutacao

    Altera o gene (número de casa) de uma frase filho (cópia), 
	substituindo-o por um número de 1 a NxN.

	Parâmetros:
		filho - Um dos indivíduos da população.
		
	Retorno: 
		O indivíduo da população após ser mutado.
*/
INDIVIDUO mutacao(INDIVIDUO filho){ 
	INDIVIDUO individuo = filho;
	
	int r = gerarNumAleatorio(100);
	if(r <= MUTACAO);
		individuo.tour[gerarNumAleatorio(TABULEIRO)] = rand() % TABULEIRO + 1;

	return individuo;
}

/* Função: mutacaoVizinhos... (não está sendo usada)*/
INDIVIDUO mutacaoVizinhos(INDIVIDUO filho){ 
	INDIVIDUO individuo = filho;
	
	int r = gerarNumAleatorio(100);
	if(r <= MUTACAO){
		int aux = 0;
		do{
			aux = gerarNumAleatorio(TABULEIRO);
		}while(aux == 0 || aux == TABULEIRO-1);
		individuo.tour[aux+1] = vetorMovimentos(individuo.tour[aux]);
	}
	return individuo;
}
    
}

INDIVIDUO selecaoPorTorneio(INDIVIDUO *populacao){
	INDIVIDUO melhor;
	melhor.fitness = -1;

	for(int i = 1; i < TORNEIO; i++){
		INDIVIDUO aux = populacao[gerarNumAleatorio(POPULACAO-1)];
		if(melhor.fitness == -1 || aux.fitness > melhor.fitness){
			melhor = aux;
		}
	}

	return melhor;
}

/* Função: torneioDosDissimilares

	Se a flag for True o torneio privilegiará
	o INDIVÍDUO de maior fitness, do contrário, o de menor 
	fitness.

	Parâmetros:
		populacao - população de indivíduos (frases cópias).
		parametros - PARAMETROS.
		
	Retorno: 
		O indivíduo com o fitness (maior ou menor) obtido pelo torneio.
*/
INDIVIDUO torneioDosDissimilares(INDIVIDUO *populacao, bool flag){
	INDIVIDUO melhor;

	if(flag){
		melhor.fitness = -1;

		for(int i = 1; i < TORNEIO; i++){
			INDIVIDUO aux = populacao[gerarNumAleatorio(POPULACAO-1)];
			if(melhor.fitness == -1 || aux.fitness > melhor.fitness){
				melhor = aux;
			}
		}
	}else{
		melhor.fitness = TABULEIRO;

		for(int i = 1; i < TORNEIO; i++){
			INDIVIDUO aux = populacao[gerarNumAleatorio(POPULACAO-1)];
			if(melhor.fitness == TABULEIRO || aux.fitness < melhor.fitness){
				melhor = aux;
			}
		}
	}

	return melhor;
}


INDIVIDUO recombinacaoUniforme(INDIVIDUO pai, INDIVIDUO mae){
	INDIVIDUO filho;

	for(int i = 0; i < TABULEIRO; i++){
		if(gerarNumAleatorio(2))
			filho.tour[i] = pai.tour[i];
		else
			filho.tour[i] = mae.tour[i];
	}

	// Aplica recombinação intermediária para o parâmetro sigma
	// O filho herda a média da estratégia de mutação (sigma) dos pais.
	// Isso permite que boas estratégias de busca sejam combinadas e passadas adiante.
	filho.sigma = (pai.sigma + mae.sigma) / 2.0;

	return filho;
}


/* Função: comparacao ... */
int comparacao(const void* A, const void* B){
	INDIVIDUO C = *(INDIVIDUO*)A;
	INDIVIDUO D = *(INDIVIDUO*)B;
	if(C.fitness < D.fitness) return 1;
	else return -1;
}

/* Função: elitismo ... */
int elitismo(INDIVIDUO *populacao){
	int selecionados = POPULACAO*ELITISMO/100;
	qsort(populacao, POPULACAO, sizeof(populacao[0]), comparacao);
	return selecionados;
}


/* Função: reproducao ... */
INDIVIDUO reproducao(INDIVIDUO *populacao, int geracao){
	INDIVIDUO *novaPopulacao, melhor, pai, mae, filho;
	novaPopulacao = (INDIVIDUO *)malloc(POPULACAO * sizeof(INDIVIDUO));

	int taxaDeElitismo = elitismo(populacao);
	melhor = populacao[0];

	for(int i = 0; i < POPULACAO-taxaDeElitismo; i++){
		pai = torneioDosDissimilares(populacao, true);
		mae = torneioDosDissimilares(populacao, false);

          // 1. Muta-se cada pai individualmente
        INDIVIDUO pai_mutado = mutacaoES(pai);
        INDIVIDUO mae_mutada = mutacaoES(mae);

        // Recombina-se os pais JÁ MUTADOS
		filho = recombinacaoUniforme(pai_mutado, mae_mutada);
		
		// ALTERAÇÃO - Substituição da mutação original pela mutação auto-adaptável das ES que usa a mutaçao do vizinho
		// As funções de mutação originais ('mutacao' e 'mutacaoVizinhos') não são usadas mais
		// para dar lugar à nova abordagem ES, que permite ao algoritmo aprender a mutar seus passos.
		// filho = mutacao(filho);
		// filho = mutacaoVizinhos(filho);
		// filho = mutacaoES(filho); // Chamada para a nova função de mutação baseada em ES

		filho = fitness(filho);

		if(filho.fitness > melhor.fitness)
			melhor = filho;

		novaPopulacao[i] = filho;
	}

	for(int i = taxaDeElitismo, j = 0; i < POPULACAO; i++, j++)
		populacao[i] = novaPopulacao[j];

	free(novaPopulacao);
	return melhor;
}

 /* Função: imprimir melhor individuo da população, e a geração que foi ... */ 
void imprimirMelhorIndividuo(INDIVIDUO *populacao, int geracao) {
    // Laço principal que percorre cada indivíduo do array 'populacao'
    for (int i = 0; i < POPULACAO; i++) {
        // Imprimir o percurso (tour) e o sigma
         if (populacao[i].fitness == (TABULEIRO-1)) {
            printf("-----------------------------");
            printf("\nIndividuo %d da Geracao %d", i, geracao);
            printf("\nFitness: %d\n", populacao[i].fitness);
            printf("Sigma: %f\n", populacao[i].sigma);
            printf("Tour: ");
            for (int j = 0; j < TABULEIRO; j++) {
                // Acessamos cada casa do percurso do indivíduo atual
                printf("%d ", populacao[i].tour[j]);
            
            }
            return 0;
        }
    }
}

/* Função: AE ... */
void AE()
{
	clock_t inicio, fim;
	double total = 0;
	inicio = clock();

	INDIVIDUO *populacao, melhor;
	populacao = (INDIVIDUO *)malloc(POPULACAO * sizeof(INDIVIDUO));
	int geracao = 0;

	srand(time(NULL));
	inicializaCentro(populacao);
    // inicializa(populacao);

	for(geracao = 1; geracao <= GERACOES; geracao++){
		melhor = reproducao(populacao, geracao);
		printf("\nIteracao %d, melhor fitness %d.\n", geracao, melhor.fitness);
		if(melhor.fitness == TABULEIRO-1)
			break;
	}

	if(geracao > GERACOES) geracao-=1;

	fim = clock();
	total = (double)(fim-inicio)/CLOCKS_PER_SEC;
    // imprimirMelhorIndividuo(populacao, geracao);
     printf("\n-----------------------------");
	printf("\nTempo total gasto pela CPU: %lf\n", total);
	// escreveRelatorio(total,melhor.fitness, geracao);
	free(populacao);
}

/* Função: main ...*/
int main(void){

	// for(int i = 0; i < 10; i++)
		AE();
	
	return 0;
}