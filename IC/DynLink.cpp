#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynLink.h"
#include "DynLinkLambda.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	Link::Link()
		: pNodeFrom(NULL), pNodeTo(NULL), destination(-1), conversor(false), packetsSent(0), packetsDropped(0), cost(1)
	{

	}

	Link::Link(int destination, bool conversor, int numLambdas)
		: pNodeFrom(NULL), pNodeTo(NULL), packetsSent(0), packetsDropped(0), destination(destination), conversor(conversor), cost(1)
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

	int Link::NextAvailableLambda()
	{
		for (int i = 0; i < (int)lambdas.size(); i++)
		{
			if (!lambdas[i].allocated)
				return i;
		}

		return -1;
	}

	bool Link::AddPacket(Packet *pPacket, int lambda)
	{
		packetsSent++;

		if (lambda == -1)
		{
			// nao temos uma preferencia por algum lambda aqui, entao escolhe o proximo disponivel
			lambda = NextAvailableLambda();

			if (lambda == -1)
			{
				// nao tem mais nenhum disponivel
				packetsDropped++;
				return false;
			}
		}

		if (lambdas[lambda].allocated)
		{
			// conversao automatica feita pelo Link (talvez seja melhor deixar isso no Node)
			int nextLambda = (conversor ? NextAvailableLambda() : -1);

			if (nextLambda != -1)
			{
				lambda = nextLambda;
			}
			else
			{
				packetsDropped++;
				return false;
			}
		}

		if (lambdas[lambda].AddPacket(pPacket))
		{

			DBG_PRINTF_INFO("[INFO] [Node %d: Link %d] [ADD] AddPacket packet P%d added on lambda %d\n", pNodeFrom->id, id, pPacket->id, pPacket->lastLambda);
			//TODO: propagar para o proximo node e se desligar da conexao (caso traga o processamento do trafego para o lambda em vez do Link)
			return true;
		}

		packetsDropped++;
		return false;
	}
};
