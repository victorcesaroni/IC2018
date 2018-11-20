#include "stdafx.h"
#include "NetworkInfo.h"
#include "Network.h"

Link::Link(const std::string& to, int cost, bool conversor) :
	to(to), cost(cost), conversor(conversor)
{

}

Node::Node()
{

}

Node::Node(int id, const std::vector<Link>& links)
	: id(id), links(links)
{

}

void Network::AddNode(const std::string& name, const std::vector<Link>& links)
{	
	if (nodes.find(name) != nodes.end())
		throw new std::exception(std::string(name + " already exists").c_str());

	nodes[name] = Node(nodeCount++, links);
}

std::string Network::FindNameById(int id)
{
	for (const auto& node : nodes)
		if (node.second.id == id)
			return node.first;

	throw new std::exception("Node not found");
}

void Network::PrintNodeMatrixNotInf(const Matrix<int>& matrix)
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

void Network::PrintNodeMatrix(const Matrix<int>& matrix)
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

void Network::PrintPairMatrix(const Matrix<int>& matrix)
{
	for (int i = 0; i < matrix.N; i++)
	{
		for (int j = 0; j < matrix.N; j++)
		{
			if (i == j || matrix[i][j] == 0)
				continue;
			printf("(%s->%s) ", FindNameById(i).c_str(), FindNameById(j).c_str());
			printf("%d", matrix[i][j]);
			printf("\n");
		}
	}
}

void Network::PrintLambdaPairMatrix(const Matrix<std::vector<LambdaAllocInfo>>& matrix)
{
	for (int i = 0; i < matrix.N; i++)
	{
		for (int j = 0; j < matrix.N; j++)
		{
			if (i == j || matrix[i][j].empty())
				continue;
			/*printf("(%s,%s) %d caminhos alocados ", FindNameById(i).c_str(), FindNameById(j).c_str(), matrix[i][j].size());

			for (const auto& l : matrix[i][j])
			{
			printf("[L%d alocado de %s ate %s] ", l.lambda, FindNameById(l.from).c_str(), FindNameById(l.to).c_str());
			}
			printf("\n");*/

			printf("(%s->%s) ", FindNameById(i).c_str(), FindNameById(j).c_str());
			std::vector<int> lambdas = {};

			for (const auto& l : matrix[i][j])
				lambdas.push_back(l.lambda);

			std::set<int> s(lambdas.begin(), lambdas.end());
			lambdas.assign(s.begin(), s.end());

			printf("(%d lambdas) ", lambdas.size());
			for (const auto& l : lambdas)
				printf("L%d ", l);
			printf("\n");
		}
	}
}
