#pragma once
#include <algorithm>

#include "UnitTreeNode.hpp"

using namespace std;

class UnitTree
{
public:
	int usedSpace;
	//vector<UnitTreeNode> nodesVector;

	UnitTreeNode *Root;

	UnitTree()
	{
		Root = NULL;
		usedSpace = 0;
		//nodesVector = vector<UnitTreeNode>(units.size());
	}

	~UnitTree()
	{
		Clear();
	}

	void Clear()
	{
		delete Root;
		Root = NULL;
	}

	void Insert(IUnit* unit)
	{
		Root = Insert(Root, unit);
	}

	void Remove(IUnit* unit)
	{
		Root = Remove(Root, unit);
	}

	void FindInRange(Vec2& orgin, float radius, vector<IUnit*>& result)
	{
		FindInRange(Root, orgin, radius, result);
	}

	void FindInRangeSlower(Vec2& orgin, float radius, vector<IUnit*>& result)
	{
		vector<IUnit*> temp;
		FindInRect(Root, orgin.x - radius, orgin.x + radius, orgin.y - radius, orgin.y + radius, temp);
		for (auto u : temp)
		{
			if ((u->GetPosition().To2D() - orgin).LengthSqr() < radius * radius) {
				result.push_back(u);
			}
		}
	}

private:
	UnitTreeNode* RemoveMin(UnitTreeNode* node)
	{
		if (node->less == NULL)
			return node->greater;
		node->less = RemoveMin(node->less);
		return node;
	}

	UnitTreeNode* Min(UnitTreeNode* node) {
		if (node->less == NULL)
			return node;
		else
			return Min(node->less);
	}

	UnitTreeNode* Remove(UnitTreeNode* node, IUnit* unit, int depth = 0)
	{
		if (node == NULL)
			return NULL;
		if (node->key == unit->GetPosition().To2D())
		{
			if (node->greater == NULL)
				return node->less;
			auto temp = node;
			node = Min(node->greater);
			node->greater = RemoveMin(node->greater);
			node->less = temp->less;
			//Clean up
			temp->greater = NULL;
			temp->less = NULL;
			delete temp;

			return node;
		}

		if (node->splitaxis == 0)
		{
			if ((node->key.x > unit->GetPosition().x))
				node->greater = Remove(node->greater, unit, depth + 1);
			else
				node->less = Remove(node->less, unit, depth + 1);
		}
		else
		{
			if ((node->key.y > unit->GetPosition().y))
				node->greater = Remove(node->greater, unit, depth + 1);
			else
				node->less = Remove(node->less, unit, depth + 1);
		}
		return node;
	}

	UnitTreeNode* Insert(UnitTreeNode* node, IUnit* unit, int depth = 0)
	{
		if (node == NULL)
		{
			auto node = new UnitTreeNode(unit, depth % 2);//GetNextNodeSpace();
														  //node->Init(unit, depth % 2);
			return node;
		}
		auto pos = unit->GetPosition().To2D();
		if (node->key == pos)
		{
			node->values.push_back(unit);
			return node;
		}

		if (node->splitaxis == 0)
		{
			if ((node->key.x > pos.x))
				node->greater = Insert(node->greater, unit, depth + 1);
			else
				node->less = Insert(node->less, unit, depth + 1);
		}
		else
		{
			if ((node->key.y > pos.y))
				node->greater = Insert(node->greater, unit, depth + 1);
			else
				node->less = Insert(node->less, unit, depth + 1);
		}
		return node;
	}

	void FindInRect(UnitTreeNode* node, float xmin, float xmax, float ymin, float ymax, vector<IUnit*>& result)
	{
		if (node == NULL)
			return;
		Vec2 key = node->key;
		if (xmin <= key.x && xmax >= key.x && ymin <= key.y && ymax >= key.y)
			result.insert(result.end(), node->values.begin(), node->values.end());
		//X
		if (node->splitaxis == 0)
		{
			if (key.x < xmin)
			{
				FindInRect(node->less, xmin, xmax, ymin, ymax, result);
				return;
			}
			if (key.x > xmax)
			{
				FindInRect(node->greater, xmin, xmax, ymin, ymax, result);
				return;
			}
			FindInRect(node->less, xmin, xmax, ymin, ymax, result);
			FindInRect(node->greater, xmin, xmax, ymin, ymax, result);
		}
		else
		{
			if (key.y < ymin)
			{
				FindInRect(node->less, xmin, xmax, ymin, ymax, result);
				return;
			}
			if (key.y > ymax)
			{
				FindInRect(node->greater, xmin, xmax, ymin, ymax, result);
				return;
			}
			FindInRect(node->less, xmin, xmax, ymin, ymax, result);
			FindInRect(node->greater, xmin, xmax, ymin, ymax, result);
		}
	}

	void FindInRange(UnitTreeNode* node, Vec2& orgin, float radius, vector<IUnit*>& result)
	{
		if (node == NULL)
			return;
		if ((node->key - orgin).LengthSqr() < radius * radius) {
			result.insert(result.end(), node->values.begin(), node->values.end());
		}

		//X
		if (node->splitaxis == 0)
		{
			if (node->key.x - orgin.x > radius) {
				FindInRange(node->greater, orgin, radius, result);
				return;
			}
			if (node->key.x - orgin.x < -radius) {
				FindInRange(node->less, orgin, radius, result);
				return;
			}
			FindInRange(node->greater, orgin, radius, result);
			FindInRange(node->less, orgin, radius, result);
		}
		else
		{
			if (node->key.y - orgin.y > radius) {
				FindInRange(node->greater, orgin, radius, result);
				return;
			}
			if (node->key.y - orgin.y < -radius) {
				FindInRange(node->less, orgin, radius, result);
				return;
			}
			FindInRange(node->greater, orgin, radius, result);
			FindInRange(node->less, orgin, radius, result);
		}
	}

	/* Slow as fuk!
	void MakeTree(UnitTreeNode* head, vector<IUnit*>& plist, int depth)
	{
	if (!plist.empty())
	{
	int axis = depth % 2;

	std::vector<IUnit*> left_list;
	std::vector<IUnit*> right_list;
	IUnit* median = FindMedian(plist, left_list, right_list, axis);
	head->Init(median, axis);

	UnitTreeNode* left_node = new UnitTreeNode();
	UnitTreeNode* right_node = new UnitTreeNode();

	MakeTree(left_node, left_list, depth + 1);
	if (!left_list.empty()) head->less = left_node;

	MakeTree(right_node, right_list, depth + 1);
	if (!right_list.empty()) head->greater = right_node;
	}
	}

	IUnit* FindMedian(vector<IUnit*> &plist, vector<IUnit*> &left,
	vector<IUnit*> &right, int axis)
	{
	IUnit* median = NULL;
	int size = plist.size();
	int med = ceil(float(size) / float(2));
	int count = 0;

	if (size == 1)
	return plist.front();

	// Using lambda function here, to define comparison function--parametrized by 'axis'
	sort(plist.begin(), plist.end(),
	[&](IUnit* a, IUnit* b) -> bool
	{
	return (axis%2==0)?(a->GetPosition().x > b->GetPosition().x) : (a->GetPosition().y > b->GetPosition().y);
	});

	for (auto& x : plist)
	{
	if (count < med)
	left.push_back(x);
	else
	right.push_back(x);
	++count;
	}
	median = left.back();
	left.pop_back();
	return median;
	}*/


};