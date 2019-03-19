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

	Matrix<int> connectionsMatrix; // matriz que diz se existe conex�o entre (i,j)
	Matrix<int> costMatrix; // matriz que diz qual o custo para ir diretamente (de i, at� j), infinito caso n�o seja poss�vel
	Matrix<int> hopCostMatrix; // matriz que diz se e poss�vel ir diretamente (de i, at� j), infinito caso n�o seja poss�vel
	Matrix<int> minimumDistanceMatrix; // matriz que possui a distancia m�nima para ir, passando por um ou mais nos, (de i, at� j), infinito caso n�o seja poss�vel
	Matrix<int> minimumHopsMatrix; // matriz que possui a distancia m�nima para ir, passando por um ou mais nos, (de i, at� j), infinito caso n�o seja poss�vel
	Matrix<int> utilizationMatrix; // matriz que possui o numero de vezes que um caminho minimo passa por (i, j)
	Matrix<bool> hasConversorMatrix; // matriz que se existe conversor entre (i,j)	
	Matrix<std::vector<LambdaAllocInfo>> lambdaMatrix; // matriz que possui as aloca��es de comprimento de onda (de i, at� j) das rotas de caminho m�nimo, utilizando o menor lambda poss�vel por caminho
	
	Matrix<std::vector<PathInfo>> allPaths; // matriz com todos os caminhos poss�veis (de i, at� j)
	std::vector<PathInfo> minimumCostPaths; // lista de caminhos de dist�ncia m�nimas
	std::vector<PathInfo> minimumHopPaths; // lista de caminhos de saltos m�nimos
	std::vector<int> conversionCounts; // lista que possui o numero de vezes que houve uma convers�o entre no n� i
	int maxLambda; // numero de lambda m�ximo

	float avgHops; // n�mero m�dio de hops, utilizando o caminho m�nimo
	
	LambdaAllocationStrategy bestStrategyToReduceLambda;
	LambdaAllocationStrategy bestStrategyToReduceConversion;
	LambdaAllocationStrategy bestStrategyToReduceFailure;
	int strategiesLambdasCount[STRATEGIES_COUNT]; // quantidade de lambdas por estrat�gia
	int strategiesConversionCount[STRATEGIES_COUNT]; // n�mero de convers�es por estrat�gia
	int strategiesFailCount[STRATEGIES_COUNT]; // n�mero de vezes que a aloca��o de lambda falhou por estrategia

	int debugLevel; // indica se deve exibir mensagens de depura��o do c�digoo

	StaticSimulation(Network *pNetwork, int maxLambda = INFINITE_VAL);
	~StaticSimulation();
	
	//TODO:finish me
	void Run();

	/// encontra todos os caminhos minimos e todos os caminhos partindo de todos os nos ate todos os nos
	void PreparePathsAndMinimumDistanceMatrix();

	/// encontra o menor n�mero poss�vel N no set (sendo N >= 1 e N < infinito)
	/// used: set com n�meros N (sendo N >= 1 e N < infinito)
	/// prefered: um lambda de prefer�ncia
	int FindSmallestLambdaAvailable(const std::set<int>& used, int prefered = -1);

	//TODO:finish me
	void LambdaAllocation(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const std::vector<PathInfo>& paths, std::set<int>& usedLambdas, LambdaAllocationStrategy strategy, int& conversionCount, std::vector<int>& conversionCounts, int& lambdaAllocationFailCount);

	//TODO:finish me
	void DiscoverUsedLambdasInThePath(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdasInThePath);

	//TODO:finish me
	void AllocateLambda(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdas, int& conversionCount, std::vector<int>& conversionCounts, int& lambdaAllocationFailCount);
		
	/*//TODO:finish me
	void AllocateLambdaWithConversor(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdas);*/
	
	/// m�todo auxiliar do m�todo FindAllPaths parar encontrar todos os caminhos poss�veis em um grafo dada uma matriz adjac�ncia
	/// from: n� de origem
	/// to: n� de destino
	/// current: n� atual
	/// cost: matriz com custo para ir (de i, at� j)
	/// N: n�mero de n�s na matriz (n�mero de linhas e colunas)
	/// visited: lista de n�s j� visitados
	/// path: lista de n�s visitados para o caminho atual
	/// level: n�vel atual de busca 
	/// allPaths: matriz com todos os caminhos (de i, at� j)
	void FindAllPathsDfs(int from, int to, int current, const Matrix<int>& cost, int N, std::vector<bool>& visited, std::vector<int>& path, int& level, Matrix<std::vector<PathInfo>>& allPaths);

	/// encontra todos os caminhos poss�veis em um grafo dada uma matriz de adjac�ncia
	/// from: n� de origem
	/// to: n� de destino
	/// cost: matriz com custo para ir (de i, at� j)
	/// allPaths: matriz com todos os caminhos (de i, at� j)
	/// N: n�mero de n�s na matriz (n�mero de linhas e colunas)
	void FindAllPaths(int from, int to, const Matrix<int>& cost, Matrix<std::vector<PathInfo>>& allPaths, int N);

	/// encontra todos os caminhos m�nimos em um grafo dada uma matriz de adjac�ncia
	/// from: n� de origem
	/// cost: matriz com custo para ir (de i, at� j)
	/// paths: lista de caminhos
	/// N: n�mero de n�s na matriz (n�mero de linhas e colunas)
	void FindShortestPaths(int from, const Matrix<int>& cost, std::vector<PathInfo>& paths, int *distance, int N);
};
