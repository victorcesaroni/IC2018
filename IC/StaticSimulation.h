#pragma once

#include "Constants.h"

//TODO:finish me
enum LambdaAllocationStrategy
{
	BASIC,
	RANDOM,
	FIRST_FIT,
	LEAST_USED,
	STRATEGIES_COUNT,
};

static const char *strategiesName[STRATEGIES_COUNT] = { "BASIC", "RANDOM", "FIRST_FIT", "LEAST_USED" };

//TODO:finish me
class StaticSimulation
{
public:
	Network *pNetwork;

	Matrix<int> connectionsMatrix; // matriz que diz se existe conexão entre (i,j)
	Matrix<int> costMatrix; // matriz que diz qual o custo para ir diretamente (de i, até j), infinito caso não seja possível
	Matrix<int> hopCostMatrix; // matriz que diz se e possível ir diretamente (de i, até j), infinito caso não seja possível
	Matrix<int> minimumDistanceMatrix; // matriz que possui a distancia mínima para ir, passando por um ou mais nos, (de i, até j), infinito caso não seja possível
	Matrix<int> minimumHopsMatrix; // matriz que possui a distancia mínima para ir, passando por um ou mais nos, (de i, até j), infinito caso não seja possível
	Matrix<int> utilizationMatrix; // matriz que possui o numero de vezes que um caminho minimo passa por (i, j)
	Matrix<bool> hasConversorMatrix; // matriz que se existe conversor entre (i,j)	
	Matrix<std::vector<LambdaAllocInfo>> lambdaMatrix; // matriz que possui as alocações de comprimento de onda (de i, até j) das rotas de caminho mínimo, utilizando o menor lambda possível por caminho
	
	Matrix<std::vector<PathInfo>> allPaths; // matriz com todos os caminhos possíveis (de i, até j)
	std::vector<PathInfo> minimumCostPaths; // lista de caminhos de distância mínimas
	std::vector<PathInfo> minimumHopPaths; // lista de caminhos de saltos mínimos
	std::vector<int> conversionCounts; // lista que possui o numero de vezes que houve uma conversão entre no nó i
	int maxLambda; // numero de lambda máximo

	float avgHops; // número médio de hops, utilizando o caminho mínimo
	
	LambdaAllocationStrategy bestStrategyToReduceLambda;
	LambdaAllocationStrategy bestStrategyToReduceConversion;
	LambdaAllocationStrategy bestStrategyToReduceFailure;
	int strategiesLambdasCount[STRATEGIES_COUNT]; // quantidade de lambdas por estratégia
	int strategiesConversionCount[STRATEGIES_COUNT]; // número de conversões por estratégia
	int strategiesFailCount[STRATEGIES_COUNT]; // número de vezes que a alocação de lambda falhou por estrategia

	int debugLevel; // indica se deve exibir mensagens de depuração do códigoo

	StaticSimulation(Network *pNetwork, int maxLambda = INFINITE_VAL);
	~StaticSimulation();
	
	//TODO:finish me
	void Run();

	/// encontra todos os caminhos minimos e todos os caminhos partindo de todos os nos ate todos os nos
	void PreparePathsAndMinimumDistanceMatrix();

	/// encontra o menor número possível N no set (sendo N >= 1 e N < infinito)
	/// used: set com números N (sendo N >= 1 e N < infinito)
	/// prefered: um lambda de preferência
	int FindSmallestLambdaAvailable(const std::set<int>& used, int prefered = -1);

	//TODO:finish me
	void LambdaAllocation(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const std::vector<PathInfo>& paths, std::set<int>& usedLambdas, LambdaAllocationStrategy strategy, int& conversionCount, std::vector<int>& conversionCounts, int& lambdaAllocationFailCount);

	//TODO:finish me
	void DiscoverUsedLambdasInThePath(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdasInThePath);

	//TODO:finish me
	void AllocateLambda(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdas, int& conversionCount, std::vector<int>& conversionCounts, int& lambdaAllocationFailCount);
		
	/*//TODO:finish me
	void AllocateLambdaWithConversor(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdas);*/
	
	/// método auxiliar do método FindAllPaths parar encontrar todos os caminhos possíveis em um grafo dada uma matriz adjacência
	/// from: nó de origem
	/// to: nó de destino
	/// current: nó atual
	/// cost: matriz com custo para ir (de i, até j)
	/// N: número de nós na matriz (número de linhas e colunas)
	/// visited: lista de nós já visitados
	/// path: lista de nós visitados para o caminho atual
	/// level: nível atual de busca 
	/// allPaths: matriz com todos os caminhos (de i, até j)
	void FindAllPathsDfs(int from, int to, int current, const Matrix<int>& cost, int N, std::vector<bool>& visited, std::vector<int>& path, int& level, Matrix<std::vector<PathInfo>>& allPaths);

	/// encontra todos os caminhos possíveis em um grafo dada uma matriz de adjacência
	/// from: nó de origem
	/// to: nó de destino
	/// cost: matriz com custo para ir (de i, até j)
	/// allPaths: matriz com todos os caminhos (de i, até j)
	/// N: número de nós na matriz (número de linhas e colunas)
	void FindAllPaths(int from, int to, const Matrix<int>& cost, Matrix<std::vector<PathInfo>>& allPaths, int N);

	/// encontra todos os caminhos mínimos em um grafo dada uma matriz de adjacência
	/// from: nó de origem
	/// cost: matriz com custo para ir (de i, até j)
	/// paths: lista de caminhos
	/// N: número de nós na matriz (número de linhas e colunas)
	void FindShortestPaths(int from, const Matrix<int>& cost, std::vector<PathInfo>& paths, int *distance, int N);
};
