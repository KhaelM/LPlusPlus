#pragma once
#include <set>

#include "Vectors.hpp"

struct UnitTreeNode {
	Vec2 key;
	std::vector<IUnit*> values;

	int splitaxis;// 0 == X, 1 == Y

	UnitTreeNode* greater;
	UnitTreeNode* less;

	UnitTreeNode() {
		key = Vec2(0, 0);
		splitaxis = 0;
		greater = NULL;
		less = NULL;
	}

	UnitTreeNode(IUnit* unit, int axis) {
		Init(unit, axis);
	}

	void Init(IUnit* unit, int axis)
	{
		values.push_back(unit);
		key = unit->GetPosition().To2D();
		splitaxis = axis;
		greater = NULL;
		less = NULL;
	}

	~UnitTreeNode()
	{
		delete greater;
		delete less;
	}

};
