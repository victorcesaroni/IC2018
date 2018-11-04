#include "stdafx.h"
#include "Network.h"
#include "NetworkInfo.h"
#include "StaticSimulation.h"

StaticSimulation::StaticSimulation(Network *pNetwork)
	: pNetwork(pNetwork)
{
	int N = pNetwork->nodeCount;

	printf("Inicializando matrizes %dx%d\n", N, N);

	// inicializacao das matrizes
	connectionsMatrix.Create(N, N);
	costMatrix.Create(N, N);
	hopCostMatrix.Create(N, N);
	minimumDistanceMatrix.Create(N, N);
	utilizationMatrix.Create(N, N);
	lambdaMatrix.Create(N, N);
	lambdaMatrixWithConversor.Create(N, N);
	allPaths.Create(N, N);

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			connectionsMatrix[i][j] = 0;
			costMatrix[i][j] = INFINITE_VAL;
			hopCostMatrix[i][j] = INFINITE_VAL;
			minimumDistanceMatrix[i][j] = 0;
			utilizationMatrix[i][j] = 0;
			lambdaMatrix[i][j].clear();
			lambdaMatrixWithConversor[i][j].clear();
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
		}
	}

	printf("Inicializando listas\n");

	minimumCostPaths.clear();
}

void StaticSimulation::PreparePathsAndMinimumDistanceMatrix(bool useUniformCost)
{
	printf("Inicializando caminhos minimos\n");

	// descobre todos os caminhos mínimos		
	for (const auto& node : pNetwork->nodes)
	{
		auto i = node.second.id;
		FindShortestPaths(i, useUniformCost ? hopCostMatrix : costMatrix, minimumCostPaths, minimumDistanceMatrix[i], pNetwork->nodeCount);
	}

	printf("Inicializando caminhos\n");

	// descobre todos os caminhos
	for (const auto& node : pNetwork->nodes)
	{
		auto i = node.second.id;
		for (const auto& node2 : pNetwork->nodes)
		{
			auto j = node2.second.id;

			if (i == j)
				continue;

			FindAllPaths(i, j, useUniformCost ? hopCostMatrix : costMatrix, allPaths, pNetwork->nodeCount);
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

void StaticSimulation::LambdaAllocation(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const std::vector<PathInfo>& paths, std::set<int>& usedLambdas, bool useConversor, LambdaAllocationStrategy strategy)
{
	if (strategy == LambdaAllocationStrategy::BASIC)
	{
		for (const auto& p : paths) // aloca sequencialmente
		{
			if (!p.hops.empty())
			{
				// aloca
				if (!useConversor)
					AllocateLambda(lambdaMatrix, p, usedLambdas);
				else
					AllocateLambdaWithConversor(lambdaMatrix, p, usedLambdas);
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
				if (!useConversor)
					AllocateLambda(lambdaMatrix, availablePaths[idx], usedLambdas);
				else
					AllocateLambdaWithConversor(lambdaMatrix, availablePaths[idx], usedLambdas);
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
				if (!useConversor)
					AllocateLambda(lambdaMatrix, availablePaths[idx], usedLambdas);
				else
					AllocateLambdaWithConversor(lambdaMatrix, availablePaths[idx], usedLambdas);
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
				if (!useConversor)
					AllocateLambda(lambdaMatrix, availablePaths[idx], usedLambdas);
				else
					AllocateLambdaWithConversor(lambdaMatrix, availablePaths[idx], usedLambdas);
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

void StaticSimulation::AllocateLambda(Matrix<std::vector<LambdaAllocInfo>>& lambdaMatrix, const PathInfo& path, std::set<int>& usedLambdas)
{
	std::set<int> usedLambdasInThePath;

	// descobre os lambdas usados no caminho
	DiscoverUsedLambdasInThePath(lambdaMatrix, path, usedLambdasInThePath);

	// descobre o menor lambda possivel para uso
	int newLambda = FindSmallestLambdaAvailable(usedLambdasInThePath);

	usedLambdas.emplace(newLambda);

	// aloca o lambda no caminho (adiciona o lambda escolhido a lista de lambdas de todos os nos no caminho)
	int lastHop = path.from;
	for (const auto& h : path.hops)
	{
		lambdaMatrix[lastHop][h].push_back(LambdaAllocInfo(path.from, path.to, newLambda));
		lastHop = h;
	}
}

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
}

void StaticSimulation::Run()
{
	PreparePathsAndMinimumDistanceMatrix(true);

	// numero medio de hops
	int sum = 0;
	int k = 0;
	int N = pNetwork->nodeCount;

	{
		printf(". Calculando numero medio de hops\n");

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (minimumDistanceMatrix[i][j] != INFINITE_VAL && minimumDistanceMatrix[i][j] > 0)
				{
					sum += minimumDistanceMatrix[i][j];
					k++;
				}
			}
		}

		avgHops = (float)sum / (float)k;

		printf("  avgHops = %f\n", avgHops);
	}

	{
		printf(". Gerando matriz de utilizacao de enlace por caminhos minimos\n");

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

		pNetwork->PrintNodeMatrix(utilizationMatrix);
	}

	{
		// alocacao de lambda por caminho minimo
		// (o caminho inteiro so pode utilizar um unico numero de lambda)

		//std::map<int, bool> usedLambdas;
		//LambdaAllocation(lambdaMatrix, minimumCostPaths, usedLambdas, false, LambdaAllocationStrategy::BASIC);
	}
	{
		// alocacao de lambda por caminho minimo utilizando conversor
		// (o caminho pode utilizar um diferentes numeros de lambda)
		//std::map<int, bool> usedLambdas;
		//LambdaAllocation(lambdaMatrixWithConversor, minimumCostPaths, usedLambdas, true, LambdaAllocationStrategy::BASIC);
	}

	auto AllocLambda = [&](bool useConversor)
	{
		Matrix<std::vector<LambdaAllocInfo>> tmpLambdaMatrix;
		tmpLambdaMatrix.Create(N, N);
		size_t bestLambdaUseCount = INFINITE_VAL;
		LambdaAllocationStrategy bestStrategy = LambdaAllocationStrategy::BASIC;

		auto ClearTempMatrix = [&]()
		{
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N; j++)
					tmpLambdaMatrix[i][j].clear();
			}
		};

		auto SaveTempMatrix = [&](LambdaAllocationStrategy strategy)
		{
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N; j++)
				{
					if (!useConversor)
						lambdaMatrix[i][j] = tmpLambdaMatrix[i][j];
					else
						lambdaMatrixWithConversor[i][j] = tmpLambdaMatrix[i][j];
				}
			}

			bestStrategy = strategy;
		};

		const LambdaAllocationStrategy strategies[STRATEGIES_COUNT] = { BASIC, RANDOM, FIRST_FIT, LEAST_USED };
		const char *strategiesName[STRATEGIES_COUNT] = { "BASIC", "RANDOM", "FIRST_FIT", "LEAST_USED" };
		int strategiesLambdasCount[STRATEGIES_COUNT];

		for (int i = 0; i < STRATEGIES_COUNT; i++)
		{
			ClearTempMatrix();

			// executa a alocacao de acordo com a estrategia
			std::set<int> usedLambdas;
			LambdaAllocation(tmpLambdaMatrix, minimumCostPaths, usedLambdas, useConversor, strategies[i]);

			strategiesLambdasCount[strategies[i]] = usedLambdas.size();

			if (usedLambdas.size() < bestLambdaUseCount)
			{
				bestLambdaUseCount = usedLambdas.size();

				// salva a matriz que utiliza a menor quantidade de lambdas
				SaveTempMatrix(strategies[i]);
			}
		}

		for (int i = 0; i < STRATEGIES_COUNT; i++)
		{
			printf("  %s %d lambdas usados\n", strategiesName[strategies[i]], strategiesLambdasCount[strategies[i]]);
		}

		printf("  Melhor estrategia encontrada: %s\n", strategiesName[bestStrategy]);
	};

	printf(". Alocando lambdas sem conversor\n");
	AllocLambda(false);

	printf(". Alocando lambdas com conversor\n");
	AllocLambda(true);
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
