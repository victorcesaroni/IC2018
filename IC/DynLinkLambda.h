#pragma once

namespace DynamicSimulation
{
	class Packet;

	class LinkLambda
	{
	public:
		int lambda;
		bool allocated;
		Packet packet;

		LinkLambda(int lambda);

		void Deallocate();

		bool AddPacket(Packet *pPacket);
	};
};
