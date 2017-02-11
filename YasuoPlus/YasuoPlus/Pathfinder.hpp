#pragma once
#include <set>

#include "PluginData.h"
#include "UnitTree.hpp"

using namespace std;

class Pathfinder
{
	UnitTree Tree;
	set<IUnit*> Closed;
public:
	Pathfinder(UnitTree tree)
	{
		Tree = tree;
	}

	void GenerateGraphAround(Vec2 orgin, float radius)
	{
		
	}

private:
	float CalcTimeFrom


};
