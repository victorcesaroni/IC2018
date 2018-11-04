#include "stdafx.h"
#include "Network.h"
#include "NetworkConfigParser.h"

static void Parse(const char *path, Network *pNetwork)
{
	FILE *file = NULL;
	fopen_s(&file, path, "r");

	if (!file)
	{
		printf("Cannot read file.\n");
		return;
	}

	char buf[2048] = {};
	char buf2[2048] = {};

	while (!feof(file))
	{
		if (!fgets(buf, sizeof(buf), file))
			break;

		if (buf[0] == '#' || buf[0] == '\r' || buf[0] == '\n' || buf[0] == ' ')
			continue;

		char nodeName[256] = {};
		int linkCount = 0;
		sscanf(buf, "%s %d", nodeName, &linkCount);

		std::vector<Link> links = {};

		for (int i = 0; i < linkCount; i++)
		{
			if (!fgets(buf2, sizeof(buf2), file))
				break;

			char nodeName2[256] = {};
			int cost = 0;
			sscanf(buf2, "%s %d", nodeName2, &cost);

			links.push_back(Link(nodeName2, cost));
		}

		pNetwork->AddNode(nodeName, links);
	}

	fclose(file);
}
