#pragma once

namespace DynamicSimulation
{
	class Node;
	class Link;

	struct Route
	{
		int from;
		int to;
		int cost;
		std::vector<int> hops;
	};

	class Network
	{
	public:

		std::deque<Node*> nodes;
		std::vector<DebugConnection> debugConnections;
		std::vector<Route> routes;

		int nodeCount;
		int linkCount;

		Network();
		~Network();

		void AddNode(std::string name, std::vector<Link> links);
		void UpdatePaths();
		void FindAllPaths(Node *from, Node *to);
		void FindAllPathsDfs(Node *from, Node *to, Node *current, int currentCost, std::vector<bool>& visited, std::vector<int>& cost, std::vector<int>& path, int& level);

		int GetNextHopToPacket(int currentHop, int destination);
	};
};
