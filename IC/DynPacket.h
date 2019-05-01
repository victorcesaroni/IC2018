#pragma once

namespace DynamicSimulation
{
	class Node;

	class Packet
	{
	public:
		int id;
		int size;
		int source;
		int destination;
		//Node *pNextNode; // proximo no que o pacote vai ir, so vai ficar ativo (diferente de NULL) depois que o pacote for adicionado na rede (enviado)
		//Node *pCurrentNode;
		int currentNode;
		int nextNode;
		bool dropped;

		int lastLambda;

		Packet();
		Packet(int id, int size, int source, int destination);

		void Drop();
	};
};
