#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynPacket.h"
#include "DynNode.h"

namespace DynamicSimulation
{
	Packet::Packet()
		: id(-1), size(-1), source(-1), destination(-1), pNextNode(NULL), pCurrentNode(NULL)
	{
		dropped = false;
	}

	Packet::Packet(int id, int size, int source, int destination)
		: id(id), size(size), source(source), destination(destination), pNextNode(NULL), pCurrentNode(NULL)
	{
		dropped = false;
	}

	void Packet::Drop()
	{
		dropped = true;
	}
};
