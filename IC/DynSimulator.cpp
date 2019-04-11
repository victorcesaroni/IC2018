#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynSimulator.h"
#include "DynNetwork.h"

namespace DynamicSimulation
{
	Simulator::Simulator(Network *pNetwork, int tickInterval, size_t ticksToRun)
		: pNetwork(pNetwork), tickInterval(tickInterval), ticksToRun(ticksToRun)
	{
		globalTickCount = 0;
		packetsSent = 0;
		packetsFailed = 0;
		running = false;
	}

	void Simulator::Run()
	{
		running = true;
	}
	void Simulator::Pause()
	{
		running = false;
	}
	void Simulator::Reset()
	{
		running = false;
		globalTickCount = 0;
		waitingToSend.clear();
	}

	void Simulator::DoSimulation()
	{
		while (running)
		{
			OnTickUpdate(globalTickCount);

			globalTickCount += tickInterval;

			if (globalTickCount >= ticksToRun)
				Pause();

			HandleTickcountOverflow();
		}
	}

	void Simulator::OnTickUpdate(tick_t tick)
	{
		printf("---[Tick %d]---\n", tick);

		for (Node *node : pNetwork->nodes)
		{
			node->OnTickUpdate(globalTickCount);
		}


		// envia os pacotes em ordem aleatoria, para garantir a validade da concorrencia entre os nos
		while (!waitingToSend.empty())
		{
			int idx = rand() % waitingToSend.size();
			Packet& packet = waitingToSend[idx];

			printf("Sending packet %d to Node %d\n", packet.id, packet.destination);
			// envia o pacote que vai ser processado pelo lambda no proximo tick
			//TODO: talvez tenha que mudar essa questao de processar no proximo tick
			
			//packet.pNextNode = &pNetwork->nodes[nextNode];
			//packet.pCurrentNode->SendPacket(&packet);
			pNetwork->nodes[packet.currentNode]->SendPacket(&packet);
	
			waitingToSend.erase(waitingToSend.begin() + idx);
		}

		// coleta as estatisticas para cada no
		for (Node *node : pNetwork->nodes)
		{
			packetsSent += node->packetsSent;
			packetsFailed += node->packetsDropped;

			node->ResetPacketsCounter();
		}
	}

	void Simulator::HandleTickcountOverflow()
	{
		size_t maxVal = 2 << (sizeof(tick_t) * 8);

		if (globalTickCount + tickInterval >= maxVal - tickInterval)
		{
			//TODO: handle overflow
			// ideia basica do momento eh ajustar todos os timers ativos para suportar o novo globalTickCount
			globalTickCount = 0;
		}
	}
};
