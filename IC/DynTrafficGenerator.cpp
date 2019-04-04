#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynTrafficGenerator.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	TrafficGenerator::TrafficGenerator()
	{
		packetCounter = 0;
		maxNodes = 0;
	}

	bool TrafficGenerator::CreatePacket(tick_t tick, Packet &packet)
	{
		//TODO: criar pacote de acordo com probabilidade

		if (tick % 3 == 0)
			return false;

		int source = packet.pCurrentNode->id;

		auto tmp = availableNodes;
		tmp.erase(tmp.begin() + source); // remove current node from available to nodes send list
		int destination = tmp[rand() % tmp.size()];
		
		packet.id = packetCounter;
		packet.size = 1;
		packet.source = source;
		packet.destination = destination;

		packetCounter++;

		return true;
	}
	
	void TrafficGenerator::UpdateNodeCount(int newCount)
	{
		maxNodes = newCount;
		availableNodes.clear();

		for (int i = 0; i < newCount; i++)
			availableNodes.push_back(i);
	}
};
