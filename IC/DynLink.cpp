#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynLink.h"
#include "DynLinkLambda.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	Link::Link()
	{
		pNode = NULL;
		packetsDropped = 0;
	}

	Link::Link(Node *pNode, int destination, bool conversor, int numLambdas)
		: pNode(pNode), destination(destination), conversor(conversor)
	{
		lambdas.clear();
		for (int i = 0; i < numLambdas; i++)
			lambdas.push_back(LinkLambda(i));		

		packetsDropped = 0;
	}

	Link::Link(int destination, bool conversor, int numLambdas)
		: Link::Link(NULL, destination, conversor, numLambdas)
	{

	}

	void Link::OnTickUpdate(tick_t tick)
	{
		if (pNode == NULL)
			printf("[%d] FATAL ERROR pNode == NULL\n", tick);

		//TODO: processar o trafego dos pacote atraves dos lambdas (talvez seja melhor deixar aqui mesmo por questoes de implementacao)

		for (LinkLambda& linkLambda : lambdas)
		{
			if (linkLambda.allocated)
			{
				if (linkLambda.packet.nextNode == -1) // nunca deve acontecer
					printf("[%d] FATAL ERROR with packet %d nextNode == -1\n", tick, linkLambda.packet.id);

				Node *pNextNode = pNode->pNetwork->nodes[linkLambda.packet.nextNode];

				if (pNextNode == NULL) // nunca deve acontecer
					printf("[%d] FATAL ERROR with packet %d\n", tick, linkLambda.packet.id);

				pNextNode->ReceivePacket(&linkLambda.packet, &linkLambda);

				// release the lambda
				linkLambda.Deallocate();
			}
		}
	}

	int Link::NextAvailableLambda()
	{
		if (!conversor)
			return -1;

		return -1;
	}

	bool Link::AddPacket(Packet *pPacket, int lambda)
	{
		if (lambda == -1)
		{
			printf("[%d] AddPacket FATAL ERROR with packet %d\n", pPacket->id);
			return false;
		}

		if (lambdas[lambda].allocated)
		{
			// conversao automatica feita pelo Link (talvez seja melhor deixar isso no Node)
			int nextLambda = NextAvailableLambda();

			if (nextLambda != -1)
				lambda = nextLambda;
			else
				return false;
		}

		if (lambdas[lambda].AddPacket(pPacket))
		{
			//TODO: propagar para o proximo node e se desligar da conexao (caso traga o processamento do trafego para o lambda em vez do Link)
			return true;
		}

		return false;
	}
};
