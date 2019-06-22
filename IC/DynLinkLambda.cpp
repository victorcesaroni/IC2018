#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynLinkLambda.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	LinkLambda::LinkLambda(int lambda)
		: lambda(lambda), useCount(0)
	{
		allocated = false;
		packet.destination = -1;
		useCount = 0;
	}

	void LinkLambda::Deallocate()
	{
		allocated = false;
		packet.destination = -1;
	}

	bool LinkLambda::AddPacket(Packet *pPacket)
	{
		if (allocated)
		{
			DBG_PRINTF_ERROR("[ERROR] LinkLambda::AddPacket already allocated (P%d)\n", pPacket->id);
			return false;
		}

		this->packet = *pPacket;
		allocated = true;

		return true;
	}
};
