#pragma once
#include "UnitTreeNode.hpp"

using namespace std;

class UnitTree
{
public:
	UnitTreeNode *Root;

	UnitTree()
	{
		Root = NULL;
	}

	~UnitTree()
	{
		Clear();
	}

	void Clear()
	{
		delete Root;
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
			return new UnitTreeNode(unit, depth % 2);

		if (node->key == unit->GetPosition().To2D())
		{
			node->values.push_back(unit);
			return node;
		}

		if (node->splitaxis == 0)
		{
			if ((node->key.x > unit->GetPosition().x))
				node->greater = Insert(node->greater, unit, depth + 1);
			else
				node->less = Insert(node->less, unit, depth + 1);
		}
		else
		{
			if ((node->key.y > unit->GetPosition().y))
				node->greater = Insert(node->greater, unit, depth + 1);
			else
				node->less = Insert(node->less, unit, depth + 1);
		}
		return node;
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


};