#pragma once

namespace DynamicSimulation
{
	class DynamicItem
	{
	public:
		virtual void OnTickUpdate(tick_t tick) = 0;
	};
};
