#include "stdafx.h"
#include "NetworkInfo.h"

PathInfo::PathInfo()
{

}

PathInfo::PathInfo(int from, int to, const std::vector<int>& hops)
	: from(from), to(to), hops(hops)
{
	
}

LambdaAllocInfo::LambdaAllocInfo(int from, int to, int lambda) 
	: from(from), to(to), lambda(lambda)
{

}
