// IC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

const int INFINITE_VAL = 999999;

void OutOfMemoryError()
{
	printf("Out of memory\n");
	getchar();
	exit(1);
}

template<class T> class Matrix
{
public:
	int N, M;
	T *data;

	Matrix() { }

	Matrix(int N, int M)
	{
		Create(N, M);
	}

	void Create(int N, int M)
	{
		this->N = N;
		this->M = M;

		data = new T[N * M];

		if (!data)
			OutOfMemoryError();
	}

	void Fill(T val)
	{
		for (int i = 0; i < N * M; i++)
			data[i] = val;
	}

	~Matrix()
	{
		delete[] data;
	}

	T* operator[](const int& i) const
	{
		return &data[i * M];
	}
};

class Link
{
public:
	Link(std::string to, int cost) : to(to), cost(cost) { }
	std::string to; // nome do no de destino dessa conexao
	int cost; // custo para chegar ate o destino
};

class Node
{
public:
	Node() { }
	Node(int id, std::vector<Link> links) : id(id), links(links) { }
	int id; // id do no
	std::vector<Link> links = {}; // conexoes que o no possui
};

class PathInfo
{
public:
	PathInfo() { }
	PathInfo(int from, int to, std::vector<int> hops) : from(from), to(to), hops(hops) { }
	int from; // id do no de origem
	int to; // id do no de destino
	std::vector<int> hops = {}; // id dos nos do caminho, excluindo origem
};

class LambdaAllocInfo
{
public:
	LambdaAllocInfo(int from, int to, int lambda) : from(from), to(to), lambda(lambda) {}
	int from; // id do no de origem
	int to; // id do no de destino
	int lambda; // numero do lambda alocado
};

/*	
	auto PrintMatrix = [](int **m, int N)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (m[i][j] != INFINITE_VAL)
					printf("%d ", m[i][j]);
				else
					printf("X ");
			}
			printf("\n");
		}
		printf("\n");
	};
	auto PrintVector = [](int *m, int N)
	{
		for (int i = 0; i < N; i++)
		{
			if (m[i] != INFINITE_VAL)
				printf("%d ", m[i]);
			else
				printf("X ");
		}
		printf("\n");
	};
*/

class Network
{
public:
	int nodeCount = 0;
	std::map<std::string, Node> nodes = {};

	// adiciona um no na rede
	void AddNode(std::string name, std::vector<Link> links)
	{
		nodes[name] = Node(nodeCount++, links);
	}

	// retorna o nome de um no a partir de seu id
	std::string FindNameById(int id)
	{
		for (const auto& node : nodes)
			if (node.second.id == id)
				return node.first;
		return "NOT FOUND";
	}
	
	// exibe a matriz com os nomes dos nos colocando X em valores infinitos
	void PrintNodeMatrixNotInf(const Matrix<int>& matrix)
	{
		printf("   ");
		for (int i = 0; i < matrix.N; i++)
		{
			printf("%3s ", FindNameById(i).c_str());
		}
		printf("\n");
		for (int i = 0; i < matrix.N; i++)
		{
			printf("%3s ", FindNameById(i).c_str());
			for (int j = 0; j < matrix.N; j++)
			{
				if (matrix[i][j] < INFINITE_VAL)
					printf("%-3d ", matrix[i][j]);
				else
					printf("XXX ");
			}
			printf("\n");
		}
	}

	// exibe a matriz com os nomes dos nos
	void PrintNodeMatrix(const Matrix<int>& matrix)
	{
		printf("   ");
		for (int i = 0; i < matrix.N; i++)
		{
			printf("%3s ", FindNameById(i).c_str());
		}
		printf("\n");
		for (int i = 0; i < matrix.N; i++)
		{
			printf("%3s ", FindNameById(i).c_str());
			for (int j = 0; j < matrix.N; j++)
			{
				printf("%-3d ", matrix[i][j]);
			}
			printf("\n");
		}
	}

	// exibe o valores dos pares da matriz
	void PrintPairMatrix(const Matrix<int>& matrix)
	{
		for (int i = 0; i < matrix.N; i++)
		{
			for (int j = 0; j < matrix.N; j++)
			{
				if (i == j || matrix[i][j] == 0)
					continue;
				printf("(%s,%s) ", FindNameById(i).c_str(), FindNameById(j).c_str());
				printf("%d", matrix[i][j]);
				printf("\n");
			}
		}
	}

	// exibe as alocacoes de lambda por par da matriz
	void PrintLambdaPairMatrix(const Matrix<std::vector<LambdaAllocInfo>>& matrix)
	{
		for (int i = 0; i < matrix.N; i++)
		{
			for (int j = 0; j < matrix.N; j++)
			{
				if (i == j || matrix[i][j].empty())
					continue;
				printf("(%s,%s) %d caminhos alocados ", FindNameById(i).c_str(), FindNameById(j).c_str(), matrix[i][j].size());

				for (const auto& l : matrix[i][j])
				{
					printf("[L%d alocado de %s ate %s] ", l.lambda, FindNameById(l.from).c_str(), FindNameById(l.to).c_str());
				}
				printf("\n");
			}
		}
	}
};

class StaticSimulation
{
public:
	Network *pNetwork;

	Matrix<int> connectionsMatrix; // matriz que diz se existe conexao entre (i,j)
	Matrix<int> costMatrix; // matriz que diz qual o custo para ir diretamente (de i, ate j), infinito caso nao seja possivel
	Matrix<int> hopCostMatrix; // matriz que diz se e possivel ir diretamente (de i, ate j), infinito caso nao seja possivel
	Matrix<int> minimumDistanceMatrix; // matriz que possui a distancia minima para ir, passando por um ou mais nos, (de i, ate j), infinito caso nao seja possivel
	Matrix<int> utilizationMatrix; // matriz que possui o numero de vezes que um caminho minimo passa por (i, j)
	Matrix<std::vector<LambdaAllocInfo>> lambdaMatrix; // matriz que possui o numero de lambda (de i, ate j) da conexão por caminho mínimo, utilizando o menor lambda possivel por caminho
	Matrix<std::vector<LambdaAllocInfo>> lambdaMatrixWithConversor; // matriz que possui o numero de lambda (de i, ate j) da conexão por caminho mínimo, utilizando conversores em todos os nos

	std::vector<PathInfo> paths; // lista de caminhos minimos

	float avgHops; // numero medio de hops, utilizando o caminho minimo
	
	StaticSimulation(Network *pNetwork) 
		: pNetwork(pNetwork)
	{
		int N = pNetwork->nodeCount;
		
		// inicializacao das matrizes
		connectionsMatrix.Create(N, N);
		costMatrix.Create(N, N);
		hopCostMatrix.Create(N, N);
		minimumDistanceMatrix.Create(N, N);
		utilizationMatrix.Create(N, N);
		lambdaMatrix.Create(N, N);
		lambdaMatrixWithConversor.Create(N, N);

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
	}

	// funcao que encontra todos os caminhos minimos partindo de todos os nos ate todos os nos
	// useUniformCost: deve-se usar a matriz com custo 1 entre todos os nos?
	void PreparePathsAndMinimumDistanceMatrix(bool useUniformCost)
	{
		paths.clear();

		for (const auto& node : pNetwork->nodes)
		{
			auto i = node.second.id;
			FindShortestPaths(i, useUniformCost ? hopCostMatrix : costMatrix, minimumDistanceMatrix[i], pNetwork->nodeCount);
		}
	}

	void Run()
	{
		PreparePathsAndMinimumDistanceMatrix(true);

		// numero medio de hops
		int sum = 0;
		int k = 0;
		int N = pNetwork->nodeCount;

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

		// matriz de utilizacao de enlaces por caminhos minimos
		for (const auto& p : paths) // percorre todos os caminhos encontrados anteriormente
		{
			int from = p.from;
			for (unsigned i = 0; i < p.hops.size(); i++) // percorre todos os hops do caminho da iteracao atual
			{
				int to = p.hops[i];
				utilizationMatrix[from][to]++;
				from = to;
			}
		}

		// alocacao de lambdas
		auto FindSmallerLambdaAvailable = [](const std::map<int, bool>& v) // funcao que retorna o menor lambda disponivel
		{
			int lambda = 1;
			while (v.find(lambda) != v.end() && lambda < INFINITE_VAL)
				lambda++;			

			if (lambda == INFINITE_VAL)
				printf("FindSmallerLambdaAvailable() ERROR INFINITE_VAL\n");

			return lambda;
		};

		// alocacao de lambda por caminho minimo
		// (o caminho inteiro so pode utilizar um unico numero de lambda)
		for (const auto& p : paths) // percorre todos os caminhos encontrados anteriormente
		{			
			if (p.hops.empty())
				continue;

			std::map<int, bool> usedLambdas;

			// descobre os lambdas usados no caminho
			int lastHop = p.from;
			for (const auto& h : p.hops)
			{
				for (const auto& l : lambdaMatrix[lastHop][h]) // ida
					usedLambdas[l.lambda] = true;
				
				for (const auto& l : lambdaMatrix[h][lastHop]) // volta
					usedLambdas[l.lambda] = true;
				
				lastHop = h;
			}
			
			// descobre o menor lambda possivel para uso
			int newLambda = FindSmallerLambdaAvailable(usedLambdas);

			// aloca o lambda no caminho (adiciona o lambda escolhido a lista de lambdas de todos os nos no caminho)
			lastHop = p.from;
			for (const auto& h : p.hops)
			{
				lambdaMatrix[lastHop][h].push_back(LambdaAllocInfo(p.from, p.to, newLambda));
				lastHop = h;
			}			
		}

		//TODO:TESTME
		// alocacao de lambda por caminho minimo utilizando conversor
		// (o caminho pode utilizar um diferentes numeros de lambda)
		for (const auto& p : paths) // percorre todos os caminhos encontrados anteriormente
		{
			if (p.hops.empty())
				continue;
			
			int lastHop = p.from;
			for (const auto& h : p.hops)
			{
				std::map<int, bool> usedLambdas;

				// descobre os lambdas utilizados do no anterior ate o no atual e vice versa
				for (const auto& l : lambdaMatrixWithConversor[lastHop][h]) // ida
					usedLambdas[l.lambda] = true;
				for (const auto& l : lambdaMatrixWithConversor[h][lastHop]) // volta
					usedLambdas[l.lambda] = true;

				// descobre o menor lambda possivel para uso
				int newLambda = FindSmallerLambdaAvailable(usedLambdas);

				// aloca o lambda na parte do caminho (adiciona o lambda escolhido a lista de lambdas do no anterior ate o no atual)
				lambdaMatrixWithConversor[lastHop][h].push_back(LambdaAllocInfo(p.from, p.to, newLambda));

				lastHop = h;
			}
		}
	}

	void FindShortestPaths(int from, const Matrix<int>& cost, int *distance, int N)
	{
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
};

class NetworkConfigParser
{
public:
	static void Parse(const char *path, Network *pNetwork) 
	{
		FILE *file = NULL;
		fopen_s(&file, path, "r");

		if (!file)
		{
			printf("Cannot read file.\n");
			return;
		}

		char buf[2048] = {};
		char buf2[2048] = {};

		while (!feof(file))
		{
			if (!fgets(buf, sizeof(buf), file))
				break;

			if (buf[0] == '#' || buf[0] == '\r' || buf[0] == '\n' || buf[0] == ' ')
				continue;
			
			char nodeName[256] = {};
			int linkCount = 0;
			sscanf(buf, "%s %d", nodeName, &linkCount);

			std::vector<Link> links = {};

			for (int i = 0; i < linkCount; i++)
			{
				if (!fgets(buf2, sizeof(buf2), file))
					break;

				char nodeName2[256] = {};
				int cost = 0;
				sscanf(buf2, "%s %d", nodeName2, &cost);

				links.push_back(Link(nodeName2, cost));
			}

			pNetwork->AddNode(nodeName, links);
		}

		fclose(file);
	}
};

int main()
{
	/*{
		Matrix<int> test;
		test.Create(2, 4);
		test.Fill(0);
		test[0][0] = 1;
		test[0][1] = 2;
		test[0][2] = 3;
		test[0][3] = 4;
		//test[1][0] = 5;
		//test[1][1] = 6;
		test[1][2] = 7;
		test[1][3] = 8;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				printf("%d ", test[i][j]);
			}
			printf("\n");
		}
	}

	{
		Matrix<std::vector<int>> test;
		test.Create(2, 4);
		test.Fill(std::vector<int> {0});
		test[0][0].push_back(10);
		test[0][1].push_back(20);
		test[0][2].push_back(30);
		test[0][3].push_back(40);
		test[1][1].push_back(60);
		
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				printf("(%d,%d) ", i, j);
				for (auto& k : test[i][j])
					printf("%d ", k);
				printf("\n");
			}
		}
	}*/

	Network network;
	/*
	1 - 4
	|   |
	2 - 3	
	*/
	/*network.AddNode("n1", { Link("n2", 1), Link("n4", 1) });
	network.AddNode("n2", { Link("n1", 1), Link("n3", 1) });
	network.AddNode("n3", { Link("n2", 1), Link("n4", 1) });
	network.AddNode("n4", { Link("n1", 1), Link("n3", 1) });*/

	NetworkConfigParser::Parse("config.txt", &network);

	StaticSimulation staticSim = StaticSimulation(&network);
	staticSim.Run();

	printf("* Matriz de conexoes:\n");
	network.PrintNodeMatrix(staticSim.connectionsMatrix);
	printf("\n");

	printf("* Matriz de distancia minima:\n");
	network.PrintNodeMatrixNotInf(staticSim.minimumDistanceMatrix);
	printf("\n");

	printf("* Rotas:\n");
	int last = -1;
	for (const auto& p : staticSim.paths)
	{
		auto fromName = network.FindNameById(p.from);
		auto toName = network.FindNameById(p.to);

		if (p.from != last)
		{
			printf("Partindo de %s\n", fromName.c_str());
		}
		if (p.hops.size() > 0)
		{
			printf(" ate %s: ", toName.c_str());

			printf("%s->", fromName.c_str());
			for (unsigned i = 0; i < p.hops.size(); i++)
			{
				auto& h = p.hops[i];
				printf("%s", network.FindNameById(h).c_str());
				if (i != p.hops.size() - 1)
					printf("->");
			}
		}
		else
		{
			printf(" nao pode chegar ate %s.", toName.c_str());
		}

		last = p.from;
		printf("\n");
	}
	printf("\n");

	printf("* Numero medio de hops: ");
	printf("%f\n", staticSim.avgHops);
	printf("\n");

	printf("* Matriz de utilizacao de enlace:\n");
	network.PrintNodeMatrix(staticSim.utilizationMatrix);
	printf("\n");
	network.PrintPairMatrix(staticSim.utilizationMatrix);
	printf("\n");
	
	printf("* Matriz de alocacao de lambda por caminho minimo (sem conversor):\n");
	network.PrintLambdaPairMatrix(staticSim.lambdaMatrix);

	{
		std::map<int, bool> usedLambdas;

		Matrix<int> tmp; // matriz que mostra o numero de lambda alocado de (de i, ate j)
		tmp.Create(staticSim.lambdaMatrix.N, staticSim.lambdaMatrix.M);
		tmp.Fill(0);

		for (int i = 0; i < staticSim.lambdaMatrix.N; i++)
		{
			for (int j = 0; j < staticSim.lambdaMatrix.M; j++)
			{
				if (i == j)
					continue;

				for (const auto& k : staticSim.lambdaMatrix[i][j]) // percorre a lista de lambdas que foram alocados de i ate j
				{
					usedLambdas[k.lambda] = true; // salva o lambda para exibir depois
					tmp[k.from][k.to] = k.lambda; // salva o numero de lambda que foi escolhido para o caminho do lambda atual
				}
			}
		}

		printf("Lambdas usados: ");
		for (const auto& k : usedLambdas)
		{
			printf("L%d ", k.first);
		}
		printf("\n");

		network.PrintNodeMatrix(tmp);
	}

	printf("* Matriz de alocacao de lambda por caminho minimo (com conversor):\n");
	network.PrintLambdaPairMatrix(staticSim.lambdaMatrixWithConversor);

	{
		std::map<int, bool> usedLambdas;

		Matrix<int> tmp; // matriz que mostra o numero de lambda alocado de (de i, ate j)
		tmp.Create(staticSim.lambdaMatrix.N, staticSim.lambdaMatrixWithConversor.M);
		tmp.Fill(0);

		for (int i = 0; i < staticSim.lambdaMatrixWithConversor.N; i++)
		{
			for (int j = 0; j < staticSim.lambdaMatrixWithConversor.M; j++)
			{
				if (i == j)
					continue;

				for (const auto& k : staticSim.lambdaMatrixWithConversor[i][j]) // percorre a lista de lambdas que foram alocados de i ate j
				{
					usedLambdas[k.lambda] = true; // salva o lambda para exibir depois
					tmp[k.from][k.to] = k.lambda; // salva o numero de lambda que foi escolhido para o caminho do lambda atual
				}
			}
		}

		printf("Lambdas usados: ");
		for (const auto& k : usedLambdas)
		{
			printf("L%d ", k.first);
		}
		printf("\n");

		network.PrintNodeMatrix(tmp);
	}

	printf("\n");

	getchar();

	return 0;
}

