#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynNetwork.h"
#include "DynNode.h"
#include "DynDebugConnection.h"

namespace DynamicSimulation
{
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
