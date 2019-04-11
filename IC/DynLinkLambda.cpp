#include "stdafx.h"
#include "DynamicSimulation.h"
#include "DynLinkLambda.h"
#include "DynPacket.h"

namespace DynamicSimulation
{
	LinkLambda::LinkLambda(int lambda)
		: lambda(lambda)
	{
		allocated = false;
		packet.destination = -1;
	}

	void LinkLambda::Deallocate()
	{
		allocated = false;
		packet.destination = -1;
	}

	bool LinkLambda::AddPacket(Packet *pPacket)
	{
		if (allocated)
			return false;

		this->packet = *pPacket;
		allocated = true;

		return true;
	}
};
