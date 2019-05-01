#pragma once

namespace DynamicSimulation
{
	class Packet;

	class Simulator
	{
	public:
		std::deque<Packet> waitingToSend;
		int tickInterval;
		bool running;
		Network *pNetwork;
		size_t ticksToRun;

		tick_t globalTickCount;

		Simulator(Network *pNetwork, int tickInterval, size_t ticksToRun);

		void Run();
		void Pause();
		void Reset();

		void DoSimulation();

	private:
		void OnTickUpdate(tick_t tick);

		void HandleTickcountOverflow();
	};
};
