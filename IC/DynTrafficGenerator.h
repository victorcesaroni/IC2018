#pragma once

namespace DynamicSimulation
{
	class TrafficGenerator
	{
	public:
		int interval;
		int numberOfPackets;
		int sizeOfPacket;
		float chance;
		tick_t lastCreatedTick;

		int maxNodes;

		TrafficGenerator();

		std::vector<int> availableNodes;

		bool CreatePackets(tick_t tick, int currentNode, std::vector<Packet>& packets);
		void UpdateNodeCount(int newCount);
	};
};
