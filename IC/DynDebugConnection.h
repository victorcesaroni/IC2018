#pragma once

namespace DynamicSimulation
{
	class DebugConnection
	{
	public:
		int lambda;
		int source;
		int destination;
		std::vector<int> hops;
	};
};
