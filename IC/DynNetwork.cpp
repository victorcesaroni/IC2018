#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynNode.h"
#include "DynNetwork.h"
#include "DynDebugConnection.h"

namespace DynamicSimulation
{
	Network::Network()
		: nodeCount(0)
	{

	}

	Network::~Network()
	{
		for (auto& n : nodes)
			delete n;
	}

	void Network::AddNode(std::string name, std::vector<Link> links)
	{
		nodes.push_back(new Node());

		for (auto& l : links)
			l.pNode = nodes[nodeCount];
		*nodes[nodeCount] = Node(this, nodeCount, name, links);

		nodeCount++;
	}

	int Network::GetNextHopToPacket(int currentHop, int destination)
	{
		//TODO: selecionar o proximo hop do caminho ate o destino

		for (Node *node : nodes)
		{
			if (node->id == destination)
				return node->id;
		}

		return -1;
	}
};
