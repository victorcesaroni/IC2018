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
		running = false;

		for (auto node : pNetwork->nodes)
		{
			node->UpdateWaitingToSendList(&waitingToSend); // atualiza a lista de pacotes sendo enviados para ser a mesma para todos os nos da rede
		}
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
		DBG_PRINTF_INFO("---[Tick %d]---\n", tick);

		std::vector<size_t> randomOrder(pNetwork->nodes.size());
		for (size_t i = 0; i < pNetwork->nodes.size(); i++)
			randomOrder[i] = i;
		std::random_shuffle(randomOrder.begin(), randomOrder.end());

		for (size_t idx : randomOrder)
		{
			Node* node = pNetwork->nodes[idx];
			node->OnTickUpdate(globalTickCount);
		}

		// envia os pacotes em ordem aleatoria, para garantir a validade da concorrencia entre os nos
		while (!waitingToSend.empty())
		{
			int idx = rand() % waitingToSend.size();
			Packet& packet = waitingToSend[idx];

			//printf("[INFO] Sending packet %d to Node %d\n", packet.id, packet.destination);
			// envia o pacote que vai ser processado pelo lambda no proximo tick
			//TODO: talvez tenha que mudar essa questao de processar no proximo tick
			
			//packet.pNextNode = &pNetwork->nodes[nextNode];
			//packet.pCurrentNode->SendPacket(&packet);
			pNetwork->nodes[packet.currentNode]->SendPacket(&packet);
	
			waitingToSend.erase(waitingToSend.begin() + idx);
		}
	}

	void Simulator::HandleTickcountOverflow()
	{
		size_t maxVal = 2 << (sizeof(tick_t) * 8 - 1) - 1;

		if (globalTickCount + tickInterval >= maxVal - tickInterval)
		{
			//TODO: handle overflow
			// ideia basica do momento eh ajustar todos os timers ativos para suportar o novo globalTickCount
			globalTickCount = 0;

			for (Node *node : pNetwork->nodes)
			{
				node->trafficGenerator.lastCreatedTick = 0;
			}
		}
	}
};
