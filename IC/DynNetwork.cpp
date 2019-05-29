#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynNode.h"
#include "DynNetwork.h"
#include "DynDebugConnection.h"

namespace DynamicSimulation
{
	Network::Network()
		: nodeCount(0), linkCount(0)
	{

	}

	Network::~Network()
	{
		for (auto& n : nodes)
			delete n;
	}

	int Network::GetNodeIdByName(std::string name)
	{
		for (Node *node : nodes)
		{
			if (node->name == name)
				return node->id;
		}
		
		DBG_PRINTF_ERROR("[ERROR] GetNodeIdByName %s\n", name);

		return -1;
	}

	void Network::Commit()
	{
		// inicializa os ids dos links corretos
		for (Node *node : nodes)
		{
			node->UpdateNodesLinks();
			node->trafficGenerator.UpdateNodeCount(nodes.size());
		}

		UpdatePaths();
	}

	void Network::AddNode(std::string name, std::vector<Link> links)
	{
		nodes.push_back(new Node());

		for (auto& l : links)
		{
			l.id = linkCount;
			linkCount++;
		}

		*nodes[nodeCount] = Node(this, nodeCount, name, links);
		
		nodeCount++;
	}

	int Network::GetNextHopToPacket(int currentHop, int destination)
	{
		// a lista de rotas esta ordenada em ordem crescente, o que facilita para pegar o proximo hop do caminho minimo (sendo o route.hops[0])
		// tendo todos os caminhos possiveis permite que o algoritmo seja alterado para escolher dinamicamente o proximo hop em vez de ser o caminho minimo
		for (const Route& route: routes)
		{
			if (route.from == currentHop && route.to == destination)
			{
				if (!route.hops.empty())
					return route.hops[0];
			}
		}

		return -1;
	}

	void Network::UpdatePaths()
	{
		DBG_PRINTF_INFO("[INFO] Searching routes...\n");

		routes.clear();

		for (Node *nodeA : nodes)
		{
			for (Node *nodeB : nodes)
			{
				if (nodeA->id != nodeB->id)
					FindAllPaths(nodeA, nodeB);
			}
		}

		// ordena as rotas por menor custo
		std::sort(routes.begin(), routes.end(), [](const Route& a, const Route& b) { return a.cost < b.cost; });

		DBG_PRINTF_INFO("[INFO] Found in total %d routes.\n", routes.size());

		for (const Route& route : routes)
		{
			DBG_PRINTF_INFO("%d->%d (cost %d): %d ", route.from, route.to, route.cost, route.from);
			for (int h : route.hops)
				DBG_PRINTF_INFO("%d ", h);
			DBG_PRINTF_INFO("\n");
		}
	}

	void Network::FindAllPaths(Node *from, Node *to)
	{
		std::vector<bool> visited(nodeCount);
		std::vector<int> path(nodeCount);
		std::vector<int> cost(nodeCount);
		int level = 0;

		for (Node *node : nodes)
		{
			visited[node->id] = false;
			cost[node->id] = 0;
		}

		FindAllPathsDfs(from, to, from, 0, visited, path, cost, level);
	}

	void Network::FindAllPathsDfs(Node *from, Node *to, Node *current, int currentCost, std::vector<bool>& visited, std::vector<int>& cost, std::vector<int>& path, int& level)
	{
		// adaptado: https://www.geeksforgeeks.org/find-paths-given-source-destination/

		visited[current->id] = true;
		path[level] = current->id;
		cost[level] = currentCost;

		level++;

		if (current->id == to->id)
		{
			Route tmp;
			tmp.from = from->id;
			tmp.to = to->id;
			tmp.hops.clear();
			tmp.cost = cost[0];

			for (int i = 1; i < level; i++)
			{
				tmp.hops.push_back(path[i]);
				tmp.cost += cost[i];
			}

			routes.push_back(tmp);
		}
		else
		{
			for (const Link& link : current->links)
			{
				if (!visited[link.destination])
				{
					// OBS: esse algoritmo nao suporta que exista mais de um link (aresta) entre 2 nós vizinhos
					FindAllPathsDfs(from, to, link.pNodeTo, link.cost, visited, cost, path, level);
				}
			}
		}

		level--;
	}


};
