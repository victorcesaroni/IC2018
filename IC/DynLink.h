#pragma once

namespace DynamicSimulation
{
	class LinkLambda;
	class Node;

	class Link : public DynamicItem
	{
	public:
		int id;
		Node *pNodeFrom;
		Node *pNodeTo;
		std::vector<LinkLambda> lambdas;
		int destination;
		std::string destinationNodeName;
		int maxPacketsPerTick;
		bool conversor;
		int cost;

		unsigned long packetsSent;
		unsigned long packetsDropped;
		unsigned long conversionCount;

		Link();
		Link(std::string destinationNodeName, bool conversor, int numLambdas);

		void OnTickUpdate(tick_t tick);
		
		int NextAvailableLambda(const std::set<int>& badLambda);

		bool AddPacket(Packet *pPacket, int lambda);
	};
};
