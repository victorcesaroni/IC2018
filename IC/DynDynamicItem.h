#pragma once

namespace DynamicSimulation
{
	class DynamicItem
	{
		virtual void OnTickUpdate(tick_t tick) = 0;
	};
};
