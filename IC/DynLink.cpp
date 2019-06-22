#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynLink.h"
#include "DynLinkLambda.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	Link::Link()
		: pNodeFrom(NULL), pNodeTo(NULL), destination(-1), destinationNodeName("invalid"), conversor(false), packetsSent(0), packetsDropped(0), conversionCount(0), cost(1)
	{

	}

	Link::Link(std::string destinationNodeName, bool conversor, int numLambdas)
		: pNodeFrom(NULL), pNodeTo(NULL), packetsSent(0), packetsDropped(0), destination(-1), destinationNodeName(destinationNodeName), conversor(conversor), conversionCount(0), cost(1)
	{
		lambdas.clear();
		for (int i = 0; i < numLambdas; i++)
			lambdas.push_back(LinkLambda(i));
	}

	void Link::OnTickUpdate(tick_t tick)
	{
		if (pNodeFrom == NULL)
			DBG_PRINTF_ERROR("[%d] FATAL ERROR pNodeFrom == NULL\n", tick);

		//TODO: processar o trafego dos pacote atraves dos lambdas (talvez seja melhor deixar aqui mesmo por questoes de implementacao)

		for (LinkLambda& linkLambda : lambdas)
		{
			if (linkLambda.allocated)
			{
				if (linkLambda.packet.nextNode == -1) // nunca deve acontecer
					DBG_PRINTF_ERROR("[%d] FATAL ERROR with packet %d nextNode == -1\n", tick, linkLambda.packet.id);

				Node *pNextNode = pNodeFrom->pNetwork->nodes[linkLambda.packet.nextNode];

				if (pNextNode == NULL) // nunca deve acontecer
					DBG_PRINTF_ERROR("[%d] FATAL ERROR with packet %d\n", tick, linkLambda.packet.id);

				pNextNode->ReceivePacket(&linkLambda.packet, &linkLambda);

				// release the lambda
				linkLambda.Deallocate();
			}
		}
	}
	
	int Link::NextAvailableLambda(const std::set<int>& badLambda)
	{	
		for (const LinkLambda& linkLambda : lambdas)
		{
			if (!linkLambda.allocated // nao esta alocado
				&& badLambda.find(linkLambda.lambda) == badLambda.end() // nao esta sendo usado pelo link de volta
				)
				return linkLambda.lambda;
		}

		return -1;
	}

	bool Link::AddPacket(Packet *pPacket, int lambda)
	{
		std::set<int> badLambda = {};

		// considera que os links de ida e volta sao fisicamente os mesmos
		for (const Link& link : this->pNodeTo->links)
		{
			if (link.destination == this->id)
			{
				for (const LinkLambda& linkLambda : link.lambdas)
				{
					if (linkLambda.allocated)
						badLambda.emplace(linkLambda.lambda);
				}
			}
		}

		packetsSent++;
		
		if (lambda == -1)
		{
			// nao temos uma preferencia por algum lambda aqui, entao escolhe o proximo disponivel
			lambda = NextAvailableLambda(badLambda);

			if (lambda == -1)
			{
				// nao tem mais nenhum disponivel
				packetsDropped++;
				return false;
			}
		}

		if (lambdas[lambda].allocated || badLambda.find(lambda) != badLambda.end()) // esta alocado ou o link de volta esta alocado
		{
			// conversao automatica feita pelo Link (talvez seja melhor deixar isso no Node)
			int nextLambda = NextAvailableLambda(badLambda);

			if (!conversor || nextLambda == -1)
			{
				// nao pode converter ou nao tem mais nenhum disponivel
				packetsDropped++;
				return false;
			}

			if (lambda != nextLambda)
				conversionCount++;

			lambda = nextLambda;
		}

		if (lambdas[lambda].AddPacket(pPacket))
		{
			lambdas[lambda].useCount++; // atualiza a quantidade de vezes que o lambda foi utilizado

			// 
			//for (Link& link : this->pNodeTo->links)
			//{
			//	if (link.destination == this->id) // link de volta para o no atual
			//	{
			//		for (LinkLambda& linkLambda : link.lambdas)
			//		{
			//			linkLambda.useCount++; // atualiza a quantidade de vezes que o lambda foi utilizado (no link de volta, pois o link de ida e volta sao considerados os mesmos)
			//		}
			//	}
			//}

			DBG_PRINTF_INFO("[INFO] [Node %d: Link %d] [ADD] AddPacket packet P%d added on lambda %d (last lambda = %d)\n", pNodeFrom->id, id, pPacket->id, lambda, pPacket->lastLambda);
			//TODO: propagar para o proximo node e se desligar da conexao (caso traga o processamento do trafego para o lambda em vez do Link)
			return true;
		}

		packetsDropped++;
		return false;
	}
};
