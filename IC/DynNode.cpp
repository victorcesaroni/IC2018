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
		std::vector<Packet> packets;
		if (trafficGenerator.CreatePackets(tick, this->id, packets))
		{
			for (Packet packet : packets)
			{
				AddPacket(&packet);
			}
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
		printf("[INFO] [Node %d] Adding packet P%d\n", id, pPacket->id);

		pPacketList->emplace_back(*pPacket);
	}

	// "async"
	bool Node::SendPacket(Packet *pPacket)
	{
		packetsSent++;

		printf("[INFO] [Node %d] Sending packet P%d to Node %d\n", id, pPacket->id, pPacket->destination);

		if (pPacket->destination == id)
		{
			printf("[WARNING] [Node %d] SendPacket packet P%d SENDING TO MYSELF %d\n", id, pPacket->id, pPacket->nextNode);
		}

		//pPacket->pCurrentNode = this;
		pPacket->currentNode = this->id;
		pPacket->source = id; // atualiza a origem do pacote para ser o no atual
		pPacket->nextNode = pNetwork->GetNextHopToPacket(pPacket->source, pPacket->destination);
		
		if (pPacket->nextNode == -1)
			printf("[ERROR] [Node %d] SendPacket packet P%d HOP NOT FOUND\n", id, pPacket->id);
		
		bool found = false;

		for (Link& link : links)
		{
			if (link.destination == pPacket->nextNode)
			{
				found = true;

				int lambda = GetBestLambdaForLink(link);

				if (lambda != -1 && link.AddPacket(pPacket, lambda))
				{
					printf("[INFO] [Node %d] SendPacket packet P%d added on lambda %d\n", id, pPacket->id, lambda);
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
			printf("[ERROR] [Node %d] SendPacket packet P%d INVALID DESTINATION %d\n", id, pPacket->id, pPacket->nextNode);
		}

		printf("[INFO] [Node %d] SendPacket dropped packet P%d\n", id, pPacket->id);

		pPacket->Drop();
		packetsDropped++;

		return false;
	}

	bool Node::ReceivePacket(Packet *pPacket, LinkLambda *lambdaFrom)
	{
		if (id != pPacket->nextNode)
		{
			// nunca deve acontecer
			printf("[ERROR] ReceivePacket packet P%d INVALID DESTINATION %d\n", pPacket->id, pPacket->nextNode);

			pPacket->Drop();
			packetsDropped++;
			return false;
		}

		if (pPacket->nextNode == id)
		{
			// pacote eh para mim
			printf("[INFO] [Node %d] Succefully received packet P%d\n", id, pPacket->id);
			return true;
		}

		AddPacket(pPacket);

		return true;
	}
};
