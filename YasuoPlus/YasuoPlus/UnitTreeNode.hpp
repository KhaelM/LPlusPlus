#pragma once
#include "PluginSDK.h"

struct UnitTreeNode {
	Vec2 key;
	std::vector<IUnit*> values;

	int splitaxis;// 0 == X, 1 == Y

	UnitTreeNode* greater;
	UnitTreeNode* less;

	bool IsLeaf()
	{
		return greater == 0 && less == 0;
	}

	UnitTreeNode(IUnit* unit, int axis) {
		values.push_back(unit);
		key = unit->GetPosition().To2D();
		splitaxis = axis;
	}

	~UnitTreeNode()
	{
		delete greater;
		delete less;
	}

};
