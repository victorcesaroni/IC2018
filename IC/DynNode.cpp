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
	
	// adiciona o pacote na rede
	void Node::AddPacket(Packet *pPacket)
	{
		DBG_PRINTF_INFO("[INFO] [Node %d] Adding packet P%d\n", id, pPacket->id);

		pPacketList->emplace_back(*pPacket);
	}

	// "async"
	bool Node::SendPacket(Packet *pPacket)
	{
		packetsSent++;

		DBG_PRINTF_INFO("[INFO] [Node %d] Sending packet P%d to Node %d\n", id, pPacket->id, pPacket->destination);

		if (pPacket->destination == id)
		{
			DBG_PRINTF_WARN("[WARNING] [Node %d] SendPacket packet P%d SENDING TO MYSELF %d\n", id, pPacket->id, pPacket->nextNode);
		}

		//pPacket->pCurrentNode = this;
		pPacket->currentNode = this->id;
		pPacket->source = id; // atualiza a origem do pacote para ser o no atual
		pPacket->nextNode = pNetwork->GetNextHopToPacket(pPacket->source, pPacket->destination);
		
		if (pPacket->nextNode == -1)
			DBG_PRINTF_ERROR("[ERROR] [Node %d] SendPacket packet P%d HOP NOT FOUND\n", id, pPacket->id);
		
		bool found = false;

		for (Link& link : links)
		{
			if (link.destination == pPacket->nextNode)
			{
				found = true;

				if (link.AddPacket(pPacket, pPacket->lastLambda))
				{
					return true;
				}
			}
		}

		if (!found)
		{
			DBG_PRINTF_ERROR("[ERROR] [Node %d] SendPacket packet P%d INVALID DESTINATION %d\n", id, pPacket->id, pPacket->nextNode);
		}

		DBG_PRINTF_INFO("[INFO] [Node %d] [DROP] SendPacket dropped packet P%d\n", id, pPacket->id);

		pPacket->Drop();
		packetsDropped++;

		return false;
	}

	bool Node::ReceivePacket(Packet *pPacket, LinkLambda *lambdaFrom)
	{
		// TODO: implementar delay de recebimento colocando um contador no pPacket e decrementar ele cada vez q receive packet for chamado (talvez tenha q mudar a estrutura da lista de pacotes esperando e transferir o controle de quando se retira um pacote dessa lista para aqui, pois podemos manter o pacote nesse lista pelo tempo necessario)

		if (id != pPacket->nextNode)
		{
			// nunca deve acontecer
			DBG_PRINTF_ERROR("[ERROR] ReceivePacket packet P%d INVALID DESTINATION %d\n", pPacket->id, pPacket->nextNode);

			pPacket->Drop();
			packetsDropped++;
			return false;
		}

		if (pPacket->destination == id)
		{
			// pacote eh para mim
			DBG_PRINTF_INFO("[INFO] [Node %d] [RECV] Succefully received packet P%d\n", id, pPacket->id);
			return true;
		}

		DBG_PRINTF_INFO("[INFO] [Node %d] [RECV] Retransmitting packet P%d (received on lambda %d)\n", id, pPacket->id, lambdaFrom->lambda);

		pPacket->lastLambda = lambdaFrom->lambda;
		AddPacket(pPacket);

		return true;
	}
};
