#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynTrafficGenerator.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	static tick_t gPacketCounter = 0;

	TrafficGenerator::TrafficGenerator()
	{
		maxNodes = 0;
	}

	bool TrafficGenerator::CreatePacket(tick_t tick, Packet &packet)
	{
		//TODO: criar pacote de acordo com probabilidade

		if (tick % 3 == 0)
			return false;

		int source = packet.currentNode;

		auto tmp = availableNodes;
		tmp.erase(tmp.begin() + source); // remove current node from available to nodes send list
		int destination = tmp[rand() % tmp.size()];
		
		packet.id = gPacketCounter;
		packet.size = 1;
		packet.source = source;
		packet.destination = destination;

		gPacketCounter++;

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
