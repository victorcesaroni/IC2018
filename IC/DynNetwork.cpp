#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynNetwork.h"
#include "DynNode.h"
#include "DynDebugConnection.h"

namespace DynamicSimulation
{
	Network::Network()
		: nodeCount(0)
	{

	}

	void Network::AddNode(std::string name, std::vector<Link> links)
	{
		nodes.push_back(Node(this, nodeCount, name, links));

		// TODO: fix hierarchy
		for (auto& l : nodes[nodeCount].links)
			l.pNode = &nodes[nodeCount];		

		nodeCount++;
	}

	int Network::GetNextHopToPacket(int currentHop, int destination)
	{
		//TODO: selecionar o proximo hop do caminho ate o destino

		for (const Node& node : nodes)
		{
			if (node.id == destination)
				return node.id;
		}

		return -1;
	}
};
