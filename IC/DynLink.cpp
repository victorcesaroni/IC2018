#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynLink.h"
#include "DynLinkLambda.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	Link::Link()
	{
		packetsDropped = 0;
	}

	Link::Link(int destination, bool conversor, std::vector<LinkLambda> lambdas)
		: destination(destination), conversor(conversor), lambdas(lambdas)
	{
		packetsDropped = 0;
	}

	void Link::OnTickUpdate(tick_t tick)
	{
		//TODO: processar o trafego dos pacote atraves dos lambdas

		for (LinkLambda& linkLambda : lambdas)
		{
			if (linkLambda.allocated)
			{
				//TODO: talvez adicionar um ponteiro a Network, para poder eliminar esse pDestinationNode, e acessar pelos ids dos Node
				if (linkLambda.pPacket->pNextNode == NULL) // nunca deve acontecer
					printf("[%d] FATAL ERROR with packet %d\n", tick, linkLambda.pPacket->id);

				linkLambda.pPacket->pNextNode->ReceivePacket(linkLambda.pPacket);
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
			//TODO: propagar para o proximo node e se desligar da conexao
			return true;
		}

		return false;
	}
};
