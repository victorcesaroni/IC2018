#pragma once

namespace DynamicSimulation
{
	class Node;
	class Link;

	class Network
	{
	public:
		std::vector<Node> nodes;
		std::vector<DebugConnection> debugConnections;
		int nodeCount;

		Network();

		void AddNode(std::string name, std::vector<Link> links);

		int GetNextHopToPacket(int currentHop, int destination);
	};
};
