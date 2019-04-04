#pragma once

namespace DynamicSimulation
{
	class LinkLambda;

	class Link : public DynamicItem
	{
	public:
		std::vector<LinkLambda> lambdas;
		int destination;
		int maxPacketsPerTick;
		bool conversor;

		int packetsDropped;

		Link();
		Link(int destination, bool conversor, std::vector<LinkLambda> lambdas);

		void OnTickUpdate(tick_t tick);

		int NextAvailableLambda();

		bool AddPacket(Packet *pPacket, int lambda);
	};
};
