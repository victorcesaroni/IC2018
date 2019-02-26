#pragma once

#include <string>
#include <vector>

namespace DynamicSimulation 
{
	enum NodeState
	{
		IDLE,
		RECEIVING_PACKET,
		SENDING_PACKET,
		ALLOCATING_CONNECTION,
	};
	
	class TrafficGenerator
	{
		double interval;
		int numberOfPackets;
		int sizeOfPacket;
	};

	class Packet
	{
		int size;
		int source;
		int destination;
		double arrivalTime;
	};

	class Connection
	{
		int lambda;
		int source;
		int destination;
		std::vector<int> hops;
	};

	class LinkLambda
	{
		bool allocated;
		int lambda;
	};

	class Node
	{
		int id;	
		std::string name;
		NodeState state;
		std::vector<Link> links;
		TrafficGenerator trafficGenerator;
		double nodeProcessingTime;
	};

	class Link
	{
		std::vector<LinkLambda> lambdas;
		int to;
		double bandwidth;
		double propagationTime;
		bool conversor;
	};

	class Network
	{
		std::vector<Node> nodes;
		std::vector<Connection> connections;
	};
};

