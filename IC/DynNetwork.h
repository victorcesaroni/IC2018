#pragma once

namespace DynamicSimulation
{
	class Node;

	class Network
	{
	public:
		std::vector<Node> nodes;
		std::vector<DebugConnection> debugConnections;

		int GetNextHopToPacket(int currentHop, int destination);
	};
};
