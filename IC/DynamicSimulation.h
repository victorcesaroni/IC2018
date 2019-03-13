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

	class Node;
	
	class TrafficGenerator
	{
	public:
		int interval;
		int numberOfPackets;
		int sizeOfPacket;

		int maxNodes;
		int packetCounter;

		bool CreatePacket(tick_t tick, Packet &packet)
		{
			//TODO: criar pacote de acordo com probabilidade

			if (tick % 3 == 0)
				return false;

			int source = rand() % maxNodes;
			int destination = rand() % maxNodes;

			if (source == destination)
				destination = (source + 1) % maxNodes;

			packet = Packet(packetCounter, 1, source, destination);
			packetCounter++;

			return true;
		}
	};

	class Packet
	{
	public:
		int id;
		int size;
		int source;
		int destination;
		Node *pNextNode; // proximo no que o pacote vai ir, so vai ficar ativo (diferente de NULL) depois que o pacote for adicionado na rede (enviado)
		bool dropped;

		Packet()
			: id(-1), size(-1), source(-1), destination(-1), pNextNode(NULL)
		{
			dropped = false;
		}

		Packet(int id, int size, int source, int destination)
			: id(id), size(size), source(source), destination(destination), pNextNode(NULL)
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

	class Simulator;

	class Node : public DynamicItem
	{
	public:
		int id;	
		std::string name;
		std::vector<Link> links;
		TrafficGenerator trafficGenerator;
		int packetsDropped;
		int packetsSended;
		std::deque<Packet> *pPacketList;

		Node(Simulator *pSimulator)
			: pPacketList(pPacketList)
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
			Packet packet;
			if (trafficGenerator.CreatePacket(tick, packet))
				AddPacket(&packet);

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

		// adiciona o pacote na rede
		void AddPacket(Packet *pPacket)
		{
			pPacketList->emplace_back(*pPacket);
		}

		// "async"
		bool SendPacket(Packet *pPacket)
		{
			bool found = false;

			for (Link& link : links)
			{
				if (link.destination == pPacket->pNextNode->id)
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
				printf("[ERROR] SendPacket Packet %d INVALID DESTINATION %d\n", pPacket->id, pPacket->pNextNode->id);
			}			

			pPacket->Drop();
			packetsDropped++;

			return false;
		}

		bool ReceivePacket(Packet *pPacket)
		{
			if (id != pPacket->pNextNode->id)
			{
				// nunca deve acontecer
				printf("[ERROR] ReceivePacket Packet %d INVALID DESTINATION %d\n", pPacket->id, pPacket->pNextNode->id);
				
				pPacket->Drop();
				packetsDropped++;
				return false;
			}

			if (pPacket->pNextNode->id == id)
			{
				// pacote eh para mim
				return true;
			}

			AddPacket(pPacket);

			return true;
		}
	};

	class Link : public DynamicItem
	{
	public:
		std::vector<LinkLambda> lambdas;
		int destination;
		int maxPacketsPerTick;
		bool conversor;

		int packetsDropped;

		Link()
		{
			packetsDropped = 0;
		}

		void OnTickUpdate(tick_t tick)
		{
			//TODO: processar o trafego dos pacote atraves dos lambdas

			for (LinkLambda& linkLambda : lambdas)
			{
				if (linkLambda.allocated)
				{
					//TODO: talvez adicionar um ponteiro a Network, para poder eliminar esse pDestinationNode, e acessar pelos ids dos Node
					if (linkLambda.pPacket->pNextNode == NULL) // nunca deve acontecer
						printf("[%d] FATAL ERROR with packet %d\n", linkLambda.pPacket->id);
					
					linkLambda.pPacket->pNextNode->ReceivePacket(linkLambda.pPacket);
					linkLambda.Deallocate();
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
					packet.pNextNode = &pNetwork->nodes[nextNode];
					packet.pNextNode->SendPacket(&packet);
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

