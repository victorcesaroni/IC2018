#pragma once

#include <string>
#include <vector>
#include <deque>

namespace DynamicSimulation 
{
	using tick_t = size_t; // tipo da variavel que conta os ticks
	
	class DynamicItem
	{
		virtual void OnTickUpdate(tick_t tick) = 0;
	};
	
	class TrafficGenerator : public DynamicItem
	{
	public:
		int interval;
		int numberOfPackets;
		int sizeOfPacket;

		void OnTickUpdate(tick_t tick)
		{

		}
	};

	class Packet
	{
	public:
		int id;
		int size;
		int source;
		int destination;
		Node *pDestinationNode; // so vai ficar ativo (diferente de NULL) depois que o pacote for adicionado na rede (enviado)
		bool dropped;

		Packet(int id, int size, int source, int destination)
			: id(id), size(size), source(source), destination(destination), pDestinationNode(NULL)
		{
			dropped = false;
		}

		void Drop()
		{
			dropped = true;
		}
	};

	class DebugConnection
	{
	public:
		int lambda;
		int source;
		int destination;
		std::vector<int> hops;
	};

	class LinkLambda
	{
	public:
		int lambda;
		bool allocated;
		Packet *pPacket;

		LinkLambda()
		{
			allocated = false;
			pPacket = NULL;
		}

		void Deallocate()
		{
			allocated = false;
			pPacket = NULL;
		}

		bool AddPacket(Packet *pPacket)
		{
			if (allocated)
				return false;

			this->pPacket = pPacket;
			allocated = true;
			
			return true;
		}
	};

	class Node : public DynamicItem
	{
	public:
		int id;	
		std::string name;
		std::vector<Link> links;
		TrafficGenerator trafficGenerator;
		int packetsDropped;
		int packetsSended;

		Node()
		{
			packetsSended = 0;
			packetsDropped = 0;
		}

		void ResetPacketsCounter()
		{
			packetsDropped = 0;
			packetsSended = 0;
		}

		void OnTickUpdate(tick_t tick)
		{
			trafficGenerator.OnTickUpdate(tick);

			for (Link& link : links)
			{
				link.OnTickUpdate(tick);
			}
		}
		
		int GetBestLambdaForLink(const Link& link)
		{
			for (unsigned i = 0; i != link.lambdas.size(); i++)
			{
				const LinkLambda& linkLambda = link.lambdas[i];

				if (!linkLambda.allocated)				
					return (int)i;				
			}

			return -1;
		}

		// "async"
		bool SendPacket(Packet *pPacket)
		{
			bool found = false;

			for (Link& link : links)
			{
				if (link.destination == pPacket->destination)
				{
					found = true;

					int lambda = GetBestLambdaForLink(link);

					if (link.AddPacket(pPacket, lambda))
					{
						pPacket->source = id; // atualiza a origem do pacote para ser o no atual
						packetsSended++;
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
				printf("[ERROR] Packet %d INVALID DESTINATION %d\n", pPacket->id, pPacket->destination);
			}			

			pPacket->Drop();
			packetsDropped++;

			return false;
		}

		bool ReceivePacket(Packet *pPacket)
		{
			if (id != pPacket->destination)
				return false;
		}
	};

	class Link : public DynamicItem
	{
	public:
		std::vector<LinkLambda> lambdas;
		int destination;
		int maxPacketsPerTick;
		bool conversor;

		Link()
		{

		}

		void OnTickUpdate(tick_t tick)
		{
			//TODO: processar o trafego dos pacote atraves dos lambdas

			for (LinkLambda& linkLambda : lambdas)
			{
				if (linkLambda.allocated)
				{
					if (linkLambda.pPacket.)
				}
			}
		}

		int NextAvailableLambda()
		{
			if (!conversor)
				return -1;

			return -1;
		}

		bool AddPacket(Packet *pPacket, int lambda)
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

	class Network
	{
	public:
		std::vector<Node> nodes;
		std::vector<DebugConnection> debugConnections;

		int GetNextHopToPacket(int currentHop, int destination)
		{
			//TODO: selecionar o proximo hop do caminho ate o destino

			return destination;
		}
	};

	class Simulator
	{
	public:
		size_t packetsSended;
		size_t packetsFailed;
		std::deque<Packet> waitingToSend;
		int tickInterval;
		bool running;
		Network *pNetwork;

		tick_t globalTickCount;

		Simulator(int tickInterval)
		{
			globalTickCount = 0;
			packetsSended = 0;
			packetsFailed = 0;
			running = false;
			this->tickInterval = tickInterval;
		}

		void Run()
		{
			running = true;
		}
		void Pause()
		{
			running = false;
		}
		void Reset()
		{
			running = false;
			globalTickCount = 0;
			waitingToSend.clear();
		}
		
		void DoSimulation()
		{
			while (running)
			{
				OnTickUpdate(globalTickCount);

				globalTickCount += tickInterval;
				HandleTickcountOverflow();
			}
		}

	private:
		void OnTickUpdate(tick_t tick)
		{
			for (Node& node : pNetwork->nodes)
			{
				node.OnTickUpdate(globalTickCount);
			}

			// envia os pacotes em ordem aleatoria, para garantir a validade da concorrencia entre os nos
			while (!waitingToSend.empty())
			{
				int idx = rand() % waitingToSend.size();
				Packet& packet = waitingToSend[idx];

				int nextNode = pNetwork->GetNextHopToPacket(packet.source, packet.destination);
				
				if (nextNode != -1)
				{
					printf("[%d] Sending packet %d\n", tick, packet.id);
					// envia o pacote que vai ser processado pelo lambda no proximo tick
					//TODO: talvez tenha que mudar essa questao de processar no proximo tick
					packet.pDestinationNode = &pNetwork->nodes[nextNode];
					packet.pDestinationNode->SendPacket(&packet);
				}
				else
				{
					printf("[%d] Packet %d cannot find a valid path\n", packet.id);
					packet.Drop();
					packetsFailed++;
				}

				waitingToSend.erase(waitingToSend.begin() + idx, waitingToSend.begin() + idx);
			}

			// coleta as estatisticas para cada no
			for (Node& node : pNetwork->nodes)
			{
				packetsSended += node.packetsSended;
				packetsFailed += node.packetsDropped;

				node.ResetPacketsCounter();
			}			
		}

		void HandleTickcountOverflow()
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

};

