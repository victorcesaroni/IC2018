#pragma once

namespace DynamicSimulation
{
	class TrafficGenerator
	{
	public:
		int interval;
		int numberOfPackets;
		int sizeOfPacket;

		int maxNodes;
		int packetCounter;

		TrafficGenerator();

		std::vector<int> availableNodes;

		bool CreatePacket(tick_t tick, Packet &packet);
		void UpdateNodeCount(int newCount);
	};
};
