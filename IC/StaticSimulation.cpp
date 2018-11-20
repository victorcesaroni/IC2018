#include "stdafx.h"
#include "Network.h"
#include "NetworkInfo.h"
#include "StaticSimulation.h"

#define DBG_PRINTF(level, ...) {if (debugLevel >= level) { printf(__VA_ARGS__); }}

StaticSimulation::StaticSimulation(Network *pNetwork)
	: pNetwork(pNetwork)
{
	int N = pNetwork->nodeCount;

	DBG_PRINTF(3, "Inicializando matrizes %dx%d\n", N, N);

	// inicializacao das matrizes
	connectionsMatrix.Create(N, N);
	costMatrix.Create(N, N);
	hopCostMatrix.Create(N, N);
	minimumDistanceMatrix.Create(N, N);
	utilizationMatrix.Create(N, N);
	lambdaMatrix.Create(N, N);
	allPaths.Create(N, N);
	hasConversorMatrix.Create(N, N);
	minimumHopsMatrix.Create(N, N);

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			connectionsMatrix[i][j] = 0;
			costMatrix[i][j] = INFINITE_VAL;
			hopCostMatrix[i][j] = INFINITE_VAL;
			minimumDistanceMatrix[i][j] = 0;
			utilizationMatrix[i][j] = 0;
			hasConversorMatrix[i][j] = 0;
			lambdaMatrix[i][j].clear();
			allPaths[i][j].clear();
		}
	}

	for (const auto& node : pNetwork->nodes)
	{
		auto i = node.second.id;
		costMatrix[i][i] = 0;
		hopCostMatrix[i][i] = 0;
		for (const auto& link : node.second.links)
		{
			auto j = pNetwork->nodes[link.to].id;
			connectionsMatrix[i][j] = 1;
			costMatrix[i][j] = link.cost;
			hopCostMatrix[i][j] = 1;
			hasConversorMatrix[i][j] = link.conversor;
		}
	}

	DBG_PRINTF(3, "Inicializando listas\n");

	minimumCostPaths.clear();
	minimumHopPaths.clear();
}

StaticSimulation::~StaticSimulation()
{

}

void StaticSimulation::PreparePathsAndMinimumDistanceMatrix()
{
	DBG_PRINTF(3, "Inicializando caminhos minimos\n");

	auto N = pNetwork->nodeCount;

	// descobre todos os caminhos mínimos		
	for (const auto& node : pNetwork->nodes)
	{
		auto i = node.second.id;
		FindShortestPaths(i, costMatrix, minimumCostPaths, minimumDistanceMatrix[i], N);
		FindShortestPaths(i, hopCostMatrix, minimumHopPaths, minimumHopsMatrix[i], N);		
	}

	DBG_PRINTF(3, "Inicializando caminhos\n");

	// descobre todos os caminhos
	for (const auto& node : pNetwork->nodes)
	{
		auto i = node.second.id;
		for (const auto& node2 : pNetwork->nodes)
		{
			auto j = node2.second.id;

			if (i == j)
				continue;

			FindAllPaths(i, j, costMatrix, allPaths, N);
		}
	}
}

int StaticSimulation::FindSmallestLambdaAvailable(const std::set<int>& used) // funcao que retorna o menor lambda disponivel
{
	int lambda = 1;
	while (used.find(lambda) != used.end() && lambda < INFINITE_VAL)
		lambda++;

	if (lambda == INFINITE_VAL)
		printf("FindSmallestLambdaAvailable() ERROR INFINITE_VAL\n");

	return lambda;
}

void StaticSimulation::LambdaAllocation(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const std::vector<PathInfo>& paths, std::set<int>& usedLambdas, LambdaAllocationStrategy strategy, int& conversionCount)
{
	if (strategy == LambdaAllocationStrategy::BASIC)
	{
		for (const auto& p : paths) // aloca sequencialmente
		{
			if (!p.hops.empty())
			{
				// aloca
				AllocateLambda(lambdaMatrix, p, usedLambdas, conversionCount);
			}
		}
	}
	else if (strategy == LambdaAllocationStrategy::RANDOM)
	{
		std::vector<PathInfo> availablePaths = paths;

		while (!availablePaths.empty())
		{
			// escolhe um caminho aleatorio
			unsigned idx = rand() % availablePaths.size();

			if (!availablePaths[idx].hops.empty())
			{
				// aloca
				AllocateLambda(lambdaMatrix, availablePaths[idx], usedLambdas, conversionCount);
			}

			availablePaths.erase(availablePaths.begin() + idx);
		}
	}
	else if (strategy == LambdaAllocationStrategy::FIRST_FIT)
	{
		std::vector<PathInfo> availablePaths = paths;

		while (!availablePaths.empty())
		{
			// descobre o caminho com o menor lambda
			unsigned idx = 0;
			int best = INFINITE_VAL;

			for (unsigned i = 0; i < availablePaths.size(); i++)
			{
				std::set<int> usedLambdasInThePath;
				DiscoverUsedLambdasInThePath(lambdaMatrix, availablePaths[i], usedLambdasInThePath);
				int smallest = FindSmallestLambdaAvailable(usedLambdasInThePath);

				if (smallest < best)
				{
					best = smallest;
					idx = i;
				}
			}

			if (!availablePaths[idx].hops.empty())
			{
				// aloca
				AllocateLambda(lambdaMatrix, availablePaths[idx], usedLambdas, conversionCount);
			}

			availablePaths.erase(availablePaths.begin() + idx);
		}
	}
	else if (strategy == LambdaAllocationStrategy::LEAST_USED)
	{
		std::vector<PathInfo> availablePaths = paths;

		while (!availablePaths.empty())
		{
			// descobre o caminho com o menor quantidade de lambdas utilizados
			unsigned idx = 0;
			unsigned best = INFINITE_VAL;

			for (unsigned i = 0; i < availablePaths.size(); i++)
			{
				std::set<int> usedLambdasInThePath;
				DiscoverUsedLambdasInThePath(lambdaMatrix, availablePaths[i], usedLambdasInThePath);

				if (usedLambdasInThePath.size() < best)
				{
					best = usedLambdasInThePath.size();
					idx = i;
				}
			}

			if (!availablePaths[idx].hops.empty())
			{
				// aloca
				AllocateLambda(lambdaMatrix, availablePaths[idx], usedLambdas, conversionCount);
			}

			availablePaths.erase(availablePaths.begin() + idx);
		}
	}
}

void StaticSimulation::DiscoverUsedLambdasInThePath(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdasInThePath)
{
	// descobre os lambdas usados no caminho
	int lastHop = path.from;
	for (const auto& h : path.hops)
	{
		for (const auto& l : lambdaMatrix[lastHop][h]) // ida
			usedLambdasInThePath.emplace(l.lambda);

		for (const auto& l : lambdaMatrix[h][lastHop]) // volta
			usedLambdasInThePath.emplace(l.lambda);

		lastHop = h;
	}
}

void StaticSimulation::AllocateLambda(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdas, int& conversionCount)
{
	// lista com caminhos parciais de um caminho completo usando conversores para separar.
	// como um conversor pode alterar o lambda é como se fosse outro caminho, ou seja, não precisa
	// se preocupar em manter o mesmo lambda
	std::vector<PathInfo> subPathsList; 

	{
		PathInfo tmpPath;
		tmpPath.from = path.from;
		tmpPath.to = path.to;
		tmpPath.hops.clear();

		int lastHop = path.from;

		// descobre os segmentos que não possuem conversores
		for (const auto& h : path.hops)
		{
			tmpPath.hops.push_back(h);

			if (hasConversorMatrix[lastHop][h])
			{
				// nesse ponto existe um conversor entre nó anterior e nó atual
				// salva o caminho sem conversores na lista e inicia um novo
				subPathsList.push_back(tmpPath);
				tmpPath.hops.clear();

				// cria novo segmento adicionando o nó atual no começo do próximo item da lista
				tmpPath.from = h;
				tmpPath.to = path.to;
			}

			lastHop = h;
		}

		if (!tmpPath.hops.empty())
		{
			// existe um caminho que não encontrou um conversor nesse ponto, pois acabaram-se os hops
			subPathsList.push_back(tmpPath);
			tmpPath.hops.clear();
		}
	}

	if (debugLevel >= 3)
	{
		DBG_PRINTF(3, "Encontrou %d caminho(s) parciais.\n", subPathsList.size());

		for (auto it = subPathsList.begin(); it != subPathsList.end(); ++it)
		{
			const auto& subPath = *it;

			if (it != subPathsList.begin())
			{
				DBG_PRINTF(3, "[CONVERSAO]%s", pNetwork->FindNameById(subPath.from).c_str());
			}
			else
			{
				DBG_PRINTF(3, " %s", pNetwork->FindNameById(subPath.from).c_str());
			}

			for (const auto& h : subPath.hops)
			{
				DBG_PRINTF(3, "->%s", pNetwork->FindNameById(h).c_str());
			}
		}
		DBG_PRINTF(3, "\n");
	}

	int lastLambda = -1;
	for (auto it = subPathsList.begin(); it != subPathsList.end(); ++it)
	{
		const auto& subPath = *it;

		// nesse ponto o caminho precisa alocar um lambda no caminho parcial inteiro pois essa lista é dividida por conversores
		std::set<int> usedLambdasInThePath;

		// descobre os lambdas usados no caminho parcial
		DiscoverUsedLambdasInThePath(lambdaMatrix, subPath, usedLambdasInThePath);

		// descobre o menor lambda possivel para uso
		int newLambda = FindSmallestLambdaAvailable(usedLambdasInThePath);

		usedLambdas.emplace(newLambda);

		// aloca o lambda no caminho parcial (adiciona o lambda escolhido a lista de lambdas de todos os nos no caminho)
		int lastHop = subPath.from;

		DBG_PRINTF(2, "%s", pNetwork->FindNameById(lastHop).c_str());
		for (const auto& h : subPath.hops)
		{
			DBG_PRINTF(2, "->%s", pNetwork->FindNameById(h).c_str());
			lambdaMatrix[lastHop][h].push_back(LambdaAllocInfo(path.from, path.to, newLambda));
			lastHop = h;
		}
		DBG_PRINTF(2, ": L%d", newLambda);
		if (it + 1 != subPathsList.end())
		{
			DBG_PRINTF(2, "\n", newLambda);
		}

		if (lastLambda != -1 && newLambda != lastLambda)
		{
			conversionCount++;
		}

		lastLambda = newLambda;
	}
	DBG_PRINTF(2, "\n");
}

/*
void StaticSimulation::AllocateLambdaWithConversor(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdas)
{
	int lastHop = path.from;
	for (const auto& h : path.hops)
	{
		std::set<int> usedLambdasInTheHop;

		// descobre os lambdas utilizados do no anterior ate o no atual e vice versa
		for (const auto& l : lambdaMatrix[lastHop][h]) // ida
			usedLambdasInTheHop.emplace(l.lambda);
		for (const auto& l : lambdaMatrix[h][lastHop]) // volta
			usedLambdasInTheHop.emplace(l.lambda);

		// descobre o menor lambda possivel para uso
		int newLambda = FindSmallestLambdaAvailable(usedLambdasInTheHop);

		usedLambdas.emplace(newLambda);

		// aloca o lambda na parte do caminho (adiciona o lambda escolhido a lista de lambdas do no anterior ate o no atual)
		lambdaMatrix[lastHop][h].push_back(LambdaAllocInfo(path.from, path.to, newLambda));

		lastHop = h;
	}
}*/

void StaticSimulation::Run()
{
	PreparePathsAndMinimumDistanceMatrix();

	// numero medio de hops
	int totalHops = 0;
	int minimumPathsCount = 0;
	int N = pNetwork->nodeCount;

	{
		DBG_PRINTF(1, ". Calculando numero medio de hops\n");

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (minimumHopsMatrix[i][j] != INFINITE_VAL && minimumHopsMatrix[i][j] > 0) // existe um hop (de i, até j) ?
				{
					// a minimumHopsMatrix armazena quantos hops levam para chegar de (de i, até j)
					// armazena essa quantidade de hops na soma total e incrementa o contador 'k' para fazer a média depois
					totalHops += minimumHopsMatrix[i][j];
					minimumPathsCount++;
				}
			}
		}

		avgHops = (float)totalHops / (float)minimumPathsCount;

		DBG_PRINTF(1, "  totalHops = %d / minimumPathsCount = %d\n", totalHops, minimumPathsCount);
		DBG_PRINTF(1, "  avgHops = %f\n", avgHops);
	}

	{
		DBG_PRINTF(3, ". Gerando matriz de utilizacao de enlace por caminhos minimos\n");

		// matriz de utilizacao de enlaces por caminhos minimos
		for (const auto& p : minimumCostPaths) // percorre todos os caminhos encontrados anteriormente
		{
			int from = p.from;
			for (unsigned i = 0; i < p.hops.size(); i++) // percorre todos os hops do caminho da iteracao atual
			{
				int to = p.hops[i];
				utilizationMatrix[from][to]++;
				from = to;
			}
		}

		if (debugLevel >= 3)
			pNetwork->PrintNodeMatrix(utilizationMatrix);
	}

	DBG_PRINTF(1, ". Alocando lambdas\n");

	Matrix<std::vector<LambdaAllocInfo>> tmpLambdaMatrix;
	tmpLambdaMatrix.Create(N, N);
	size_t bestLambdaUseCount = INFINITE_VAL;
	int bestConversionCount = INFINITE_VAL;
	LambdaAllocationStrategy bestStrategyToReduceLambda = LambdaAllocationStrategy::BASIC;
	LambdaAllocationStrategy bestStrategyToReduceConversion = LambdaAllocationStrategy::BASIC;

	auto ClearTempMatrix = [&]()
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				tmpLambdaMatrix[i][j].clear();
		}
	};

	auto SaveTempMatrixToReduceLambda = [&](LambdaAllocationStrategy strategy)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				lambdaMatrix[i][j] = tmpLambdaMatrix[i][j];
			}
		}

		bestStrategyToReduceLambda = strategy;
	};

	auto SaveTempMatrixToReduceConversion = [&](LambdaAllocationStrategy strategy)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				lambdaMatrix[i][j] = tmpLambdaMatrix[i][j];
			}
		}

		bestStrategyToReduceConversion = strategy;
	};

	const LambdaAllocationStrategy strategies[STRATEGIES_COUNT] = { BASIC, RANDOM, FIRST_FIT, LEAST_USED };
	const char *strategiesName[STRATEGIES_COUNT] = { "BASIC", "RANDOM", "FIRST_FIT", "LEAST_USED" };
	int strategiesLambdasCount[STRATEGIES_COUNT];
	int strategiesConversionCount[STRATEGIES_COUNT];

	for (int i = 0; i < STRATEGIES_COUNT; i++)
	{
		ClearTempMatrix();

		// executa a alocacao de acordo com a estrategia
		std::set<int> usedLambdas;
		int conversionCount = 0;

		DBG_PRINTF(1, "  Alocando utilizando %s\n", strategiesName[strategies[i]]);
		LambdaAllocation(tmpLambdaMatrix, minimumCostPaths, usedLambdas, strategies[i], conversionCount);

		strategiesLambdasCount[strategies[i]] = usedLambdas.size();
		strategiesConversionCount[strategies[i]] = conversionCount;

		if (usedLambdas.size() < bestLambdaUseCount)
		{
			bestLambdaUseCount = usedLambdas.size();
			// salva a matriz que utiliza a menor quantidade de lambdas
			SaveTempMatrixToReduceLambda(strategies[i]);
		}

		if (conversionCount < bestConversionCount)
		{
			bestConversionCount = conversionCount;
			// salva a matriz que faz a menor quantidade de conversões
			SaveTempMatrixToReduceConversion(strategies[i]);
		}
	}

	for (int i = 0; i < STRATEGIES_COUNT; i++)
	{
		DBG_PRINTF(1, "  %s %d lambdas usados / %d conversoes feitas\n", strategiesName[strategies[i]], strategiesLambdasCount[strategies[i]], strategiesConversionCount[strategies[i]]);
	}

	DBG_PRINTF(1, "  Melhor estrategia encontrada para reduzir lambda: %s\n", strategiesName[bestStrategyToReduceLambda]);
	DBG_PRINTF(1, "  Melhor estrategia encontrada para reduzir conversao: %s\n", strategiesName[bestStrategyToReduceConversion]);
}

void StaticSimulation::FindAllPathsDfs(int from, int to, int current, const Matrix<int>& cost, int N, std::vector<bool>& visited, std::vector<int>& path, int& level, Matrix<std::vector<PathInfo>>& allPaths)
{
	// adaptado: https://www.geeksforgeeks.org/find-paths-given-source-destination/

	visited[current] = true;
	path[level] = current;

	level++;

	if (current == to)
	{
		PathInfo tmp;
		tmp.from = from;
		tmp.to = to;

		for (int i = 1; i < level; i++)
			tmp.hops.push_back(path[i]);

		allPaths[from][to].push_back(tmp);
	}
	else
	{
		for (int i = 0; i < N; i++)
		{
			if (cost[current][i] != INFINITE_VAL)
			{
				if (!visited[i])
					FindAllPathsDfs(from, to, i, cost, N, visited, path, level, allPaths);
			}
		}
	}

	level--;
}

void StaticSimulation::FindAllPaths(int from, int to, const Matrix<int>& cost, Matrix<std::vector<PathInfo>>& allPaths, int N)
{
	std::vector<bool> visited(N);
	std::vector<int> path(N);
	int level = 0;

	for (int i = 0; i < N; i++)
		visited[i] = false;

	FindAllPathsDfs(from, to, from, cost, N, visited, path, level, allPaths);
}

void StaticSimulation::FindShortestPaths(int from, const Matrix<int>& cost, std::vector<PathInfo>& paths, int *distance, int N)
{
	// adaptado: https://www.thecrazyprogrammer.com/2014/03/dijkstra-algorithm-for-finding-shortest-path-of-a-graph.html

	std::vector<bool> visited(N);
	std::vector<int> previous(N);

	for (int i = 0; i < N; i++)
	{
		if (cost[from][i] != INFINITE_VAL)
		{
			distance[i] = cost[from][i];
			previous[i] = from;
		}
		else
		{
			distance[i] = INFINITE_VAL;
			previous[i] = INFINITE_VAL;
		}

		visited[i] = false;
	}

	distance[from] = 0;
	visited[from] = true;
	int nextNode = -1;

	for (int count = 1; count < N - 1; count++)
	{
		int minDistance = INFINITE_VAL;

		for (int i = 0; i < N; i++)
		{
			if (visited[i])
				continue;

			if (distance[i] < minDistance)
			{
				minDistance = distance[i];
				nextNode = i;
			}
		}

		visited[nextNode] = true;
		for (int i = 0; i < N; i++)
		{
			if (visited[i])
				continue;

			if (minDistance + cost[nextNode][i] < distance[i])
			{
				distance[i] = minDistance + cost[nextNode][i];
				previous[i] = nextNode;
			}
		}
	}

	for (int i = 0; i < N; i++)
	{
		if (i == from)
			continue;

		std::vector<int> tmp = {};

		int j = i;
		while (j != from && previous[j] != INFINITE_VAL)
		{
			tmp.push_back(j);
			j = previous[j];
		};

		std::reverse(tmp.begin(), tmp.end());
		paths.push_back(PathInfo(from, i, tmp));
	}
}
