#pragma once

#include "stdafx.h"

#include "DynConstants.h"
#include "DynDynamicItem.h"
#include "DynPacket.h"
#include "DynLinkLambda.h"
#include "DynTrafficGenerator.h"
#include "DynDebugConnection.h"
#include "DynNetwork.h"
#include "DynSimulator.h"
#include "DynNode.h"
#include "DynLink.h"

//const int DEBUG_LEVEL = 1 | 2 | 4;
const int DEBUG_LEVEL = 1 | 2;

#define DBG_PRINTF_ERROR(...) if (DEBUG_LEVEL & 1) printf(__VA_ARGS__)
#define DBG_PRINTF_WARN(...) if (DEBUG_LEVEL & 2) printf(__VA_ARGS__)
#define DBG_PRINTF_INFO(...) if (DEBUG_LEVEL & 4) printf(__VA_ARGS__)

namespace DynamicSimulation 
{
	class DebugConnection;
	class DynamicItem;
	class Simulator;
	class Network;
	class Link;
	class LinkLambda;
	class Node;
	class Packet;
	class TrafficGenerator;
};

