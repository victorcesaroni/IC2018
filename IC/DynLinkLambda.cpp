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
		pPacket = NULL;
	}

	void LinkLambda::Deallocate()
	{
		allocated = false;
		pPacket = NULL;
	}

	bool LinkLambda::AddPacket(Packet *pPacket)
	{
		if (allocated)
			return false;

		this->pPacket = pPacket;
		allocated = true;

		return true;
	}
};
