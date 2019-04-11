#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynNode.h"
#include "DynLink.h"
#include "DynTrafficGenerator.h"
#include "DynPacket.h"
#include "DynSimulator.h"

namespace DynamicSimulation
{
	/*int id;
		std::string name;
		std::vector<Link> links;
		TrafficGenerator trafficGenerator;
		int packetsDropped;
		int packetsSent;
		*/
	Node::Node()
		: pPacketList(NULL)
	{
		packetsSent = 0;
		packetsDropped = 0;
	}

	Node::Node(Network *pNetwork, int id, std::string name, std::vector<Link> links)
		: pNetwork(pNetwork), id(id), name(name), links(links), pPacketList(NULL)
	{
		packetsSent = 0;
		packetsDropped = 0;
	}
	
	void Node::UpdateWaitingToSendList(std::deque<Packet> *pPacketList)
	{
		this->pPacketList = pPacketList;
	}

	void Node::ResetPacketsCounter()
	{
		packetsDropped = 0;
		packetsSent = 0;
	}

	void Node::OnTickUpdate(tick_t tick)
	{
		Packet packet;
		//packet.pCurrentNode = this;
		//packet.pNextNode = NULL;
		packet.nextNode = -1;
		packet.currentNode = this->id;
		if (trafficGenerator.CreatePacket(tick, packet))
		{
			AddPacket(&packet);
		}

		for (Link& link : links)
		{
			link.OnTickUpdate(tick);
		}
	}

	int Node::GetBestLambdaForLink(const Link& link)
	{
		for (unsigned i = 0; i != link.lambdas.size(); i++)
		{
			auto& linkLambda = link.lambdas[i];

			if (!linkLambda.allocated)
				return (int)i;
		}

		return -1;
	}

	// adiciona o pacote na rede
	void Node::AddPacket(Packet *pPacket)
	{
		pPacketList->emplace_back(*pPacket);
	}

	// "async"
	bool Node::SendPacket(Packet *pPacket)
	{
		if (pPacket->destination == id)
		{
			printf("[WARNING] SendPacket Packet %d SENDING TO MYSELF %d\n", pPacket->id, pPacket->nextNode);
		}

		//pPacket->pCurrentNode = this;
		pPacket->currentNode = this->id;
		pPacket->source = id; // atualiza a origem do pacote para ser o no atual
		pPacket->nextNode = pNetwork->GetNextHopToPacket(pPacket->source, pPacket->destination);
		
		if (pPacket->nextNode == -1)
			printf("[ERROR] SendPacket Packet %d HOP NOT FOUND\n", pPacket->id);
		
		bool found = false;

		for (Link& link : links)
		{
			if (link.destination == pPacket->nextNode)
			{
				found = true;

				int lambda = GetBestLambdaForLink(link);

				if (link.AddPacket(pPacket, lambda))
				{
					packetsSent++;
					return true;
				}
				else
				{
					break;
				}
			}
		}

		if (!found)
		{
			printf("[ERROR] SendPacket Packet %d INVALID DESTINATION %d\n", pPacket->id, pPacket->nextNode);
		}

		pPacket->Drop();
		packetsDropped++;

		return false;
	}

	bool Node::ReceivePacket(Packet *pPacket)
	{
		if (id != pPacket->nextNode)
		{
			// nunca deve acontecer
			printf("[ERROR] ReceivePacket Packet %d INVALID DESTINATION %d\n", pPacket->id, pPacket->nextNode);

			pPacket->Drop();
			packetsDropped++;
			return false;
		}

		if (pPacket->nextNode == id)
		{
			// pacote eh para mim
			printf("[Node %d] Succefully received packet %d\n", id, pPacket->id);
			return true;
		}

		AddPacket(pPacket);

		return true;
	}
};
