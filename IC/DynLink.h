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
		int maxPacketsPerTick;
		bool conversor;
		int cost;

		unsigned long packetsSent;
		unsigned long packetsDropped;

		Link();
		Link(int destination, bool conversor, int numLambdas);

		void OnTickUpdate(tick_t tick);

		int NextAvailableLambda();

		bool AddPacket(Packet *pPacket, int lambda);
	};
};
