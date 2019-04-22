#pragma once

namespace DynamicSimulation
{
	class Packet;
	class Link;

	class Node : public DynamicItem
	{
	public:
		int id;
		std::string name;
		Network *pNetwork;
		std::vector<Link> links;
		TrafficGenerator trafficGenerator;
		int packetsDropped;
		int packetsSent;
		std::deque<Packet> *pPacketList;

		Node();
		Node(Network *pNetwork, int id, std::string name, std::vector<Link> links);

		void UpdateWaitingToSendList(std::deque<Packet> *pPacketList);

		void ResetPacketsCounter();

		void OnTickUpdate(tick_t tick);
		
		int GetBestLambdaForLink(const Link& link);

		// adiciona o pacote na rede
		void AddPacket(Packet *pPacket);

		// "async"
		bool SendPacket(Packet *pPacket);

		bool ReceivePacket(Packet *pPacket, LinkLambda *lambdaFrom);
	};
};
